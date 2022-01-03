#pragma once
#include "Headers/Model.h"
#include "Headers/FirstPersonCamera.h"
#include "Headers/Terrain.h"

class NPC {
public:
	// Constructores
	NPC ();
	~NPC ();

	// Variables
	Model modelo;
	std::string nombre;
	glm::mat4 modelMatrixNPC = glm::mat4(1.0f);
	float escala = 0.02f;
	float tiempoAtacando = 0.0f, tiempoMuriendo = 0.0f;
	bool activo = true, atacando = false, muriendo = false;

	AbstractModel::OBB npcCollider, colliderAtaque;
	std::string nombreAtaque;

	// Funciones
	void start(std::string nombre, glm::vec3 posInicial, float rotInicial);
	void update(std::shared_ptr<FirstPersonCamera> camara, float deltaTime);
	void update(glm::mat4 modelMatrixJugador, Terrain *terreno, float deltaTime);
	void cargarModelo(const std::string & path, Shader *shader_ptr);
	void setShader(Shader *shader_ptr);
	void crearColisionador(std::map<std::string,
		std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders);
	void colisionAtaque(
		std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > *collidersOBB,
		std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it,
		std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt
	);
	void destroy();

private:
	// Variables
	double a, b, h;
	float anguloA, anguloB, distancia;
};
