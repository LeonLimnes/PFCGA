#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

// contains new std::shuffle definition
#include <algorithm>
#include <random>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
#include "Headers/ThirdPersonCamera.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

// Include Terrain
#include "Headers/Terrain.h"

#include "Headers/AnimationUtils.h"

// Include Colision headers functions
#include "Headers/Colisiones.h"

// Font Rendering
#include "Headers/FontTypeRendering.h";

// Shadowbox include 
#include "Headers/ShadowBox.h"

// OpenAL include
#include <AL/alut.h>

// Archivos de cabecera adicionales
#include "../../01-ProyectoFinal/NPC.h"
#include "../../01-ProyectoFinal/Jugador.h"
#include "../../01-ProyectoFinal/ImagenUI.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

int screenWidth, screenHeight;
const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
GLFWwindow *window;

Shader shader;
Shader shaderSkybox;				// Shader con skybox
Shader shaderMulLighting;			// Shader con multiples luces
Shader shaderTerrain;				// Shader para el terreno
Shader shaderParticlesFountain;		// Shader para las particulas de fountain
Shader shaderParticlesFire;			// Shader para las particulas de fuego
Shader shaderParticlesFire2;
Shader shaderViewDepth;				// Shader para visualizar el buffer de profundidad
Shader shaderDepth;					// Shader para dibujar el buffer de profunidad
Shader shaderUI;					// Shader para imágenes UI

std::shared_ptr<Camera> camera(new ThirdPersonCamera(15.0f, -15.0f, 65.0f));
float distanceFromTarget = 7.0;

Sphere skyboxSphere(20, 20);
Box boxCollider;
Sphere sphereCollider(10, 10);
Box boxViewDepth;
Box boxLightViewBox;

ShadowBox *shadowBox;
ImagenUI iconoSalud, iconoEnemigo;
ImagenUI introduccionUI1, introduccionUI2, introduccionUI3, controlesPC, controlesMando;
FontTypeRendering::FontTypeRendering *modelText;

// Models complex instances
Model modelAircraft;
//Casas
Model modelCasa, modelCasa2, modelCasa3, modelCasa4, modelCasa5, modelCasa6;

//Cerca
Model modelCerca;
// Lamps
Model modelLamp1;
// Trees
Model modelTree;
// Hierba
Model modelGrass;
// Fountain
Model modelFountain;

// Modelos animados
Model modelHazmat;				// Hazmat		
Jugador jugador;				// Mayow
NPC bobAnimate, mitziAnimate, rosieAnimate, oliviaAnimate, kikiAnimate, tangyAnimate;
NPC billyAnimate, nanAnimate, chevreAnimate, gruffAnimate, velmaAnimate;
NPC budAnimate, octavianAnimate, chiefAnimate, cjAnimate;

// Variables para activar otro NPC en tiempo de ejecución
bool activandoNPC = false, npcExtrasCargados = false;
int idNPC = 0;

// Variables para la partida
int estadoPrograma = 0, paginaIntroduccion = 0;
bool botonApresionado = false;

// Terrain model instance
Terrain terrain(-1, -1, 200, 16, "../Textures/AlturasMapa.png");

GLuint textureCespedID, textureWallID, textureWindowID, textureHighwayID, textureLandingPadID;
GLuint textureTerrainBackgroundID, textureTerrainRID, textureTerrainGID, textureTerrainBID, textureTerrainBlendMapID;
GLuint textureParticleFountainID, textureParticleFireID, texId;
GLuint skyboxTextureID;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { "../Textures/distant_sunset/distant_sunset_ft.tga",
		"../Textures/distant_sunset/distant_sunset_bk.tga",
		"../Textures/distant_sunset/distant_sunset_up.tga",
		"../Textures/distant_sunset/distant_sunset_dn.tga",
		"../Textures/distant_sunset/distant_sunset_rt.tga",
		"../Textures/distant_sunset/distant_sunset_lf.tga" };

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

// Model matrix definitions
glm::mat4 modelMatrixAircraft = glm::mat4(1.0);
//Casas
glm::mat4 modelMatrixCasa = glm::mat4(1.0);
glm::mat4 modelMatrixCasa2 = glm::mat4(1.0);
glm::mat4 modelMatrixCasa3 = glm::mat4(1.0);
glm::mat4 modelMatrixCasa4 = glm::mat4(1.0);
glm::mat4 modelMatrixCasa5 = glm::mat4(1.0);
glm::mat4 modelMatrixCasa6 = glm::mat4(1.0);

glm::mat4 modelMatrixFountain = glm::mat4(1.0f);	// Fuente de agua
glm::mat4 modelMatrixHazmat = glm::mat4(1.0f);		// hazmat

//Cerca
glm::mat4 modelMatrixCerca = glm::mat4(1.0f);
glm::mat4 modelMatrixCerca2 = glm::mat4(1.0f);
glm::mat4 modelMatrixCerca3 = glm::mat4(1.0f);
glm::mat4 modelMatrixCerca4 = glm::mat4(1.0f);

// Lamps positions
std::vector<glm::vec3> lamp1Position = { glm::vec3(-53.51, 0, -20.70), glm::vec3(-22.26, 0, -2.73), 
		glm::vec3(-10.15, 0, 13.67), glm::vec3(-42.57, 0, 19.92), glm::vec3(4.29, 0, 35.65), 
		glm::vec3(16.01, 0, 14.84), glm::vec3(46.09, 0, 7.81), glm::vec3(25.39, 0, -4.29), 
		glm::vec3(38.67, 0, -29.29), glm::vec3(7.42, 0, -19.14), glm::vec3(-8.59, 0, -37.5),
		glm::vec3(-21.87, 0, -19.92)};
std::vector<float> lamp1Orientation = { -17.0, -82.67, 23.70, -17.0, -82.67, 23.70, -17.0, -82.67, 23.70 };

// Trees positions
std::vector<glm::vec3> treePosition = { 
		glm::vec3(-24.21, 0, -66.79),
		glm::vec3(-49.21, 0, -60.54),
		glm::vec3(-72.65, 0, -19.53),
		glm::vec3(-47.65, 0, 0.39),
		glm::vec3(-56.64, 0, 48.43), //5
		glm::vec3(-23.43, 0, 55.85),
		glm::vec3(23.43, 0, 45.70),
		glm::vec3(64.84, 0, 50.78),
		glm::vec3(57.81, 0, -0.39),
		glm::vec3(76.56, 0, -19.92),
		glm::vec3(29.29, 0, -61.71),
		glm::vec3(55.07, 0, -68.75)
};
std::vector<float> treeOrientation = {
	 21.37 + 90,
	-65.0 + 90,
	/*45.60 + 90,
	65.0 + 90,
	15.60 + 90,
	80.85 + 90	*/
};

//Grass position
std::vector<glm::vec3> grassPosition = {
		glm::vec3(-22.21, 1.0, -66.79),
		glm::vec3(-49.21, 1.0, -60.54),
		glm::vec3(-72.65, 1.0, -19.53),
		glm::vec3(-47.65, 1.0, 0.39),
		glm::vec3(-56.64, 1.0, 48.43), 
		glm::vec3(-23.43, 1.0, 55.85),
		glm::vec3(23.43, 1.0, 45.70),
		glm::vec3(64.84, 1.0, 50.78),
		glm::vec3(57.81, 1.0, -0.39),
		glm::vec3(76.56, 1.0, -19.92),
		glm::vec3(29.29, 1.0, -61.71),
		glm::vec3(55.07, 1.0, -68.75),
		glm::vec3(0.07, 1.0, -2.75),
};

// Blending model unsorted
std::map<std::string, glm::vec3> blendingUnsorted = {
		{"aircraft", glm::vec3(10.0, 0.0, -17.5)},
		{"fountain", glm::vec3(5.0, 0.0, -40.0)},
		{"fire", glm::vec3(0.0, 0.0, 7.0)}
};

double deltaTime;
double currTime, lastTime;

// Definition for the particle system
GLuint initVel, startTime;
GLuint VAOParticles;
GLuint nParticles = 8000;
double currTimeParticlesAnimation, lastTimeParticlesAnimation;

// Definition for the particle system fire
GLuint initVelFire, startTimeFire;
GLuint VAOParticlesFire;
GLuint nParticlesFire = 2000;
GLuint posBuf[2], velBuf[2], age[2];
GLuint particleArray[2];
GLuint feedback[2];
GLuint drawBuf = 1;
float particleSize = 0.5, particleLifetime = 3.0;
double currTimeParticlesAnimationFire, lastTimeParticlesAnimationFire;

// Colliders
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > collidersSBB;

// Framesbuffers
GLuint depthMap, depthMapFBO;

/**********************
 * OpenAL config
 **********************/

// OpenAL Defines
#define NUM_BUFFERS 8
#define NUM_SOURCES 8
#define NUM_ENVIRONMENTS 1
// Listener
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
// Source 0
ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };	ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };
// Source 1
ALfloat source1Pos[] = { 2.0, 0.0, 0.0 };	ALfloat source1Vel[] = { 0.0, 0.0, 0.0 };
// Source 2
ALfloat source2Pos[] = { 2.0, 0.0, 0.0 };	ALfloat source2Vel[] = { 0.0, 0.0, 0.0 };
// Source 4
ALfloat source4Pos[] = { 0.0, 0.0, 1.0 };	ALfloat source4Vel[] = { 0.0, 0.0, 0.0 };
// Source 6
ALfloat source6Pos[] = { 0.0, 0.0, 1.0 };	ALfloat source6Vel[] = { 0.0, 0.0, 0.0 };
// Buffers
ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];
// Configs
ALsizei size, freq;
ALenum format;
ALvoid *data;
int ch;
ALboolean loop;
std::vector<bool> sourcesPlay = {true, true, true, true, false, false, false, false};

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void initParticleBuffers();
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);
void prepareScene();
void prepareDepthScene();
void renderScene(bool renderParticles = true);
void reiniciarPartida();
void introduccionImagenes(glm::mat4 projection);
void introduccionTextos();

