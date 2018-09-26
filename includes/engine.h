#ifndef ENGINE_H
#define ENGINE_H
#include <glad\glad.h>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>
#include <lights.h>
#include <Model.h>
#include <skybox.h>
#include <textgl.h>

#include <vector>
using namespace std;

#define E_DEFAULT_OBJECT_POSITION glm::vec3(0.0f,0.0f,0.0f);
#define E_DEFAULT_OBJECT_ROTATION 0.0f
#define E_DEFAULT_OBJECT_SCALE 1.0f

class Object{
public:
	Object(Model model) {

	}

	void setPosition(glm::vec3 newPosition) {
		position = newPosition;
	}

	void setScale(glm::vec3 newScale) {
		scale = newScale;
	}

	void setRotationDir(glm::vec3 newRotationDir) {
		rotationDir = newRotationDir;
	}

	void setRotation(float newRotation) {
		rotation = newRotation;
	}
private:
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotationDir;
	float rotation;
};
namespace Engine{
	Shader ourShader("VertexShaders\\VertexShaderL.vs", "FragmentShaders\\fragmentShaderL.fs"); // Main Shader for objects
	Shader lampShader("VertexShaders\\vertexShadersLamp.vs", "FragmentShaders\\fragmentShaderLamp.fs"); //position lamp shaders
	Shader singleShader("VertexShaders\\vertexShadersLamp.vs", "FragmentShaders\\fragmentShaderSingle.fs"); //outline shaders
	Shader cubeMapShader("VertexShaders\\vertexShaderCubeMap.vs", "FragmentShaders\\fragmentShaderCubeMap.fs"); //cube map shaders

	vector<Model> prefabs;
	vector<LightPoint> pointLights;
	vector<LightDir> directionLights;
	vector<LightSpot> spotlights;

	SkyBox skybox;
}

#endif