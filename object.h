//
//  object.h
//  PaperPlane
//
//  Created by Shivanker Goel and Shubham Jindal on 31/10/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#ifndef __PaperPlane__object__
#define __PaperPlane__object__

#include "paperplane.h"

class Material	{
public:
	GLfloat s;
	glm::vec3 Ka, Kd, Ks, Ke;
	char* name;
	
	Material();
	void apply();
	~Material();
};

vector<Material>* readMTL(string file);

struct triangle	{
	glm::ivec3 v, t, n;
};

class Object	{
public:
	bool loaded, useTex, useNormal;
	GLuint texture, vertexVBO, textureVBO, normalVBO;
	vector<glm::vec3> normals, vertices;
	vector<glm::vec2> uvs;
	vector<triangle> triangles;
	glm::mat4 *modelView;
	Material *mtl;
	char* name;
	
	Object();
	
	void load();
	bool collision(Object *other);
	void render(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ, bool load = false);
	void render(GLfloat scale = 1.0f, bool load = false);
	void unload();
	
	~Object();

};

class ObjectGroup	{
public:
	vector<Object> objList;
	string name;
	int timesUsed;
	bool save;
	glm::mat4 *modelView;
	
	ObjectGroup();
	ObjectGroup(string file);

	void readOBJ(string file);
	
	void load();
	bool collision(ObjectGroup *other);
	void render(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ, bool load = false);
	void render(GLfloat scale = 1.0f, bool load = false);
	void unload();

};

bool triangleIntersect(glm::vec3 a[3], glm::vec3 b[3]);

#endif /* defined(__PaperPlane__object__) */
