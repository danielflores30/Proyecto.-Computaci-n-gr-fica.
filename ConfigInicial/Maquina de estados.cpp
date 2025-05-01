
#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void Animation();

// Window dimensions
const GLuint WIDTH = 1000, HEIGHT = 800;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;

// Animación del dron
glm::vec3 dronPos(-15.0f, 9.0f, -24.0f); // Posición inicial del dron (igual al primer waypoint)

glm::vec3 waypoints[4] = {
	glm::vec3(-15.0f, 9.0f, -24.0f),  // Esquina inferior-izquierda (A)
	glm::vec3(15.0f, 9.0f, -24.0f),   // Esquina inferior-derecha (B)
	glm::vec3(15.0f, 9.0f, 20.0f),    // Esquina superior-derecha (C) 
	glm::vec3(-15.0f, 9.0f, 20.0f)    // Esquina superior-izquierda (D) 
};


//--------------------------------------------------------------------------
//-----------------------------Movimientos de sillas------------------------
// Número de filas de sillas y cuántas sillas hay por fila
// ======================= Configuración de sillas ============================
const int filasDeSillas = 14;
const int sillasPorFila = 4;
const int numSillas = filasDeSillas * sillasPorFila;

// Variables para animación
glm::vec3 sillaPosArr[numSillas];            // Posiciones actuales
float sillaRotArr[numSillas] = { 0.0f };      // Rotaciones actuales
float rotSillaArr[numSillas] = { 0.0f };      // Auxiliar de rotación
int sillaAnimArr[numSillas] = {0};          // Estado de animación
bool sillaActiva = false;

// Parámetros para personalizar trayectorias
glm::vec3 posicionesIniciales[numSillas];     // Desde la puerta
glm::vec3 destinoIntermedio[numSillas];       // Entrada al pasillo
float rotacionObjetivo[numSillas];            // Rotación hacia pasillo
glm::vec3 posicionesFinales[numSillas];           // Frente al escritorio
float rotacionFinalAjuste[numSillas];         // Rotación final
glm::vec3 destino1[numSillas];  // Primero se mueven al frente
glm::vec3 destino2[numSillas];  // Luego giran y se mueven lateral
glm::vec3 destinoFinalAvance[numSillas];  // Agrega esta nueva variable global
float rotacionPostFinal[numSillas]; // (fuera del loop, como variable global)







int currentWaypoint = 0;
float speed = 2.5f; // Velocidad de movimiento
bool moveDron = false; // Control de movimiento
// 
bool sinusoidalMode = false;        // Modo sinusoidal activo/desactivado
float sineAmplitude = 1.0f;        // Altura de la onda
float sineFrequency = 2.0f;        // Velocidad de oscilación
float sineTime = 0.0f;             // Tiempo acumulado para el cálculo de la onda
glm::vec3 initialSinePos;          // Posición inicial al activar el modo sinusoidal

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f,2.0f, 0.0f),
	glm::vec3(0.0f,0.0f, 0.0f),
	glm::vec3(0.0f,0.0f,  0.0f),
	glm::vec3(0.0f,0.0f, 0.0f)
};

float vertices[] = {
	 -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};



glm::vec3 Light1 = glm::vec3(0);
//Anim
float rotBall = 0;
bool AnimBall = false;


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

