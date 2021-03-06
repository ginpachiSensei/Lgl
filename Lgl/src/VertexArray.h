#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
// GLAD
#include <glad/glad.h>

namespace Mirage
{
    class VertexArray
    {
    private:
        unsigned int m_RendererID;

    public:
        VertexArray();
        ~VertexArray();

        void AddBuffer(Mirage::VertexBuffer &vb, const Mirage::VertexBufferLayout &layout);

        void Bind() const;
        void Unbind() const;
    };
};