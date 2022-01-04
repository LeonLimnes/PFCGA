#pragma once
#include "Headers/Model.h"
#include "Headers/Terrain.h"

class Jugador {

public:
	// Constructores
	Jugador();
	~Jugador();

	// Variables
	Model modelo;
	std::string nombre;
	glm::mat4 modelMatrixJugador = glm::mat4(1.0f);
	int indiceAnimacion = 1;

	// Funciones
	void start(std::string nombre, glm::vec3 posInicial, float rotInicial);
	void update(Terrain *terreno);
	void cargarModelo(const std::string & path, Shader *shader_ptr);
	void setShader(Shader *shader_ptr);
	void mover(float cantidad);
	void girar(float cantidad);
	void crearColisionador(std::map<std::string,
		std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders);
	void destroy();

};