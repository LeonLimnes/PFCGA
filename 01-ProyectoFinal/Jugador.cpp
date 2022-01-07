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
	modelMatrixJugador[3][1] = terreno->getHeightTerrain(modelMatrixJugador[3][0], modelMatrixJugador[3][2]);
	glm::mat4 modelMatrixJugadorBody = glm::mat4(modelMatrixJugador);
	modelMatrixJugadorBody = glm::scale(modelMatrixJugadorBody, glm::vec3(0.021, 0.021, 0.021));
	modelo.setAnimationIndex(indiceAnimacion);
	modelo.render(modelMatrixJugadorBody);

	// Actualiza el estado de cada bala
	for (int i = 0; i < cantidadBalas; i++) {
		balas[i].update(colliders, deltaTime);
	}
}

void Jugador::moverZ(std::shared_ptr<Camera> camera, float cantidad) {
	modelMatrixJugador[0] = glm::vec4(cos(glm::radians(-rotacionY) - camera->angleAroundTarget), 0, sin(glm::radians(-rotacionY) - camera->angleAroundTarget), 0);
	modelMatrixJugador[2] = glm::vec4(-sin(glm::radians(-rotacionY) - camera->angleAroundTarget), 0, cos(glm::radians(-rotacionY) - camera->angleAroundTarget), 0);
	modelMatrixJugador = glm::translate(modelMatrixJugador, glm::vec3(0, 0, cantidad));
	rotacionY = rotacionY + glm::degrees(camera->angleAroundTarget);
	camera->angleAroundTarget = 0.0f;
	indiceAnimacion = 0;
}

void Jugador::moverX(std::shared_ptr<Camera> camera, float cantidad) {
	modelMatrixJugador[0] = glm::vec4(cos(glm::radians(-rotacionY) - camera->angleAroundTarget), 0, sin(glm::radians(-rotacionY) - camera->angleAroundTarget), 0);
	modelMatrixJugador[2] = glm::vec4(-sin(glm::radians(-rotacionY) - camera->angleAroundTarget), 0, cos(glm::radians(-rotacionY) - camera->angleAroundTarget), 0);
	modelMatrixJugador = glm::translate(modelMatrixJugador, glm::vec3(cantidad, 0, 0));
	rotacionY = rotacionY + glm::degrees(camera->angleAroundTarget);
	camera->angleAroundTarget = 0.0f;
	indiceAnimacion = 0;
}

void Jugador::disparar(std::shared_ptr<Camera> camera) {
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
			balas[i].modelMatrixBala = glm::translate(balas[i].modelMatrixBala, glm::vec3(0, 0, 1.5f));
			indiceAnimacion = 2;
		}
	}
}

void Jugador::crearColisionador(std::map<std::string,
	std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders) {

	AbstractModel::OBB jugadorCollider;
	glm::mat4 modelmatrixColliderJugador = glm::mat4(modelMatrixJugador);
	modelmatrixColliderJugador = glm::rotate(modelmatrixColliderJugador, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	// Set the orientation of collider before doing the scale
	jugadorCollider.u = glm::quat_cast(modelmatrixColliderJugador);
	modelmatrixColliderJugador = glm::scale(modelmatrixColliderJugador, glm::vec3(0.021, 0.021, 0.021));
	modelmatrixColliderJugador = glm::translate(modelmatrixColliderJugador, glm::vec3(modelo.getObb().c.x, modelo.getObb().c.y, modelo.getObb().c.z));
	jugadorCollider.e = modelo.getObb().e * glm::vec3(0.021, 0.021, 0.021) * glm::vec3(0.787401574, 0.787401574, 0.787401574);
	jugadorCollider.c = glm::vec3(modelmatrixColliderJugador[3]);
	addOrUpdateColliders(colliders, nombre, jugadorCollider, modelMatrixJugador);

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