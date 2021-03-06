// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <functional>
#include "Button.h"
#include "core/Engine.h"
#include "scene/Sprite.h"
#include "events/EventHandler.h"
#include "events/EventDispatcher.h"
#include "scene/Layer.h"
#include "scene/Camera.h"
#include "Label.h"
#include "utils/Utils.h"
#include "scene/TextDrawable.h"

namespace ouzel
{
    namespace gui
    {
        Button::Button():
            eventHandler(EventHandler::PRIORITY_MAX + 1)
        {
        }

        Button::Button(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage,
                       const std::string& label, const graphics::Color& labelColor, const std::string& font):
            eventHandler(EventHandler::PRIORITY_MAX + 1)
        {
            init(normalImage, selectedImage, pressedImage, disabledImage, label, labelColor, font);
        }

        Button::~Button()
        {
            sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
        }

        bool Button::init(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage,
                          const std::string& label, const graphics::Color& labelColor, const std::string& font)
        {

            eventHandler.gamepadHandler = std::bind(&Button::handleGamepad, this, std::placeholders::_1, std::placeholders::_2);
            eventHandler.uiHandler = std::bind(&Button::handleUI, this, std::placeholders::_1, std::placeholders::_2);

            sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

            if (!normalImage.empty())
            {
                normalSprite = std::make_shared<scene::Sprite>();
                if (normalSprite->initFromFile(normalImage, false))
                {
                    addDrawable(normalSprite);
                }
            }

            if (!selectedImage.empty())
            {
                selectedSprite = std::make_shared<scene::Sprite>();
                if (selectedSprite->initFromFile(selectedImage, false))
                {
                    addDrawable(selectedSprite);
                }
            }

            if (!pressedImage.empty())
            {
                pressedSprite = std::make_shared<scene::Sprite>();
                if (pressedSprite->initFromFile(pressedImage, false))
                {
                    addDrawable(pressedSprite);
                }
            }

            if (!disabledImage.empty())
            {
                disabledSprite = std::make_shared<scene::Sprite>();
                if (disabledSprite->initFromFile(disabledImage, false))
                {
                    addDrawable(disabledSprite);
                }
            }

            if (!label.empty())
            {
                labelDrawable = std::make_shared<scene::TextDrawable>(font, label);
                if (labelDrawable)
                {
                    labelDrawable->setColor(labelColor);
                    addDrawable(labelDrawable);
                }
            }

            updateSprite();

            return true;
        }

        void Button::setEnabled(bool enabled)
        {
            Widget::setEnabled(enabled);

            selected = false;
            pointerOver = false;
            pressed = false;
            receiveInput = enabled;

            updateSprite();
        }

        bool Button::handleGamepad(Event::Type, const GamepadEvent&)
        {
            return true;
        }

        bool Button::handleUI(Event::Type type, const UIEvent& event)
        {
            if (!enabled) return true;

            if (event.node.get() == this)
            {
                if (type == Event::Type::UI_ENTER_NODE)
                {
                    pointerOver = true;
                    updateSprite();
                }
                else if (type == Event::Type::UI_LEAVE_NODE)
                {
                    pointerOver = false;
                    updateSprite();
                }
                else if (type == Event::Type::UI_PRESS_NODE)
                {
                    pressed = true;
                    updateSprite();
                }
                else if (type == Event::Type::UI_RELEASE_NODE)
                {
                    if (pressed)
                    {
                        pressed = false;
                        updateSprite();
                    }
                }
                else if (type == Event::Type::UI_CLICK_NODE)
                {
                    if (pressed)
                    {
                        pressed = false;
                        updateSprite();
                    }
                }
            }

            return true;
        }

        void Button::updateSprite()
        {
            if (normalSprite) normalSprite->setVisible(false);
            if (selectedSprite) selectedSprite->setVisible(false);
            if (pressedSprite) pressedSprite->setVisible(false);
            if (disabledSprite) disabledSprite->setVisible(false);

            if (enabled)
            {
                if (pressed && pointerOver && pressedSprite)
                {
                    pressedSprite->setVisible(true);
                }
                else if (selected && selectedSprite)
                {
                    selectedSprite->setVisible(true);
                }
                else if (normalSprite)
                {
                    normalSprite->setVisible(true);
                }
            }
            else
            {
                if (disabledSprite)
                {
                    disabledSprite->setVisible(true);
                }
                else if (normalSprite)
                {
                    normalSprite->setVisible(true);
                }
            }
        }
    } // namespace gui
} // namespace ouzel
