// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Window.h"
#include "Engine.h"
#include "graphics/Renderer.h"
#include "scene/SceneManager.h"

namespace ouzel
{
    Window::Window(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle):
        size(pSize),
        resizable(pResizable),
        fullscreen(pFullscreen),
        title(pTitle)
    {

    }

    Window::~Window()
    {

    }

    bool Window::init()
    {
        return true;
    }

    void Window::close()
    {

    }

    void Window::setSize(const Size2& newSize)
    {
        if (size != newSize)
        {
            size = newSize;
            sharedEngine->getRenderer()->setSize(size);

            Event event;
            event.type = Event::Type::WINDOW_SIZE_CHANGE;

            event.windowEvent.window = shared_from_this();
            event.windowEvent.size = size;
            event.windowEvent.title = title;
            event.windowEvent.fullscreen = fullscreen;

            sharedEngine->getEventDispatcher()->dispatchEvent(event);
        }
    }

    void Window::setFullscreen(bool newFullscreen)
    {
        if (fullscreen != newFullscreen)
        {
            fullscreen = newFullscreen;

            sharedEngine->getRenderer()->setFullscreen(fullscreen);

            Event event;
            event.type = Event::Type::WINDOW_FULLSCREEN_CHANGE;

            event.windowEvent.window = shared_from_this();
            event.windowEvent.size = size;
            event.windowEvent.title = title;
            event.windowEvent.fullscreen = fullscreen;

            sharedEngine->getEventDispatcher()->dispatchEvent(event);
        }
    }

    void Window::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            title = newTitle;

            Event event;
            event.type = Event::Type::WINDOW_TITLE_CHANGE;

            event.windowEvent.window = shared_from_this();
            event.windowEvent.size = size;
            event.windowEvent.title = title;
            event.windowEvent.fullscreen = fullscreen;

            sharedEngine->getEventDispatcher()->dispatchEvent(event);
        }
    }
}
