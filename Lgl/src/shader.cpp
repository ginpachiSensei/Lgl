// Local Headers
#include "Shader.h"

// Standard Headers
#include <cassert>
#include <fstream>
#include <memory>
#include <iostream>

// Define Namespace
namespace Mirage
{
    Shader &Shader::activate()
    {
        glUseProgram(mProgram);
        return *this;
    }

    void Shader::bind(unsigned int location, float value) { glUniform1f(location, value); }
    void Shader::bind(unsigned int location, glm::mat4 const &matrix)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    Shader &Shader::attach(std::string const &filename)
    {
        // Load GLSL Shader Source from File
        std::string path = PROJECT_SOURCE_DIR "/Lgl/Shaders/";
        std::ifstream fd(path + filename);
        auto src = std::string(std::istreambuf_iterator<char>(fd),
                               (std::istreambuf_iterator<char>()));

        // Create a Shader Object
        const char *source = src.c_str();
        auto shader = create(filename);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &mStatus);

        // Display the Build Log on Error
        if (mStatus == false)
        {

            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &mLength);
            std::unique_ptr<char[]> buffer(new char[mLength]);
            glGetShaderInfoLog(shader, mLength, nullptr, buffer.get());
            std::cout << "attach error on " << filename << std::endl;
            fprintf(stderr, "%s\n%s", filename.c_str(), buffer.get());
        }

        // Attach the Shader and Free Allocated Memory
        glAttachShader(mProgram, shader);
        glDeleteShader(shader);
        return *this;
    }

    GLuint Shader::create(std::string const &filename)
    {
        auto index = filename.rfind(".");
        auto ext = filename.substr(index + 1);
        if (ext == "comp")
            return glCreateShader(GL_COMPUTE_SHADER);
        else if (ext == "frag")
            return glCreateShader(GL_FRAGMENT_SHADER);
        else if (ext == "geom")
            return glCreateShader(GL_GEOMETRY_SHADER);
        else if (ext == "vert")
            return glCreateShader(GL_VERTEX_SHADER);
        else
            return false;
    }

    Shader &Shader::link()
    {
        glLinkProgram(mProgram);
        glGetProgramiv(mProgram, GL_LINK_STATUS, &mStatus);
        if (mStatus == false)
        {
            glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &mLength);
            std::unique_ptr<char[]> buffer(new char[mLength]);
            glGetProgramInfoLog(mProgram, mLength, nullptr, buffer.get());
            std::cout << "link error on " << std::endl;
            fprintf(stderr, "%s", buffer.get());
        }
        assert(mStatus == true);
        return *this;
    }
};
