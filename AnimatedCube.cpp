/*
 * AnimatedCube.cpp
 *
 *  Created on: Dec 14, 2020
 *      Author: 1509742_snhu
 */

/* Headers Inclusions */
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

//GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
//Standard namespace

#define WINDOW_TITLE "Animated Cube" // Window title Macro

/* Shader program macro */
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

/* Variable declarations for shader, wiindow size initialization, buffer and array objects */
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, VAO, EBO, texture;

/* Function Prototypes */
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);

/* Vertex Shader Source Code */
const GLchar * vertexShaderSource = GLSL(330,
		layout (location = 0) i vec3 position; // Vertex data from Vertex Attrib Pointer 0
		layout (location = 1) in vec3 color;// Color data from Vertex Attrib Pointer 1

		out vec3 mobileColor;// variable to transfer color data to the fragment shader

		// Global variables for the transform matrics
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projecttion;

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0f); // transform vertices to clip coordinates
	mobileColor = color;// references incoming color data
	}
);

/* Fragment Shader Source Code */
const GLchar * fragmentShaderSource = GLSL(330,
		in vec3 mobileColor; // Variable to hold incoming color data from veretex shader

		out vec4 gpuColor;// Variable to pass color data to the GPU

		void main() {
			gpuColor = vec4(mobileColor, 1.0); // Sends color data to the GPU for rending
		}
);

//Main Program
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow(WINDOW_TITLE);

	glutReshapeFunc(UResizeWindow);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	UCreateShader();

	UCreateBuffers();

	//Use the Shader Program
	glUseProgram(shaderProgram);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Set background color

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glutDisplayFunc(URenderGraphics);

	glutMainLoop();

	// Destroys Buffer objects once used
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	return 0;
}

/* Resizes the window */
void UResizeWindow(int w, int h) {
	WindowWidth = w;
	WindowHeight = h;
	glViewport(0, 0, WindowWidth, WindowHeight);
}

//Renders Graphics
void URenderGraphics(void) {
	glEnable(GL_DEPTH_TEST); //Enable z-depth

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clears the screen

	glBindVertexArray(VAO); //Activate the vertex array object before rendering and transforming them

	//Transforms the object
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.5, 0.0f, 0.0f)); //Place the object at the center of the viewport

	// Rotate the cube every second by 0.0005f degrees in XYZ
	model = glm::rotate(model, glutGet(GLUT_ELAPSED_TIME) * -0.0005f,
			glm::vec3(0.0, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f)); //Increase the object size by a scale of 2

	//Transforms the camera
	glm::mat4 view;
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f)); //Moves the world 0.5 units on x and -5 units in z

	//creates a perspective projection
	glm::mat4 projection;
	projection = glm::perspective(45.0f,
			(GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);

	//Retrieves and passes transform matrices to the shader program
	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glutPostRedisplay();

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0); // Deactivate the Verteex Array Object

	glutSwapBuffers(); // Flips the back buffer with the front buffer everey frame. Similiar to GL Flush
}

//Creates the shader program
void UCreateShader()
	{
		//Vertex shader
		GLint vertexShader = glCreateShader(GL_VERTEX_SHADER); // creates the vertex shader
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); //Attaches the vertex shader to the source code
		glCompileShader(vertexShader); //compiles the vertex shader

		//Fragment shader
		GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //Creates the fragment shader
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); //Attaches the fragment shader to the source code
		glCompileShader(fragmentShader); //compiles the fragment shader

		//Shader Program
		shaderProgram = glCreateProgram(); //Creates the shader program and returns an id
		glAttachShader(shaderProgram, vertexShader); //Attach vertex shader to the shader program
		glAttachShader(shaderProgram, fragmentShader); //Attach fragment shader to the shader program
		glLinkProgram(shaderProgram); //link vertex and fragment shader to shader program

		//delete the vertex and fragment shaders once linked
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

/* Creates the Buffer and Array Objects */
void UCreateBuffers()
{
	// Position and Color data
	GLfloat vertices[] = {
							// Vertex Position      // Colors
							0.5f,  0.5f, 0.0f, 		1.0f, 0.0f, 0.0f, // Top Right Vertex 0
							0.5f, -0.5f, 0.0f, 		0.0f, 1.0f, 0.0f, // Bottom Right Veerteex 1
						   -0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f, // Bottom Left Vertex 2
						   -0.5f,  0.5f, 0.0f,		1.0f, 0.0f, 1.0f, // Top Left Verteex 3

							0.5f, -0.5f, -1.0f,     0.5f, 0.5f, 1.0f, // 4 br right
							0.5f, 0.5f, -1.0f, 		1.0f, 1.0f, 0.5f, // 5tl right
							-0.5f, 0.5f, -1.0f, 	0.2f, 0.2f, 0.5f, // 6 tl top
							-0.5f, -0.5f, -1.0f, 	1.0f, 0.0f, 1.0f // 7 bl black
						};

// Index data to share position data
	GLuint indices[] = {
							0, 1, 3, // Triangle 1
							1, 2, 3, // Triangle 2
							0, 1, 4, // Traingle 3
							0, 4, 5, // Triangle 4
							0, 5, 6, // Triangle 5
							0, 3, 6, // Triangle 6
							4, 5, 6, // Triangle 7
							4, 6, 7, // Triangle 8
							2, 3, 6, // Triangle 9
							2, 6, 7, // Triangle 10
							1, 4, 7, // Triangle 11
							1, 2, 7 // Triangle 12
					};

	// Generate the buffer ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Activate the Vertex Array Object before binding and setting any VBOs and Vertex Attribute Pointers.
	glBindVertexArray(VAO);

	// Activate the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copy vertices to VBO

	// Activate the Element Buffer Object / Indiicees
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Copy indices to EBO

	// Set attribute pointer 0 to hold Position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); // Enables vertex attribute

	// Set attribute pointer 1 to hold Color data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1); // Enables vertex attribute

	glBindVertexArray(0); // Deactivates the VAO which is good practice
}





