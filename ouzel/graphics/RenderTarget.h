// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "utils/Types.h"
#include "utils/Noncopyable.h"
#include "graphics/Resource.h"
#include "graphics/Color.h"
#include "math/Size2.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;
        class Texture;

        class RenderTarget: public Resource, public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~RenderTarget();
            virtual void free() override;

            virtual bool init(const Size2& newSize, bool useDepthBuffer);

            virtual void setClearColor(Color color) { clearColor = color; }
            virtual Color getClearColor() const { return clearColor; }

            TexturePtr getTexture() const { return texture; }

            bool isReady() const { return ready; }

        protected:
            RenderTarget();

            Size2 size;
            bool depthBuffer = false;

            Color clearColor;
            TexturePtr texture;

            bool ready = false;
        };
    } // namespace graphics
} // namespace ouzel
