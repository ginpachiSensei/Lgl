#pragma once
// GLAD
#include <glad/glad.h>
namespace Mirage
{
    class IndexBuffer
    {
    private:
        unsigned int m_RendererID;
        unsigned int m_Count;

    public:
        IndexBuffer(const unsigned int *data, unsigned int count);
        ~IndexBuffer();

        void Bind();
        void Unbind();

        inline unsigned int GetCount() const { return m_Count; }
    };
};