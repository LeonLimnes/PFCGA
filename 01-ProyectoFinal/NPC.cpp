#include "NPC.h"
#include "Headers/Model.h"
#include "Headers/Terrain.h"
extern void addOrUpdateColliders(std::map<std::string,
	std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders,
	std::string name, AbstractModel::OBB collider, glm::mat4 transform);

void NPC::start(std::string nombre, glm::vec3 posInicial, float rotInicial) {
	this->nombre = nombre;
	nombreAtaque = nombre + "-Ataque";
	modelMatrixNPC = glm::translate(modelMatrixNPC, posInicial);
	modelMatrixNPC = glm::scale(modelMatrixNPC, glm::vec3(escala, escala, escala));
	modelMatrixNPC = glm::rotate(modelMatrixNPC, glm::radians(rotInicial), glm::vec3(0, 1, 0));
}

void NPC::cargarModelo(const std::string & path, Shader *shader_ptr) {
	modelo.loadModel(path);
	modelo.setShader(shader_ptr);
}

void NPC::setShader(Shader *shader_ptr) {
	modelo.setShader(shader_ptr);
}

void NPC::destroy() {
	modelo.destroy();
}

void NPC::update(glm::mat4 modelMatrixJugador, Terrain *terreno, float deltaTime) {

	if (activo == true) {
		// El NPC mira hacia la cámara en todo momento
		glm::vec3 jdrPos = modelMatrixJugador[3];
		glm::vec3 npcPos = modelMatrixNPC[3];

		// Distancia entre el NPC y el jugador
		distancia = glm::distance(npcPos, jdrPos);

		if (distancia <= 1.8f && !atacando) {
			modelo.runningTime = 0.0f;
			distancia = 1.5f;
			atacando = true;
		}
		else if (distancia <= 12.0f && !atacando && !muriendo) {
			a = jdrPos.z - npcPos.z;
			b = jdrPos.x - npcPos.x;
			h = sqrt(a*a + b * b);
			anguloA = asin(b / h);
			anguloB = asin(a / h);

			// Rotación del NPC
			modelMatrixNPC[0] = glm::vec4(0.02 * sin(anguloB), 0, 0.02 * sin(-anguloA), 0);
			modelMatrixNPC[2] = glm::vec4(0.02 * -sin(-anguloA), 0, 0.02 * sin(anguloB), 0);

			// El NPC corre hacia el jugador
			modelMatrixNPC = glm::translate(modelMatrixNPC, glm::vec3(0.0f, 0.0f, 2.0f));
			modelo.setAnimationIndex(1);
		}

		// El NPC está atacando al jugador
		if (atacando) {
			tiempoAtacando += deltaTime;
			modelo.setAnimationIndex(2);

			if (tiempoAtacando >= 1.0f) {
				tiempoAtacando = 0.0f;
				atacando = false;
			}
		}

		modelMatrixNPC[3][1] = terreno->getHeightTerrain(modelMatrixNPC[3][0], modelMatrixNPC[3][2]);
		glm::mat4 modelMatrixNPCBody = glm::mat4(modelMatrixNPC);
		modelo.render(modelMatrixNPCBody, deltaTime);
		modelo.setAnimationIndex(0);	// NPC en reposo
	}
}

void NPC::update(std::shared_ptr<FirstPersonCamera> camara, float deltaTime) {

	if (activo == true) {
		// El NPC mira hacia la cámara en todo momento
		glm::vec3 npcPos = modelMatrixNPC[3];
		double a = camara->getPosition().z - npcPos.z;
		double b = camara->getPosition().x - npcPos.x;
		double h = sqrt(a*a + b * b);
		float anguloA = asin(b / h);
		float anguloB = asin(a / h);

		// Rotación del NPC
		modelMatrixNPC[0] = glm::vec4(0.02 * sin(anguloB), 0, 0.02 * sin(-anguloA), 0);
		modelMatrixNPC[2] = glm::vec4(0.02 * -sin(-anguloA), 0, 0.02 * sin(anguloB), 0);

		glm::mat4 modelMatrixNPCBody = glm::mat4(modelMatrixNPC);
		modelo.render(modelMatrixNPCBody, deltaTime);
		modelo.setAnimationIndex(0);
	}
}

void NPC::crearColisionador(std::map<std::string,
	std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders) {

	// Colisionador del NPC
	glm::mat4 modelMatrixColliderNPC = glm::mat4(1.0);
	// Set the orientation of collider before doing the scale
	npcCollider.u = glm::quat_cast(modelMatrixColliderNPC);
	modelMatrixColliderNPC = glm::scale(modelMatrixColliderNPC, glm::vec3(1.0, 1.0, 1.0));
	modelMatrixColliderNPC = glm::translate(modelMatrixColliderNPC, glm::vec3(modelMatrixNPC[3]));
	npcCollider.c = glm::vec3(modelMatrixColliderNPC[3]) + glm::vec3(0, 1.5f, 0);
	npcCollider.e = modelo.getObb().e * glm::vec3(1.5, 2.0, 1.5);
	addOrUpdateColliders(colliders, nombre, npcCollider, modelMatrixNPC);

	// Collider (trigger) para el ataque
	if (tiempoAtacando <= 0.51f && tiempoAtacando >= 0.5f) {
		glm::mat4 modelMatrixColliderAtaqueNPC = glm::mat4(1.0);
		// Set the orientation of collider before doing the scale
		colliderAtaque.u = glm::quat_cast(modelMatrixColliderAtaqueNPC);
		modelMatrixColliderAtaqueNPC = glm::translate(modelMatrixColliderAtaqueNPC, glm::vec3(modelMatrixNPC[3]) + glm::vec3(-2 * sin(-anguloA), 0, 2 * sin(anguloB)));
		colliderAtaque.c = glm::vec3(modelMatrixColliderAtaqueNPC[3]) + glm::vec3(0, 1.5f, 0);
		colliderAtaque.e = modelo.getObb().e * glm::vec3(1.5, 2.0, 1.5);
		addOrUpdateColliders(colliders, nombreAtaque, colliderAtaque, modelMatrixNPC);
	}
}

void NPC::colisionAtaque(
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > *collidersOBB,
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it,
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt) {

	// Trigger para el ataque
	if (tiempoAtacando <= 0.52f && tiempoAtacando >= 0.51f) {
		// Detecta si el trigger del ataque colisiona con otro objeto
		if (it->first.compare(nombreAtaque.c_str()) == 0) {

			// Si el trigger del ataque con el jugador, entonces hay daño
			if (jt->first.compare("mayow") == 0) {
				printf("Hay dano\n");
			}
		}
	}

	collidersOBB->erase(nombreAtaque);
}

NPC::NPC() {}

NPC::~NPC() {}