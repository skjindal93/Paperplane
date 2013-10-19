//
//  terrain.h
//  PaperPlane
//
//  Created by Shivanker Goel on 19/10/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#ifndef __PaperPlane__terrain__
#define __PaperPlane__terrain__

#include <iostream>
using namespace std;

#include "ppm.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Terrain {
public:
	int w, h;
	float** heights;
	glm::vec3** normals;

	Terrain(Image*);
	void computeNormals();
	
	~Terrain();
};


#endif /* defined(__PaperPlane__terrain__) */
