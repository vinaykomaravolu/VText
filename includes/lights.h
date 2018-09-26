#ifndef LIGHTS_H
#define LIGHTS_H

#include <iostream>
#include <glad\glad.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shader.h>
#include <string>

#define L_DEFAULT_POSITION glm::vec3(0.0f);
#define L_DEFAULT_AMBIENT glm::vec3(0.3f);
#define L_DEFAULT_DIFFUSE glm::vec3(0.6f);
#define L_DEFAULT_SPECULAR glm::vec3(1.0f);
#define L_DEFAULT_CONSTANT 1.0f;
#define L_DEFAULT_LINEAR 0.09f;
#define L_DEFAULT_QUADRATIC 0.032f;
#define L_DEFAULT_POSITION glm::vec3(0.0f);
#define L_DEFAULT_DIRECTION glm::vec3(0.0f,0.0f,-1.0f);
#define L_DEFAULT_CUTOFF glm::cos(glm::radians(12.5f))
#define L_DEFAULT_OUTERCUTOFF glm::cos(glm::radians(17.5f))
#define L_DEFAULT_ENABLE true;

using namespace std;

const string L_FRAG_SHADER_ARRAY_POINT = "lightPoint";
const string L_FRAG_SHADER_ARRAY_DIR = "lightDir";
const string L_FRAG_SHADER_ARRAY_SPOT = "lightSpot";

struct Light {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
	bool enable;
};

class LightSpot {
public:
	LightSpot() {
		light.direction = L_DEFAULT_DIRECTION;
		light.position = L_DEFAULT_POSITION;
		light.ambient = L_DEFAULT_AMBIENT;
		light.diffuse = L_DEFAULT_DIFFUSE;
		light.specular = L_DEFAULT_SPECULAR;
		light.constant = L_DEFAULT_CONSTANT;
		light.linear = L_DEFAULT_LINEAR;
		light.quadratic = L_DEFAULT_QUADRATIC;
		light.cutOff = L_DEFAULT_CUTOFF;
		light.outerCutOff = L_DEFAULT_OUTERCUTOFF;
		light.enable = L_DEFAULT_ENABLE;
		lightNumber = spotLightNumber;
		spotLightNumber++;
	}

	LightSpot(glm::vec3 position, glm::vec3 direction) {
		light.direction = direction;
		light.position = position;
		light.ambient = L_DEFAULT_AMBIENT;
		light.diffuse = L_DEFAULT_DIFFUSE;
		light.specular = L_DEFAULT_SPECULAR;
		light.constant = L_DEFAULT_CONSTANT;
		light.linear = L_DEFAULT_LINEAR;
		light.quadratic = L_DEFAULT_QUADRATIC;
		light.cutOff = L_DEFAULT_CUTOFF;
		light.outerCutOff = L_DEFAULT_OUTERCUTOFF;
		light.enable = L_DEFAULT_ENABLE;
		lightNumber = spotLightNumber;
		spotLightNumber++;
	}

	LightSpot(Light light) {
		this->light = light;
		lightNumber = spotLightNumber;
		spotLightNumber++;
	}

	string print() {
		return to_string(light.position.x) + " " + to_string(light.position.y) + " " + to_string(light.position.z) + "\n" + to_string(light.direction.x) + " " + to_string(light.direction.y) + " " + to_string(light.direction.z);
	}

