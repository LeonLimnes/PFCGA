#pragma once
#include "Headers/Model.h"

class Bala {
public:
	// Constructores
	Bala();
	~Bala();

	// Variables
	std::string nombre;
	Model modelo;
	glm::mat4 modelMatrixBala = glm::mat4(1.0f);
	bool activa = false;
	float velocidadDisparo = 1.5f, tiempoMaximoVida = 5.0f, tiempoVidaActual = 0.0f;

	// Funciones
	void inicializar(int indice);
	void update(std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders, float deltaTime);
	void crearColisionador(std::map<std::string,
		std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders);
	void colisionBala(
		std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > *collidersOBB,
		std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it,
		std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt
	);
};
