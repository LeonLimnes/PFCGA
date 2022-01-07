#include "Jugador.h"
extern void addOrUpdateColliders(std::map<std::string,
	std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders,
	std::string name, AbstractModel::OBB collider, glm::mat4 transform);

void Jugador::start(std::string nombre, glm::vec3 posInicial, float rotInicial) {
	this->nombre = nombre;
	modelMatrixJugador = glm::translate(modelMatrixJugador, posInicial);
	modelMatrixJugador = glm::rotate(modelMatrixJugador, glm::radians(rotInicial), glm::vec3(0, 1, 0));
	rotacionY = rotInicial;

	// Inicialización de balas
	for (int i = 0; i < cantidadBalas; i++) {
		balas[i].inicializar(i);
	}
}

void Jugador::cargarModelo(const std::string & path, Shader *shader_ptr) {
	modelo.loadModel(path);
	modelo.setShader(shader_ptr);
}

void Jugador::update(std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders,
	Terrain *terreno, float deltaTime) {

	if (activo) {
		// Animación de disparo
		if (disparando) {
			tiempoDisparando += deltaTime;
			indiceAnimacion = 2;

			if (tiempoDisparando >= 0.75f) {
				tiempoDisparando = 0.0f;
				disparando = false;
			}
		}

		// El jugador muere (pierde el juego)
		if (salud <= 0) {
			tiempoDisparando = 0.0f;
			tiempoMuriendo = 0.0f;
			modelo.runningTime = 0.0f;
			salud = 0;
			colliders.erase(nombre);
			disparando = false;
			muriendo = true;
			activo = false;
			printf("Fin del juego\n");
		}
	}

	if (muriendo) {
		tiempoMuriendo += deltaTime;
		indiceAnimacion = 3;
		
		if (tiempoMuriendo >= 4.3f) {
			tiempoMuriendo = 0.0f;
			muriendo = false;
			muerto = true;
		}
	}

	if (muerto) {
		indiceAnimacion = 3;
		modelo.runningTime = 4.3f;
	}

	// Renderizado del jugador
	modelMatrixJugador[3][1] = terreno->getHeightTerrain(modelMatrixJugador[3][0], modelMatrixJugador[3][2]);
	glm::mat4 modelMatrixJugadorBody = glm::mat4(modelMatrixJugador);
	modelMatrixJugadorBody = glm::scale(modelMatrixJugadorBody, glm::vec3(0.021, 0.021, 0.021));
	modelo.setAnimationIndex(indiceAnimacion);
	modelo.render(modelMatrixJugadorBody, deltaTime / 1.5f);

	// Actualiza el estado de cada bala
	for (int i = 0; i < cantidadBalas; i++) {
		balas[i].update(colliders, deltaTime);
	}
}

void Jugador::moverZ(std::shared_ptr<Camera> camera, float cantidad) {
	if (activo) {
		modelMatrixJugador[0] = glm::vec4(cos(glm::radians(-rotacionY) - camera->angleAroundTarget), 0, sin(glm::radians(-rotacionY) - camera->angleAroundTarget), 0);
		modelMatrixJugador[2] = glm::vec4(-sin(glm::radians(-rotacionY) - camera->angleAroundTarget), 0, cos(glm::radians(-rotacionY) - camera->angleAroundTarget), 0);
		modelMatrixJugador = glm::translate(modelMatrixJugador, glm::vec3(0, 0, cantidad));
		rotacionY = rotacionY + glm::degrees(camera->angleAroundTarget);
		camera->angleAroundTarget = 0.0f;
		indiceAnimacion = 0;
	}
}