	void use(Shader shader) {
		shader.setVec3(L_FRAG_SHADER_ARRAY_SPOT + '[' + to_string(lightNumber) + "].ambient", light.ambient);
		shader.setVec3(L_FRAG_SHADER_ARRAY_SPOT + '[' + to_string(lightNumber) + "].diffuse", light.diffuse);
		shader.setVec3(L_FRAG_SHADER_ARRAY_SPOT + '[' + to_string(lightNumber) + "].specular", light.specular);
		shader.setFloat(L_FRAG_SHADER_ARRAY_SPOT + '[' + to_string(lightNumber) + "].constant", light.constant);
		shader.setFloat(L_FRAG_SHADER_ARRAY_SPOT + '[' + to_string(lightNumber) + "].linear", light.linear);
		shader.setFloat(L_FRAG_SHADER_ARRAY_SPOT + '[' + to_string(lightNumber) + "].quadratic", light.quadratic);
		shader.setVec3(L_FRAG_SHADER_ARRAY_SPOT + '[' + to_string(lightNumber) + "].position", light.position);
		shader.setVec3(L_FRAG_SHADER_ARRAY_SPOT + '[' + to_string(lightNumber) + "].direction", light.direction);
		shader.setFloat(L_FRAG_SHADER_ARRAY_SPOT + '[' + to_string(lightNumber) + "].cutOff", light.cutOff);
		shader.setFloat(L_FRAG_SHADER_ARRAY_SPOT + '[' + to_string(lightNumber) + "].outerCutOff", light.outerCutOff);
		shader.setBool(L_FRAG_SHADER_ARRAY_SPOT + '[' + to_string(lightNumber) + "].enable", light.enable);
	}

	//set attenuation by range
	void setAttenuation(int range) {
		this->light.constant = 1.0;
		if (range <= 7) {
			this->light.linear = 0.7f;
			this->light.quadratic = 1.8f;
		}
		else if (8 <= range <= 13) {
			this->light.linear = 0.35f;
			this->light.quadratic = 0.44f;
		}
		else if (14 <= range <= 20) {
			this->light.linear = 0.22f;
			this->light.quadratic = 0.20f;
		}
		else if (21 <= range <= 32) {
			this->light.linear = 0.14f;
			this->light.quadratic = 0.07f;
		}
		else if (33 <= range <= 50) {
			this->light.linear = 0.09f;
			this->light.quadratic = 0.032f;
		}
		else if (51 <= range <= 65) {
			this->light.linear = 0.07f;
			this->light.quadratic = 0.017f;
		}
		else if (66 <= range <= 100) {
			this->light.linear = 0.045f;
			this->light.quadratic = 0.0075f;
		}
		else if (101 <= range <= 160) {
			this->light.linear = 0.027;
			this->light.quadratic = 0.0028f;
		}
		else if (161 <= range <= 200) {
			this->light.linear = 0.022f;
			this->light.quadratic = 0.0019f;
		}
		else if (201 <= range <= 325) {
			this->light.linear = 0.014f;
			this->light.quadratic = 0.0007f;
		}
		else if (326 <= range <= 600) {
			this->light.linear = 0.007f;
			this->light.quadratic = 0.0002f;
		}
		else if (601 <= range) {
			this->light.linear = 0.0014f;
			this->light.quadratic = 0.000007f;
		}
	}

	//set light properties with struct
	void setLight(Light initLight) {
		this->light = initLight;
	}

	//set position
	void setPosition(glm::vec3 position) {
		this->light.position = position;
	}

	//set direction
	void setDirection(glm::vec3 direction) {
		this->light.direction = direction;
	}

	//set cutoff
	void setCutOff(float cutOff) {
		this->light.cutOff = glm::cos(glm::radians(cutOff));
	}

	//set cutoff
	void setOuterCutOff(float outerCutOff) {
		this->light.outerCutOff = glm::cos(glm::radians(outerCutOff));
	}

	//set ambient
	void setAmbient(glm::vec3 ambient) {
		this->light.ambient = ambient;
	}

	//set diffuse
	void setDiffuse(glm::vec3 diffuse) {
		this->light.diffuse = diffuse;
	}

	//set specular
	void setSpecular(glm::vec3 specular) {
		this->light.specular = specular;
	}

	//set constant
	void setConstant(float constant) {
		this->light.constant = constant;
	}

	//set linaer
	void setLinear(float linear) {
		this->light.linear = linear;
	}

	//set quadratic
	void setQuadratic(float quadratic) {
		this->light.quadratic = quadratic;
	}

	//set enabler
	void setEnable(bool enable) {
		this->light.enable = enable;
	}

private:
	Light light;
	static unsigned int spotLightNumber;
	unsigned int lightNumber;
};

