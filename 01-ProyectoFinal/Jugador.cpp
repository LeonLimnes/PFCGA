#include "Jugador.h"
extern void addOrUpdateColliders(std::map<std::string,
	std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders,
	std::string name, AbstractModel::OBB collider, glm::mat4 transform);

void Jugador::start(std::string nombre, glm::vec3 posInicial, float rotInicial) {
	this->nombre = nombre;
	modelMatrixJugador = glm::translate(modelMatrixJugador, posInicial);
	modelMatrixJugador = glm::rotate(modelMatrixJugador, glm::radians(rotInicial), glm::vec3(0, 1, 0));
}

void Jugador::cargarModelo(const std::string & path, Shader *shader_ptr) {
	modelo.loadModel(path);
	modelo.setShader(shader_ptr);
}

void Jugador::update(Terrain *terreno) {
	modelMatrixJugador[3][1] = terreno->getHeightTerrain(modelMatrixJugador[3][0], modelMatrixJugador[3][2]);
	glm::mat4 modelMatrixJugadorBody = glm::mat4(modelMatrixJugador);
	modelMatrixJugadorBody = glm::scale(modelMatrixJugadorBody, glm::vec3(0.021, 0.021, 0.021));
	modelo.setAnimationIndex(indiceAnimacion);
	modelo.render(modelMatrixJugadorBody);
}

void Jugador::mover(float cantidad) {
	modelMatrixJugador = glm::translate(modelMatrixJugador, glm::vec3(0, 0, cantidad));
	indiceAnimacion = 0;
}

void Jugador::girar(float cantidad) {
	modelMatrixJugador = glm::rotate(modelMatrixJugador, glm::radians(cantidad), glm::vec3(0, 1, 0));
	indiceAnimacion = 0;
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
	addOrUpdateColliders(colliders, "mayow", jugadorCollider, modelMatrixJugador);
}

void Jugador::setShader(Shader *shader_ptr) {
	modelo.setShader(shader_ptr);
}

void Jugador::destroy() {
	modelo.destroy();
}

Jugador::Jugador() {}

Jugador::~Jugador() {}