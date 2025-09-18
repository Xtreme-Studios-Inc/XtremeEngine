#pragma once

#include <codeframe>

#include <stb_image.h>
#include "glad/glad.h"

class OpenGL_Texture {
  public:
    OpenGL_Texture();
    OpenGL_Texture(const fs::path &fileLocation);
    ~OpenGL_Texture();

    bool LoadTexture();
    bool LoadTextureA();
    void UseTexture();
    void ClearTexture();

  private:
    GLuint textureID;
    int width, height, bitDepth;

    fs::path fileLocation;
};
