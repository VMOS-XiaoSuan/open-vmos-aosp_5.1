/*
* Copyright (C) 2011 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "HostConnection.h"
#define USE_TCP 0
#if USE_TCP==1
#include "TcpStream.h"
#else
#include "UnixStream.h"
#endif
#include "QemuPipeStream.h"
#include "ThreadInfo.h"
#include <cutils/log.h>
#include "GLEncoder.h"
#include "GL2Encoder.h"
#include <cutils/properties.h>

#define STREAM_BUFFER_SIZE  4*4*1024*1024
#define STREAM_PORT_NUM     22468

#if 0
/* Set to 1 to use a QEMU pipe, or 0 for a TCP connection */
#define  USE_QEMU_PIPE  1
#else
#define  USE_QEMU_PIPE  0   // use unix sockets
#endif
HostConnection::HostConnection() :
    m_stream(NULL),
    m_glEnc(NULL),
    m_gl2Enc(NULL),
    m_rcEnc(NULL)
{
}

HostConnection::~HostConnection()
{
    delete m_stream;
    delete m_glEnc;
    delete m_gl2Enc;
    delete m_rcEnc;
}

HostConnection *HostConnection::get()
{
    /* TODO: Make this configurable with a system property */
    const int useQemuPipe = USE_QEMU_PIPE;

    // Get thread info
    EGLThreadInfo *tinfo = getEGLThreadInfo();
    if (!tinfo) {
        return NULL;
    }

    if (tinfo->hostConn == NULL) {
        HostConnection *con = new HostConnection();
        if (NULL == con) {
            return NULL;
        }

        if (useQemuPipe) {
            QemuPipeStream *stream = new QemuPipeStream(STREAM_BUFFER_SIZE);
            if (!stream) {
                ALOGE("Failed to create QemuPipeStream for host connection!!!\n");
                delete con;
                return NULL;
            }
            if (stream->connect() < 0) {
                ALOGE("Failed to connect to host (QemuPipeStream)!!!\n");
                delete stream;
                delete con;
                return NULL;
            }
            con->m_stream = stream;
        }
        else /* !useQemuPipe */
        {
            char buf[512];
            char portbuf[512];
            property_get("ro.rf.render.ip", buf, "10.254.18.33");
            
            int ret = property_get("ro.rf.render.portnum", portbuf, "0");
            ALOGE("ret=%d\n", ret);

#if USE_TCP==1
            TcpStream *stream = new TcpStream(STREAM_BUFFER_SIZE);
            if (!stream) {
                ALOGE("Failed to create TcpStream for host connection!!!\n");
                delete con;
                return NULL;
            }
#if 1
            
            if (stream->connect(buf, (unsigned short)22500+atoi(portbuf)) < 0) {
#else
            if (stream->connect("10.254.18.83", 22469) < 0) {
#endif
                ALOGE("Failed to connect to host (TcpStream)!!!\n");
                delete stream;
                delete con;
                return NULL;
            }
            con->m_stream = stream;
#else
    // use unix stream
            UnixStream *stream = new UnixStream(STREAM_BUFFER_SIZE);
            if (!stream) {
                ALOGE("Failed to create UnixStream for host connection!!!\n");
                delete con;
                return NULL;
            }

#if 0
//            if (stream->connect("/data/local/tmp/lxc/ttt/rfopengles", STREAM_PORT_NUM) < 0) {
            if (stream->connect("/data/local/tmp/lxc/ttt/rfopengles", STREAM_PORT_NUM) < 0) {
#else
        
        char sin[512]={0};
        snprintf(sin, 512, "/dev/socket/socket/rfopengles%d", 22500+atoi(portbuf));
        ALOGE(sin);
            if (stream->connect(sin, STREAM_PORT_NUM) < 0) {
#endif
                ALOGE("Failed to connect to host (UnixStream)!!!\n");
                delete stream;
                delete con;
                return NULL;
            }
            con->m_stream = stream;

#endif
        }

        // send zero 'clientFlags' to the host.
        unsigned int *pClientFlags =
                (unsigned int *)con->m_stream->allocBuffer(sizeof(unsigned int));
        *pClientFlags = 0;
        con->m_stream->commitBuffer(sizeof(unsigned int));

        ALOGD("HostConnection::get() New Host Connection established %p, tid %d\n", con, gettid());
        tinfo->hostConn = con;
    }

    return tinfo->hostConn;
}

GLEncoder *HostConnection::glEncoder()
{
    if (!m_glEnc) {
        m_glEnc = new GLEncoder(m_stream);
        DBG("HostConnection::glEncoder new encoder %p, tid %d", m_glEnc, gettid());
        m_glEnc->setContextAccessor(s_getGLContext);
    }
    return m_glEnc;
}

GL2Encoder *HostConnection::gl2Encoder()
{
    if (!m_gl2Enc) {
        m_gl2Enc = new GL2Encoder(m_stream);
        DBG("HostConnection::gl2Encoder new encoder %p, tid %d", m_gl2Enc, gettid());
        m_gl2Enc->setContextAccessor(s_getGL2Context);
    }
    return m_gl2Enc;
}

renderControl_encoder_context_t *HostConnection::rcEncoder()
{
    if (!m_rcEnc) {
        m_rcEnc = new renderControl_encoder_context_t(m_stream);
    }
    return m_rcEnc;
}

gl_client_context_t *HostConnection::s_getGLContext()
{
    EGLThreadInfo *ti = getEGLThreadInfo();
    if (ti->hostConn) {
        return ti->hostConn->m_glEnc;
    }
    return NULL;
}

gl2_client_context_t *HostConnection::s_getGL2Context()
{
    EGLThreadInfo *ti = getEGLThreadInfo();
    if (ti->hostConn) {
        return ti->hostConn->m_gl2Enc;
    }
    return NULL;
}
