////////////////////////////////////////////////////////////////////////////////
// \author   Jonathan Dupuy
//
////////////////////////////////////////////////////////////////////////////////

// gui
#define _ANT_ENABLE

// GL libraries
#include "glew.hpp"
#include "GL/freeglut.h"

#ifdef _ANT_ENABLE
#	include "AntTweakBar.h"
#endif // _ANT_ENABLE

// Custom libraries
#include "Algebra.hpp"      // Basic algebra library
#include "Transform.hpp"    // Basic transformations
#include "Framework.hpp"    // utility classes/functions

// Standard librabries
#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <cmath>


////////////////////////////////////////////////////////////////////////////////
// Global variables
//
////////////////////////////////////////////////////////////////////////////////

// Constants
const float PI   = 3.14159265;
const float FOVY = PI*0.5f;

enum {
	// buffers
	BUFFER1 = 0,
	BUFFER2,
	BUFFER3,
	BUFFER_COUNT,

	// vertex arrays
	VERTEX_ARRAY_ONE = 0,
	VERTEX_ARRAY_COUNT,

	// samplers
	SAMPLER_ONE = 0,
	SAMPLER_COUNT,

	// textures
	TEXTURE_ONE = 0,
	TEXTURE_COUNT,

	// programs
	PROGRAM_ONE = 0,
	PROGRAM_COUNT
};

// OpenGL objects
GLuint *buffers      = NULL;
GLuint *vertexArrays = NULL;
GLuint *textures     = NULL;
GLuint *samplers     = NULL;
GLuint *programs     = NULL;

bool mouseLeft  = false;
bool mouseRight = false;
GLfloat deltaTicks = 0.0f;

#ifdef _ANT_ENABLE
GLfloat speed = 0.0f; // app speed (in ms)
#endif

////////////////////////////////////////////////////////////////////////////////
// Functions
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _ANT_ENABLE

static void TW_CALL toggle_fullscreen(void *data) {
	glutFullScreenToggle();
}

#endif

struct Vertex {
	Vertex() {}
	Vertex(float px, float py, float pz, 
	       float nx, float ny, float nz, 
	       float tx, float ty, float tz) {
		x = px;
		y = py;
		z = pz;
	}

	float x, y, z;
};

