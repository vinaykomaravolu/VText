#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <glad\glad.h>
#include <list>
#include <iterator>
#include <string>
#ifndef  STB_IMAGE_H_LOCK
#define STB_IMAGE_H_LOCK
#include <stb_image.h>
#endif // ! STB_IMAGE_H_LOCK
#define TEXTURE_OBJECT_MAX_BINDINGS 32
#define TEXTURE_FIRST GL_TEXTURE0
#define TEXTURE_DEFAULT_FSNAME "texture_diffuse"

using namespace std;

class Texture2D {
private:
	int width;
	int height;
	int nrChannels;
	unsigned int texture;
	const char *textureName;
	string fragShaderTextureName;
	int ID;
public:
	
	Texture2D(const char *textureFileName) {
		stbi_set_flip_vertically_on_load(true);
		textureName = textureFileName;
		unsigned char *data = stbi_load(textureName, &width, &height, &nrChannels, 0);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		//set filtering options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Bind(unsigned int program,string name,GLint location) {
		fragShaderTextureName = name;
		glUniform1i(glGetUniformLocation(program, name.c_str()), location);
	}

	void Bind(unsigned int program, GLint location) {
		glUniform1i(glGetUniformLocation(program, fragShaderTextureName.c_str()), location);
	}

	void setFSTName(string name) {
		fragShaderTextureName = name;
	}

	string getFSTName() {
		return fragShaderTextureName;
	}

	void use() {
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	void stop() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	int getWidth() {
		return width;
	}

	int getHeight() {
		return height;
	}

	int getNRChannels() {
		return nrChannels;
	}

	unsigned int getTextureID() {
		return texture;
	}

	void destroyObject() {
		delete this;
	}

	const char * getName() {
		return textureName;
	}

	Texture2D getThis() {
		return * this;
	}

	bool operator ==(const Texture2D& testTexture) {
		return testTexture.texture == this->getTextureID();
	}

	bool operator !=(const Texture2D& testTexture) {

		return testTexture.texture != this->getTextureID();
	}
};

class TextureArray {
private:
	list <Texture2D> textureList;
	list <Texture2D> ::iterator TLIterator;
public:
	TextureArray() {
	}

	void addTextureByObject(Texture2D newTexture) {
		if (textureList.size() <= 32) {
			textureList.push_back(newTexture);
		}
		else {
			cout << "ERROR::TEXTURE:: Texture limit reached" << endl;
		}
	}

	void addTextureByName(const char * textureName) {
		if (textureList.size() <= 32) {
			textureList.push_back(*(new Texture2D(textureName)));
		}
		else {
			cout << "ERROR::TEXTURE:: Texture limit reached" << endl;
		}
	}

	void removeTextureByObject(Texture2D newTexture) {
		textureList.remove(newTexture);
	}

	void removeTextureByName(const char * textureName) {
		for (TLIterator = textureList.begin(); TLIterator != textureList.end(); ++TLIterator) {
			if (TLIterator->getName() == textureName) {
				textureList.remove(TLIterator->getThis());
				break;
			}
		}
	}

	void bindArray(unsigned int program) {
		int textureIndex = 0;
		for (TLIterator = textureList.begin(); TLIterator != textureList.end(); ++TLIterator) {
			TLIterator->Bind(program,TEXTURE_DEFAULT_FSNAME + to_string(textureIndex + 1) ,textureIndex);
			textureIndex++;
		}
	}

	void useArray() {
		int textureIndex = 0;
		for (TLIterator = textureList.begin(); TLIterator != textureList.end(); ++TLIterator) {
			glActiveTexture(TEXTURE_FIRST + textureIndex);
			glBindTexture(GL_TEXTURE_2D, TLIterator->getTextureID());
			textureIndex++;
		}
	}

	void stopArray() {
		int textureIndex = 0;
		for (TLIterator = textureList.begin(); TLIterator != textureList.end(); ++TLIterator) {
			glActiveTexture(TEXTURE_FIRST + textureIndex);
			glBindTexture(0, TLIterator->getTextureID());
			textureIndex++;
		}
	}
};


#endif