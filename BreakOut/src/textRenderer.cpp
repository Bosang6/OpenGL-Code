#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "textRenderer.h"
#include "resource_manager.h"

TextRenderer::TextRenderer(unsigned int width, unsigned int height){
    // 着色器加载，以及projection矩阵传递
    this->TextShader = ResourceManager::LoadShader("shader/text.vs", "shader/text.fs", nullptr, "text");
    this->TextShader.SetMatrix4("projection", glm::ortho(0.0f, (float)width, (float)height, 0.0f), true);
    this->TextShader.SetInteger("text", 0);

    // 配置VAO 和 VBO
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::Load(std::string font, unsigned int fontSize){
    // 清除字符表
    this->Characters.clear();
    // 初始化FreeType库
    FT_Library ft;
    if(FT_Init_FreeType(&ft)){
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }
    // 读取字体
    FT_Face face;
    if(FT_New_Face(ft, font.c_str(), 0, &face)){
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }
    // 设置字体大小         0 根据 fontSize计算
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    // 解除4byte对齐
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // 读取ascii的前128个字符
    for(GLubyte c = 0; c < 128; c++){
        // 读取字符的glyph
        if(FT_Load_Char(face, c, FT_LOAD_RENDER)){
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // 生成字符的texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 将字符所需信息保存在Character结构体内
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (unsigned int)face->glyph->advance.x
        };
        // ???
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // 清除FreeType库
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextRenderer::RenderText(std::string text, float x, float y, float scale, glm::vec3 color){
    //启动shader，并传递颜色信息
    this->TextShader.Use();
    this->TextShader.SetVector3f("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);

    // 遍历所有字符
    std::string::const_iterator c;
    for(c = text.begin(); c != text.end(); c++){
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        // y为基准线，H字符占领整个bearing.y,也就是说H的Bearing.y = 0 然后根据不同字符的bearing.y进行上移对齐基准线
        float ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // 为每个字符更新VBO
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f }
        };
        // 绑定字符对应的texture
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // 更新VBO buffer对应数据
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // 绘制
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // 字符光标右移
        x += (ch.Advance >> 6) * scale; // 26.6   26(int) 6(float)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}