// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Renderer.h"
#include "Engine.h"
#include "Texture.h"
#include "Node.h"
#include "Utils.h"
#include "Shader.h"
#include "Camera.h"
#include "EventHander.h"
#include "SceneManager.h"
#include "MeshBuffer.h"
#include "EventDispatcher.h"
#include "RenderTarget.h"
#include "BlendState.h"

namespace ouzel
{
    namespace graphics
    {
        Renderer::Renderer(Driver driver):
            _driver(driver)
        {

        }

        Renderer::~Renderer()
        {

        }

        bool Renderer::init(const Size2& size, bool fullscreen)
        {
            _size = size;
            _fullscreen = fullscreen;

            return true;
        }

        void Renderer::clear()
        {
            _drawCallCount = 0;
        }

        void Renderer::present()
        {

        }

        void Renderer::flush()
        {
        }

        std::vector<Size2> Renderer::getSupportedResolutions() const
        {
            return std::vector<Size2>();
        }

        void Renderer::setSize(const Size2& size)
        {
            _size = size;
        }

        void Renderer::setFullscreen(bool fullscreen)
        {
            _fullscreen = fullscreen;
        }

        BlendState* Renderer::createBlendState(bool enableBlending,
                                                 BlendState::BlendFactor colorBlendSource, BlendState::BlendFactor colorBlendDest,
                                                 BlendState::BlendOperation colorOperation,
                                                 BlendState::BlendFactor alphaBlendSource, BlendState::BlendFactor alphaBlendDest,
                                                 BlendState::BlendOperation alphaOperation)
        {
            BlendState* blendState(new BlendState());
            if (!blendState->init(enableBlending,
                                  colorBlendSource, colorBlendDest,
                                  colorOperation,
                                  alphaBlendSource, alphaBlendDest,
                                  alphaOperation))
            {
                blendState->release();
                blendState = nullptr;
            }

            return blendState;
        }

        bool Renderer::activateBlendState(BlendState* blendState)
        {
            _activeBlendState = blendState;

            return true;
        }

        Texture* Renderer::createTexture(const Size2& size, bool dynamic, bool mipmaps)
        {
            Texture* texture(new Texture());
            if (!texture->init(size, dynamic, mipmaps))
            {
                texture->release();
                texture = nullptr;
            }

            return texture;
        }

        bool Renderer::activateTexture(Texture* texture, uint32_t layer)
        {
            _activeTextures[layer] = texture;

            return true;
        }

        Texture* Renderer::loadTextureFromFile(const std::string& filename, bool dynamic, bool mipmaps)
        {
            Texture* texture(new Texture());

            if (!texture->initFromFile(filename, dynamic, mipmaps))
            {
                texture->release();
                texture = nullptr;
            }

            return texture;
        }

        Texture* Renderer::loadTextureFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps)
        {
            Texture* texture(new Texture());

            if (!texture->initFromData(data, size, dynamic, mipmaps))
            {
                texture->release();
                texture = nullptr;
            }

            return texture;
        }

        RenderTarget* Renderer::createRenderTarget(const Size2& size, bool depthBuffer)
        {
            RenderTarget* renderTarget(new RenderTarget());

            if (!renderTarget->init(size, depthBuffer))
            {
                renderTarget->release();
                renderTarget = nullptr;
            }

            return renderTarget;
        }

        bool Renderer::activateRenderTarget(RenderTarget* renderTarget)
        {
            _activeRenderTarget = renderTarget;

            return true;
        }

        Shader* Renderer::loadShaderFromFiles(const std::string& pixelShader,
                                                const std::string& vertexShader,
                                                uint32_t vertexAttributes,
                                                const std::string& pixelShaderFunction,
                                                const std::string& vertexShaderFunction)
        {
            Shader* shader(new Shader());

            if (!shader->initFromFiles(pixelShader, vertexShader, vertexAttributes, pixelShaderFunction, vertexShaderFunction))
            {
                shader->release();
                shader = nullptr;
            }

            return shader;
        }