int main(){

	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Animacion basica", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// GLFW Options
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);



	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

	//models
	Model Piso((char*)"Models/Proyecto/piso.obj");
	Model dron((char*)"Models/Proyecto/dron/model.obj");
	Model full((char*)"Models/Proyecto/salon/salonnn.obj");
	Model escribe((char*)"Models/Proyecto/escribe/escribe.obj");
	Model nuevosalon((char*)"Models/Proyecto/nuevosalon/salon_nuevo.obj");
	Model escritorionuevo((char*)"Models/Proyecto/mesanueva/model.obj");
	Model sillanueva((char*)"Models/Proyecto/sillanueva/silla.obj");
	Model imac((char*)"Models/Proyecto/imac/model.obj");
	Model logo((char*)"Models/Proyecto/logo/model.obj");




	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		Animation();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// OpenGL options
		glEnable(GL_DEPTH_TEST);






		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();

		glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "specular"),1);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);


		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.3f);


		// Point light 1
		glm::vec3 lightColor;
		lightColor.x = abs(sin(glfwGetTime() * Light1.x));
		lightColor.y = abs(sin(glfwGetTime() * Light1.y));
		lightColor.z = sin(glfwGetTime() * Light1.z);


		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);


		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.3f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));


		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 5.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glm::mat4 model(1);



		////Carga de modelo 
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//Piso.Draw(lightingShader);

		///Salon
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-9.0f, -1.0f, 25.0f)); // Ajusta X,Y,Z
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		full.Draw(lightingShader);

		/////Salón Nueevoooo
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::translate(model, glm::vec3(15.1f, -0.1f, 14.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		nuevosalon.Draw(lightingShader);

		////////////////////////////////////////////////////Modelos de sillas nuevas//////////////////////
		////////////////////////////////// Primer mesa de la derecha  /////////////////////////////////////////////////////////
		// Inicializa el modelo con matriz identidad

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(12.6f, -1.2f, -23.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(9.73f, -1.2f, -23.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(6.9f, -1.2f, -23.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(3.73f, -1.2f, -23.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//////////////////////////////// Segunda mesa de la derecha  /////////////////////////////////////////////////////////
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(12.6f, -1.2f, -16.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(9.73f, -1.2f, -16.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(6.9f, -1.2f, -16.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(3.73f, -1.2f, -16.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);


		//		//////////////////////////// Tercera mesa de la derecha  /////////////////////////////////////////////////////////
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(12.6f, -1.2f, -9.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(9.73f, -1.2f, -9.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(6.9f, -1.2f, -9.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(3.73f, -1.2f, -9.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);


		//		////////////////////////////// Cuarta mesa de la derecha  /////////////////////////////////////////////////////////
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(12.6f, -1.2f, -4.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);


		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(9.73f, -1.2f, -4.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(6.9f, -1.2f, -4.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(3.73f, -1.2f, -4.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);


		//		////////////////////////////// Quinta mesa de la derecha  /////////////////////////////////////////////////////////     /////////////////////// 5 computadoras
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(13.0f, -1.2f, 2.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(10.5f, -1.2f, 2.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(8.0f, -1.2f, 2.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(5.5f, -1.2f, 2.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(3.0f, -1.2f, 2.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);




		//		////////////////////////////// Sexta mesa de la derecha  /////////////////////////////////////////////////////////
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(13.0f, -1.2f, 8.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(10.5f, -1.2f, 8.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(8.0f, -1.2f, 8.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(5.5f, -1.2f, 8.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(3.0f, -1.2f, 8.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		////		////////////////////////////// Septima mesa de la derecha  /////////////////////////////////////////////////////////     /////////////////////// 5 computadoras
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(13.0f, -1.2f, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(10.5f, -1.2f, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(8.0f, -1.2f, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(5.5f, -1.2f, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(3.0f, -1.2f, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

//
//////////////////////////////////////////////////////Modelos de sillas nuevas//////////////////////
//
////////////////////////////////// Primer mesa de la izquierda  /////////////////////////////////////////////////////////

		for (int i = 0; i < numSillas; i++) {
			view = camera.GetViewMatrix();
			model = glm::mat4(1.0f);
			model = glm::translate(model, posicionesIniciales[i] + sillaPosArr[i]);
			model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
			model = glm::rotate(model, glm::radians(sillaRotArr[i] + 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillanueva.Draw(lightingShader);
		}

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-27.6f, -1.2f, -23.2f) + sillaPos);
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(sillaRot + 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-24.73f, -1.2f, -23.2f) + sillaPos);
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(sillaRot + 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-21.9f, -1.2f, -23.2f) + sillaPos);
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(sillaRot + 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

		//view = camera.GetViewMatrix();
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-18.73f, -1.2f, -23.2f) + sillaPos); //le sumamos + 15
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, glm::radians(sillaRot + 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//sillanueva.Draw(lightingShader);

//
//////////////////////////////// Segunda mesa de la derecha  /////////////////////////////////////////////////////////
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-12.6f, -1.2f, -16.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-9.73f, -1.2f, -16.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-6.9f, -1.2f, -16.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-3.73f, -1.2f, -16.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//
//				//////////////////////////// Tercera mesa de la derecha  /////////////////////////////////////////////////////////
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-12.6f, -1.2f, -9.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-9.73f, -1.2f, -9.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-6.9f, -1.2f, -9.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-3.73f, -1.2f, -9.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//
//				////////////////////////////// Cuarta mesa de la derecha  /////////////////////////////////////////////////////////
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-12.6f, -1.2f, -4.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-9.73f, -1.2f, -4.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-6.9f, -1.2f, -4.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-3.73f, -1.2f, -4.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//
//				////////////////////////////// Quinta mesa de la derecha  /////////////////////////////////////////////////////////     /////////////////////// 5 computadoras
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-13.0f, -1.2f, 2.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-10.5f, -1.2f, 2.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-8.0f, -1.2f, 2.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-5.5f, -1.2f, 2.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-3.0f, -1.2f, 2.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//
//
//
//				////////////////////////////// Sexta mesa de la derecha  /////////////////////////////////////////////////////////
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-13.0f, -1.2f, 8.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-10.5f, -1.2f, 8.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-8.0f, -1.2f, 8.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-5.5f, -1.2f, 8.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-3.0f, -1.2f, 8.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		//		////////////////////////////// Septima mesa de la derecha  /////////////////////////////////////////////////////////     /////////////////////// 5 computadoras
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-13.0f, -1.2f, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-10.5f, -1.2f, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-8.0f, -1.2f, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-5.5f, -1.2f, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//
//		view = camera.GetViewMatrix();
//		model = glm::mat4(1);
//		model = glm::translate(model, glm::vec3(-3.0f, -1.2f, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
//		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//		sillanueva.Draw(lightingShader);
//



	//////////////////////////////////	/*Modelos de escritorios nuevos*///////////////////////////////////////////////////////
		//Derecha
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
		model = glm::translate(model, glm::vec3(9.0f, 0.9f, -13.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		escritorionuevo.Draw(lightingShader);

		//Izquierda
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
		model = glm::translate(model, glm::vec3(3.6f, 0.9f, -13.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		escritorionuevo.Draw(lightingShader);

		//Derecha
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
		model = glm::translate(model, glm::vec3(9.0f, 0.9f, -9.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		escritorionuevo.Draw(lightingShader);

		//Izquierda
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
		model = glm::translate(model, glm::vec3(3.6f, 0.9f, -9.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		escritorionuevo.Draw(lightingShader);

		///////Derecha
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
		model = glm::translate(model, glm::vec3(9.0f, 0.9f, -5.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		escritorionuevo.Draw(lightingShader);

		//Izquierda
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
		model = glm::translate(model, glm::vec3(3.6f, 0.9f, -5.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		escritorionuevo.Draw(lightingShader);


		//Derecha
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
		model = glm::translate(model, glm::vec3(9.0f, 0.9f, -1.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		escritorionuevo.Draw(lightingShader);

		//Izquierda
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
		model = glm::translate(model, glm::vec3(3.6f, 0.9f, -1.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		escritorionuevo.Draw(lightingShader);


		//Derecha
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
		model = glm::translate(model, glm::vec3(9.0f, 0.9f, 3.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		escritorionuevo.Draw(lightingShader);

		//Izquierda
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
		model = glm::translate(model, glm::vec3(3.6f, 0.9f, 3.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		escritorionuevo.Draw(lightingShader);


		//Derecha
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
		model = glm::translate(model, glm::vec3(9.0f, 0.9f, 7.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		escritorionuevo.Draw(lightingShader);

		//Izquierda
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
		model = glm::translate(model, glm::vec3(3.6f, 0.9f, 7.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		escritorionuevo.Draw(lightingShader);


		//Derecha
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
		model = glm::translate(model, glm::vec3(9.0f, 0.9f, 11.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		escritorionuevo.Draw(lightingShader);

		//Izquierda
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
		model = glm::translate(model, glm::vec3(3.6f, 0.9f, 11.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		escritorionuevo.Draw(lightingShader);



		//Mesa profesor //Derecha
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(1.3f, 1.5f, 1.5f));
		model = glm::translate(model, glm::vec3(15.1f, 0.9f, 13.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		escritorionuevo.Draw(lightingShader);

		/////////////////////////////////////////////////////////////Modelos de iMac////////////////////////////////////////////////////////////////////////////////////

		//Modelo de iMac
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
		model = glm::translate(model, glm::vec3(10.8f, 2.4f, -19.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		imac.Draw(lightingShader);

		//////////////////////////////////////////////////////////////////Modelo de logos////////////////////////////////////////////////////////////////////////////////////////////////////
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(27.0f, 27.0f, 27.0f));
		model = glm::translate(model, glm::vec3(-0.48f, 0.22f, 0.88f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		logo.Draw(lightingShader);

		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(27.0f, 27.0f, 27.0f));
		model = glm::translate(model, glm::vec3(0.56f, 0.22f, 0.87f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		logo.Draw(lightingShader);

		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(27.0f, 27.0f, 27.9f));
		model = glm::translate(model, glm::vec3(-0.45f, 0.22f, -0.95f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		logo.Draw(lightingShader);

		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(27.0f, 27.0f, -27.0f));
		model = glm::translate(model, glm::vec3(0.56f, 0.22f, 0.968f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		logo.Draw(lightingShader);

		////Modelo del Dron
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//model = glm::mat4(1);
		//model = glm::translate(model, dronPos); // Aplicar posición actual
		//model = glm::scale(model, glm::vec3(0.8f));
		//model = glm::rotate(model, glm::radians(rotBall), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación en Y
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//dron.Draw(lightingShader);
		//glBindVertexArray(0);

		////////////Salon Nuevo



		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)

		model = glm::mat4(1);
		model = glm::translate(model, pointLightPositions[0]);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);



		// Swap the screen buffers
		glfwSwapBuffers(window);
	}


	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();



	return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{

	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);


	}

	if (keys[GLFW_KEY_T])
	{
		pointLightPositions[0].x += 0.01f;
	}
	if (keys[GLFW_KEY_G])
	{
		pointLightPositions[0].x -= 0.01f;
	}

	if (keys[GLFW_KEY_Y])
	{
		pointLightPositions[0].y += 0.01f;
	}

	if (keys[GLFW_KEY_H])
	{
		pointLightPositions[0].y -= 0.01f;
	}
	if (keys[GLFW_KEY_U])
	{
		pointLightPositions[0].z -= 0.1f;
	}
	if (keys[GLFW_KEY_J])
	{
		pointLightPositions[0].z += 0.01f;
	}

}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
		{
			Light1 = glm::vec3(1.0f, 1.0f, 0.0f);

		}
		else
		{
			Light1 = glm::vec3(0);//Cuado es solo un valor en los 3 vectores pueden dejar solo una componente
		}
	}
	if (keys[GLFW_KEY_N])
	{
		AnimBall = !AnimBall;

	}
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		sinusoidalMode = !sinusoidalMode;
		if (sinusoidalMode) {
			initialSinePos = dronPos;
			sineTime = 0.0f;
		}
		else {
			currentWaypoint = 0; // Reiniciar al primer waypoint
			dronPos = waypoints[0]; // Opcional: posicionar dron en el inicio
		}
	}
	
// INICIALIZACIÓN DEL MOVIMIENTO DE SILLAS (al presionar 'B')
// ===========================================================
// Este bloque configura todas las posiciones, rotaciones y trayectorias iniciales para las sillas. Agrupa las sillas
// en diferentes zonas del pasillo y escritorios según su índice.

	if (keys[GLFW_KEY_B] && !sillaActiva) {
		sillaActiva = true;  // Se activa el sistema de animación

		for (int i = 0; i < numSillas; i++) {
			// --------- Cálculo de fila y columna ---------
			int fila = i / sillasPorFila;
			int columna = i % sillasPorFila;

			// --------- Configuración de posición y rotación inicial ---------

			// Posiciones iniciales: todas las sillas entran desde la izquierda,
			// separadas ligeramente por columnas
			posicionesIniciales[i] = glm::vec3(-23.7f + columna * 0.25f, -1.2f, 20.2f);

			// Desplazamiento relativo inicial (sin movimiento aún)
			sillaPosArr[i] = glm::vec3(0.0f);

			// Rotaciones iniciales (sin giro aún)
			sillaRotArr[i] = 0.0f;
			rotSillaArr[i] = 0.0f;

			// Inicializamos el primer movimiento de la primera silla
			sillaAnimArr[0] = 1;

			// --------- Destinos y objetivos de rotación ---------

			// Destino intermedio: llegar al pasillo central (eje X = 0.0f)
			destinoIntermedio[i] = glm::vec3(0.0f, -1.2f, posicionesIniciales[i].z);

			// Giro hacia el pasillo (90° a la izquierda)
			rotacionObjetivo[i] = 90.0f;

			// Avance final en Z hasta el fondo del pasillo (-23.0f)
			destinoFinalAvance[i] = glm::vec3(destinoIntermedio[i].x, -1.2f, -23.0f);

			// Rotación final después del avance por el pasillo
			rotacionFinalAjuste[i] = 180.0f;

			// Rotación lateral final al posicionarse
			rotacionPostFinal[i] = 270.0f;

			// ===========================================================
			// AGRUPAMIENTO DE SILLAS EN DIFERENTES ZONAS DEL SALÓN
			// ===========================================================
			// Cada grupo corresponde a un escritorio o posición distinta
			// y se acomoda a la izquierda o derecha del pasillo.
			// ===========================================================

			if (i < 4) {
				// 🔴 Grupo 1 - izquierda (fondo del salón)
				posicionesFinales[i] = glm::vec3(-12.9f + i * 3.0f, -1.2f, -23.0f);
				rotacionFinalAjuste[i] = 180.0f;
				rotacionPostFinal[i] = 270.0f;
			}
			else if (i >= 4 && i < 8) {
				// Grupo 1 - derecha (fondo del salón)
				posicionesFinales[i] = glm::vec3(12.9f - (i - 4) * 3.0f, -1.2f, -23.0f);
				rotacionFinalAjuste[i] = 0.0f;
				rotacionPostFinal[i] = -90.0f;
			}
			else if (i >= 8 && i < 12) {
				// Grupo 2 - izquierda (Z = -16.5)
				posicionesFinales[i] = glm::vec3(-12.9f + (i - 8) * 3.0f, -1.2f, -16.5f);
				rotacionFinalAjuste[i] = 180.0f;
				rotacionPostFinal[i] = 270.0f;
			}
			else if (i >= 12 && i < 16) {
				// Grupo 2 - derecha
				posicionesFinales[i] = glm::vec3(12.9f - (i - 12) * 3.0f, -1.2f, -16.5f);
				rotacionFinalAjuste[i] = 0.0f;
				rotacionPostFinal[i] = -90.0f;
			}
			else if (i >= 16 && i < 20) {
				// Grupo 3 - izquierda (Z = -9.8)
				posicionesFinales[i] = glm::vec3(-12.9f + (i - 16) * 3.0f, -1.2f, -9.8f);
				rotacionFinalAjuste[i] = 180.0f;
				rotacionPostFinal[i] = 270.0f;
			}
			else if (i >= 20 && i < 24) {
				// Grupo 3 - derecha
				posicionesFinales[i] = glm::vec3(12.9f - (i - 20) * 3.0f, -1.2f, -9.8f);
				rotacionFinalAjuste[i] = 0.0f;
				rotacionPostFinal[i] = -90.0f;
			}
			else if (i >= 24 && i < 28) {
				// Grupo 4 - izquierda (Z = -4.1)
				posicionesFinales[i] = glm::vec3(-12.9f + (i - 24) * 3.0f, -1.2f, -4.1f);
				rotacionFinalAjuste[i] = 180.0f;
				rotacionPostFinal[i] = 270.0f;
			}
			else if (i >= 28 && i < 32) {
				// Grupo 4 - derecha
				posicionesFinales[i] = glm::vec3(12.9f - (i - 28) * 3.0f, -1.2f, -4.1f);
				rotacionFinalAjuste[i] = 0.0f;
				rotacionPostFinal[i] = -90.0f;
			}
			else if (i >= 32 && i < 36) {
				// Grupo 5 - izquierda (Z = 2.6)
				posicionesFinales[i] = glm::vec3(-12.9f + (i - 32) * 3.0f, -1.2f, 2.6f);
				rotacionFinalAjuste[i] = 180.0f;
				rotacionPostFinal[i] = 270.0f;
			}
			else if (i >= 36 && i < 40) {
				// Grupo 5 - derecha
				posicionesFinales[i] = glm::vec3(12.9f - (i - 36) * 3.0f, -1.2f, 2.6f);
				rotacionFinalAjuste[i] = 0.0f;
				rotacionPostFinal[i] = -90.0f;
			}
			else if (i >= 40 && i < 44) {
				// Grupo 6 - izquierda (Z = 8.3)
				posicionesFinales[i] = glm::vec3(-12.9f + (i - 40) * 3.0f, -1.2f, 8.3f);
				rotacionFinalAjuste[i] = 180.0f;
				rotacionPostFinal[i] = 270.0f;
			}
			else if (i >= 44 && i < 48) {
				// Grupo 6 - derecha
				posicionesFinales[i] = glm::vec3(12.9f - (i - 44) * 3.0f, -1.2f, 8.3f);
				rotacionFinalAjuste[i] = 0.0f;
				rotacionPostFinal[i] = -90.0f;
			}
			else if (i >= 48 && i < 52) {
				// Grupo 7 - izquierda (Z = 14.8)
				posicionesFinales[i] = glm::vec3(-12.9f + (i - 48) * 3.0f, -1.2f, 14.8f);
				rotacionFinalAjuste[i] = 180.0f;
				rotacionPostFinal[i] = 270.0f;
			}
			else if (i >= 52 && i < 56) {
				// Grupo 7 - derecha
				posicionesFinales[i] = glm::vec3(12.9f - (i - 52) * 3.0f, -1.2f, 14.8f);
				rotacionFinalAjuste[i] = 0.0f;
				rotacionPostFinal[i] = -90.0f;
			}
		}

		// Iniciar el movimiento de la primera silla
		sillaAnimArr[0] = 1;
	}


}
void Animation() {
	// --------- ANIMACIÓN DE LA SILLA EN ORDEN ----------
	if (sillaActiva) {
		for (int i = 0; i < numSillas; i++) {
			switch (sillaAnimArr[i]) {

			case 0: // Espera secuencial
				if (i == 0 || sillaAnimArr[i - 1] >= 2)
					sillaAnimArr[i] = 1;
				break;

			case 1: // Movimiento en X hacia el pasillo
				if (sillaPosArr[i].x < destinoIntermedio[i].x - posicionesIniciales[i].x)
					sillaPosArr[i].x += 0.18f;
				else
					sillaAnimArr[i] = 2;
				break;

			case 2: // Primer giro (todos hacia 90°)
				if (sillaRotArr[i] < rotacionObjetivo[i])
					sillaRotArr[i] += 0.55f;
				else {
					sillaRotArr[i] = rotacionObjetivo[i];
					sillaAnimArr[i] = 3;
				}
				break;

			case 3:
				if (sillaPosArr[i].z > posicionesFinales[i].z - posicionesIniciales[i].z)
					sillaPosArr[i].z -= 0.18f;
				else {
					sillaPosArr[i].z = posicionesFinales[i].z - posicionesIniciales[i].z;
					sillaRotArr[i] = 90.0f;
					sillaAnimArr[i] = 4;
				}
				break;

			case 4:
				if (i < 4) {
					// izquierda fondo
					if (sillaRotArr[i] < rotacionFinalAjuste[i])
						sillaRotArr[i] += 0.55f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 5;
					}
				}
				else if (i >= 4 && i < 8) {
					// derecha fondo
					if (sillaRotArr[i] > rotacionFinalAjuste[i] + 0.1f)
						sillaRotArr[i] -= 0.55f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 10;
					}
				}
				else if (i >= 8 && i < 12) {
					// grupo 2
					if (sillaRotArr[i] < rotacionFinalAjuste[i])
						sillaRotArr[i] += 0.55f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 5; 
					}
				}
				else if (i >= 12 && i < 16) {
					// grupo 2 derecho
					if (sillaRotArr[i] > rotacionFinalAjuste[i] + 0.1f)
						sillaRotArr[i] -= 0.55f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 10;  
					}
				}
				else if (i >= 16 && i < 20) {
					// Grupo 3 izquierdo
					if (sillaRotArr[i] < rotacionFinalAjuste[i])
						sillaRotArr[i] += 0.55f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 5;  // 
					}
				}
				else if (i >= 20 && i < 24) {
					// Grupo 3 derecho
					if (sillaRotArr[i] > rotacionFinalAjuste[i] + 0.1f)
						sillaRotArr[i] -= 0.55f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 10;  // 
					}
				}
				else if (i >= 24 && i < 28) {
					// Grupo 4 izquierda
					if (sillaRotArr[i] < rotacionFinalAjuste[i])
						sillaRotArr[i] += 0.55f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 5;
					}
				}
				else if (i >= 28 && i < 32) {
					// Grupo 4 derecha
					if (sillaRotArr[i] > rotacionFinalAjuste[i] + 0.1f)
						sillaRotArr[i] -= 0.55f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 10;
					}
				}
				else if (i >= 32 && i < 36) {
					if (sillaRotArr[i] < rotacionFinalAjuste[i])
						sillaRotArr[i] += 0.55f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 5;  // Izquierda
					}
				}
				else if (i >= 36 && i < 40) {
					if (sillaRotArr[i] > rotacionFinalAjuste[i] + 0.1f)
						sillaRotArr[i] -= 0.55f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 10; // Derecha
					}
				}
				else if (i >= 40 && i < 44) {
					if (sillaRotArr[i] < rotacionFinalAjuste[i])
						sillaRotArr[i] += 0.55f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 5;  // Izquierda
					}
				}
				else if (i >= 44 && i < 48) {
					if (sillaRotArr[i] > rotacionFinalAjuste[i] + 0.1f)
						sillaRotArr[i] -= 0.55f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 10;  // Derecha
					}
				}
				else if (i >= 48 && i < 52) {
					if (sillaRotArr[i] < rotacionFinalAjuste[i])
						sillaRotArr[i] += 0.55f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 5;  // avanzar a la izquierda
					}
				}
				else if (i >= 52 && i < 56) {
					if (sillaRotArr[i] > rotacionFinalAjuste[i] + 0.1f)
						sillaRotArr[i] -= 0.55f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 10;  // avanzar a la derecha
					}
				}
				break;


			case 5:
				if (i < 4 || (i >= 8 && i < 12) || (i >= 16 && i < 20) || (i >= 24 && i < 28) || (i >= 32 && i < 36) || (i >= 40 && i < 44) || (i >= 48 && i < 52)) {

					float destinoX = posicionesFinales[i].x - posicionesIniciales[i].x;

					if (sillaPosArr[i].x < destinoX)
						sillaPosArr[i].x += 0.18f;
					else {
						sillaPosArr[i].x = destinoX;
						sillaAnimArr[i] = 6;
					}
				}
				break;

			case 6:
			{
				float diff = rotacionPostFinal[i] - sillaRotArr[i];

				if (fabs(diff) > 0.5f) {
					if (diff > 0)
						sillaRotArr[i] += 0.55f;
					else
						sillaRotArr[i] -= 0.55f;
				}
				else {
					sillaRotArr[i] = rotacionPostFinal[i];
					sillaAnimArr[i] = 7;
				}
				break;
			}


			case 7:
				break;

			case 10:
				if ((i >= 4 && i < 8) || (i >= 12 && i < 16) || (i >= 20 && i < 24) || (i >= 28 && i < 32) || (i >= 36 && i < 40) || (i >= 44 && i < 48) || (i >= 52 && i < 56)) {
					float destinoX = posicionesFinales[i].x - posicionesIniciales[i].x;

					if (sillaPosArr[i].x < destinoX)
						sillaPosArr[i].x += 0.18f;
					else {
						sillaPosArr[i].x = destinoX;
						sillaAnimArr[i] = 11;
					}
				}
				break;

			case 11: // Giro lateral final (grupo derecho)
			{
				float diff = rotacionPostFinal[i] - sillaRotArr[i]; //

				if (fabs(diff) > 0.5f) {
					if (diff > 0)
						sillaRotArr[i] += 0.55f;
					else
						sillaRotArr[i] -= 0.55f;
				}
				else {
					sillaRotArr[i] = rotacionPostFinal[i];
					sillaAnimArr[i] = 12; // Finalizado
				}
				break;
			}

			case 12:
				// Animación final completa
				break;
			}

		}
	}

	// --------- ANIMACIÓN DE LA PELOTA (BALL/DRON) ----------
	if (AnimBall) {
		if (sinusoidalMode) {
			// Movimiento sinusoidal
			sineTime += deltaTime;

			dronPos.x = initialSinePos.x + sin(sineTime * sineFrequency) * sineAmplitude;
			dronPos.z = initialSinePos.z + speed * sineTime;

			float velocityX = cos(sineTime * sineFrequency) * sineFrequency * sineAmplitude;
			float velocityZ = speed;
			float angle = atan2(velocityX, velocityZ);
			rotBall = glm::degrees(angle);
		}
		else {
			// Movimiento rectangular
			glm::vec3 target = waypoints[currentWaypoint];
			glm::vec3 direction = target - dronPos;
			float distanceToTarget = glm::length(direction);

			if (distanceToTarget > 0.1f) {
				direction = glm::normalize(direction);
				dronPos += direction * speed * deltaTime;
				float angle = atan2(direction.x, direction.z);
				rotBall = glm::degrees(angle);
			}
			else {
				currentWaypoint = (currentWaypoint + 1) % 4;
			}
		}
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}
