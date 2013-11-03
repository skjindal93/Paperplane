//
//  terrain.cpp
//  PaperPlane
//
//  Created by Shivanker Goel on 19/10/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#include "paperplane.h"

Terrain::Terrain(Image* img, const char* texFile)	{
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
	if(texFile)
		texture = loadTexture(texFile);
	
	buildArrayBuffers();
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
			normals[x][z] = sum;
		}
}

void Terrain::buildArrayBuffers()	{
	ntri = (w-1)*(h-1)*2;
	nvert = ntri*3;
	
	glm::vec3 *vertexArray, *normalArray;
	vertexArray = new glm::vec3[nvert];
	normalArray = new glm::vec3[nvert];

	glm::vec2 *textureArray;
	textureArray = new glm::vec2[nvert];
	
	for(int i = 0; i < ntri; ++i)	{
		int z = i / (2*(w-1));
		int x = (i % (2*(w-1)))/2;
		int vx[3] = {x, x+(i&1), x+1}, vz[3] = {z+1, z+(i&1), z};
		
		int bv = i*3;
		for(int j = 0; j < 3; ++j)	{
			int v = bv + j;
			vertexArray[v].x = vx[j];
			vertexArray[v].y = heights[vx[j]][vz[j]];
			vertexArray[v].z = vz[j];
			
			textureArray[v].x = vertexArray[v].x / w;
			textureArray[v].y = vertexArray[v].z / h;
			
			normalArray[v].x = normals[vx[j]][vz[j]][0];
			normalArray[v].y = normals[vx[j]][vz[j]][1];
			normalArray[v].z = normals[vx[j]][vz[j]][2];
		}
	}
	
	glGenBuffers(1, &vertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, nvert * 3 * sizeof(float), vertexArray, GL_STATIC_DRAW);

	glGenBuffers(1, &textureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
	glBufferData(GL_ARRAY_BUFFER, nvert * 2 * sizeof(float), textureArray, GL_STATIC_DRAW);
	
	glGenBuffers(1, &normalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, nvert * 3 * sizeof(float), normalArray, GL_STATIC_DRAW);
	
	delete[] vertexArray;
	delete[] normalArray;
	delete[] textureArray;
}

void Terrain::render(GLfloat height, GLfloat size, GLfloat starting, GLfloat fraction)	{
	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	
	glTranslatef(-size/2, 0.0f, 0.0f);
	float scale = size / max(w - 1, h - 1);
    glScalef(scale, height, scale);
	
	if(texture)	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}
	
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glNormalPointer(GL_FLOAT, 0, NULL);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	
	while(fraction > 0.0f)	{
		
		// Display terrain starting at starting% and fraction% of the total.
		// {(nvert*starting) - (nvert*starting) % 3} because we must start at a multiple of 3
		glDrawArrays( GL_TRIANGLES,
					  3 * ((int)(nvert * starting) / 3),
					  3 * ((int)(nvert * min(fraction, 1.0f - starting)) / 3));
		
		glTranslatef(0.0f, 0.0f, size / scale);
		fraction -= 1.0f - starting;
		starting = 0.0f;
		
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	if(texture)	{
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	
	// TODO: Can we actually do away with per-vertex normals?
	glPopAttrib();
	glPopMatrix();
}

Terrain::~Terrain()	{
	
	if(texture)
		glDeleteTextures( 1, &texture );
	if(textureVBO)
		glDeleteBuffers(1, &textureVBO);
	if(vertexVBO)
		glDeleteBuffers(1, &vertexVBO);
	if(normalVBO)
		glDeleteBuffers(1, &normalVBO);
	
	for (int i = 0; i < w; ++i) {
		delete[] heights[i];
		delete[] normals[i];
	}
	delete[] heights;
	delete[] normals;
}