void initParticleBuffers() {
	// Generate the buffers
	glGenBuffers(1, &initVel);   // Initial velocity buffer
	glGenBuffers(1, &startTime); // Start time buffer

	// Allocate space for all buffers
	int size = nParticles * 3 * sizeof(float);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_STATIC_DRAW);

	// Fill the first velocity buffer with random velocities
	glm::vec3 v(0.0f);
	float velocity, theta, phi;
	GLfloat *data = new GLfloat[nParticles * 3];
	for (unsigned int i = 0; i < nParticles; i++) {

		theta = glm::mix(0.0f, glm::pi<float>() / 6.0f, ((float)rand() / RAND_MAX));
		phi = glm::mix(0.0f, glm::two_pi<float>(), ((float)rand() / RAND_MAX));

		v.x = sinf(theta) * cosf(phi);
		v.y = cosf(theta);
		v.z = sinf(theta) * sinf(phi);

		velocity = glm::mix(0.6f, 0.8f, ((float)rand() / RAND_MAX));
		v = glm::normalize(v) * velocity;

		data[3 * i] = v.x;
		data[3 * i + 1] = v.y;
		data[3 * i + 2] = v.z;
	}
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	// Fill the start time buffer
	delete[] data;
	data = new GLfloat[nParticles];
	float time = 0.0f;
	float rate = 0.00075f;
	for (unsigned int i = 0; i < nParticles; i++) {
		data[i] = time;
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] data;

	glGenVertexArrays(1, &VAOParticles);
	glBindVertexArray(VAOParticles);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void initParticleBuffersFire() {
	// Generate the buffers
	glGenBuffers(2, posBuf);    // position buffers
	glGenBuffers(2, velBuf);    // velocity buffers
	glGenBuffers(2, age);       // age buffers

	// Allocate space for all buffers
	int size = nParticlesFire * sizeof(GLfloat);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);

	// Fill the first age buffer
	std::vector<GLfloat> initialAge(nParticlesFire);
	float rate = particleLifetime / nParticlesFire;
	for(unsigned int i = 0; i < nParticlesFire; i++ ) {
		int index = i - nParticlesFire;
		float result = rate * index;
		initialAge[i] = result;
	}
	// Shuffle them for better looking results
	//Random::shuffle(initialAge);
	auto rng = std::default_random_engine {};
	std::shuffle(initialAge.begin(), initialAge.end(), rng);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, initialAge.data());

	glBindBuffer(GL_ARRAY_BUFFER,0);

	// Create vertex arrays for each set of buffers
	glGenVertexArrays(2, particleArray);

	// Set up particle array 0
	glBindVertexArray(particleArray[0]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Set up particle array 1
	glBindVertexArray(particleArray[1]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// Setup the feedback objects
	glGenTransformFeedbacks(2, feedback);

	// Transform feedback 0
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);

	// Transform feedback 1
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	#pragma region Inicializacion Ventana del programa
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(), glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr, nullptr);

	if (window == nullptr) {
		std::cerr
				<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
				<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	#pragma endregion

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	#pragma region Inicializacion de shaders
	// Inicialización de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox_fog.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation_shadow.vs", "../Shaders/multipleLights_shadow.fs");
	shaderTerrain.initialize("../Shaders/terrain_shadow.vs", "../Shaders/terrain_shadow.fs");
	shaderParticlesFountain.initialize("../Shaders/particlesFountain.vs", "../Shaders/particlesFountain.fs");
	shaderParticlesFire.initialize("../Shaders/particlesSmoke.vs", "../Shaders/particlesSmoke.fs", {"Position", "Velocity", "Age"});
	shaderParticlesFire2.initialize("../Shaders/particlesSmoke.vs", "../Shaders/particlesSmoke.fs", { "Position", "Velocity", "Age" });
	shaderViewDepth.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado_depth_view.fs");
	shaderDepth.initialize("../Shaders/shadow_mapping_depth.vs", "../Shaders/shadow_mapping_depth.fs");
	shaderUI.initialize("../Shaders/imagenUI.vs", "../Shaders/imagenUI.fs");
	#pragma endregion

	#pragma region Shader para imagenes UI
	iconoSalud.shader = shaderUI;
	iconoEnemigo.shader = shaderUI;
	introduccionUI1.shader = shaderUI;
	introduccionUI2.shader = shaderUI;
	introduccionUI3.shader = shaderUI;
	controlesPC.shader = shaderUI;
	controlesMando.shader = shaderUI;
	#pragma endregion	

	#pragma region Inicializacion otros objetos 3D
	// Inicializacion de los objetos.
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(100.0f, 100.0f, 100.0f));

	boxCollider.init();
	boxCollider.setShader(&shader);
	boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	sphereCollider.init();
	sphereCollider.setShader(&shader);
	sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	boxViewDepth.init();
	boxViewDepth.setShader(&shaderViewDepth);

	boxLightViewBox.init();
	boxLightViewBox.setShader(&shaderViewDepth);

	modelAircraft.loadModel("../models/Aircraft/Plane.obj");
	modelAircraft.setShader(&shaderMulLighting);
	#pragma endregion

	#pragma region Inicializacion de casas
	//Casas
	modelCasa.loadModel("../models/AccumulaTownHouse/Accumula Town House.obj");
	modelCasa.setShader(&shaderMulLighting);

	modelCasa2.loadModel("../models/MistraltonCityHouse/MistraltonCityHouse.obj");
	modelCasa2.setShader(&shaderMulLighting);

	modelCasa3.loadModel("../models/PokemonDayCare/PokemonDayCare.obj");
	modelCasa3.setShader(&shaderMulLighting);

	modelCasa4.loadModel("../models/PastoriaCityHouse/PastoriaCityHouse.obj");
	modelCasa4.setShader(&shaderMulLighting);

	modelCasa5.loadModel("../models/CianwoodCityHouse/CianwoodCityHouse.obj");
	modelCasa5.setShader(&shaderMulLighting);

	modelCasa6.loadModel("../models/CelesticTownHouse/CelesticTownHouse.obj");
	modelCasa6.setShader(&shaderMulLighting);

	modelCerca.loadModel("../models/Fence/Fence.obj");
	modelCerca.setShader(&shaderMulLighting);
	#pragma endregion

	terrain.init();
	terrain.setShader(&shaderTerrain);
	terrain.setPosition(glm::vec3(100, 0, 100));

	#pragma region Inicializacion arbol hazmat
	// Hazmat
	modelHazmat.loadModel("../models/AcaciaTree/acaciaTree.obj");
	modelHazmat.setShader(&shaderMulLighting);
	#pragma endregion

	#pragma region Inicializacion postes de luz
	//Lamp models
	modelLamp1.loadModel("../models/RoundStreetlight/lamp.obj"); //Street-Lamp-Black/objLamp.obj");
	modelLamp1.setShader(&shaderMulLighting);

	modelTree.loadModel("../models/AcaciaTree/acaciaTree.obj");
	modelTree.setShader(&shaderMulLighting);
	#pragma endregion

	#pragma region Inicializacion pasto 3D
	//Grass
	modelGrass.loadModel("../models/grass/grassModel.obj");
	modelGrass.setShader(&shaderMulLighting);
	#pragma endregion

	#pragma region Inicializacion fuente de agua
	//Fountain
	modelFountain.loadModel("../models/FountainCollege/fountaincollege_tslocator_gmdc.obj");
	modelFountain.setShader(&shaderMulLighting);
	#pragma endregion

	#pragma region Inicializacion de Jugador
	// Jugador (Bart)
	jugador.cargarModelo("../models/Bart (Bartman)/BartAnim.fbx", &shaderMulLighting);
	#pragma endregion
	
	#pragma region Inicializacion NPCs
	// Gatos
	bobAnimate.cargarModelo("../models/AnimalCrossing/Cats/Bob.fbx", &shaderMulLighting);		// Bob
	mitziAnimate.cargarModelo("../models/AnimalCrossing/Cats/Mitzi.fbx", &shaderMulLighting);	// Mitzi
	rosieAnimate.cargarModelo("../models/AnimalCrossing/Cats/Rosie.fbx", &shaderMulLighting);	// Rosie
	oliviaAnimate.cargarModelo("../models/AnimalCrossing/Cats/Olivia.fbx", &shaderMulLighting);	// Olivia
	kikiAnimate.cargarModelo("../models/AnimalCrossing/Cats/Kiki.fbx", &shaderMulLighting);		// Kiki
	tangyAnimate.cargarModelo("../models/AnimalCrossing/Cats/Tangy.fbx", &shaderMulLighting);	// Tangy
	#pragma endregion

	camera->setPosition(glm::vec3(0.0, 0.0, 10.0));
	camera->setDistanceFromTarget(distanceFromTarget);
	camera->setSensitivity(1.0);

	// Definimos el tamanio de la imagen
	int imageWidth, imageHeight;
	FIBITMAP *bitmap;
	unsigned char *data;

	#pragma region Inicializacion de la skybox
	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		FIBITMAP *bitmap = skyboxTexture.loadImage(true);
		unsigned char *data = skyboxTexture.convertToData(bitmap, imageWidth, imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		} else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage(bitmap);
	}
	#pragma endregion

	#pragma region Textura para el terreno
	// Definiendo la textura a utilizar
	Texture textureTerrainBackground("../Textures/terrenoPasto.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBackground.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBackground.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBackgroundID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBackground.freeImage(bitmap);
	#pragma endregion

	#pragma region Textura terreno canal R
	// Definiendo la textura a utilizar
	Texture textureTerrainR("../Textures/piedras.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainR.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainR.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainRID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainR.freeImage(bitmap);
	#pragma endregion

	#pragma region Textura terreno canal G
	// Definiendo la textura a utilizar
	Texture textureTerrainG("../Textures/floresAmarillas.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainG.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainG.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainGID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainG.freeImage(bitmap);
	#pragma endregion

	#pragma region Textura terreno canal B
	// Definiendo la textura a utilizar
	Texture textureTerrainB("../Textures/camino.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainB.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainB.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainB.freeImage(bitmap);
	#pragma endregion

	#pragma region Textura blendmap
	// Definiendo la textura a utilizar
	Texture textureTerrainBlendMap("../Textures/blendMap1.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBlendMap.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBlendMap.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBlendMapID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBlendMap.freeImage(bitmap);
	#pragma endregion

	#pragma region Textura particulas de agua
	Texture textureParticlesFountain("../Textures/bluewaterF.png");
	bitmap = textureParticlesFountain.loadImage();
	data = textureParticlesFountain.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureParticleFountainID);
	glBindTexture(GL_TEXTURE_2D, textureParticleFountainID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureParticlesFountain.freeImage(bitmap);
	#pragma endregion

	#pragma region Textura particulas de fuego
	Texture textureParticleFire("../Textures/smoke.png");
	bitmap = textureParticleFire.loadImage();
	data = textureParticleFire.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureParticleFireID);
	glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureParticleFire.freeImage(bitmap);
	#pragma endregion

	#pragma region Sistema perticulas retroalimentadas
	std::uniform_real_distribution<float> distr01 = std::uniform_real_distribution<float>(0.0f, 1.0f);
	std::mt19937 generator;
	std::random_device rd;
	generator.seed(rd());
	int size = nParticlesFire * 2;
	std::vector<GLfloat> randData(size);
	for( int i = 0; i < randData.size(); i++ ) {
		randData[i] = distr01(generator);
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_1D, texId);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, size);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, size, GL_RED, GL_FLOAT, randData.data());
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	shaderParticlesFire.setInt("Pass", 1);
	shaderParticlesFire.setInt("ParticleTex", 0);
	shaderParticlesFire.setInt("RandomTex", 1);
	shaderParticlesFire.setFloat("ParticleLifetime", particleLifetime);
	shaderParticlesFire.setFloat("ParticleSize", particleSize);
	shaderParticlesFire.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0.0f,0.01f,0.0f)));
	shaderParticlesFire.setVectorFloat3("Emitter", glm::value_ptr(glm::vec3(0.0f)));

	shaderParticlesFire2.setInt("Pass", 1);
	shaderParticlesFire2.setInt("ParticleTex", 0);
	shaderParticlesFire2.setInt("RandomTex", 1);
	shaderParticlesFire2.setFloat("ParticleLifetime", particleLifetime);
	shaderParticlesFire2.setFloat("ParticleSize", particleSize);
	shaderParticlesFire2.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0.0f, 0.01f, 0.0f)));
	shaderParticlesFire2.setVectorFloat3("Emitter", glm::value_ptr(glm::vec3(0.0f)));

	glm::mat3 basis;
	glm::vec3 u, v, n;
	v = glm::vec3(0,1,0);
	n = glm::cross(glm::vec3(1,0,0), v);
	if( glm::length(n) < 0.00001f ) {
		n = glm::cross(glm::vec3(0,1,0), v);
	}
	u = glm::cross(v,n);
	basis[0] = glm::normalize(u);
	basis[1] = glm::normalize(v);
	basis[2] = glm::normalize(n);
	shaderParticlesFire.setMatrix3("EmitterBasis", 1, false, glm::value_ptr(basis));
	shaderParticlesFire2.setMatrix3("EmitterBasis", 1, false, glm::value_ptr(basis));
	#pragma endregion

	#pragma region Inicializacion de imagenes UI
	iconoSalud.inicializar("../Textures/UI/Salud.png", imageWidth, imageHeight, bitmap, data);
	iconoEnemigo.inicializar("../Textures/UI/Enemigo.png", imageWidth, imageHeight, bitmap, data);
	introduccionUI1.inicializar("../Textures/UI/Introduccion1.png", imageWidth, imageHeight, bitmap, data);
	introduccionUI2.inicializar("../Textures/UI/Introduccion2.png", imageWidth, imageHeight, bitmap, data);
	introduccionUI3.inicializar("../Textures/UI/Introduccion3.png", imageWidth, imageHeight, bitmap, data);
	controlesPC.inicializar("../Textures/UI/Controles PC.png", imageWidth, imageHeight, bitmap, data);
	controlesMando.inicializar("../Textures/UI/Controles Mando.png", imageWidth, imageHeight, bitmap, data);
	#pragma endregion

	/*******************************************
	 * Inicializacion de los buffers de la fuente
	 *******************************************/
	initParticleBuffers();

	/*******************************************
	 * Inicializacion de los buffers del fuego
	 *******************************************/
	initParticleBuffersFire();

	/*******************************************
	 * Inicializacion del framebuffer para
	 * almacenar el buffer de profunidadad
	 *******************************************/
	#pragma region Configuracion de sombras
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	#pragma endregion

	/*******************************************
	 * Inicialización de Texto en pantalla
	 *******************************************/
	modelText = new FontTypeRendering::FontTypeRendering(screenWidth, screenHeight);
	modelText->Initialize();

	/*******************************************
	 * OpenAL init
	 *******************************************/
	#pragma region Inicializacion de audios
	alutInit(0, nullptr);
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	alGetError(); // clear any error messages
	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else {
		printf("init() - No errors yet.");
	}
	// Config source 0
	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);
	buffer[0] = alutCreateBufferFromFile("../sounds/Fuente de Agua.wav");
	buffer[1] = alutCreateBufferFromFile("../sounds/Fuego de Chimenea.wav");
	buffer[2] = alutCreateBufferFromFile("../sounds/Fuego de Chimenea.wav");
	buffer[3] = alutCreateBufferFromFile("../sounds/Animal Crossing Main Theme.wav");
	buffer[4] = alutCreateBufferFromFile("../sounds/Disparo Laser.wav");
	buffer[5] = alutCreateBufferFromFile("../sounds/Disparo Acertado.wav");
	buffer[6] = alutCreateBufferFromFile("../sounds/NPC Muerte.wav");
	buffer[7] = alutCreateBufferFromFile("../sounds/NPC Golpe.wav");
	int errorAlut = alutGetError();
	if (errorAlut != ALUT_ERROR_NO_ERROR){
		printf("- Error open files with alut %d !!\n", errorAlut);
		exit(2);
	}

	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else {
		printf("init - no errors after alGenSources\n");
	}
	// Fuente de Agua
	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 1.5f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_TRUE);
	alSourcef(source[0], AL_MAX_DISTANCE, 2000);

	// Fuego de Chimenea
	alSourcef(source[1], AL_PITCH, 1.0f);
	alSourcef(source[1], AL_GAIN, 2.0f);
	alSourcefv(source[1], AL_POSITION, source1Pos);
	alSourcefv(source[1], AL_VELOCITY, source1Vel);
	alSourcei(source[1], AL_BUFFER, buffer[1]);
	alSourcei(source[1], AL_LOOPING, AL_TRUE);
	alSourcef(source[1], AL_MAX_DISTANCE, 2000);

	// Fuego de Chimenea
	alSourcef(source[2], AL_PITCH, 1.0f);
	alSourcef(source[2], AL_GAIN, 2.0f);
	alSourcefv(source[2], AL_POSITION, source2Pos);
	alSourcefv(source[2], AL_VELOCITY, source2Vel);
	alSourcei(source[2], AL_BUFFER, buffer[2]);
	alSourcei(source[2], AL_LOOPING, AL_TRUE);
	alSourcef(source[2], AL_MAX_DISTANCE, 500);

	// Música
	alSourcef(source[3], AL_PITCH, 1.0f);
	alSourcef(source[3], AL_GAIN, 0.6f);
	alSourcei(source[3], AL_SOURCE_RELATIVE, AL_TRUE);	// Audio 2D
	alSourcei(source[3], AL_BUFFER, buffer[3]);
	alSourcei(source[3], AL_LOOPING, AL_TRUE);

	// Disparo láser
	alSourcef(source[4], AL_PITCH, 1.0f);
	alSourcef(source[4], AL_GAIN, 1.0f);
	alSourcefv(source[4], AL_POSITION, source4Pos);
	alSourcefv(source[4], AL_VELOCITY, source4Vel);
	alSourcei(source[4], AL_BUFFER, buffer[4]);
	alSourcei(source[4], AL_LOOPING, AL_FALSE);		// Se reproduce una sola vez
	alSourcef(source[4], AL_MAX_DISTANCE, 500);

	// Disparo acertado
	alSourcef(source[5], AL_PITCH, 1.0f);
	alSourcef(source[5], AL_GAIN, 0.6f);
	alSourcei(source[5], AL_SOURCE_RELATIVE, AL_TRUE);	// Audio 2D
	alSourcei(source[5], AL_BUFFER, buffer[5]);
	alSourcei(source[5], AL_LOOPING, AL_FALSE);		// Se reproduce una sola vez

	// Muerte NPC
	alSourcef(source[6], AL_PITCH, 1.0f);
	alSourcef(source[6], AL_GAIN, 1.0f);
	alSourcefv(source[6], AL_POSITION, source6Pos);
	alSourcefv(source[6], AL_VELOCITY, source6Vel);
	alSourcei(source[6], AL_BUFFER, buffer[6]);
	alSourcei(source[6], AL_LOOPING, AL_FALSE);		// Se reproduce una sola vez
	alSourcef(source[6], AL_MAX_DISTANCE, 500);

	// NPC Golpe
	alSourcef(source[7], AL_PITCH, 1.0f);
	alSourcef(source[7], AL_GAIN, 0.6f);
	alSourcei(source[7], AL_SOURCE_RELATIVE, AL_TRUE);	// Audio 2D
	alSourcei(source[7], AL_BUFFER, buffer[7]);
	alSourcei(source[7], AL_LOOPING, AL_FALSE);		// Se reproduce una sola vez
	#pragma endregion
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Shaders Delete
	shader.destroy();
	shaderMulLighting.destroy();
	shaderSkybox.destroy();
	shaderTerrain.destroy();
	shaderParticlesFountain.destroy();
	shaderParticlesFire.destroy();
	shaderParticlesFire2.destroy();
	// Basic objects Delete
	skyboxSphere.destroy();
	boxCollider.destroy();
	sphereCollider.destroy();
	boxViewDepth.destroy();
	boxLightViewBox.destroy();

	// Terrains objects Delete
	terrain.destroy();

	// Custom objects Delete
	modelAircraft.destroy();

	//Casas
	modelCasa.destroy();
	modelCasa2.destroy();
	modelCasa3.destroy();
	modelCasa4.destroy();
	modelCasa5.destroy();
	modelCasa6.destroy();
	//Cerca
	modelCerca.destroy();

	modelLamp1.destroy();
	modelTree.destroy();
	modelGrass.destroy();
	modelFountain.destroy();
	
	modelHazmat.destroy();			// hazmat

	// Custom objects animate
	jugador.destroy();				// Bart
	bobAnimate.destroy();
	mitziAnimate.destroy();
	rosieAnimate.destroy();
	oliviaAnimate.destroy();
	kikiAnimate.destroy();
	tangyAnimate.destroy();
	billyAnimate.destroy();
	nanAnimate.destroy();
	chevreAnimate.destroy();
	gruffAnimate.destroy();
	velmaAnimate.destroy();
	budAnimate.destroy();
	octavianAnimate.destroy();
	chiefAnimate.destroy();
	cjAnimate.destroy();

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureCespedID);
	glDeleteTextures(1, &textureWallID);
	glDeleteTextures(1, &textureWindowID);
	glDeleteTextures(1, &textureHighwayID);
	glDeleteTextures(1, &textureLandingPadID);
	glDeleteTextures(1, &textureTerrainBackgroundID);
	glDeleteTextures(1, &textureTerrainRID);
	glDeleteTextures(1, &textureTerrainGID);
	glDeleteTextures(1, &textureTerrainBID);
	glDeleteTextures(1, &textureTerrainBlendMapID);
	glDeleteTextures(1, &textureParticleFountainID);
	glDeleteTextures(1, &textureParticleFireID);

	// Elimina imágenes UI
	iconoSalud.destroy();
	iconoEnemigo.destroy();
	introduccionUI1.destroy();
	introduccionUI2.destroy();
	introduccionUI3.destroy();
	controlesPC.destroy();
	controlesMando.destroy();

	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);

	// Remove the buffer of the fountain particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &initVel);
	glDeleteBuffers(1, &startTime);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticles);

	// Remove the buffer of the fire particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, posBuf);
	glDeleteBuffers(2, velBuf);
	glDeleteBuffers(2, age);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glDeleteTransformFeedbacks(2, feedback);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticlesFire);
}