////////////////////////////////////////////////////////////////////////////////
// on init cb
void on_init() {
	// alloc names
	buffers      = new GLuint[BUFFER_COUNT];
	vertexArrays = new GLuint[VERTEX_ARRAY_COUNT];
	textures     = new GLuint[TEXTURE_COUNT];
	samplers     = new GLuint[SAMPLER_COUNT];
	programs     = new GLuint[PROGRAM_COUNT];

	fw::init_debug_output(std::cout);

	// gen names
	glGenBuffers(BUFFER_COUNT, buffers);
	glGenVertexArrays(VERTEX_ARRAY_COUNT, vertexArrays);
	glGenTextures(TEXTURE_COUNT, textures);
	glGenSamplers(SAMPLER_COUNT, samplers);
	for(GLuint i=0; i<PROGRAM_COUNT;++i)
		programs[i] = glCreateProgram();

	std::vector<std::string> keyframes;
#if 0
	keyframes.push_back("explode00.tga");
	keyframes.push_back("explode01.tga");
	keyframes.push_back("explode02.tga");
	keyframes.push_back("explode03.tga");
	keyframes.push_back("explode04.tga");
	keyframes.push_back("explode05.tga");
#else
	keyframes.push_back("run0.tga");
	keyframes.push_back("run1.tga");
	keyframes.push_back("run2.tga");
	keyframes.push_back("run3.tga");
#endif
	glActiveTexture(GL_TEXTURE0+TEXTURE_ONE);
	glBindTexture(GL_TEXTURE_3D, textures[TEXTURE_ONE]);
		fw::tex_tga_sprites_image3D(keyframes, GL_TRUE, GL_TRUE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	fw::build_glsl_program(programs[PROGRAM_ONE], "render.glsl", "", GL_TRUE);
	glUseProgram(programs[PROGRAM_ONE]);
	glUniform1i(glGetUniformLocation(programs[PROGRAM_ONE], "sTexture"),
	            TEXTURE_ONE);
	glUseProgram(0);

	glBindVertexArray(vertexArrays[VERTEX_ARRAY_ONE]);
	glBindVertexArray(0);

	glClearColor(0,1,0,0);

#ifdef _ANT_ENABLE
	// start ant
	TwInit(TW_OPENGL_CORE, NULL);
	// send the ''glutGetModifers'' function pointer to AntTweakBar
	TwGLUTModifiersFunc(glutGetModifiers);

	// Create a new bar
	TwBar* menuBar = TwNewBar("menu");
	TwDefine("menu size='220 170'");
	TwDefine("menu position='0 0'");
	TwDefine("menu alpha='255'");
	TwDefine("menu valueswidth=85");

	TwAddVarRO(menuBar,
	           "speed (ms)",
	           TW_TYPE_FLOAT,
	           &speed,
	           "");

	TwAddButton( menuBar,
	             "fullscreen",
	             &toggle_fullscreen,
	             NULL,
	             "label='toggle fullscreen'");

#endif // _ANT_ENABLE
	fw::check_gl_error();
}


////////////////////////////////////////////////////////////////////////////////
// on clean cb
void on_clean() {
	// delete objects
	glDeleteBuffers(BUFFER_COUNT, buffers);
	glDeleteVertexArrays(VERTEX_ARRAY_COUNT, vertexArrays);
	glDeleteTextures(TEXTURE_COUNT, textures);
	glDeleteSamplers(SAMPLER_COUNT, samplers);
	for(GLuint i=0; i<PROGRAM_COUNT;++i)
		glDeleteProgram(programs[i]);

	// release memory
	delete[] buffers;
	delete[] vertexArrays;
	delete[] textures;
	delete[] samplers;
	delete[] programs;

#ifdef _ANT_ENABLE
	TwTerminate();
#endif // _ANT_ENABLE

	fw::check_gl_error();
}


////////////////////////////////////////////////////////////////////////////////
// on update cb
void on_update() {
	// Variables
	static fw::Timer deltaTimer;
//	GLint windowWidth  = glutGet(GLUT_WINDOW_WIDTH);
//	GLint windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

	// stop timing and set delta
	deltaTimer.Stop();
	deltaTicks = deltaTimer.Ticks();
#ifdef _ANT_ENABLE
	speed = deltaTicks*1000.0f;
#endif

	// set viewport
	glViewport(100,50,128,128);

	// clear back buffer
	glClear(GL_COLOR_BUFFER_BIT);

	static GLfloat test = 0.0f;
	test+= deltaTicks*4.5f;
	test = test - std::floor(test);
	glBindVertexArray(vertexArrays[VERTEX_ARRAY_ONE]);
	glUseProgram(programs[PROGRAM_ONE]);
	glUniform1f(glGetUniformLocation(programs[PROGRAM_ONE], "uLayer"),
	            (test));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glUseProgram(0);

	// start ticking
	deltaTimer.Start();

#ifdef _ANT_ENABLE
//	TwDraw();
#endif // _ANT_ENABLE

	fw::check_gl_error();

	glutSwapBuffers();
	glutPostRedisplay();
}


////////////////////////////////////////////////////////////////////////////////
// on resize cb
void on_resize(GLint w, GLint h) {
#ifdef _ANT_ENABLE
	TwWindowSize(w, h);
#endif
}


////////////////////////////////////////////////////////////////////////////////
// on key down cb
void on_key_down(GLubyte key, GLint x, GLint y) {
#ifdef _ANT_ENABLE
	if(1==TwEventKeyboardGLUT(key, x, y))
		return;
#endif
	if (key==27) // escape
		glutLeaveMainLoop();
	if(key=='f')
		glutFullScreenToggle();
	if(key=='p')
		fw::save_gl_front_buffer(0,
		                         0,
		                         glutGet(GLUT_WINDOW_WIDTH),
		                         glutGet(GLUT_WINDOW_HEIGHT));

}


////////////////////////////////////////////////////////////////////////////////
// on mouse button cb
void on_mouse_button(GLint button, GLint state, GLint x, GLint y) {
#ifdef _ANT_ENABLE
	if(1 == TwEventMouseButtonGLUT(button, state, x, y))
		return;
#endif // _ANT_ENABLE
	if(state==GLUT_DOWN) {
		mouseLeft  |= button == GLUT_LEFT_BUTTON;
		mouseRight |= button == GLUT_RIGHT_BUTTON;
	}
	else {
		mouseLeft  &= button == GLUT_LEFT_BUTTON ? false : mouseLeft;
		mouseRight  &= button == GLUT_RIGHT_BUTTON ? false : mouseRight;
	}
}


////////////////////////////////////////////////////////////////////////////////
// on mouse motion cb
void on_mouse_motion(GLint x, GLint y) {
#ifdef _ANT_ENABLE
	if(1 == TwEventMouseMotionGLUT(x,y))
		return;
#endif // _ANT_ENABLE

	static GLint sMousePreviousX = 0;
	static GLint sMousePreviousY = 0;
	const GLint MOUSE_XREL = x-sMousePreviousX;
	const GLint MOUSE_YREL = y-sMousePreviousY;
	sMousePreviousX = x;
	sMousePreviousY = y;
}


////////////////////////////////////////////////////////////////////////////////
// on mouse wheel cb
void on_mouse_wheel(GLint wheel, GLint direction, GLint x, GLint y) {
#ifdef _ANT_ENABLE
	if(1 == TwMouseWheel(wheel))
		return;
#endif // _ANT_ENABLE
}


////////////////////////////////////////////////////////////////////////////////
// Main
//
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) {
	const GLuint CONTEXT_MAJOR = 3;
	const GLuint CONTEXT_MINOR = 3;

	// init glut
	glutInit(&argc, argv);
	glutInitContextVersion(CONTEXT_MAJOR ,CONTEXT_MINOR);
	glutInitContextFlags(GLUT_DEBUG | GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);


	// build window
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("OpenGL");

	// init glew
	glewExperimental = GL_TRUE; // segfault on GenVertexArrays on Nvidia otherwise
	GLenum err = glewInit();
	if(GLEW_OK != err) {
		std::stringstream ss;
		ss << err;
		std::cerr << "glewInit() gave error " << ss.str() << std::endl;
		return 1;
	}

	// glewInit generates an INVALID_ENUM error for some reason...
	glGetError();

	// set callbacks
	glutCloseFunc(&on_clean);
	glutReshapeFunc(&on_resize);
	glutDisplayFunc(&on_update);
	glutKeyboardFunc(&on_key_down);
	glutMouseFunc(&on_mouse_button);
	glutPassiveMotionFunc(&on_mouse_motion);
	glutMotionFunc(&on_mouse_motion);
	glutMouseWheelFunc(&on_mouse_wheel);

	// run
	try {
		// run demo
		on_init();
		glutMainLoop();
	}
	catch(std::exception& e) {
		std::cerr << "Fatal exception: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}

