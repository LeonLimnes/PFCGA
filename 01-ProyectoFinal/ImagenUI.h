#pragma once
#include "Headers/Texture.h"
#include "Headers/Shader.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//std includes
#include <string>
#include <iostream>

class ImagenUI {
public:
	// Constructores
	ImagenUI();
	~ImagenUI();

	// Variables
	Shader shader;

	// Funciones
	void inicializar(std::string rutaImagen, int imageWidth, int imageHeight, FIBITMAP *bitmap, unsigned char *data);
	void render(glm::vec2 posicion, glm::vec2 tamano, float rotacion, glm::vec3 color, glm::mat4 proyeccion);
	void destroy();

private:
	GLuint textureID;
	unsigned int imagenVAO;
};
