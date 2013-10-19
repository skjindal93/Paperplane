//
//  terrain.cpp
//  PaperPlane
//
//  Created by Shivanker Goel on 19/10/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#include "terrain.h"

Terrain::Terrain(Image* img)	{
    w = img->w;
	h = img->h;
	
	// Initialize vectors
	heights = new float*[w];
	for(int i = 0; i < w; i++)
		heights[i] = new float[h];

	// Initialize vectors
	normals = new glm::vec3*[w];
	for(int i = 0; i < w; i++)
		normals[i] = new glm::vec3[h];
	
    for(int i = 0; i < w; ++i)
		for(int j = 0; j < h; ++j)
			heights[i][j] = (img->pixel[i][j][0] + img->pixel[i][j][1] + img->pixel[i][j][2])/3;
    
	computeNormals();
}

void Terrain::computeNormals()	{
	glm::vec3** normals2 = new glm::vec3*[w];
	for(int i = 0; i < w; i++) {
		normals2[i] = new glm::vec3[h];
	}
	
	for(int z = 0; z < h; z++)
		for(int x = 0; x < w; x++) {
			glm::vec3 sum(0.0f, 0.0f, 0.0f);
			
			glm::vec3 out(0.0f);
			if (z > 0)
				out = glm::vec3(0.0f, heights[x][z - 1] - heights[x][z], -1.0f);
			
			glm::vec3 in(0.0f);
			if (z < h - 1)
				in = glm::vec3(0.0f, heights[x][z + 1] - heights[x][z], 1.0f);
			
			glm::vec3 left(0.0f);
			if (x > 0)
				left = glm::vec3(-1.0f, heights[x - 1][z] - heights[x][z], 0.0f);
			
			glm::vec3 right(0.0f);
			if (x < w - 1)
				right = glm::vec3(1.0f, heights[x + 1][z] - heights[x][z], 0.0f);
			
			
			if (x > 0 && z > 0)
				sum += glm::normalize(glm::cross(out, left));
			if (x > 0 && z < h - 1)
				sum += glm::normalize(glm::cross(left, in));
			if (x < w - 1 && z < h - 1)
				sum += glm::normalize(glm::cross(in, right));
			if (x < w - 1 && z > 0)
				sum += glm::normalize(glm::cross(right, out));
			
			normals2[x][z] = sum;
		}
		
	// Normal Anti-aliasing
	const float FALLOUT_RATIO = 0.3f;
	for(int z = 0; z < h; z++)
		for(int x = 0; x < w; x++) {
			glm::vec3 sum = normals2[x][z];
			
			if (x > 0)
				sum += normals2[x - 1][z] * FALLOUT_RATIO;
			if (x < w - 1)
				sum += normals2[x + 1][z] * FALLOUT_RATIO;
			if (z > 0)
				sum += normals2[x][z - 1] * FALLOUT_RATIO;
			if (z < 0)
				sum += normals2[x][z + 1] * FALLOUT_RATIO;
			
			if (glm::length(sum) < 1e-4)
				sum = glm::vec3(0.0f, 1.0f, 0.0f);
			normals[x][z] = glm::normalize(sum);
		}
}

Terrain::~Terrain()	{
	for (int i = 0; i < w; ++i) {
		delete[] heights[i];
		delete[] normals[i];
	}
	delete[] heights;
	delete[] normals;
}

