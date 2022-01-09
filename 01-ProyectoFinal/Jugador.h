#pragma once
#include "Headers/Model.h"
#include "Headers/Terrain.h"
#include "Headers/Camera.h"
#include <AL/alut.h>
#include "Bala.h"

class Jugador {

public:
	// Constructores
	Jugador();
	~Jugador();

	// Variables
	Model modelo;
	std::string nombre;
	int salud = 100, puntuacion = 0;
	glm::mat4 modelMatrixJugador = glm::mat4(1.0f);
	int indiceAnimacion = 1;
	float rotacionY;
	Bala balas[10];
	int cantidadBalas = 10;
	bool activo = true, disparando = false, muerto = false;

	// Funciones
	void start(std::string nombre, glm::vec3 posInicial, float rotInicial);
	void update(std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders,
		Terrain *terreno, float deltaTime, int &estadoPrograma);
	void cargarModelo(const std::string & path, Shader *shader_ptr);
	void setShader(Shader *shader_ptr);
	void moverX(std::shared_ptr<Camera> camera, float cantidad);
	void moverZ(std::shared_ptr<Camera> camera, float cantidad);
	void disparar(std::shared_ptr<Camera> camera, ALint audioDisparo, ALfloat *posAudioDisparo);
	void crearColisionador(std::map<std::string,
		std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders);
	void colisionesBalas(
		std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > *collidersOBB,
		std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it,
		std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt
	);
	void destroy();

private:
	// Variables
	bool muriendo = false;
	float tiempoDisparando = 0.0f, tiempoMuriendo = 0.0f;
};