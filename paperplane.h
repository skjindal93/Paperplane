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
#include <string>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <vector>
#include <ctime>
#include <cstdarg>
#include <ctime>
#include <iostream>
#include <fstream>
#include <limits>
using namespace std;


/* OpenGL libraries */
#ifdef __APPLE__
#include <GLUT/glut.h>
#define PATH "/Users/shivanker/Workplace/V Semester/Graphics/PaperPlane/PaperPlane/"

#else
#ifdef _WIN32
#include <windows.h>
#include <GL/freeglut.h>
#include <gl/glext.h>
#define PATH "paperplane\\"

#else
#include <GL/glut.h>
#define PATH ""

#endif
#endif

/* GLM */
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

/* Project Libraries */
#include "image.h"
#include "terrain.h"
#include "object.h"

#endif