class LightDir {
public:
	//Default Constructor
	LightDir() {
		light.direction = L_DEFAULT_DIRECTION;
		light.ambient = L_DEFAULT_AMBIENT;
		light.diffuse = L_DEFAULT_DIFFUSE;
		light.specular = L_DEFAULT_SPECULAR;
		light.enable = L_DEFAULT_ENABLE;
		lightNumber = dirLightNumber;
		dirLightNumber++;
	}

	//Position Constructor
	LightDir(glm::vec3 direction) {
		light.direction = direction;
		light.ambient = L_DEFAULT_AMBIENT;
		light.diffuse = L_DEFAULT_DIFFUSE;
		light.specular = L_DEFAULT_SPECULAR;
		light.enable = L_DEFAULT_ENABLE;
		lightNumber = dirLightNumber;
		dirLightNumber++;
	}

	//Struct Light Constructor
	LightDir(Light initLight) {
		this->light = initLight;
		lightNumber = dirLightNumber;
		dirLightNumber++;
	}

	//use Light
	void use(Shader shader) {
		shader.setVec3(L_FRAG_SHADER_ARRAY_DIR + '[' + to_string(lightNumber) + "].ambient", light.ambient);
		shader.setVec3(L_FRAG_SHADER_ARRAY_DIR + '[' + to_string(lightNumber) + "].diffuse", light.diffuse);
		shader.setVec3(L_FRAG_SHADER_ARRAY_DIR + '[' + to_string(lightNumber) + "].specular", light.specular);
		shader.setVec3(L_FRAG_SHADER_ARRAY_DIR + '[' + to_string(lightNumber) + "].direction", light.direction);
		shader.setBool(L_FRAG_SHADER_ARRAY_DIR + '[' + to_string(lightNumber) + "].enable", light.enable);
	}

	//set light properties with struct
	void setLight(Light initLight) {
		this->light = initLight;
	}

	//set direction
	void setDirection(glm::vec3 direction) {
		this->light.direction = direction;
	}

	//set ambient
	void setAmbient(glm::vec3 ambient) {
		this->light.ambient = ambient;
	}

	//set diffuse
	void setDiffuse(glm::vec3 diffuse) {
		this->light.diffuse = diffuse;
	}

	//set specular
	void setSpecular(glm::vec3 specular) {
		this->light.specular = specular;
	}

	//set enabler
	void setEnable(bool enable) {
		this->light.enable = enable;
	}
private:
	Light light;
	static unsigned int dirLightNumber;
	unsigned int lightNumber;

};

class LightPoint{
public:
	//Default Constructor
	LightPoint(){
		light.position = L_DEFAULT_POSITION;
		light.ambient = L_DEFAULT_AMBIENT;
		light.diffuse = L_DEFAULT_DIFFUSE;
		light.specular = L_DEFAULT_SPECULAR;
		light.constant = L_DEFAULT_CONSTANT;
		light.linear = L_DEFAULT_LINEAR;
		light.quadratic = L_DEFAULT_QUADRATIC;
		light.enable = L_DEFAULT_ENABLE;
		lightNumber = pointLightNumber;
		pointLightNumber++;
	}

	//Position Constructor
	LightPoint(glm::vec3 position) {
		light.position = position;
		light.ambient = L_DEFAULT_AMBIENT;
		light.diffuse = L_DEFAULT_DIFFUSE;
		light.specular = L_DEFAULT_SPECULAR;
		light.constant = L_DEFAULT_CONSTANT;
		light.linear = L_DEFAULT_LINEAR;
		light.quadratic = L_DEFAULT_QUADRATIC;
		light.enable = L_DEFAULT_ENABLE;
		lightNumber = pointLightNumber;
		pointLightNumber++;
	}

