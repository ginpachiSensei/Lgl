#pragma once

#include <vector>
// GLAD
#include <glad/glad.h>

template <typename T>
struct identity
{
    typedef T type;
};

struct VertexBufferLayoutElement
{
    unsigned int type;
    unsigned int count;
    unsigned char normalised;

    static unsigned int GetSizeOfType(unsigned int type)
    {
        switch (type)
        {
        case GL_FLOAT:
            return 4;
        case GL_UNSIGNED_INT:
            return 4;
        case GL_UNSIGNED_BYTE:
            return 1;
        }
        return 0;
    }
};
namespace Mirage
{
    class VertexBufferLayout
    {
    private:
        std::vector<VertexBufferLayoutElement> m_Elements;
        unsigned int m_stride;

        void push(unsigned int count, identity<float>)
        {
            m_Elements.push_back({GL_FLOAT, count, GL_FALSE});
            m_stride += VertexBufferLayoutElement::GetSizeOfType(GL_FLOAT) * count;
        }

        void push(unsigned int count, identity<unsigned int>)
        {
            m_Elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
            m_stride += VertexBufferLayoutElement::GetSizeOfType(GL_UNSIGNED_INT) * count;
        }

        void push(unsigned int count, identity<unsigned char>)
        {
            m_Elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
            m_stride += VertexBufferLayoutElement::GetSizeOfType(GL_UNSIGNED_BYTE) * count;
        }

    public:
        VertexBufferLayout()
            : m_stride(0) {}

        template <typename T>
        void push(unsigned int count)
        {
            push(count, identity<T>());
        }

        inline const std::vector<VertexBufferLayoutElement> GetElements() const { return m_Elements; }
        inline unsigned int GetStride() const { return m_stride; }
    };
};
