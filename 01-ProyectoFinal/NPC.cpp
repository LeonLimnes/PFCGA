#include "NPC.h"
extern void addOrUpdateColliders(std::map<std::string,
	std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders,
	std::string name, AbstractModel::OBB collider, glm::mat4 transform);

void NPC::start(std::string nombre, glm::vec3 posInicial, float rotInicial) {
	this->nombre = nombre;
	nombreAtaque = nombre + "-Ataque";
	modelMatrixNPC = glm::translate(modelMatrixNPC, posInicial);

	modelMatrixRed = modelMatrixNPC;
	modelMatrixRed = glm::scale(modelMatrixRed, glm::vec3(escalaRed, escalaRed, escalaRed));

	modelMatrixNPC = glm::scale(modelMatrixNPC, glm::vec3(escalaModelo, escalaModelo, escalaModelo));
	modelMatrixNPC = glm::rotate(modelMatrixNPC, glm::radians(rotInicial), glm::vec3(0, 1, 0));

	anguloA = sin(glm::radians(rotInicial));
	anguloB = cos(glm::radians(rotInicial));
}

void NPC::cargarModelo(const std::string & path, Shader *shader_ptr) {
	modelo.loadModel(path);
	modelo.setShader(shader_ptr);

	red.loadModel("../models/AnimalCrossing/Flimsy Net/Red.fbx");
	red.setShader(shader_ptr);
}

void NPC::setShader(Shader *shader_ptr) {
	modelo.setShader(shader_ptr);
	red.setShader(shader_ptr);
}

void NPC::destroy() {
	modelo.destroy();
	red.destroy();
}

void NPC::update(glm::mat4 modelMatrixJugador, Terrain *terreno, 
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders, float deltaTime) {

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
			tiempoAtacando += deltaTime/2;
			modelo.setAnimationIndex(2);

			// Genera el trigger para el ataque
			if (tiempoAtacando >= 0.5f && !golpeando) {
				crearColisionadorAtaque(colliders);
				golpeando = true;
			}

			if (tiempoAtacando >= 1.0f) {
				tiempoAtacando = 0.0f;
				estadoRed = 0; rotacionXRed = -60.0f;
				atacando = false;
				golpeando = false;
			}

			// Máquina de estados para animar el ataque de la red
			switch (estadoRed) {
			case 0:
				rotacionXRed -= 1.4f;

				if (tiempoAtacando >= 0.333f) {
					estadoRed = 1;
				}
				break;

			case 1:
				rotacionXRed += 15.5f;
				if (tiempoAtacando >= 0.5f) {
					estadoRed = 2;
				}
				break;

			case 2:
				if (tiempoAtacando >= 0.8333f) {
					estadoRed = 3;
				}
				break;

			case 3:
				rotacionXRed -= 7.5f;
				if (tiempoAtacando >= 1.0f) {
					estadoRed = 0;
				}

			default:
				break;
			}
		}

		modelMatrixNPC[3][1] = terreno->getHeightTerrain(modelMatrixNPC[3][0], modelMatrixNPC[3][2]);
		glm::mat4 modelMatrixNPCBody = glm::mat4(modelMatrixNPC);
		modelo.render(modelMatrixNPCBody, deltaTime);
		modelo.setAnimationIndex(0);	// NPC en reposo

		// Renderiza la red que porta el npc
		rotacionY = calcularRotacionY(anguloA, anguloB);

		modelMatrixRed[0] = glm::vec4(escalaRed * cos(glm::radians(rotacionY)), 0, escalaRed * -sin(glm::radians(rotacionY)), 0);
		modelMatrixRed[2] = glm::vec4(escalaRed * sin(glm::radians(rotacionY)), 0, escalaRed * cos(glm::radians(rotacionY)), 0);
		modelMatrixRed[3] = glm::vec4(modelMatrixNPC[3][0] - 0.5f*cos(glm::radians(rotacionY)), modelMatrixNPC[3][1] + 1.2f, modelMatrixNPC[3][2] + 0.5*sin(glm::radians(rotacionY)), 1);
		glm::mat4 modelMatrixRedBody = modelMatrixRed;
		modelMatrixRedBody = glm::rotate(modelMatrixRedBody, glm::radians(rotacionXRed), glm::vec3(1,0,0));
		red.render(modelMatrixRedBody);
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
}

void NPC::crearColisionadorAtaque(std::map<std::string,
	std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders) {

	glm::mat4 modelMatrixColliderAtaqueNPC = glm::mat4(1.0);
	// Set the orientation of collider before doing the scale
	colliderAtaque.u = glm::quat_cast(modelMatrixColliderAtaqueNPC);
	modelMatrixColliderAtaqueNPC = glm::translate(modelMatrixColliderAtaqueNPC, glm::vec3(modelMatrixNPC[3]) + glm::vec3(-2 * sin(-anguloA), 0, 2 * sin(anguloB)));
	colliderAtaque.c = glm::vec3(modelMatrixColliderAtaqueNPC[3]) + glm::vec3(0, 1.5f, 0);
	colliderAtaque.e = modelo.getObb().e * glm::vec3(1.5, 2.0, 1.5);
	addOrUpdateColliders(colliders, nombreAtaque, colliderAtaque, modelMatrixNPC);
}

void NPC::colisionAtaque(
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > *collidersOBB,
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it,
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt) {

	// Trigger para el ataque
	if (golpeando) {
		// Detecta si el trigger del ataque colisiona con otro objeto
		if (it->first.compare(nombreAtaque.c_str()) == 0) {

			// Si hay trigger del ataque con el jugador, entonces hay daño
			if (jt->first.compare("mayow") == 0) {
				printf("Hay dano\n");
			}
		}
	}
	collidersOBB->erase(nombreAtaque);
}

float NPC::calcularRotacionY(float anguloA, float anguloB) {
	float angRot;

	if (anguloA >= 0) {
		if (anguloB >= 0) {
			angRot = 90 * sin(anguloA);
		}
		else {
			angRot = 90 + 90 * cos(anguloA);
		}
	}
	else {
		if (anguloB < 0) {
			angRot = 180 + 90 * sin(-anguloA);
		}
		else {
			angRot = 270 + 90 * cos(-anguloA);
		}
	}

	return angRot;
}

NPC::NPC() {}

NPC::~NPC() {}