        Shader* Renderer::loadShaderFromBuffers(const uint8_t* pixelShader,
                                                  uint32_t pixelShaderSize,
                                                  const uint8_t* vertexShader,
                                                  uint32_t vertexShaderSize,
                                                  uint32_t vertexAttributes,
                                                  const std::string& pixelShaderFunction,
                                                  const std::string& vertexShaderFunction)
        {
            Shader* shader(new Shader());

            if (!shader->initFromBuffers(pixelShader, pixelShaderSize, vertexShader, vertexShaderSize, vertexAttributes, pixelShaderFunction, vertexShaderFunction))
            {
                shader->release();
                shader = nullptr;
            }

            return shader;
        }

        bool Renderer::activateShader(Shader* shader)
        {
            _activeShader = shader;

            return true;
        }

        MeshBuffer* Renderer::createMeshBuffer()
        {
            MeshBuffer* meshBuffer(new MeshBuffer());

            if (!meshBuffer->init())
            {
                meshBuffer->release();
                meshBuffer = nullptr;
            }

            return meshBuffer;
        }

        MeshBuffer* Renderer::createMeshBufferFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer)
        {
            MeshBuffer* meshBuffer(new MeshBuffer());

            if (!meshBuffer->initFromData(indices, indexSize, indexCount, dynamicIndexBuffer, vertices, vertexAttributes, vertexCount, dynamicVertexBuffer))
            {
                meshBuffer->release();
                meshBuffer = nullptr;
            }

            return meshBuffer;
        }

        bool Renderer::drawMeshBuffer(MeshBuffer* meshBuffer, uint32_t indexCount, DrawMode drawMode, uint32_t startIndex)
        {
            OUZEL_UNUSED(drawMode);
            OUZEL_UNUSED(startIndex);

            if (_activeShader)
            {
                if (meshBuffer->getVertexAttributes() != _activeShader->getVertexAttributes())
                {
                    return false;
                }

                if (indexCount > meshBuffer->getIndexCount())
                {
                    return false;
                }
            }
            else
            {
                return false;
            }

            _drawCallCount++;

            return true;
        }

        Vector2 Renderer::viewToScreenLocation(const Vector2& position)
        {
            float x = 2.0f * position.x / _size.width - 1.0f;
            float y = 2.0f * (_size.height - position.y) / _size.height - 1.0f;

            return Vector2(x, y);
        }

        Vector2 Renderer::screenToViewLocation(const Vector2& position)
        {
            float x = (position.x + 1.0f) / 2.0f * _size.width;
            float y = _size.height - (position.y + 1.0f) / 2.0f * _size.height;

            return Vector2(x, y);
        }

        bool Renderer::checkVisibility(const Matrix4& transform, const AABB2& boundingBox, scene::Camera* camera)
        {
            Rectangle visibleRect(0.0f, 0.0f, _size.width, _size.height);

            // transform center point to screen space
            Vector2 diff = boundingBox.max - boundingBox.min;

            Vector3 v3p(boundingBox.min.x + diff.x / 2.0f, boundingBox.min.y + diff.y / 2.0f, 0.0f);
            diff *= camera->getZoom();
            diff.x *= camera->getContentScale().x;
            diff.y *= camera->getContentScale().y;

            transform.transformPoint(v3p);

            Vector2 v2p = camera->projectPoint(v3p);

            Size2 halfSize(diff.x / 2.0f, diff.y / 2.0f);

            // convert content size to world coordinates
            Size2 halfWorldSize;

            halfWorldSize.width = std::max(fabsf(halfSize.width * transform.m[0] + halfSize.height * transform.m[4]), fabsf(halfSize.width * transform.m[0] - halfSize.height * transform.m[4]));
            halfWorldSize.height = std::max(fabsf(halfSize.width * transform.m[1] + halfSize.height * transform.m[5]), fabsf(halfSize.width * transform.m[1] - halfSize.height * transform.m[5]));

            // enlarge visible rect half size in screen coord
            visibleRect.x -= halfWorldSize.width;
            visibleRect.y -= halfWorldSize.height;
            visibleRect.width += halfWorldSize.width * 2.0f;
            visibleRect.height += halfWorldSize.height * 2.0f;

            return visibleRect.containsPoint(v2p);
        }

        bool Renderer::saveScreenshot(const std::string& filename)
        {
            OUZEL_UNUSED(filename);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
