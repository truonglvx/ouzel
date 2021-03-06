// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <GL/gl.h>
#include <GL/glext.h>
#include "WindowLinux.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"
#include "opengl/RendererOGL.h"
#include "utils/Utils.h"

namespace ouzel
{
    WindowLinux::WindowLinux(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle):
        Window(pSize, pResizable, pFullscreen, pTitle)
    {

    }

    WindowLinux::~WindowLinux()
    {
        if (display)
        {
            if (context)
            {
                glXDestroyContext(display, context);
            }

            if (window)
            {
                XDestroyWindow(display, window);
            }

            XCloseDisplay(display);
        }
    }

    bool WindowLinux::init()
    {
        std::shared_ptr<graphics::RendererOGL> rendererOGL = std::static_pointer_cast<graphics::RendererOGL>(sharedEngine->getRenderer());

        // open a connection to the X server
        display = XOpenDisplay(nullptr);

        if (!display)
        {
            ouzel::log("Failed to open display");
            return false;
        }

        int screen = DefaultScreen(display);

        XVisualInfo* vi = nullptr;

        // make sure OpenGL's GLX extension supported
        int dummy;
        if (!glXQueryExtension(display, &dummy, &dummy))
        {
            ouzel::log("X server has no OpenGL GLX extension");
            return false;
        }

        int fbcount = 0;

        static const int attributes[] = {
            GLX_RENDER_TYPE, GLX_RGBA_BIT,
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
            GLX_DOUBLEBUFFER, GL_TRUE,
            GLX_RED_SIZE, 1,
            GLX_GREEN_SIZE, 1,
            GLX_BLUE_SIZE, 1,
            None
        };

        GLXFBConfig* framebufferConfig = glXChooseFBConfig(display, screen, attributes, &fbcount);
        if (!framebufferConfig)
        {
            ouzel::log("Failed to get frame buffer");
        }
        else
        {
            // create an OpenGL rendering context
            static const int contextAttribs[] = {
                GLX_CONTEXT_PROFILE_MASK_ARB,
                GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
                GLX_CONTEXT_MAJOR_VERSION_ARB,
                3,
                GLX_CONTEXT_MINOR_VERSION_ARB,
                2,
                None
            };

            PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB"));

            if (glXCreateContextAttribsARB)
            {
                context = glXCreateContextAttribsARB(display, framebufferConfig[0], NULL, GL_TRUE, contextAttribs);

                if (context)
                {
                    rendererOGL->setAPIVersion(3);
                    log("Using OpenGL 3.2");

                    vi = glXGetVisualFromFBConfig(display, framebufferConfig[0]);

                    if (!vi)
                    {
                        ouzel::log("Failed to get OpenGL visual");
                        context = nullptr;
                    }
                }
                else
                {
                    log("Failed to crete OpenGL 3.2 rendering context");
                }
            }
            else
            {
                log("Could not find glXCreateContextAttribsARB");
            }
        }

        if (!context)
        {
            // find an OpenGL-capable RGB visual with depth buffer
            static int doubleBuffer[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};

            vi = glXChooseVisual(display, screen, doubleBuffer);
            if (!vi)
            {
                ouzel::log("Failed to choose OpenGL visual");
                return false;
            }
            if (vi->c_class != TrueColor)
            {
                ouzel::log("TrueColor visual required for this program");
                return false;
            }

            context = glXCreateContext(display, vi, None, GL_TRUE);

            if (context)
            {
                rendererOGL->setAPIVersion(2);
                log("Using OpenGL 2");
            }
            else
            {
                log("Failed to crete OpenGL 2 rendering context");
                return false;
            }
        }

        // create an X colormap since probably not using default visual
        Colormap cmap = XCreateColormap(display, RootWindow(display, vi->screen), vi->visual, AllocNone);
        XSetWindowAttributes swa;
        swa.colormap = cmap;
        swa.border_pixel = 0;
        swa.event_mask = FocusChangeMask | KeyPressMask | KeyRelease | ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;
        window = XCreateWindow(display, RootWindow(display, vi->screen), 0, 0,
                               static_cast<unsigned int>(size.width), static_cast<unsigned int>(size.height), 0,
                               vi->depth, InputOutput, vi->visual,
                               CWBorderPixel | CWColormap | CWEventMask, &swa);
        XSetStandardProperties(display, window, title.c_str(), title.c_str(), None, sharedApplication->getArgv(), sharedApplication->getArgc(), nullptr);

        // bind the rendering context to the window
        glXMakeCurrent(display, window, context);

        // request the X window to be displayed on the screen
        XMapWindow(display, window);
        
        deleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, window, &deleteMessage, 1);

        PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXSwapIntervalEXT"));

        if (glXSwapIntervalEXT)
        {
            glXSwapIntervalEXT(display, window, sharedEngine->getSettings().verticalSync ? 1 : 0);
        }

        return Window::init();
    }

    void WindowLinux::setSize(const Size2& newSize)
    {
        sharedApplication->execute([this, newSize] {
            XWindowChanges changes;
            changes.width = static_cast<int>(newSize.width);
            changes.height = static_cast<int>(newSize.height);
            XConfigureWindow(display, window, CWWidth | CWHeight, &changes);
        });
        
        Window::setSize(newSize);
    }

    void WindowLinux::setFullscreen(bool newFullscreen)
    {
        Window::setFullscreen(newFullscreen);
    }

    void WindowLinux::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            sharedApplication->execute([this, newTitle] {
                XStoreName(display, window, newTitle.c_str());
            });
        }

        Window::setTitle(newTitle);
    }
}
