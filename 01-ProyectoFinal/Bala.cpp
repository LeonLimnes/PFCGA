#include "Bala.h"
extern void addOrUpdateColliders(std::map<std::string,
	std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders,
	std::string name, AbstractModel::OBB collider, glm::mat4 transform);

void Bala::inicializar(int indice, Shader *shader_ptr) {
	nombre = "Bala-" + std::to_string(indice);
	modeloBala.loadModel("../models/Bala/Bala.obj");
	modeloBala.setShader(shader_ptr);
}

void Bala::update(std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders,
	float deltaTime) {

	// Si la bala está activa, se renderiza
	if (activa) {
		modelMatrixBala = glm::translate(modelMatrixBala, glm::vec3(0, 0, velocidadDisparo));
		tiempoVidaActual += deltaTime;

		// Renderizar bala
		glm::mat4 modelMatrixBalaBody = glm::mat4(modelMatrixBala);
		modeloBala.render(modelMatrixBalaBody);

		if (tiempoVidaActual >= tiempoMaximoVida) {
			colliders.erase(nombre);
			tiempoVidaActual = 0.0f;
			activa = false;
		}
	}
}

void Bala::crearColisionador(std::map<std::string,
	std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders) {

	if (activa) {
		AbstractModel::OBB balaCollider;
		glm::mat4 modelmatrixColliderBala = glm::mat4(modelMatrixBala);
		// Set the orientation of collider before doing the scale
		balaCollider.u = glm::quat_cast(modelmatrixColliderBala);
		modelmatrixColliderBala = glm::scale(modelmatrixColliderBala, glm::vec3(1.0, 1.0, 1.0));
		balaCollider.c = glm::vec3(modelmatrixColliderBala[3]);
		balaCollider.e = glm::vec3(0.4, 0.4, 0.7);
		addOrUpdateColliders(colliders, nombre, balaCollider, modelMatrixBala);
	}
}

void Bala::colisionBala(
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > *collidersOBB,
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it,
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt
) {
	// Si la bala colisionó con otro objeto, la elimina
	if (jt->first.compare(nombre.c_str()) == 0) {
		collidersOBB->erase(jt->first);
		tiempoVidaActual = 0.0f;
		activa = false;
	}
}

void Bala::destroy() {
	modeloBala.destroy();
}

Bala::Bala() { }

Bala::~Bala() { }
