// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetOGL.h"

namespace ouzel
{
    RenderTargetOGL::RenderTargetOGL()
    {
        
    }
    
    RenderTargetOGL::~RenderTargetOGL()
    {
        clean();
    }
    
    void RenderTargetOGL::clean()
    {
        if (_textureId) glDeleteTextures(1, &_textureId);
        if (_depthBufferId) glDeleteRenderbuffers(1, &_depthBufferId);
        if (_framebufferId) glDeleteFramebuffers(1, &_framebufferId);
    }
    
    bool RenderTargetOGL::init(Size2 const& size, bool depthBuffer)
    {
        if (RenderTarget::init(size, depthBuffer))
        {
            return false;
        }
        
        clean();
        
        glGenFramebuffers(1, &_framebufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);
        
        glBindTexture(GL_TEXTURE_2D, _textureId);
        
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB,
                     static_cast<GLsizei>(_size.width),
                     static_cast<GLsizei>(_size.height),
                     0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
        if (depthBuffer)
        {
            glGenRenderbuffers(1, &_depthBufferId);
            glBindRenderbuffer(GL_RENDERBUFFER, _depthBufferId);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                  static_cast<GLsizei>(_size.width),
                                  static_cast<GLsizei>(_size.height));
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBufferId);
        }
        
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textureId, 0);
        GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, drawBuffers);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            return false;
        }
    
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        return true;
    }
}
