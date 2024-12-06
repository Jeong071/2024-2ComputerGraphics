#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include<gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>


#include "Initialize.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Mesh.h"
#include "Model.h"
#include "InputManager.h"

extern GLuint shaderProgramID;
extern GLuint vertexShader;
extern GLuint fragmentShader;



extern Model gModel;
extern InputManager gInput;
extern Camera gCamera;

#define PLAYER_SPEED	0.08
#define WIDTH			1280
#define HEIGHT			960