void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
	if (estadoPrograma != 1) {
		distanceFromTarget -= yoffset;

		if (distanceFromTarget <= 3.5f) {
			distanceFromTarget = 3.5f;
		}
		else if (distanceFromTarget >= 10.0f) {
			distanceFromTarget = 10.0f;
		}

		camera->setDistanceFromTarget(distanceFromTarget);
	}
}

void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			break;

		case GLFW_MOUSE_BUTTON_LEFT:
			if (!jugador.disparando && estadoPrograma == 2) {
				jugador.disparar(camera, source[4], source4Pos);
			}
			break;

		case GLFW_MOUSE_BUTTON_MIDDLE:
			break;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}
	
	#pragma region Controles Mando Xbox One
	// Control de Xbox One
	if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GL_TRUE) {
		int axesCount, buttonCount;

		const float * axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		const unsigned char * buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

		// Acercar cámara (Botón RB)
		if (buttons[5] == GLFW_PRESS) {
			distanceFromTarget -= 0.3f;
		}
		// Alejar cámara (Botón LB)
		else if (buttons[4] == GLFW_PRESS) {
			distanceFromTarget += 0.3f;
		}

		if (distanceFromTarget <= 3.5f) {
			distanceFromTarget = 3.5f;
		}
		else if (distanceFromTarget >= 10.0f) {
			distanceFromTarget = 10.0f;
		}
		camera->setDistanceFromTarget(distanceFromTarget);

		if (estadoPrograma != 1) {
			// Movimiento de la cámara
			if (fabs(axes[2]) > 0.2f) {
				camera->mouseMoveCamera(2.5f * axes[2], 0.0, deltaTime);
			}
			if (fabs(axes[3]) > 0.2f) {
				camera->mouseMoveCamera(0.0, -2.5f * axes[3], deltaTime);
			}
		}

		switch (estadoPrograma) {
		// Pantalla de inicio
		case 0:
			if (buttons[0] == GLFW_PRESS && !botonApresionado) {
				botonApresionado = true;

				// Aquellos NPC adicionales se cargan una sola vez al iniciar la primer partida
				if (!npcExtrasCargados) {
					billyAnimate.cargarModelo("../models/AnimalCrossing/Goats/Billy.fbx", &shaderMulLighting);		// Billy
					nanAnimate.cargarModelo("../models/AnimalCrossing/Goats/Nan.fbx", &shaderMulLighting);			// Nan
					chevreAnimate.cargarModelo("../models/AnimalCrossing/Goats/Chevre.fbx", &shaderMulLighting);	// Chevre
					gruffAnimate.cargarModelo("../models/AnimalCrossing/Goats/Gruff.fbx", &shaderMulLighting);		// Gruff
					velmaAnimate.cargarModelo("../models/AnimalCrossing/Goats/Velma.fbx", &shaderMulLighting);		// Velma

					budAnimate.cargarModelo("../models/AnimalCrossing/Lions/Bud.fbx", &shaderMulLighting);					// Bud
					octavianAnimate.cargarModelo("../models/AnimalCrossing/Octopuses/Octavian.fbx", &shaderMulLighting);	// Octavian
					chiefAnimate.cargarModelo("../models/AnimalCrossing/Wolves/Chief.fbx", &shaderMulLighting);				// Chief
					cjAnimate.cargarModelo("../models/AnimalCrossing/C.J/CJAnimacion.fbx", &shaderMulLighting);				// CJ

					npcExtrasCargados = true;
				}

				estadoPrograma = 1;
			}
			else if (buttons[0] == GLFW_RELEASE) {
				botonApresionado = false;
			}
			break;

		// Pantalla de introducción
		case 1:
			if (buttons[0] == GLFW_PRESS && !botonApresionado) {
				botonApresionado = true;
				paginaIntroduccion++;

				if (paginaIntroduccion > 6) {
					paginaIntroduccion = 0;
					estadoPrograma = 2;
				}
			}
			else if (buttons[0] == GLFW_RELEASE) {
				botonApresionado = false;
			}
			break;

		// Partida (Gameplay)
		case 2:
			// Movimiento del jugador
			if (fabs(axes[1]) > 0.2f) {
				jugador.moverZ(camera, axes[1] * 0.22f);
			}
			if (fabs(axes[0]) > 0.2f) {
				jugador.moverX(camera, axes[0] * -0.22f);
			}

			// Disparar (Gatillo derecho)
			if (axes[5] > 0.4f && !jugador.disparando) {
				jugador.disparar(camera, source[4], source4Pos);
			}
			break;

		// Fin del juego (muere el jugador)
		case 3:
			// Se vuelve a la pantalla de inicio
			if (buttons[0] == GLFW_PRESS) {
				reiniciarPartida();
			}
			break;

		// Fin del juego (el jugador gana la partida)
		case 4:
			// Se vuelve a la pantalla de inicio
			if (buttons[0] == GLFW_PRESS) {
				reiniciarPartida();
			}
			break;

		default:
			break;
		}

	}
	#pragma endregion

	#pragma region Controles Teclado
	if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GL_FALSE) {

		// Mueve la cámara sin necesidad de presionar los botones
		if (estadoPrograma != 1) {
			camera->mouseMoveCamera(0.5f * offsetX, 0.0, deltaTime);
			camera->mouseMoveCamera(0.0, 0.5f * offsetY, deltaTime);
		}
		offsetX = 0; offsetY = 0;

		switch (estadoPrograma) {
		// Pantalla de inicio
		case 0:
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !botonApresionado) {
				botonApresionado = true;

				// Aquellos NPC adicionales se cargan una sola vez al iniciar la primer partida
				if (!npcExtrasCargados) {
					billyAnimate.cargarModelo("../models/AnimalCrossing/Goats/Billy.fbx", &shaderMulLighting);		// Billy
					nanAnimate.cargarModelo("../models/AnimalCrossing/Goats/Nan.fbx", &shaderMulLighting);			// Nan
					chevreAnimate.cargarModelo("../models/AnimalCrossing/Goats/Chevre.fbx", &shaderMulLighting);	// Chevre
					gruffAnimate.cargarModelo("../models/AnimalCrossing/Goats/Gruff.fbx", &shaderMulLighting);		// Gruff
					velmaAnimate.cargarModelo("../models/AnimalCrossing/Goats/Velma.fbx", &shaderMulLighting);		// Velma

					budAnimate.cargarModelo("../models/AnimalCrossing/Lions/Bud.fbx", &shaderMulLighting);					// Bud
					octavianAnimate.cargarModelo("../models/AnimalCrossing/Octopuses/Octavian.fbx", &shaderMulLighting);	// Octavian
					chiefAnimate.cargarModelo("../models/AnimalCrossing/Wolves/Chief.fbx", &shaderMulLighting);				// Chief
					cjAnimate.cargarModelo("../models/AnimalCrossing/C.J/CJAnimacion.fbx", &shaderMulLighting);				// CJ

					npcExtrasCargados = true;
				}

				estadoPrograma = 1;
			}
			else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
				botonApresionado = false;
			}
			break;

		// Pantalla de introducción
		case 1:
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !botonApresionado) {
				botonApresionado = true;
				paginaIntroduccion++;

				if (paginaIntroduccion > 6) {
					paginaIntroduccion = 0;
					estadoPrograma = 2;
				}
			}
			else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
				botonApresionado = false;
			}
			break;

		// Partida (Gameplay)
		case 2:
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
				jugador.moverX(camera, 0.2f);
			}
			else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
				jugador.moverX(camera, -0.2f);
			}

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
				jugador.moverZ(camera, 0.2f);
			}
			else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
				jugador.moverZ(camera, -0.2f);
			}
			break;

		// Fin del juego (muere el jugador)
		case 3:
			// Se vuelve a la pantalla de inicio
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
				reiniciarPartida();
			}
			break;

		// Fin del juego (el jugador gana la partida)
		case 4:
			// Se vuelve a la pantalla de inicio
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
				reiniciarPartida();
			}
			break;

		default:
			break;
		}
	}
	
	#pragma endregion

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	glm::mat4 view;
	glm::vec3 axis;
	glm::vec3 target;
	float angleTarget;

	#pragma region Transformaciones iniciales modelos 3D
	modelMatrixAircraft = glm::translate(modelMatrixAircraft, glm::vec3(10.0, 2.0, -17.5));
	modelMatrixAircraft = glm::scale(modelMatrixAircraft, glm::vec3(0.008, 0.008, 0.008));

	//Casas
	modelMatrixCasa = glm::translate(modelMatrixCasa, glm::vec3(56.25, 0.0, -37.89));
	modelMatrixCasa = glm::scale(modelMatrixCasa, glm::vec3(0.1f, 0.1f, 0.1f));

	modelMatrixCasa2 = glm::translate(modelMatrixCasa2, glm::vec3(-1.56, 0.0, -50.39));
	modelMatrixCasa2 = glm::scale(modelMatrixCasa2, glm::vec3(0.1f, 0.1f, 0.1f));

	modelMatrixCasa3 = glm::translate(modelMatrixCasa3, glm::vec3(57.42, 0.0, 21.09));
	modelMatrixCasa3 = glm::scale(modelMatrixCasa3, glm::vec3(0.1f, 0.1f, 0.1f));

	modelMatrixCasa4 = glm::translate(modelMatrixCasa4, glm::vec3(-1.95, 0.0, 46.87));
	modelMatrixCasa4 = glm::scale(modelMatrixCasa4, glm::vec3(0.1f, 0.1f, 0.1f));

	modelMatrixCasa5 = glm::translate(modelMatrixCasa5, glm::vec3(-60.93, 0.0, 19.53));
	modelMatrixCasa5 = glm::scale(modelMatrixCasa5, glm::vec3(0.1f, 0.1f, 0.1f));

	modelMatrixCasa6 = glm::translate(modelMatrixCasa6, glm::vec3(-60.93, 0.0, -38.28));
	modelMatrixCasa6 = glm::scale(modelMatrixCasa6, glm::vec3(0.1f, 0.1f, 0.1f));

	glm::mat4 tempCerca = modelMatrixCerca;
	tempCerca = glm::scale(modelMatrixCerca, glm::vec3(2.5f, 1.0f, 1.0f));
	modelMatrixCerca = glm::translate(tempCerca, glm::vec3(0.0, 0.0, -75.0));

	modelMatrixCerca2 = glm::translate(tempCerca, glm::vec3(0.0, 0.0, 70.0));
	
	modelMatrixCerca3 = glm::translate(tempCerca, glm::vec3(34.0, 0.0, 1.0));
	modelMatrixCerca3 = glm::rotate(modelMatrixCerca3, glm::radians(90.0f), glm::vec3(0.0f,1.0f,0.0f));
	modelMatrixCerca3 = glm::scale(modelMatrixCerca3, glm::vec3(2.0f, 1.0f, 1.0f));

	modelMatrixCerca4 = glm::translate(tempCerca, glm::vec3(-34.0, 0.0, 1.0));
	modelMatrixCerca4 = glm::rotate(modelMatrixCerca4, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrixCerca4 = glm::scale(modelMatrixCerca4, glm::vec3(2.0f, 1.0f, 1.0f));

	// Fuente de agua
	modelMatrixFountain = glm::translate(modelMatrixFountain, glm::vec3(-0.6, 0.0, -4.5));
	modelMatrixFountain[3][1] = terrain.getHeightTerrain(modelMatrixFountain[3][0] , modelMatrixFountain[3][2]); //+ 0.2
	modelMatrixFountain = glm::scale(modelMatrixFountain, glm::vec3(5.0f, 5.0f, 5.0f));

	// Hazmat
	modelMatrixHazmat = glm::translate(modelMatrixHazmat, glm::vec3(0.0, 0.0, 0.0));
	modelMatrixHazmat = glm::scale(modelMatrixFountain, glm::vec3(0.009f, 0.009f, 0.009f));

	// Bart (jugador)
	jugador.start("Bart", glm::vec3(13.0f, 0.05f, -9.0f), -90.0f);

	// NPCs (enemigos)
	bobAnimate.start("NPC-Bob", glm::vec3(0, 0, 20), 15.0f, true);
	mitziAnimate.start("NPC-Mitzi", glm::vec3(13.5, 0, 40), 12.0f, true);
	rosieAnimate.start("NPC-Rosie", glm::vec3(-8, 0, 10), 320.0f, true);
	oliviaAnimate.start("NPC-Olivia", glm::vec3(-25, 0, 20), 220.0f, true);
	kikiAnimate.start("NPC-Kiki", glm::vec3(-25, 0, -20), 135.0f, true);
	tangyAnimate.start("NPC-Tangy", glm::vec3(70, 0, 50), 85.0f, true);

	billyAnimate.start("NPC-Billy", glm::vec3(13, 0, 15), 170.0f, false);
	nanAnimate.start("NPC-Nan", glm::vec3(-13, 0, 5), 35.0f, false);
	chevreAnimate.start("NPC-Chevre", glm::vec3(-70, 0, 0), 10.0f, false);
	gruffAnimate.start("NPC-Gruff", glm::vec3(-50, 0, -35), 340.0f, false);
	velmaAnimate.start("NPC-Velma", glm::vec3(-40, 0, -50), 0.0f, false);

	budAnimate.start("NPC-Bud", glm::vec3(80, 0, -30), 0.0f, false);
	octavianAnimate.start("NPC-Octavian", glm::vec3(50, 0, 65), 120.0f, false);
	chiefAnimate.start("NPC-Chief", glm::vec3(-80, 0, 10), 95.0f, false);
	cjAnimate.start("NPC-CJ", glm::vec3(-80, 0, 35), 180.0f, false);
	#pragma endregion

	lastTime = TimeManager::Instance().GetTime();

	// Time for the particles animation
	currTimeParticlesAnimation = lastTime;
	lastTimeParticlesAnimation = lastTime;
	currTimeParticlesAnimationFire = lastTime;
	lastTimeParticlesAnimationFire = lastTime;

	glm::vec3 lightPos = glm::vec3(10.0, 10.0, 0.0);
	shadowBox = new ShadowBox(-lightPos, camera.get(), 15.0f, 0.1f, 45.0f);

	while (psi) {
		currTime = TimeManager::Instance().GetTime();
		if(currTime - lastTime < 0.016666667){
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;
		psi = processInput(true);

		std::map<std::string, bool> collisionDetection;
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) screenWidth / (float) screenHeight, 0.1f, 100.0f);

		#pragma region Camara Tercera Persona
		axis = glm::axis(glm::quat_cast(jugador.modelMatrixJugador));
		angleTarget = glm::angle(glm::quat_cast(jugador.modelMatrixJugador));
		target = jugador.modelMatrixJugador[3] + glm::vec4(0, 2.2f, 0, 0);

		if(std::isnan(angleTarget))
			angleTarget = 0.0;
		if (axis.y < 0)
			angleTarget = -angleTarget + glm::radians(360.0f);
		camera->setCameraTarget(target);
		camera->setAngleTarget(angleTarget);
		camera->updateCamera();
		view = camera->getViewMatrix();
		#pragma endregion

		#pragma region Optimizacion de sombras
		// Matriz de proyección del shadow mapping
		glm::mat4 lightProjection = glm::mat4(1.0f), lightView = glm::mat4(1.0f);
		shadowBox->update(screenWidth, screenHeight);
		glm::vec3 centerBox = shadowBox->getCenter();
		glm::mat4 lightSpaceMatrix;
		lightView = glm::lookAt(centerBox, centerBox + glm::normalize(-lightPos), glm::vec3(0.0, 1.0, 0.0));
		lightProjection[0][0] = 2.0f / shadowBox->getWidth();
		lightProjection[1][1] = 2.0f / shadowBox->getHeight();
		lightProjection[2][2] = -2.0f / shadowBox->getLength();
		lightProjection[3][3] = 1.0f;
		lightSpaceMatrix = lightProjection * lightView;
		shaderDepth.setMatrix4("lightSpaceMatrix", 1, false, glm::value_ptr(lightSpaceMatrix));
		#pragma endregion

		#pragma region Asignacion de shaders
		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));
		// Settea la matriz de vista y projection al shader con skybox
		shaderSkybox.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(glm::mat3(view))));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderMulLighting.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false, glm::value_ptr(view));
		shaderMulLighting.setMatrix4("lightSpaceMatrix", 1, false, glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderTerrain.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderTerrain.setMatrix4("view", 1, false, glm::value_ptr(view));
		shaderTerrain.setMatrix4("lightSpaceMatrix", 1, false, glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader para el fountain
		shaderParticlesFountain.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderParticlesFountain.setMatrix4("view", 1, false, glm::value_ptr(view));
		// Settea la matriz de vista y projection al shader para el fuego
		shaderParticlesFire.setInt("Pass", 2);
		shaderParticlesFire.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderParticlesFire.setMatrix4("view", 1, false, glm::value_ptr(view));

		shaderParticlesFire2.setInt("Pass", 2);
		shaderParticlesFire2.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderParticlesFire2.setMatrix4("view", 1, false, glm::value_ptr(view));
		#pragma endregion

		#pragma region Renderizado de la neblina
		/*******************************************
		 * Propiedades de neblina
		 *******************************************/
		shaderMulLighting.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));
		shaderTerrain.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));
		shaderSkybox.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));
		#pragma endregion

		#pragma region Renderizado luz direccional
		/*******************************************
		 * Propiedades Luz direccional
		 *******************************************/
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.80, 0.35, 0.06)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.00, 0.29, 0.0)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.8, 0.8, 0.8)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		 * Propiedades Luz direccional Terrain
		 *******************************************/
		shaderTerrain.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderTerrain.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.80, 0.35, 0.06)));
		shaderTerrain.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.00, 0.29, 0.0)));
		shaderTerrain.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.8, 0.8, 0.8)));
		shaderTerrain.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));
		#pragma endregion

		#pragma region Renderizado luces spotlight
		/*******************************************
		 * Propiedades SpotLights
		 *******************************************/
		glm::vec3 spotPosition = glm::vec3( glm::vec4(0.32437, 0.226053, 1.79149, 1.0));
		shaderMulLighting.setInt("spotLightCount", 1);
		shaderTerrain.setInt("spotLightCount", 1);
		shaderMulLighting.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.2, 0.3, 0.2)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderMulLighting.setVectorFloat3("spotLights[0].position", glm::value_ptr(spotPosition));
		shaderMulLighting.setVectorFloat3("spotLights[0].direction", glm::value_ptr(glm::vec3(0, -1, 0)));
		shaderMulLighting.setFloat("spotLights[0].constant", 1.0);
		shaderMulLighting.setFloat("spotLights[0].linear", 0.074);
		shaderMulLighting.setFloat("spotLights[0].quadratic", 0.03);
		shaderMulLighting.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5f)));
		shaderMulLighting.setFloat("spotLights[0].outerCutOff", cos(glm::radians(15.0f)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.2, 0.3, 0.2)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderTerrain.setVectorFloat3("spotLights[0].position", glm::value_ptr(spotPosition));
		shaderTerrain.setVectorFloat3("spotLights[0].direction", glm::value_ptr(glm::vec3(0, -1, 0)));
		shaderTerrain.setFloat("spotLights[0].constant", 1.0);
		shaderTerrain.setFloat("spotLights[0].linear", 0.074);
		shaderTerrain.setFloat("spotLights[0].quadratic", 0.03);
		shaderTerrain.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5f)));
		shaderTerrain.setFloat("spotLights[0].outerCutOff", cos(glm::radians(15.0f)));
		#pragma endregion

		#pragma region Renderizado luces pointlight
		/*******************************************
		 * Propiedades PointLights
		 *******************************************/
		shaderMulLighting.setInt("pointLightCount", lamp1Position.size()); //+lamp2Orientation.size());
		shaderTerrain.setInt("pointLightCount", lamp1Position.size()); //+lamp2Orientation.size());
		for (int i = 0; i < lamp1Position.size(); i++){
			glm::mat4 matrixAdjustLamp = glm::mat4(1.0f);
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, lamp1Position[i]);
			matrixAdjustLamp = glm::rotate(matrixAdjustLamp, glm::radians(lamp1Orientation[i]), glm::vec3(0, 1, 0));
			matrixAdjustLamp = glm::scale(matrixAdjustLamp, glm::vec3(0.5, 0.5, 0.5));
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, glm::vec3(0, 10.3585, 0));
			glm::vec3 lampPosition = glm::vec3(matrixAdjustLamp[3]);
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.0, 1.0, 1.0)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(lampPosition));
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0);
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09);
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.01);
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.0, 1.0, 1.0)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(lampPosition));
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0);
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09);
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.02);
		}
		#pragma endregion

		#pragma region Renderizado de sombras
		/*******************************************
		 * 1.- We render the depth buffer
		 *******************************************/
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// render scene from light's point of view
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glCullFace(GL_FRONT);
		prepareDepthScene();
		renderScene(false);
		//glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/*******************************************
		 * 2.- We render the normal objects
		 *******************************************/
		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		prepareScene();
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		shaderMulLighting.setInt("shadowMap", 10);
		shaderTerrain.setInt("shadowMap", 10);
		#pragma endregion

		#pragma region Renderizado de la skybox
		/*******************************************
		 * Skybox
		 *******************************************/
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderSkybox.setFloat("skybox", 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0);
		skyboxSphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
		#pragma endregion

		renderScene();		// ¡¡ Renderizado del escenario !!

		/*******************************************
		 * Creacion de colliders
		 * IMPORTANT do this before interpolations
		 *******************************************/
		#pragma region Colisionadores NPCs
		bobAnimate.crearColisionador(collidersOBB);
		mitziAnimate.crearColisionador(collidersOBB);
		rosieAnimate.crearColisionador(collidersOBB);
		oliviaAnimate.crearColisionador(collidersOBB);
		kikiAnimate.crearColisionador(collidersOBB);
		tangyAnimate.crearColisionador(collidersOBB);

		billyAnimate.crearColisionador(collidersOBB);
		nanAnimate.crearColisionador(collidersOBB);
		chevreAnimate.crearColisionador(collidersOBB);
		gruffAnimate.crearColisionador(collidersOBB);
		velmaAnimate.crearColisionador(collidersOBB);

		budAnimate.crearColisionador(collidersOBB);
		octavianAnimate.crearColisionador(collidersOBB);
		chiefAnimate.crearColisionador(collidersOBB);
		cjAnimate.crearColisionador(collidersOBB);
		#pragma endregion

		#pragma region Colisionador Fuente
		// Collider de fuente
		glm::mat4 modelMatrixColliderFountain =  glm::mat4(modelMatrixFountain);
		AbstractModel::OBB fountaintCollider;
		// Set the orientation of collider before doing the scale
		fountaintCollider.u = glm::quat_cast(modelMatrixFountain);
		modelMatrixColliderFountain = glm::scale(modelMatrixColliderFountain, glm::vec3(4.8, 7.0, 4.9));
		modelMatrixColliderFountain = glm::translate(modelMatrixColliderFountain, glm::vec3(-0.1, 0.0, 0.10));
		fountaintCollider.c = glm::vec3(modelMatrixColliderFountain[3]);
		fountaintCollider.e = modelFountain.getObb().e * glm::vec3(4.8, 7.0, 4.9);
		addOrUpdateColliders(collidersOBB, "fuente", fountaintCollider, modelMatrixFountain);

		#pragma endregion

		#pragma region Colisionador nave espacial
		// Collider del aricraft
		glm::mat4 modelMatrixColliderAircraft = glm::mat4(modelMatrixAircraft);
		AbstractModel::OBB aircraftCollider;
		// Set the orientation of collider before doing the scale
		aircraftCollider.u = glm::quat_cast(modelMatrixAircraft);
		modelMatrixColliderAircraft = glm::scale(modelMatrixColliderAircraft,glm::vec3(0.008, 0.008, 0.008));
		modelMatrixColliderAircraft = glm::translate(modelMatrixColliderAircraft, modelAircraft.getObb().c);
		aircraftCollider.c = glm::vec3(modelMatrixColliderAircraft[3]);
		aircraftCollider.e = modelAircraft.getObb().e * glm::vec3(0.008, 0.008, 0.008);
		addOrUpdateColliders(collidersOBB, "aircraft", aircraftCollider, modelMatrixAircraft);
		#pragma endregion

		#pragma region Colisionadores casas
		//Casas
		glm::mat4 modelMatrixColliderCasa = glm::mat4(modelMatrixCasa);
		AbstractModel::OBB casaCollider;
		// Set the orientation of collider before doing the scale
		casaCollider.u = glm::quat_cast(modelMatrixCasa);
		modelMatrixColliderCasa = glm::scale(modelMatrixColliderCasa, glm::vec3(0.085, 0.1, 0.085));
		modelMatrixColliderCasa = glm::translate(modelMatrixColliderCasa, modelCasa.getObb().c);
		modelMatrixColliderCasa = glm::rotate(modelMatrixColliderCasa, glm::radians(-140.0f), glm::vec3(0, 1, 0));
		casaCollider.c = glm::vec3(modelMatrixColliderCasa[3]);
		casaCollider.e = modelCasa.getObb().e * glm::vec3(0.085, 0.1, 0.085);
		addOrUpdateColliders(collidersOBB, "casa", casaCollider, modelMatrixCasa);

		glm::mat4 modelMatrixColliderCasa2 = glm::mat4(modelMatrixCasa2);
		AbstractModel::OBB casaCollider2;
		// Set the orientation of collider before doing the scale
		casaCollider2.u = glm::quat_cast(modelMatrixCasa2);
		modelMatrixColliderCasa2 = glm::scale(modelMatrixColliderCasa2, glm::vec3(0.085, 0.1, 0.085));
		modelMatrixColliderCasa2 = glm::translate(modelMatrixColliderCasa2, modelCasa2.getObb().c);
		casaCollider2.c = glm::vec3(modelMatrixColliderCasa2[3]);
		casaCollider2.e = modelCasa2.getObb().e * glm::vec3(0.085, 0.1, 0.085);
		addOrUpdateColliders(collidersOBB, "casa2", casaCollider2, modelMatrixCasa2);

		glm::mat4 modelMatrixColliderCerca = glm::mat4(modelMatrixCerca);
		AbstractModel::OBB cercaCollider;
		// Set the orientation of collider before doing the scale
		cercaCollider.u = glm::quat_cast(modelMatrixCerca);
		modelMatrixColliderCerca = glm::scale(modelMatrixColliderCerca, glm::vec3(2.4, 1.0, 1.0));
		modelMatrixColliderCerca = glm::translate(modelMatrixColliderCerca, modelCerca.getObb().c);
		cercaCollider.c = glm::vec3(modelMatrixColliderCerca[3]);
		cercaCollider.e = modelCerca.getObb().e * glm::vec3(2.4, 1.0, 1.0);
		addOrUpdateColliders(collidersOBB, "cerca", cercaCollider, modelMatrixCerca);

		glm::mat4 modelMatrixColliderCerca2 = glm::mat4(modelMatrixCerca2);
		AbstractModel::OBB cerca2Collider;
		// Set the orientation of collider before doing the scale
		cerca2Collider.u = glm::quat_cast(modelMatrixCerca2);
		modelMatrixColliderCerca2 = glm::scale(modelMatrixColliderCerca2, glm::vec3(2.4, 1.0, 1.0));
		modelMatrixColliderCerca2 = glm::translate(modelMatrixColliderCerca2, modelCerca.getObb().c);
		cerca2Collider.c = glm::vec3(modelMatrixColliderCerca2[3]);
		cerca2Collider.e = modelCerca.getObb().e * glm::vec3(2.4, 1.0, 1.0);
		addOrUpdateColliders(collidersOBB, "cerca2", cerca2Collider, modelMatrixCerca2);
		
		glm::mat4 modelMatrixColliderCerca3 = glm::mat4(modelMatrixCerca3);
		AbstractModel::OBB cerca3Collider;
		// Set the orientation of collider before doing the scale
		cerca3Collider.u = glm::quat_cast(modelMatrixCerca3);
		modelMatrixColliderCerca3 = glm::scale(modelMatrixColliderCerca3, glm::vec3(5.0, 1.0, 1.0));
		modelMatrixColliderCerca3 = glm::translate(modelMatrixColliderCerca3, modelCerca.getObb().c);
		cerca3Collider.c = glm::vec3(modelMatrixColliderCerca3[3]);
		cerca3Collider.e = modelCerca.getObb().e * glm::vec3(5.0, 1.0, 1.0);
		addOrUpdateColliders(collidersOBB, "cerca3", cerca3Collider, modelMatrixCerca3);

		glm::mat4 modelMatrixColliderCerca4 = glm::mat4(modelMatrixCerca4);
		AbstractModel::OBB cerca4Collider;
		// Set the orientation of collider before doing the scale
		cerca4Collider.u = glm::quat_cast(modelMatrixCerca4);
		modelMatrixColliderCerca4 = glm::scale(modelMatrixColliderCerca4, glm::vec3(5.0, 1.0, 1.0));
		modelMatrixColliderCerca4 = glm::translate(modelMatrixColliderCerca4, modelCerca.getObb().c);
		cerca4Collider.c = glm::vec3(modelMatrixColliderCerca4[3]);
		cerca4Collider.e = modelCerca.getObb().e * glm::vec3(5.0, 1.0, 1.0);
		addOrUpdateColliders(collidersOBB, "cerca4", cerca4Collider, modelMatrixCerca4);


		glm::mat4 modelMatrixColliderCasa3 = glm::mat4(modelMatrixCasa3);
		AbstractModel::OBB casaCollider3;
		// Set the orientation of collider before doing the scale
		casaCollider3.u = glm::quat_cast(modelMatrixCasa3);
		modelMatrixColliderCasa3 = glm::scale(modelMatrixColliderCasa3, glm::vec3(0.085, 0.1, 0.085));
		modelMatrixColliderCasa3 = glm::translate(modelMatrixColliderCasa3, modelCasa3.getObb().c);
		casaCollider3.c = glm::vec3(modelMatrixColliderCasa3[3]);
		casaCollider3.e = modelCasa3.getObb().e * glm::vec3(0.085, 0.1, 0.085);
		addOrUpdateColliders(collidersOBB, "casa3", casaCollider3, modelMatrixCasa3);

		glm::mat4 modelMatrixColliderCasa4 = glm::mat4(modelMatrixCasa4);
		AbstractModel::OBB casaCollider4;
		// Set the orientation of collider before doing the scale
		casaCollider4.u = glm::quat_cast(modelMatrixCasa4);
		modelMatrixColliderCasa4 = glm::scale(modelMatrixColliderCasa4, glm::vec3(0.085, 0.1, 0.085));
		modelMatrixColliderCasa4 = glm::translate(modelMatrixColliderCasa4, modelCasa4.getObb().c);
		casaCollider4.c = glm::vec3(modelMatrixColliderCasa4[3]);
		casaCollider4.e = modelCasa4.getObb().e * glm::vec3(0.085, 0.1, 0.085);
		addOrUpdateColliders(collidersOBB, "casa4", casaCollider4, modelMatrixCasa4);

		glm::mat4 modelMatrixColliderCasa5 = glm::mat4(modelMatrixCasa5);
		AbstractModel::OBB casaCollider5;
		// Set the orientation of collider before doing the scale
		casaCollider5.u = glm::quat_cast(modelMatrixCasa5);
		modelMatrixColliderCasa5 = glm::scale(modelMatrixColliderCasa5, glm::vec3(0.085, 0.1, 0.085));
		modelMatrixColliderCasa5 = glm::translate(modelMatrixColliderCasa5, modelCasa5.getObb().c);
		casaCollider5.c = glm::vec3(modelMatrixColliderCasa5[3]);
		casaCollider5.e = modelCasa5.getObb().e * glm::vec3(0.085, 0.1, 0.085);
		addOrUpdateColliders(collidersOBB, "casa5", casaCollider5, modelMatrixCasa5);

		glm::mat4 modelMatrixColliderCasa6 = glm::mat4(modelMatrixCasa6);
		AbstractModel::OBB casaCollider6;
		// Set the orientation of collider before doing the scale
		casaCollider6.u = glm::quat_cast(modelMatrixCasa6);
		modelMatrixColliderCasa6 = glm::scale(modelMatrixColliderCasa6, glm::vec3(0.085, 0.1, 0.085));
		modelMatrixColliderCasa6 = glm::translate(modelMatrixColliderCasa6, modelCasa6.getObb().c);
		casaCollider6.c = glm::vec3(modelMatrixColliderCasa6[3]);
		casaCollider6.e = modelCasa6.getObb().e * glm::vec3(0.085, 0.1, 0.085);
		addOrUpdateColliders(collidersOBB, "casa6", casaCollider6, modelMatrixCasa6);
		#pragma endregion

		#pragma region Colisionadores postes de luz
		// Lamps1 colliders
		for (int i = 0; i < lamp1Position.size(); i++){
			AbstractModel::OBB lampCollider;
			glm::mat4 modelMatrixColliderLamp = glm::mat4(1.0);
			modelMatrixColliderLamp = glm::translate(modelMatrixColliderLamp, lamp1Position[i]);
			modelMatrixColliderLamp = glm::rotate(modelMatrixColliderLamp, glm::radians(lamp1Orientation[i]), glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "lamp1-" + std::to_string(i), lampCollider, modelMatrixColliderLamp);
			// Set the orientation of collider before doing the scale
			lampCollider.u = glm::quat_cast(modelMatrixColliderLamp);
			modelMatrixColliderLamp = glm::scale(modelMatrixColliderLamp, glm::vec3(0.08, 0.08, 0.08));
			modelMatrixColliderLamp = glm::translate(modelMatrixColliderLamp, modelLamp1.getObb().c);
			lampCollider.c = glm::vec3(modelMatrixColliderLamp[3]);
			lampCollider.e = modelLamp1.getObb().e * glm::vec3(0.08, 0.08, 0.08);
			std::get<0>(collidersOBB.find("lamp1-" + std::to_string(i))->second) = lampCollider;
		}
		#pragma endregion

		#pragma region Colisionadores arboles
		// Lamps2 colliders
		for (int i = 0; i < treePosition.size(); i++){
			AbstractModel::OBB treeCollider;
			glm::mat4 modelMatrixColliderTree = glm::mat4(1.0);
			modelMatrixColliderTree = glm::translate(modelMatrixColliderTree, treePosition[i]);
			modelMatrixColliderTree = glm::rotate(modelMatrixColliderTree, glm::radians(treeOrientation[i]), glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "Arbol-" + std::to_string(i), treeCollider, modelMatrixColliderTree);
			// Set the orientation of collider before doing the scale
			treeCollider.u = glm::quat_cast(modelMatrixColliderTree);
			modelMatrixColliderTree = glm::scale(modelMatrixColliderTree, glm::vec3(0.0095, 0.018, 0.0055));
			modelMatrixColliderTree = glm::translate(modelMatrixColliderTree, modelTree.getObb().c);
			treeCollider.c = glm::vec3(modelMatrixColliderTree[3]);
			treeCollider.e = modelTree.getObb().e * glm::vec3(0.0095, 0.018, 0.0055);
			std::get<0>(collidersOBB.find("Arbol-" + std::to_string(i))->second) = treeCollider;
		}
		#pragma endregion

		#pragma region Colisionador Jugador
		// Collider del jugador
		jugador.crearColisionador(collidersOBB);
		#pragma endregion

		/*******************************************
		 * Pruebas de colisión
		 *******************************************/
		#pragma region Funcionamiento de colisionadores
		// OBB vs OBB
		for (std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it = collidersOBB.begin(); it != collidersOBB.end(); it++) {
			bool isCollision = false;
			for (std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt = collidersOBB.begin(); jt != collidersOBB.end(); jt++) {
				if (it != jt && testOBBOBB(std::get<0>(it->second), std::get<0>(jt->second))) {
					isCollision = true;

					// Colisiones para los NPCs
					bobAnimate.colisionAtaque(&jugador, &collidersOBB, it, jt, source[7]);		bobAnimate.triggerBala(jugador.cantidadBalas, &collidersOBB, it, jt, source[5]);
					mitziAnimate.colisionAtaque(&jugador, &collidersOBB, it, jt, source[7]);	mitziAnimate.triggerBala(jugador.cantidadBalas, &collidersOBB, it, jt, source[5]);
					rosieAnimate.colisionAtaque(&jugador, &collidersOBB, it, jt, source[7]);	rosieAnimate.triggerBala(jugador.cantidadBalas, &collidersOBB, it, jt, source[5]);
					oliviaAnimate.colisionAtaque(&jugador, &collidersOBB, it, jt, source[7]);	oliviaAnimate.triggerBala(jugador.cantidadBalas, &collidersOBB, it, jt, source[5]);
					kikiAnimate.colisionAtaque(&jugador, &collidersOBB, it, jt, source[7]);		kikiAnimate.triggerBala(jugador.cantidadBalas, &collidersOBB, it, jt, source[5]);
					tangyAnimate.colisionAtaque(&jugador, &collidersOBB, it, jt, source[7]);	tangyAnimate.triggerBala(jugador.cantidadBalas, &collidersOBB, it, jt, source[5]);
					
					billyAnimate.colisionAtaque(&jugador, &collidersOBB, it, jt, source[7]);	billyAnimate.triggerBala(jugador.cantidadBalas, &collidersOBB, it, jt, source[5]);
					nanAnimate.colisionAtaque(&jugador, &collidersOBB, it, jt, source[7]);		nanAnimate.triggerBala(jugador.cantidadBalas, &collidersOBB, it, jt, source[5]);
					chevreAnimate.colisionAtaque(&jugador, &collidersOBB, it, jt, source[7]);	chevreAnimate.triggerBala(jugador.cantidadBalas, &collidersOBB, it, jt, source[5]);
					gruffAnimate.colisionAtaque(&jugador, &collidersOBB, it, jt, source[7]);	gruffAnimate.triggerBala(jugador.cantidadBalas, &collidersOBB, it, jt, source[5]);
					velmaAnimate.colisionAtaque(&jugador, &collidersOBB, it, jt, source[7]);	velmaAnimate.triggerBala(jugador.cantidadBalas, &collidersOBB, it, jt, source[5]);

					budAnimate.colisionAtaque(&jugador, &collidersOBB, it, jt, source[7]);		budAnimate.triggerBala(jugador.cantidadBalas, &collidersOBB, it, jt, source[5]);
					octavianAnimate.colisionAtaque(&jugador, &collidersOBB, it, jt, source[7]); octavianAnimate.triggerBala(jugador.cantidadBalas, &collidersOBB, it, jt, source[5]);
					chiefAnimate.colisionAtaque(&jugador, &collidersOBB, it, jt, source[7]);	chiefAnimate.triggerBala(jugador.cantidadBalas, &collidersOBB, it, jt, source[5]);
					cjAnimate.colisionAtaque(&jugador, &collidersOBB, it, jt, source[7]);		cjAnimate.triggerBala(jugador.cantidadBalas, &collidersOBB, it, jt, source[5]);

					// Colisiones para las balas del jugador
					jugador.colisionesBalas(&collidersOBB, it, jt);
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}

		// SBB vs SBB
		for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it = collidersSBB.begin(); it != collidersSBB.end(); it++) {
			bool isCollision = false;
			for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator jt = collidersSBB.begin(); jt != collidersSBB.end(); jt++) {
				if (it != jt && testSphereSphereIntersection(std::get<0>(it->second), std::get<0>(jt->second))) {
					isCollision = true;
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}

		// SBB vs OBB
		for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it = collidersSBB.begin(); it != collidersSBB.end(); it++) {
			bool isCollision = false;
			std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt = collidersOBB.begin();
			for (; jt != collidersOBB.end(); jt++) {
				if (testSphereOBox(std::get<0>(it->second), std::get<0>(jt->second))) {
					isCollision = true;
					addOrUpdateCollisionDetection(collisionDetection, jt->first, isCollision);
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}

		// Actualiza el estado de todos los colisionadores
		std::map<std::string, bool>::iterator colIt;
		for (colIt = collisionDetection.begin(); colIt != collisionDetection.end(); colIt++) {
			std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it = collidersSBB.find(colIt->first);
			std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt = collidersOBB.find(colIt->first);
			if (it != collidersSBB.end()) {
				if (!colIt->second)
					addOrUpdateColliders(collidersSBB, it->first);
			}
			if (jt != collidersOBB.end()) {
				if (!colIt->second)
					addOrUpdateColliders(collidersOBB, jt->first);
				else {
					// Verifica que el jugador y NPCs no atraviesen otros objetos
					if (jt->first.compare(jugador.nombre) == 0) {
						jugador.modelMatrixJugador = std::get<1>(jt->second);
					}
					else if (jt->first.compare(bobAnimate.nombre) == 0) {
						bobAnimate.modelMatrixNPC = std::get<1>(jt->second);
					}
					else if (jt->first.compare(mitziAnimate.nombre) == 0) {
						mitziAnimate.modelMatrixNPC = std::get<1>(jt->second);
					}
					else if (jt->first.compare(rosieAnimate.nombre) == 0) {
						rosieAnimate.modelMatrixNPC = std::get<1>(jt->second);
					}
					else if (jt->first.compare(oliviaAnimate.nombre) == 0) {
						oliviaAnimate.modelMatrixNPC = std::get<1>(jt->second);
					}
					else if (jt->first.compare(kikiAnimate.nombre) == 0) {
						kikiAnimate.modelMatrixNPC = std::get<1>(jt->second);
					}
					else if (jt->first.compare(tangyAnimate.nombre) == 0) {
						tangyAnimate.modelMatrixNPC = std::get<1>(jt->second);
					}

					else if (jt->first.compare(billyAnimate.nombre) == 0) {
						billyAnimate.modelMatrixNPC = std::get<1>(jt->second);
					}
					else if (jt->first.compare(nanAnimate.nombre) == 0) {
						nanAnimate.modelMatrixNPC = std::get<1>(jt->second);
					}
					else if (jt->first.compare(chevreAnimate.nombre) == 0) {
						chevreAnimate.modelMatrixNPC = std::get<1>(jt->second);
					}
					else if (jt->first.compare(gruffAnimate.nombre) == 0) {
						gruffAnimate.modelMatrixNPC = std::get<1>(jt->second);
					}
					else if (jt->first.compare(velmaAnimate.nombre) == 0) {
						velmaAnimate.modelMatrixNPC = std::get<1>(jt->second);
					}

					else if (jt->first.compare(budAnimate.nombre) == 0) {
						budAnimate.modelMatrixNPC = std::get<1>(jt->second);
					}
					else if (jt->first.compare(octavianAnimate.nombre) == 0) {
						octavianAnimate.modelMatrixNPC = std::get<1>(jt->second);
					}
					else if (jt->first.compare(chiefAnimate.nombre) == 0) {
						chiefAnimate.modelMatrixNPC = std::get<1>(jt->second);
					}
					else if (jt->first.compare(cjAnimate.nombre) == 0) {
						cjAnimate.modelMatrixNPC = std::get<1>(jt->second);
					}
				}
			}
		}
		#pragma endregion

		// Constantes de animaciones
		jugador.indiceAnimacion = 1;

		#pragma region Renderizado de imagenes UI
		projection = glm::ortho(0.0f, 800.0f, 700.0f, 0.0f, -1.0f, 1.0f);
		glEnable(GL_BLEND);
		
		switch (estadoPrograma) {
		// Pantalla de introducción
		case 1:
			introduccionImagenes(projection);
			break;

		// Partida (gameplay)
		case 2:
			iconoSalud.render(glm::vec2(10, 10), glm::vec2(100, 100), 180.0f, glm::vec3(1, 1, 1), projection);
			iconoEnemigo.render(glm::vec2(600, 10), glm::vec2(100, 100), 180.0f, glm::vec3(1, 1, 1), projection);
			break;

		// Fin del juego (muere el jugador)
		case 3:
			iconoSalud.render(glm::vec2(10, 10), glm::vec2(100, 100), 180.0f, glm::vec3(1, 1, 1), projection);
			iconoEnemigo.render(glm::vec2(600, 10), glm::vec2(100, 100), 180.0f, glm::vec3(1, 1, 1), projection);
			break;

		// Fin del juego (el jugador gana la partida)
		case 4:
			iconoSalud.render(glm::vec2(10, 10), glm::vec2(100, 100), 180.0f, glm::vec3(1, 1, 1), projection);
			iconoEnemigo.render(glm::vec2(600, 10), glm::vec2(100, 100), 180.0f, glm::vec3(1, 1, 1), projection);
			break;

		default:
			break;
		}
		glDisable(GL_BLEND);
		#pragma endregion

		#pragma region Renderizado de texto UI
		glEnable(GL_BLEND);

		switch (estadoPrograma) {

		// Pantalla de inicio
		case 0:
			modelText->render("Bartman en", -0.35, 0.60, 55, 1.0, 1.0, 0.0);
			modelText->render("Animal Crossing", -0.5, 0.45, 55, 1.0, 1.0, 0.0);
			modelText->render("Presione A para comenzar", -0.55, -0.65, 40, 1.0, 1.0, 1.0);
			break;

		// Pantalla de introducción
		case 1:
			introduccionTextos();
			break;

		// Partida (Gameplay)
		case 2:
			modelText->render(std::to_string(jugador.salud), -0.7, 0.8, 50, 1.0, 1.0, 0.0);				// Salud del jugador
			modelText->render(std::to_string(15 - jugador.puntuacion), 0.8, 0.8, 50, 1.0, 0.1, 0.0);	// Enemigos restantes
			break;

		// Fin del juego (muere el jugador)
		case 3:
			modelText->render(std::to_string(jugador.salud), -0.7, 0.8, 50, 1.0, 1.0, 0.0);				// Salud del jugador
			modelText->render(std::to_string(15 - jugador.puntuacion), 0.8, 0.8, 50, 1.0, 0.1, 0.0);	// Enemigos restantes
			modelText->render("Bartman", -0.25, 0.60, 55, 1.0, 1.0, 0.0);
			modelText->render("fue capturado", -0.40, 0.45, 55, 1.0, 1.0, 0.0);
			modelText->render("Presione A para volver al inicio", -0.65, -0.65, 40, 1.0, 1.0, 1.0);
			break;

		// Fin del juego (el jugador gana la partida)
		case 4:
			modelText->render(std::to_string(jugador.salud), -0.7, 0.8, 50, 1.0, 1.0, 0.0);				// Salud del jugador
			modelText->render(std::to_string(15 - jugador.puntuacion), 0.8, 0.8, 50, 1.0, 0.1, 0.0);	// Enemigos restantes
			modelText->render("Objetivo cumplido", -0.5, 0.5, 55, 1.0, 1.0, 0.0);
			modelText->render("Bartman gana", -0.35, 0.35, 55, 1.0, 1.0, 0.0);
			modelText->render("Presione A para volver al inicio", -0.65, -0.65, 40, 1.0, 1.0, 1.0);
			break;

		default:
			break;
		}

		glDisable(GL_BLEND);
		#pragma endregion

		glfwSwapBuffers(window);

		#pragma region Renderizado de audios
		/****************************+
		 * Open AL sound data
		 */
		source0Pos[0] = modelMatrixFountain[3].x;
		source0Pos[1] = modelMatrixFountain[3].y;
		source0Pos[2] = modelMatrixFountain[3].z;
		alSourcefv(source[0], AL_POSITION, source0Pos);

		// Listener for the Thris person camera
		listenerPos[0] = jugador.modelMatrixJugador[3].x;
		listenerPos[1] = jugador.modelMatrixJugador[3].y;
		listenerPos[2] = jugador.modelMatrixJugador[3].z;
		alListenerfv(AL_POSITION, listenerPos);

		glm::vec3 upModel = glm::normalize(jugador.modelMatrixJugador[1]);
		glm::vec3 frontModel = glm::normalize(jugador.modelMatrixJugador[2]);

		listenerOri[0] = frontModel.x;
		listenerOri[1] = frontModel.y;
		listenerOri[2] = frontModel.z;
		listenerOri[3] = upModel.x;
		listenerOri[4] = upModel.y;
		listenerOri[5] = upModel.z;

		alListenerfv(AL_ORIENTATION, listenerOri);

		for(unsigned int i = 0; i < sourcesPlay.size(); i++){
			if(sourcesPlay[i]){
				sourcesPlay[i] = false;
				alSourcePlay(source[i]);
			}
		}
		#pragma endregion
	}
}

void prepareScene(){
	skyboxSphere.setShader(&shaderSkybox);
	modelAircraft.setShader(&shaderMulLighting);

	// Casas
	modelCasa.setShader(&shaderMulLighting);
	modelCasa2.setShader(&shaderMulLighting);
	modelCasa3.setShader(&shaderMulLighting);
	modelCasa4.setShader(&shaderMulLighting);
	modelCasa5.setShader(&shaderMulLighting);
	modelCasa6.setShader(&shaderMulLighting);

	//cerca
	modelCerca.setShader(&shaderMulLighting);

	terrain.setShader(&shaderTerrain);

	// Lamp models
	modelLamp1.setShader(&shaderMulLighting);

	modelTree.setShader(&shaderMulLighting);

	// Grass
	modelGrass.setShader(&shaderMulLighting);

	//Fuente
	modelFountain.setShader(&shaderMulLighting);

	// Bart
	jugador.setShader(&shaderMulLighting);

	// NPCs
	bobAnimate.setShader(&shaderMulLighting);
	mitziAnimate.setShader(&shaderMulLighting);
	rosieAnimate.setShader(&shaderMulLighting);
	oliviaAnimate.setShader(&shaderMulLighting);
	kikiAnimate.setShader(&shaderMulLighting);
	tangyAnimate.setShader(&shaderMulLighting);

	billyAnimate.setShader(&shaderMulLighting);
	nanAnimate.setShader(&shaderMulLighting);
	chevreAnimate.setShader(&shaderMulLighting);
	gruffAnimate.setShader(&shaderMulLighting);
	velmaAnimate.setShader(&shaderMulLighting);

	budAnimate.setShader(&shaderMulLighting);
	octavianAnimate.setShader(&shaderMulLighting);
	chiefAnimate.setShader(&shaderMulLighting);
	cjAnimate.setShader(&shaderMulLighting);
}

void prepareDepthScene(){
	skyboxSphere.setShader(&shaderDepth);
	modelAircraft.setShader(&shaderDepth);

	// Casas
	modelCasa.setShader(&shaderDepth);
	modelCasa2.setShader(&shaderDepth);
	modelCasa3.setShader(&shaderDepth);
	modelCasa4.setShader(&shaderDepth);
	modelCasa5.setShader(&shaderDepth);
	modelCasa6.setShader(&shaderDepth);


	modelCerca.setShader(&shaderDepth);

	terrain.setShader(&shaderDepth);

	// Lamp models
	modelLamp1.setShader(&shaderDepth);

	// Fuente
	modelFountain.setShader(&shaderDepth);
	
	// Tree
	modelTree.setShader(&shaderDepth);

	// Grass
	modelGrass.setShader(&shaderDepth);

	// Bart
	jugador.setShader(&shaderDepth);

	// NPCs
	bobAnimate.setShader(&shaderDepth);
	mitziAnimate.setShader(&shaderDepth);
	rosieAnimate.setShader(&shaderDepth);
	oliviaAnimate.setShader(&shaderDepth);
	kikiAnimate.setShader(&shaderDepth);
	tangyAnimate.setShader(&shaderDepth);

	billyAnimate.setShader(&shaderDepth);
	nanAnimate.setShader(&shaderDepth);
	chevreAnimate.setShader(&shaderDepth);
	gruffAnimate.setShader(&shaderDepth);
	velmaAnimate.setShader(&shaderDepth);

	budAnimate.setShader(&shaderDepth);
	octavianAnimate.setShader(&shaderDepth);
	chiefAnimate.setShader(&shaderDepth);
	cjAnimate.setShader(&shaderDepth);
}

void renderScene(bool renderParticles){

	#pragma region Renderizado del terreno
	/*******************************************
	 * Terrain Cesped
	 *******************************************/
	glm::mat4 modelCesped = glm::mat4(1.0);
	modelCesped = glm::translate(modelCesped, glm::vec3(0.0, 0.0, 0.0));
	modelCesped = glm::scale(modelCesped, glm::vec3(200.0, 0.001, 200.0));
	// Se activa la textura del background
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	shaderTerrain.setInt("backgroundTexture", 0);
	// Se activa la textura de tierra
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	shaderTerrain.setInt("rTexture", 1);
	// Se activa la textura de hierba
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	shaderTerrain.setInt("gTexture", 2);
	// Se activa la textura del camino
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	shaderTerrain.setInt("bTexture", 3);
	// Se activa la textura del blend map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	shaderTerrain.setInt("blendMapTexture", 4);
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40, 40)));
	terrain.render();
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
	glBindTexture(GL_TEXTURE_2D, 0);
	#pragma endregion

	/*******************************************
	 * Custom objects obj
	 *******************************************/
	// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
	glActiveTexture(GL_TEXTURE0);

	#pragma region Renderizado de las casas
	//Casa
	modelMatrixCasa[3][1] = terrain.getHeightTerrain(modelMatrixCasa[3][0], modelMatrixCasa[3][2]);
	modelCasa.render(modelMatrixCasa);

	modelMatrixCasa2[3][1] = terrain.getHeightTerrain(modelMatrixCasa2[3][0], modelMatrixCasa2[3][2]);
	modelCasa2.render(modelMatrixCasa2);

	modelMatrixCasa3[3][1] = terrain.getHeightTerrain(modelMatrixCasa3[3][0], modelMatrixCasa3[3][2]);
	modelCasa3.render(modelMatrixCasa3);

	modelMatrixCasa4[3][1] = terrain.getHeightTerrain(modelMatrixCasa4[3][0], modelMatrixCasa4[3][2]);
	modelCasa4.render(modelMatrixCasa4);

	modelMatrixCasa5[3][1] = terrain.getHeightTerrain(modelMatrixCasa5[3][0], modelMatrixCasa5[3][2]);
	modelCasa5.render(modelMatrixCasa5);

	modelMatrixCasa6[3][1] = terrain.getHeightTerrain(modelMatrixCasa6[3][0], modelMatrixCasa6[3][2]);
	modelCasa6.render(modelMatrixCasa6);

	modelMatrixCerca[3][1] = terrain.getHeightTerrain(modelMatrixCerca[3][0], modelMatrixCerca[3][2]);
	modelCerca.render(modelMatrixCerca);

	modelMatrixCerca2[3][1] = terrain.getHeightTerrain(modelMatrixCerca2[3][0], modelMatrixCerca2[3][2]);
	modelCerca.render(modelMatrixCerca2);

	modelMatrixCerca3[3][1] = terrain.getHeightTerrain(modelMatrixCerca3[3][0], modelMatrixCerca3[3][2]);
	modelCerca.render(modelMatrixCerca3);

	modelMatrixCerca4[3][1] = terrain.getHeightTerrain(modelMatrixCerca4[3][0], modelMatrixCerca4[3][2]);
	modelCerca.render(modelMatrixCerca4);
	#pragma endregion

	#pragma region Renderizado postes de luz
	// Render the lamps
	for (int i = 0; i < lamp1Position.size(); i++){
		lamp1Position[i].y = terrain.getHeightTerrain(lamp1Position[i].x, lamp1Position[i].z);
		modelLamp1.setPosition(lamp1Position[i]);
		modelLamp1.setScale(glm::vec3(0.08, 0.08, 0.08));
		modelLamp1.setOrientation(glm::vec3(0, lamp1Orientation[i], 0));
		modelLamp1.render();
	}
	#pragma endregion

	#pragma region Renderizado arboles
	for (int i = 0; i < treePosition.size(); i++){
		treePosition[i].y = terrain.getHeightTerrain(treePosition[i].x, treePosition[i].z);		
		modelTree.setPosition(treePosition[i]);
		modelTree.setScale(glm::vec3(0.06, 0.06, 0.06));
		modelTree.setOrientation(glm::vec3(0, treeOrientation[i], 0));
		modelTree.render();
	}
	#pragma endregion

	#pragma region Renderizado del pasto 3D
	// Grass
	for (int i = 0; i < grassPosition.size(); i++) {
		glDisable(GL_CULL_FACE);
		//glm::vec3 grassPosition = glm::vec3(-15.0, 0.0, -3.0);
		//grassPosition[i].y = terrain.getHeightTerrain(grassPosition[i].x, grassPosition[i].z);
		modelGrass.setPosition(grassPosition[i]);
		modelGrass.setScale(glm::vec3(0.9, 0.9, 0.9));
		modelGrass.render();
		glEnable(GL_CULL_FACE);
	}
	#pragma endregion

	#pragma region Renderizado fuente de agua
	// Fountain
	glDisable(GL_CULL_FACE);
	modelFountain.render(modelMatrixFountain);
	glEnable(GL_CULL_FACE);
	#pragma endregion

	/*******************************************
	 * Custom Anim objects obj
	 *******************************************/
	#pragma region Renderizado de Jugador
	jugador.update(collidersOBB, &terrain, deltaTime, estadoPrograma);
	#pragma endregion

	#pragma region Renderizado de NPCs
	// Gatos
	bobAnimate.update(&jugador, &terrain, collidersOBB, deltaTime, activandoNPC, idNPC, estadoPrograma, source[6], source6Pos);
	mitziAnimate.update(&jugador, &terrain, collidersOBB, deltaTime, activandoNPC, idNPC, estadoPrograma, source[6], source6Pos);
	rosieAnimate.update(&jugador, &terrain, collidersOBB, deltaTime, activandoNPC, idNPC, estadoPrograma, source[6], source6Pos);
	oliviaAnimate.update(&jugador, &terrain, collidersOBB, deltaTime, activandoNPC, idNPC, estadoPrograma, source[6], source6Pos);
	kikiAnimate.update(&jugador, &terrain, collidersOBB, deltaTime, activandoNPC, idNPC, estadoPrograma, source[6], source6Pos);
	tangyAnimate.update(&jugador, &terrain, collidersOBB, deltaTime, activandoNPC, idNPC, estadoPrograma, source[6], source6Pos);

	// Cabras
	billyAnimate.update(&jugador, &terrain, collidersOBB, deltaTime, activandoNPC, idNPC, estadoPrograma, source[6], source6Pos);
	nanAnimate.update(&jugador, &terrain, collidersOBB, deltaTime, activandoNPC, idNPC, estadoPrograma, source[6], source6Pos);
	chevreAnimate.update(&jugador, &terrain, collidersOBB, deltaTime, activandoNPC, idNPC, estadoPrograma, source[6], source6Pos);
	gruffAnimate.update(&jugador, &terrain, collidersOBB, deltaTime, activandoNPC, idNPC, estadoPrograma, source[6], source6Pos);
	velmaAnimate.update(&jugador, &terrain, collidersOBB, deltaTime, activandoNPC, idNPC, estadoPrograma, source[6], source6Pos);

	// Adicionales
	budAnimate.update(&jugador, &terrain, collidersOBB, deltaTime, activandoNPC, idNPC, estadoPrograma, source[6], source6Pos);
	octavianAnimate.update(&jugador, &terrain, collidersOBB, deltaTime, activandoNPC, idNPC, estadoPrograma, source[6], source6Pos);
	chiefAnimate.update(&jugador, &terrain, collidersOBB, deltaTime, activandoNPC, idNPC, estadoPrograma, source[6], source6Pos);
	cjAnimate.update(&jugador, &terrain, collidersOBB, deltaTime, activandoNPC, idNPC, estadoPrograma, source[6], source6Pos);

	// Cuando un NPC muere, otro tomará su lugar
	if (activandoNPC == true) {
		switch (idNPC) {
		case 0: billyAnimate.activo = true; break;
		case 1: nanAnimate.activo = true; break;
		case 2: chevreAnimate.activo = true; break;
		case 3: gruffAnimate.activo = true; break;
		case 4: velmaAnimate.activo = true; break;

		case 5: budAnimate.activo = true; break;
		case 6: octavianAnimate.activo = true; break;
		case 7: chiefAnimate.activo = true; break;
		case 8: cjAnimate.activo = true; break;

		default:
			break;
		}

		activandoNPC = false;
	}
	#pragma endregion

	/**********
	 * Update the position with alpha objects
	 **********/
	#pragma region Renderizado transparencias y particulas
	// Update the aircraft
	blendingUnsorted.find("aircraft")->second = glm::vec3(modelMatrixAircraft[3]);
	//Casa
	blendingUnsorted.find("casa")->second = glm::vec3(modelMatrixCasa[3]);
	blendingUnsorted.find("casa2")->second = glm::vec3(modelMatrixCasa2[3]);
	blendingUnsorted.find("casa3")->second = glm::vec3(modelMatrixCasa3[3]);
	blendingUnsorted.find("casa4")->second = glm::vec3(modelMatrixCasa4[3]);
	blendingUnsorted.find("casa5")->second = glm::vec3(modelMatrixCasa5[3]);
	blendingUnsorted.find("casa6")->second = glm::vec3(modelMatrixCasa6[3]);

	blendingUnsorted.find("cerca")->second = glm::vec3(modelMatrixCerca[3]);

	/**********
	 * Sorter with alpha objects
	 **********/
	std::map<float, std::pair<std::string, glm::vec3>> blendingSorted;
	std::map<std::string, glm::vec3>::iterator itblend;
	for(itblend = blendingUnsorted.begin(); itblend != blendingUnsorted.end(); itblend++){
		float distanceFromView = glm::length(camera->getPosition() - itblend->second);
		blendingSorted[distanceFromView] = std::make_pair(itblend->first, itblend->second);
	}

	/**********
	 * Render de las transparencias
	 **********/
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	for(std::map<float, std::pair<std::string, glm::vec3> >::reverse_iterator it = blendingSorted.rbegin(); it != blendingSorted.rend(); it++){
		if(it->second.first.compare("aircraft") == 0){
			// Render for the aircraft model
			glm::mat4 modelMatrixAircraftBlend = glm::mat4(modelMatrixAircraft);
			modelMatrixAircraftBlend[3][1] = terrain.getHeightTerrain(modelMatrixAircraftBlend[3][0], modelMatrixAircraftBlend[3][2]) + 1.0;
			modelAircraft.render(modelMatrixAircraftBlend);
		}
		else if(renderParticles && it->second.first.compare("fountain") == 0){
			/**********
			 * Init Render particles systems
			 */
			glm::mat4 modelMatrixParticlesFountain = glm::mat4(1.0);
			modelMatrixParticlesFountain = glm::translate(modelMatrixParticlesFountain, glm::vec3(-3.13, -1.0, -2.0));
			modelMatrixParticlesFountain[3][1] = terrain.getHeightTerrain(modelMatrixParticlesFountain[3][0], modelMatrixParticlesFountain[3][2]) + 0.36 * 10.0;
			modelMatrixParticlesFountain = glm::scale(modelMatrixParticlesFountain, glm::vec3(2.8, 2.8, 2.8));
			currTimeParticlesAnimation = TimeManager::Instance().GetTime();
			if(currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
				lastTimeParticlesAnimation = currTimeParticlesAnimation;
			//glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			// Set the point size
			glPointSize(10.0f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticleFountainID);
			shaderParticlesFountain.turnOn();
			shaderParticlesFountain.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
			shaderParticlesFountain.setFloat("ParticleLifetime", 35.5f);
			shaderParticlesFountain.setInt("ParticleTex", 0);
			shaderParticlesFountain.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, -0.26f, 0.0f)));
			shaderParticlesFountain.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesFountain));
			glBindVertexArray(VAOParticles);
			glDrawArrays(GL_POINTS, 0, nParticles);
			glDepthMask(GL_TRUE);
			//glEnable(GL_DEPTH_TEST);
			shaderParticlesFountain.turnOff();
			/**********
			 * End Render particles systems
			 */
		}
		else if(renderParticles && it->second.first.compare("fire") == 0){
			/**********
			 * Init Render particles systems
			 */
			lastTimeParticlesAnimationFire = currTimeParticlesAnimationFire;
			currTimeParticlesAnimationFire = TimeManager::Instance().GetTime();

			shaderParticlesFire.setInt("Pass", 1);
			shaderParticlesFire.setFloat("Time", currTimeParticlesAnimationFire);
			shaderParticlesFire.setFloat("DeltaT", currTimeParticlesAnimationFire - lastTimeParticlesAnimationFire);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_1D, texId);
			glEnable(GL_RASTERIZER_DISCARD);
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
			glBeginTransformFeedback(GL_POINTS);
			glBindVertexArray(particleArray[1-drawBuf]);
			glVertexAttribDivisor(0,0);
			glVertexAttribDivisor(1,0);
			glVertexAttribDivisor(2,0);
			glDrawArrays(GL_POINTS, 0, nParticlesFire);
			glEndTransformFeedback();
			glDisable(GL_RASTERIZER_DISCARD);
			
			// Particulas de fuego(humo) de las casas con chimenea
			shaderParticlesFire.setInt("Pass", 2);
			glm::mat4 modelFireParticles = glm::mat4(1.0);
			modelFireParticles = glm::translate(modelFireParticles, glm::vec3(-4.24, 10.5, 46.87));
			shaderParticlesFire.setMatrix4("model", 1, false, glm::value_ptr(modelFireParticles));

			shaderParticlesFire.turnOn();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
			glDepthMask(GL_FALSE);
			glBindVertexArray(particleArray[drawBuf]);
			glVertexAttribDivisor(0,1);
			glVertexAttribDivisor(1,1);
			glVertexAttribDivisor(2,1);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesFire);
			glBindVertexArray(0);
			glDepthMask(GL_TRUE);
			drawBuf = 1 - drawBuf;
			shaderParticlesFire.turnOff();

			/****************************+
			 * Open AL sound data
			 */
			source1Pos[0] = modelFireParticles[3].x;
			source1Pos[1] = modelFireParticles[3].y;
			source1Pos[2] = modelFireParticles[3].z;
			alSourcefv(source[1], AL_POSITION, source1Pos);

			/**********
			 * End Render particles systems
			 */
		
			 /**********
			  * Init Render particles systems fire 2
			  */
			 lastTimeParticlesAnimationFire = currTimeParticlesAnimationFire;
			 currTimeParticlesAnimationFire = TimeManager::Instance().GetTime();

			 shaderParticlesFire.setInt("Pass", 1);
			 shaderParticlesFire.setFloat("Time", currTimeParticlesAnimationFire);
			 shaderParticlesFire.setFloat("DeltaT", currTimeParticlesAnimationFire - lastTimeParticlesAnimationFire);

			 glActiveTexture(GL_TEXTURE1);
			 glBindTexture(GL_TEXTURE_1D, texId);
			 glEnable(GL_RASTERIZER_DISCARD);
			 glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
			 glBeginTransformFeedback(GL_POINTS);
			 glBindVertexArray(particleArray[1 - drawBuf]);
			 glVertexAttribDivisor(0, 0);
			 glVertexAttribDivisor(1, 0);
			 glVertexAttribDivisor(2, 0);
			 glDrawArrays(GL_POINTS, 0, nParticlesFire);
			 glEndTransformFeedback();
			 glDisable(GL_RASTERIZER_DISCARD);

			 // Particulas de fuego(humo) de la casa 2
			 shaderParticlesFire2.setInt("Pass", 2);
			 glm::mat4 modelFireParticles2 = glm::mat4(1.0);
			 modelFireParticles2 = glm::translate(modelFireParticles2, glm::vec3(59.0, 10.5, -36.89));
			 shaderParticlesFire2.setMatrix4("model", 1, false, glm::value_ptr(modelFireParticles2));

			 shaderParticlesFire2.turnOn();
			 glActiveTexture(GL_TEXTURE0);
			 glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
			 glDepthMask(GL_FALSE);
			 glBindVertexArray(particleArray[drawBuf]);
			 glVertexAttribDivisor(0, 1);
			 glVertexAttribDivisor(1, 1);
			 glVertexAttribDivisor(2, 1);
			 glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesFire);
			 glBindVertexArray(0);
			 glDepthMask(GL_TRUE);
			 drawBuf = 1 - drawBuf;
			 shaderParticlesFire.turnOff();

			 /****************************
			  * Open AL sound data
			  */
			 source2Pos[0] = modelFireParticles2[3].x;
			 source2Pos[1] = modelFireParticles2[3].y;
			 source2Pos[2] = modelFireParticles2[3].z;
			 alSourcefv(source[2], AL_POSITION, source2Pos);

			 /**********
			  * End Render particles systems
			  */
		}
	}
	#pragma endregion
}

