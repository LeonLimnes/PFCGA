#pragma once
#include "Headers/Model.h"
#include "Headers/Terrain.h"
#include "Jugador.h"

class NPC {
public:
	// Constructores
	NPC ();
	~NPC ();

	// Variables
	Model modelo, red;
	std::string nombre;
	int salud = 100;
	glm::mat4 modelMatrixNPC = glm::mat4(1.0f), modelMatrixRed = glm::mat4(1.0f);
	float escalaModelo = 0.02f, rotacionY, escalaRed = 1.5f;
	float tiempoAtacando = 0.0f, tiempoMuriendo = 0.0f;
	bool activo = true, atacando = false, golpeando = false, muriendo = false;

	AbstractModel::OBB npcCollider, colliderAtaque;
	std::string nombreAtaque;

	// Funciones
	void start(std::string nombre, glm::vec3 posInicial, float rotInicial);
	void update(Jugador *jugador, Terrain *terreno,
		std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders, float deltaTime);
	void cargarModelo(const std::string & path, Shader *shader_ptr);
	void setShader(Shader *shader_ptr);
	void crearColisionador(std::map<std::string,
		std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders);
	void crearColisionadorAtaque(std::map<std::string,
		std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders);
	void colisionAtaque(
		Jugador *jugador,
		std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > *collidersOBB,
		std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it,
		std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt
	);
	void triggerBala(int cantidadBalas, 
		std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > *collidersOBB,
		std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it,
		std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt
	);
	void destroy();

private:
	// Variables
	double a, b, h;
	float anguloA, anguloB, distancia, rotacionXRed = -60.0f;
	int estadoRed = 0;

	// Funciones
	float calcularRotacionY(float anguloA, float anguloB);
};
