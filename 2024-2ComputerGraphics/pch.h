#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h> //--- ÇÊ¿äÇÑ Çì´õÆÄÀÏ include
#include<gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <filesystem>

#include "Timer.h"
#include "Initialize.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Mesh.h"
#include "Model.h"
#include "InputManager.h"
#include "Player.h"

#include "Renderer.h"

#include "Timer.h"


extern GLuint shaderProgramID;
extern GLuint vertexShader;
extern GLuint fragmentShader;



extern Model gModel;
extern InputManager gInput;
extern Camera gCamera;
extern Player gPlayer;
extern Renderer gRenderer;
extern Timer gTimer;

extern bool isVirtualMouse;
extern float playerAngle;
extern glm::mat4 rotationMatrix;
extern glm::vec3 rotationAxis;
extern float rotateYAxis;

enum SceneType {
    MAIN_MENU,
    GAME_PLAY,
    
};

extern SceneType currentScene;

#define PLAYER_SPEED	0.025 //0.025
#define WIDTH			1280
#define HEIGHT			960
#define SENSITIVITY		0.15
#define DISTANCE		0.5
#define FLOOR			0.3
#define KEY_COUNT		256