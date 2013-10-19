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
//	glm::vec3** normals2 = new Vec3f*[l];
//	for(int i = 0; i < l; i++) {
//		normals2[i] = new Vec3f[w];
//	}
//	
//	for(int z = 0; z < l; z++) {
//		for(int x = 0; x < w; x++) {
//			Vec3f sum(0.0f, 0.0f, 0.0f);
//			
//			Vec3f out;
//			if (z > 0) {
//				out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
//			}
//			Vec3f in;
//			if (z < l - 1) {
//				in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
//			}
//			Vec3f left;
//			if (x > 0) {
//				left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
//			}
//			Vec3f right;
//			if (x < w - 1) {
//				right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
//			}
//			
//			if (x > 0 && z > 0) {
//				sum += out.cross(left).normalize();
//			}
//			if (x > 0 && z < l - 1) {
//				sum += left.cross(in).normalize();
//			}
//			if (x < w - 1 && z < l - 1) {
//				sum += in.cross(right).normalize();
//			}
//			if (x < w - 1 && z > 0) {
//				sum += right.cross(out).normalize();
//			}
//			
//			normals2[z][x] = sum;
//		}
}

Terrain::~Terrain()	{
	for (int i = 0; i < w; ++i) {
		delete[] heights[i];
		delete[] normals[i];
	}
	delete[] heights;
	delete[] normals;
}

