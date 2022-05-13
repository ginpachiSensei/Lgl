#pragma once
// GLAD
#include <glad/glad.h>
namespace Mirage
{
    class VertexBuffer
    {
    private:
        unsigned int m_RendererID;

    public:
        VertexBuffer(const void *data, unsigned int size);
        ~VertexBuffer();

        void Bind() const;
        void Unbind() const;
    };
};