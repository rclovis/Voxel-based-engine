#include "TextRenderer.hpp"

TextRenderer::TextRenderer()
{
    _window = nullptr;
    _shaderProgram = 0;
    _VAO = 0;
    _VBO = 0;
    _text = {};
}

TextRenderer::~TextRenderer()
{
    glDeleteProgram(_shaderProgram);
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    std::cout << LOG_TEXT("TextRenderer destructor");
}

void TextRenderer::init(GLFWwindow* window)
{
    std::cout << LOG_TEXT("Init TextRenderer");
    _window = window;
    _shaderProgram = Shader::LoadShaders("Mist/shader/textVertexShader.glsl", "Mist/shader/textFragmentShader.glsl");

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }
    FT_Face face;
    if (FT_New_Face(ft, "assets/Minecraft.ttf", 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::cout << LOG_TEXT("Loading font");
    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
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
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_color);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, _color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 3, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void TextRenderer::draw()
{
    for (std::vector<textInfo_t>::iterator it = _text.begin(); it != _text.end(); ++it) {
        renderText(*it);
    }
}

void TextRenderer::renderText(textInfo_t info)
{
    std::string text = info.text;
    GLfloat x = info.pos.x;
    GLfloat y = info.pos.y;
    GLfloat scale = info.scale;
    glm::vec3 color = info.color;
    glUseProgram(_shaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(_VAO);

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];
        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;
        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
        float vertices[6][4] = {
            {xpos,     ypos + h,   0.0f, 0.0f},
            {xpos,     ypos,       0.0f, 1.0f},
            {xpos + w, ypos,       1.0f, 1.0f},
            {xpos,     ypos + h,   0.0f, 0.0f},
            {xpos + w, ypos,       1.0f, 1.0f},
            {xpos + w, ypos + h,   1.0f, 0.0f}
        };
        float colors[6][3] = {
            {color.x, color.y, color.z},
            {color.x, color.y, color.z},
            {color.x, color.y, color.z},
            {color.x, color.y, color.z},
            {color.x, color.y, color.z},
            {color.x, color.y, color.z}
        };
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, _color);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colors), colors);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch.advance >> 6) * scale;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}