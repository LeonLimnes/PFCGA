#include "NPC.h"
extern void addOrUpdateColliders(std::map<std::string,
	std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders,
	std::string name, AbstractModel::OBB collider, glm::mat4 transform);

void NPC::start(std::string nombre, glm::vec3 posInicial, float rotInicial, bool activo) {
	this->nombre = nombre;
	nombreAtaque = nombre + "-Ataque";
	modelMatrixNPC = glm::translate(modelMatrixNPC, posInicial);

	modelMatrixRed = modelMatrixNPC;
	modelMatrixRed = glm::scale(modelMatrixRed, glm::vec3(escalaRed, escalaRed, escalaRed));

	modelMatrixNPC = glm::scale(modelMatrixNPC, glm::vec3(escalaModelo, escalaModelo, escalaModelo));
	modelMatrixNPC = glm::rotate(modelMatrixNPC, glm::radians(rotInicial), glm::vec3(0, 1, 0));

	anguloA = sin(glm::radians(rotInicial));
	anguloB = cos(glm::radians(rotInicial));

	this->activo = activo;
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

void NPC::update(Jugador *jugador, Terrain *terreno,
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders, float deltaTime, bool &activandoNPC, int &idNPC, int estadoPrograma) {

	if (activo == true) {
		// El NPC mira hacia la cámara en todo momento
		glm::vec3 jdrPos = jugador->modelMatrixJugador[3];
		glm::vec3 npcPos = modelMatrixNPC[3];

		// Distancia entre el NPC y el jugador
		distancia = glm::distance(npcPos, jdrPos);

		// Persigue y ataca al jugador solo cuando este sigue "vivo" y se juega una pertida
		if (jugador->activo && estadoPrograma == 2) {
			if (distancia <= 1.8f && !atacando) {
				modelo.runningTime = 0.0f;
				distancia = 1.5f;
				atacando = true;
			}
			else if (distancia <= 17.0f && !atacando && !muriendo) {
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
				colliders.erase(nombreAtaque);
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

		// El NPC ha muerto
		if (salud <= 0 && !muriendo) {
			salud = 0;
			atacando = false;
			tiempoAtacando = 0.0f;
			modelo.runningTime = 0.0f;
			colliders.erase(nombre);
			muriendo = true;
		}

		if (muriendo) {
			tiempoMuriendo += deltaTime/1.5f;
			modelo.setAnimationIndex(3);

			if (tiempoMuriendo >= 1.9f) {
				tiempoMuriendo = 0.0f;
				muriendo = false;

				// Si este NPC muere, otro ocupará su lugar (estrategia para optimización de FPS)
				idNPC = jugador->puntuacion;
				activandoNPC = true;
				jugador->puntuacion++;
				activo = false;
			}
		}

		modelMatrixNPC[3][1] = terreno->getHeightTerrain(modelMatrixNPC[3][0], modelMatrixNPC[3][2]);
		glm::mat4 modelMatrixNPCBody = glm::mat4(modelMatrixNPC);
		modelo.render(modelMatrixNPCBody, deltaTime/2);
		modelo.setAnimationIndex(0);	// NPC en reposo

		if (!muriendo) {
			// Renderiza la red que porta el NPC
			rotacionY = calcularRotacionY(anguloA, anguloB);

			modelMatrixRed[0] = glm::vec4(escalaRed * cos(glm::radians(rotacionY)), 0, escalaRed * -sin(glm::radians(rotacionY)), 0);
			modelMatrixRed[2] = glm::vec4(escalaRed * sin(glm::radians(rotacionY)), 0, escalaRed * cos(glm::radians(rotacionY)), 0);
			modelMatrixRed[3] = glm::vec4(modelMatrixNPC[3][0] - 0.5f*cos(glm::radians(rotacionY)), modelMatrixNPC[3][1] + 1.2f, modelMatrixNPC[3][2] + 0.5*sin(glm::radians(rotacionY)), 1);
			glm::mat4 modelMatrixRedBody = modelMatrixRed;
			modelMatrixRedBody = glm::rotate(modelMatrixRedBody, glm::radians(rotacionXRed), glm::vec3(1,0,0));
			red.render(modelMatrixRedBody);
		}
	}
}

void NPC::crearColisionador(std::map<std::string,
	std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders) {

	if (activo && !muriendo) {
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
	Jugador *jugador,
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > *collidersOBB,
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it,
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt) {

	// Trigger para el ataque
	if (golpeando) {
		// Detecta si el trigger del ataque colisiona con otro objeto
		if (it->first.compare(nombreAtaque.c_str()) == 0) {

			// Si hay trigger del ataque con el jugador, entonces hay daño
			if (jt->first.compare(jugador->nombre) == 0) {
				jugador->salud -= 50.0f;
				printf("Vida restante: %d\n", jugador->salud);
				collidersOBB->erase(nombreAtaque);
			}
		}
	}
}

void NPC::triggerBala(int cantidadBalas,
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > *collidersOBB,
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it,
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt) {

	if (it->first.compare(nombre.c_str()) == 0) {
		for (int i = 0; i < cantidadBalas; i++) {

			// Si una bala impacta al NPC, le hace daño
			if (jt->first.compare("Bala-" + std::to_string(i)) == 0) {
				salud -= 15;
			}
		}
	}
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