void Jugador::moverX(std::shared_ptr<Camera> camera, float cantidad) {
	if (activo) {
		modelMatrixJugador[0] = glm::vec4(cos(glm::radians(-rotacionY) - camera->angleAroundTarget), 0, sin(glm::radians(-rotacionY) - camera->angleAroundTarget), 0);
		modelMatrixJugador[2] = glm::vec4(-sin(glm::radians(-rotacionY) - camera->angleAroundTarget), 0, cos(glm::radians(-rotacionY) - camera->angleAroundTarget), 0);
		modelMatrixJugador = glm::translate(modelMatrixJugador, glm::vec3(cantidad, 0, 0));
		rotacionY = rotacionY + glm::degrees(camera->angleAroundTarget);
		camera->angleAroundTarget = 0.0f;
		indiceAnimacion = 0;
	}
}

void Jugador::disparar(std::shared_ptr<Camera> camera) {

	if (activo) {
		// Reutiliza balas disponibles mediente la técnica Object Pooling
		for (int i = 0; i < cantidadBalas; i++) {
			if (balas[i].activa == false) {
				balas[i].activa = true;
				balas[i].tiempoVidaActual = 0.0f;

				// Orienta al jugador dependiendo de a dónde mire la cámara
				modelMatrixJugador[0] = glm::vec4(cos(glm::radians(-rotacionY) - camera->angleAroundTarget), 0, sin(glm::radians(-rotacionY) - camera->angleAroundTarget), 0);
				modelMatrixJugador[2] = glm::vec4(-sin(glm::radians(-rotacionY) - camera->angleAroundTarget), 0, cos(glm::radians(-rotacionY) - camera->angleAroundTarget), 0);
				rotacionY = rotacionY + glm::degrees(camera->angleAroundTarget);
				camera->angleAroundTarget = 0.0f;

				// Inicializa la bala dependiendo la posición y orientación del jugador
				balas[i].modelMatrixBala = glm::mat4(modelMatrixJugador);
				balas[i].modelMatrixBala = glm::translate(balas[i].modelMatrixBala, glm::vec3(0, 1.5f, 1.5f));
				balas[i].modelMatrixBala = glm::rotate(balas[i].modelMatrixBala, camera->getPitch(), glm::vec3(1, 0, 0));

				// Variables para activar la animación
				tiempoDisparando = 0.0f;
				modelo.runningTime = 0.0f;
				disparando = true;
			}
		}
	}
}

void Jugador::crearColisionador(std::map<std::string,
	std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders) {

	if (activo) {
		AbstractModel::OBB jugadorCollider;
		glm::mat4 modelmatrixColliderJugador = glm::mat4(modelMatrixJugador);
		// Set the orientation of collider before doing the scale
		jugadorCollider.u = glm::quat_cast(modelmatrixColliderJugador);
		modelmatrixColliderJugador = glm::translate(modelmatrixColliderJugador, modelo.getObb().c + glm::vec3(0, 0.7f, -0.1f));
		modelmatrixColliderJugador = glm::scale(modelmatrixColliderJugador, glm::vec3(2, 2, 2));
		jugadorCollider.e = glm::vec3(1.5, 1.7, 1.5) * modelo.getObb().e;
		jugadorCollider.c = glm::vec3(modelmatrixColliderJugador[3]);
		addOrUpdateColliders(colliders, nombre, jugadorCollider, modelMatrixJugador);
	}

	// Genera el colisionador para cada bala
	for (int i = 0; i < cantidadBalas; i++) {
		balas[i].crearColisionador(colliders);
	}
}

void Jugador::colisionesBalas(
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > *collidersOBB,
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it,
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt
) {

	// Realiza pruebas de colisión para cada bala
	for (int i = 0; i < cantidadBalas; i++) {
		// Verifica que la bala no colisione con el jugador
		if (jt->first.compare(nombre) != 0) {
			balas[i].colisionBala(collidersOBB, it, jt);
		}
	}
}

void Jugador::setShader(Shader *shader_ptr) {
	modelo.setShader(shader_ptr);
}

void Jugador::destroy() {
	modelo.destroy();
}

Jugador::Jugador() {}

Jugador::~Jugador() {}