int main(int argc, char **argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}

void reiniciarPartida() {
	// Reinicia al jugador
	jugador.modelMatrixJugador = glm::mat4(1.0);
	jugador.start("Bart", glm::vec3(13.0f, 0.05f, -9.0f), -90.0f);
	jugador.muerto = false; jugador.activo = true;
	jugador.salud = 100; jugador.puntuacion = 0;

	// Reinicia a los NPCs
	bobAnimate.modelMatrixNPC = glm::mat4(1.0);		bobAnimate.start("NPC-Bob", glm::vec3(0, 0, 20), 0.0f, true);			bobAnimate.salud = 100;
	mitziAnimate.modelMatrixNPC = glm::mat4(1.0);	mitziAnimate.start("NPC-Mitzi", glm::vec3(13.5, 0, 40), 0.0f, true);	mitziAnimate.salud = 100;
	rosieAnimate.modelMatrixNPC = glm::mat4(1.0);	rosieAnimate.start("NPC-Rosie", glm::vec3(-8, 0, 10), 0.0f, true);		rosieAnimate.salud = 100;
	oliviaAnimate.modelMatrixNPC = glm::mat4(1.0);	oliviaAnimate.start("NPC-Olivia", glm::vec3(-25, 0, 20), 0.0f, true);	oliviaAnimate.salud = 100;
	kikiAnimate.modelMatrixNPC = glm::mat4(1.0);	kikiAnimate.start("NPC-Kiki", glm::vec3(-25, 0, -20), 0.0f, true);		kikiAnimate.salud = 100;
	tangyAnimate.modelMatrixNPC = glm::mat4(1.0);	tangyAnimate.start("NPC-Tangy", glm::vec3(70, 0, 50), 0.0f, true);		tangyAnimate.salud = 100;

	billyAnimate.modelMatrixNPC = glm::mat4(1.0);	billyAnimate.start("NPC-Billy", glm::vec3(13, 0, 15), 0.0f, false);		billyAnimate.salud = 100;	collidersOBB.erase(billyAnimate.nombre);
	nanAnimate.modelMatrixNPC = glm::mat4(1.0);		nanAnimate.start("NPC-Nan", glm::vec3(-13, 0, 5), 0.0f, false);			nanAnimate.salud = 100;		collidersOBB.erase(nanAnimate.nombre);
	chevreAnimate.modelMatrixNPC = glm::mat4(1.0);  chevreAnimate.start("NPC-Chevre", glm::vec3(-70, 0, 0), 0.0f, false);	chevreAnimate.salud = 100;	collidersOBB.erase(chevreAnimate.nombre);
	gruffAnimate.modelMatrixNPC = glm::mat4(1.0);	gruffAnimate.start("NPC-Gruff", glm::vec3(-50, 0, -35), 0.0f, false);	gruffAnimate.salud = 100;	collidersOBB.erase(gruffAnimate.nombre);
	velmaAnimate.modelMatrixNPC = glm::mat4(1.0);	velmaAnimate.start("NPC-Velma", glm::vec3(-40, 0, -50), 0.0f, false);	velmaAnimate.salud = 100;	collidersOBB.erase(velmaAnimate.nombre);

	budAnimate.modelMatrixNPC = glm::mat4(1.0);		budAnimate.start("NPC-Bud", glm::vec3(80, 0, -30), 0.0f, false);		budAnimate.salud = 100;		collidersOBB.erase(budAnimate.nombre);
	octavianAnimate.modelMatrixNPC = glm::mat4(1.0); octavianAnimate.start("NPC-Octavian", glm::vec3(50, 0, 65), 120.0f, false);	octavianAnimate.salud = 100; collidersOBB.erase(octavianAnimate.nombre);
	chiefAnimate.modelMatrixNPC = glm::mat4(1.0);	chiefAnimate.start("NPC-Chief", glm::vec3(-80, 0, 10), 95.0f, false);	chiefAnimate.salud = 100;	collidersOBB.erase(chiefAnimate.nombre);
	cjAnimate.modelMatrixNPC = glm::mat4(1.0);		cjAnimate.start("NPC-CJ", glm::vec3(-80, 0, 35), 180.0f, false);		cjAnimate.salud = 100;		collidersOBB.erase(cjAnimate.nombre);

	botonApresionado = true;
	estadoPrograma = 0;
}

