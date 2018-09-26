#ifndef TEXTGL_H
#define TEXTGL_H
//OpenGL Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//User Headers
#include <Shader.h>;

//Text Rendering Headers
#include <ft2build.h>
#include FT_FREETYPE_H

//Standard Headers
#include <iostream>
#include <string>
#include <map>

using namespace std;

struct Character {
	GLuint TextureID; //Glyph texture ID handler
	glm::ivec2 Size; //Size of the glyph
	glm::ivec2 Bearing; //Bearing X: left of baseline, Bearing Y: Top of Baseline
	GLuint Advance; //Spacing between each glyph
};

map<GLchar, Character> Characters;

class textGL {
public:
	textGL(string const path,unsigned int size) {

		//Initialize FreeType
		FT_Library ft;
		if (FT_Init_FreeType(&ft)) {
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		}
		FT_Face face;
		//Create a New Face
		if (FT_New_Face(ft, path.c_str(), 0, &face)){
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		}


		FT_Set_Pixel_Sizes(face, 0, size);

		loadGlyphs(face);

		//Clear Data
		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		Shader textShader("VertexShaders\\textVertexShader.vs", "FragmentShaders\\textFragmentShader.fs");
		programID = textShader.ID;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void RenderText(string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, unsigned int width, unsigned int height) {
		glUseProgram(programID);
		glm::mat4 projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
		glUniformMatrix4fv(glGetUniformLocation(programID, "projection"), 1, GL_FALSE, &projection[0][0]);
		glUniform3f(glGetUniformLocation(programID, "textColor"), color.x, color.y, color.z);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);

		string::const_iterator itc;
		for (itc = text.begin(); itc != text.end(); itc++) {
			Character ch = Characters[*itc];
			GLfloat xpos = x + ch.Bearing.x * scale;
			GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
			GLfloat w = ch.Size.x * scale;
			GLfloat h = ch.Size.y * scale;

			GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
			};

			// Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

private:
	GLuint VAO;
	GLuint VBO;
	unsigned int programID;
	void loadGlyphs(FT_Face face) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Disable byte-alignment restriction

		for (GLubyte c = 0; c < 128; c++) {
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}

			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};

			Characters.insert(pair<GLchar, Character>(c, character));
		}
	}
};


#endif