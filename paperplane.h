//
//  paperplane.h
//  PaperPlane
//
//  Created by Shivanker Goel on 25/10/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#ifndef PaperPlane_paperplane_h
#define PaperPlane_paperplane_h


/* Standard C/C++ includes */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <cstdarg>
#include <iostream>
using namespace std;


/* OpenGL libraries */
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#endif

/* GLM */
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

/* Project Libraries */
#include "ppm.h"
#include "terrain.h"

#endif