void introduccionImagenes(glm::mat4 projection) {
	switch (paginaIntroduccion) {

	case 0: introduccionUI1.render(glm::vec2(700, 50), glm::vec2(-600, 350), 180.0f, glm::vec3(1, 1, 1), projection); break;
	case 1: introduccionUI2.render(glm::vec2(700, 50), glm::vec2(-600, 350), 180.0f, glm::vec3(1, 1, 1), projection); break;
	case 2: introduccionUI2.render(glm::vec2(700, 50), glm::vec2(-600, 350), 180.0f, glm::vec3(1, 1, 1), projection); break;
	case 3: introduccionUI3.render(glm::vec2(700, 50), glm::vec2(-600, 350), 180.0f, glm::vec3(1, 1, 1), projection); break;
	case 4: introduccionUI3.render(glm::vec2(700, 50), glm::vec2(-600, 350), 180.0f, glm::vec3(1, 1, 1), projection); break;
	case 5: controlesPC.render(glm::vec2(700, 50), glm::vec2(-600, 350), 180.0f, glm::vec3(1, 1, 1), projection); break;
	case 6: controlesMando.render(glm::vec2(700, 50), glm::vec2(-600, 350), 180.0f, glm::vec3(1, 1, 1), projection); break;

	default:
		break;
	}
}