	//Struct Light Constructor
	LightPoint(Light initLight) {
		this->light = initLight;
		lightNumber = pointLightNumber;
		pointLightNumber++;
	}
	void use(Shader shader) {
		shader.setVec3(L_FRAG_SHADER_ARRAY_POINT + '[' + to_string(lightNumber) + "].ambient", light.ambient);
		shader.setVec3( L_FRAG_SHADER_ARRAY_POINT + '[' + to_string(lightNumber) + "].diffuse", light.diffuse);
		shader.setVec3( L_FRAG_SHADER_ARRAY_POINT + '[' + to_string(lightNumber) + "].specular", light.specular);
		shader.setFloat( L_FRAG_SHADER_ARRAY_POINT + '[' + to_string(lightNumber) + "].constant", light.constant);
		shader.setFloat( L_FRAG_SHADER_ARRAY_POINT + '[' + to_string(lightNumber) + "].linear", light.linear);
		shader.setFloat( L_FRAG_SHADER_ARRAY_POINT + '[' + to_string(lightNumber) + "].quadratic", light.quadratic);
		shader.setVec3( L_FRAG_SHADER_ARRAY_POINT + '[' + to_string(lightNumber) + "].position", light.position);
		shader.setBool( L_FRAG_SHADER_ARRAY_POINT + '[' + to_string(lightNumber) + "].enable", light.enable);
	}

	//set attenuation by range
	void setAttenuation(int range) {
		this->light.constant = 1.0;
		if (range <= 7) {
			this->light.linear = 0.7f;
			this->light.quadratic = 1.8f;
		}
		else if (8 <= range <= 13) {
			this->light.linear = 0.35f;
			this->light.quadratic = 0.44f;
		}
		else if (14 <= range <= 20) {
			this->light.linear = 0.22f;
			this->light.quadratic = 0.20f;
		}
		else if (21 <= range <= 32) {
			this->light.linear = 0.14f;
			this->light.quadratic = 0.07f;
		}
		else if (33 <= range <= 50) {
			this->light.linear = 0.09f;
			this->light.quadratic = 0.032f;
		}
		else if (51 <= range <= 65) {
			this->light.linear = 0.07f;
			this->light.quadratic = 0.017f;
		}
		else if (66 <= range <= 100) {
			this->light.linear = 0.045f;
			this->light.quadratic = 0.0075f;
		}
		else if (101 <= range <= 160) {
			this->light.linear = 0.027;
			this->light.quadratic = 0.0028f;
		}
		else if (161 <= range <= 200) {
			this->light.linear = 0.022f;
			this->light.quadratic = 0.0019f;
		}
		else if (201 <= range <= 325) {
			this->light.linear = 0.014f;
			this->light.quadratic = 0.0007f;
		}
		else if (326 <= range <= 600) {
			this->light.linear = 0.007f;
			this->light.quadratic = 0.0002f;
		}
		else if (601 <= range) {
			this->light.linear = 0.0014f;
			this->light.quadratic = 0.000007f;
		}
	}

	//set light properties with struct
	void setLight(Light initLight) {
		this->light = initLight;
	}

	//set position
	void setPosition(glm::vec3 position) {
		this->light.position = position;
	}

	//set ambient
	void setAmbient(glm::vec3 ambient) {
		this->light.ambient = ambient;
	}

	//set diffuse
	void setDiffuse(glm::vec3 diffuse) {
		this->light.diffuse = diffuse;
	}

	//set specular
	void setSpecular(glm::vec3 specular) {
		this->light.specular = specular;
	}

	//set constant
	void setConstant(float constant) {
		this->light.constant = constant;
	}

	//set linaer
	void setLinear(float linear) {
		this->light.linear = linear;
	}

	//set quadratic
	void setQuadratic(float quadratic) {
		this->light.quadratic = quadratic;
	}

	//set enabler
	void setEnable(bool enable) {
		this->light.enable = enable;
	}
private:
	Light light;
	unsigned int lightNumber;
	static unsigned int pointLightNumber;
};

//static init
unsigned int LightPoint::pointLightNumber = 0;
unsigned int LightDir::dirLightNumber = 0;
unsigned int LightSpot::spotLightNumber = 0;

#endif // !LIGHTS_H