void introduccionTextos() {
	switch (paginaIntroduccion) {
	case 0:
		modelText->render("Un dia, Bartman aparecio misteriosamente", -0.85, -0.35, 35, 1.0, 1.0, 1.0);
		modelText->render("en el pueblo de Animal Crossing.", -0.85, -0.5, 35, 1.0, 1.0, 1.0);
		modelText->render("Presione A para continuar", -0.45, -0.85, 30, 1.0, 1.0, 1.0);
		break;

	case 1:
		modelText->render("Pero Bartman es demasiado travieso y", -0.85, -0.35, 35, 1.0, 1.0, 1.0);
		modelText->render("esto incomoda a los habitantes del pueblo.", -0.85, -0.5, 35, 1.0, 1.0, 1.0);
		modelText->render("Presione A para continuar", -0.45, -0.85, 30, 1.0, 1.0, 1.0);
		break;

	case 2:
		modelText->render("Tanto que han tenido que tomar medidas", -0.85, -0.35, 35, 1.0, 1.0, 1.0);
		modelText->render("severas para capturarlo... con redes.", -0.85, -0.5, 35, 1.0, 1.0, 1.0);
		modelText->render("Presione A para continuar", -0.45, -0.85, 30, 1.0, 1.0, 1.0);
		break;

	case 3:
		modelText->render("Sin embargo, Bartman posee un arma", -0.85, -0.35, 35, 1.0, 1.0, 1.0);
		modelText->render("laser para lidiar con el problema...", -0.85, -0.5, 35, 1.0, 1.0, 1.0);
		modelText->render("y seguir con sus travesuras.", -0.85, -0.65, 35, 1.0, 1.0, 1.0);
		modelText->render("Presione A para continuar", -0.45, -0.85, 30, 1.0, 1.0, 1.0);
		break;

	case 4:
		modelText->render("Objetivo: eliminar a todos los pueblerinos", -0.9, -0.65, 40, 0.3, 0.9, 0.7);
		modelText->render("Presione A para continuar", -0.45, -0.85, 30, 1.0, 1.0, 1.0);
		break;

	case 5:
		modelText->render("Controles (PC)", -0.35, -0.65, 40, 0.3, 0.9, 0.7);
		modelText->render("Presione A para continuar", -0.45, -0.85, 30, 1.0, 1.0, 1.0);
		break;

	case 6:
		modelText->render("Controles (Mando)", -0.4, -0.65, 40, 0.3, 0.9, 0.7);
		modelText->render("Presione A para comenzar", -0.45, -0.85, 30, 1.0, 1.0, 1.0);
		break;

	default:
		break;
	}
}
