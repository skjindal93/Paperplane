//
//  object.cpp
//  PaperPlane
//
//  Created by Shivanker Goel on 31/10/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#include "paperplane.h"

Material::Material()	{
	s = 0.0f;
	Ka = Kd = Ks = glm::vec3(1.0f);
	name = new char[32];
}

Material::~Material()	{
	delete[] name;
}

void Material::enable()	{
	glPushAttrib(GL_ENABLE_BIT);
	glPushAttrib(GL_LIGHTING_BIT);
	
	glEnable(GL_COLOR_MATERIAL);
	
	apply();
}

void Material::apply()	{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(Ka));
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(Kd));
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(Ks));
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, s);
}

void Material::disable()	{
	glDisable(GL_COLOR_MATERIAL);
	
	glPopAttrib();
	glPopAttrib();
}

vector<Material>* readMTL(const char* file)	{
	vector<Material>* materials;
	materials = new vector<Material>();
	Material *cur = NULL;
	
	// read file
	FILE *mtl = fopen(file, "r");
	if(mtl == NULL)
		cerr << "Could not read file " << file << endl;
	else	{
		while(1)	{
			char type[32];
			int res = fscanf(mtl, "%s", type);
			if(res == EOF)
				break;
			
			if (!strcmp(type, "newmtl"))	{
				
				if(cur)
					materials->push_back(*cur);
				cur = new Material();
				fscanf(mtl, "%31[^\n]", cur->name);
				
			} else if (!strcmp(type, "Ns"))	{
				fscanf(mtl, "%f", &cur->s);
			} else if (!strcmp(type, "Ka"))	{
				fscanf(mtl, "%f %f %f", &cur->Ka.x, &cur->Ka.y, &cur->Ka.z);
			} else if (!strcmp(type, "Kd"))	{
				fscanf(mtl, "%f %f %f", &cur->Kd.x, &cur->Kd.y, &cur->Kd.z);
			} else if (!strcmp(type, "Ks"))	{
				fscanf(mtl, "%f %f %f", &cur->Ks.x, &cur->Ks.y, &cur->Ks.z);
			}
			
			fscanf(mtl, "%*[^\n]");		// skip the rest of the line
			fscanf(mtl, "%*[\n]");		// and the newline
		}
		if(cur)
			materials->push_back(*cur);
	}
	
	return materials;
}


Object::Object()	{
	useTex = useNormal = 0;
	loaded = false;
	texture = vertexVBO = textureVBO = normalVBO = 0;
	normals.clear();
	vertices.clear();
	uvs.clear();
	triangles.clear();
	mtl = NULL;
	name = NULL;
}

Object::~Object()	{
	if(loaded)
		unload();
	if(texture)
		glDeleteTextures( 1, &texture );
}

vector<Object>* readOBJ(const char* file)	{
	vector<Object>* objects;
	objects = new vector<Object>();
	Object *cur = NULL;
	vector<Material> *mtllib;
	
	// read file
	FILE *obj = fopen(file, "r");
	if(obj == NULL)
		cerr << "Could not read file " << file << endl;
	else	{
		while(1)	{
			char type[32];
			int res = fscanf(obj, "%s", type);
			if(res == EOF)
				break;
			
			if (!strcmp(type, "mtllib"))	{
				
				fscanf(obj, "%31[^\n]", type);
				mtllib = readMTL(type);
				
			} else if (!strcmp(type, "o"))	{
				
				if(cur)
					objects->push_back(*cur);
				cur = new Object();
				fscanf(obj, "%31[^\n]", cur->name);
				
			} else if (!strcmp(type, "usemtl"))	{
				
				fscanf(obj, "%31[^\n]", type);
				for(vector<Material>::iterator it = mtllib->begin(); it != mtllib->end(); ++it)
					if(!strcmp(it->name, type))	{
						cur->mtl = &(*it);
						break;
					}
				
			} else if (!strcmp(type, "usetex"))	{
				
				fscanf(obj, "%31[^\n]", type);
				cur->texture = loadTexture(type);
				
			} else if (!strcmp(type, "v"))	{

				glm::vec3 vec;
				fscanf(obj, "%f %f %f", &vec.x, &vec.y, &vec.z);
				cur->vertices.push_back(vec);
				
			} else if (!strcmp(type, "vn"))	{

				glm::vec3 vec;
				fscanf(obj, "%f %f %f", &vec.x, &vec.y, &vec.z);
				cur->normals.push_back(vec);
				
			} else if (!strcmp(type, "vt"))	{
				
				glm::vec2 vec;
				fscanf(obj, "%f %f", &vec.x, &vec.y);
				cur->uvs.push_back(vec);
				
			} else if (!strcmp(type, "f"))	{
				
				cur->useTex = (cur->uvs.size() != 0);
				cur->useNormal = (cur->normals.size() != 0);
				triangle tri;
				if (cur->useTex && cur->useNormal)	{
					int r = fscanf(obj, "%d/%d/%d %d/%d/%d %d/%d/%d", &tri.v[0], &tri.t[0], &tri.n[0], &tri.v[1], &tri.t[1], &tri.n[1], &tri.v[2], &tri.t[2], &tri.n[2]);
					if(r == 9)
						cur->triangles.push_back(tri);
				} else if (cur->useNormal)	{
					int r = fscanf(obj, "%d//%d %d//%d %d//%d", &tri.v[0], &tri.n[0], &tri.v[1], &tri.n[1], &tri.v[2], &tri.n[2]);
					if(r == 6)
						cur->triangles.push_back(tri);
				} else if (cur->useTex)	{
					int r = fscanf(obj, "%d/%d/ %d/%d/ %d/%d/", &tri.v[0], &tri.t[0], &tri.v[1], &tri.t[1], &tri.v[2], &tri.t[2]);
					if(r == 6)
						cur->triangles.push_back(tri);
				} else	{
					int r = fscanf(obj, "%d// %d// %d//", &tri.v[0], &tri.v[1], &tri.v[2]);
					if(r == 3)
						cur->triangles.push_back(tri);
				}
				
			}
			
			fscanf(obj, "%*[^\n]");		// skip the rest of the line
			fscanf(obj, "%*[\n]");		// and the newline
		}
		if(cur)
			objects->push_back(*cur);
	}
	
	return objects;
}

void Object::load()	{
	
	int ntri = (int)triangles.size();
	int nvert = 3 * ntri;
	glm::vec3 *vertexArray, *normalArray;
	vertexArray = new glm::vec3[nvert];
	if(useNormal)
		normalArray = new glm::vec3[nvert];
	
	glm::vec2 *textureArray;
	if(useTex)
		textureArray = new glm::vec2[nvert];
	
	for(int i = 0; i < ntri; ++i)
		for(int j = 0; j < 3; ++j)	{
			vertexArray[i*3 + j] = vertices[triangles[i].v[j]];
			if(useTex)
				textureArray[i*3 + j] = uvs[triangles[i].t[j]];
			if(useNormal)
				normalArray[i*3 + j] = normals[triangles[i].n[j]];
		}
	
	glGenBuffers(1, &vertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, nvert * 3 * sizeof(float), vertexArray, GL_STATIC_DRAW);
	
	if(useTex)	{
		glGenBuffers(1, &textureVBO);
		glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
		glBufferData(GL_ARRAY_BUFFER, nvert * 2 * sizeof(float), textureArray, GL_STATIC_DRAW);
	}
	
	if(useNormal)	{
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, nvert * 3 * sizeof(float), normalArray, GL_STATIC_DRAW);
	}
	
	delete[] vertexArray;
	if(useNormal)
		delete[] normalArray;
	if(useTex)
		delete[] textureArray;
	
	loaded = true;
}

void Object::unload()	{
	if(textureVBO)
		glDeleteBuffers(1, &textureVBO);
	if(vertexVBO)
		glDeleteBuffers(1, &vertexVBO);
	if(normalVBO)
		glDeleteBuffers(1, &normalVBO);
	loaded = false;
}

void Object::render(GLfloat scale)	{
	render(scale, scale, scale);
}

void Object::render(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ)	{
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glPushAttrib(GL_CURRENT_BIT);
	
    glScalef(scaleX, scaleY, scaleZ);
	
	if(loaded)	{
		if(useTex)	{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture);
			glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		}
		
		if(useNormal)	{
			glEnableClientState(GL_NORMAL_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
			glNormalPointer(GL_FLOAT, 0, NULL);
		}
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		
		glDrawArrays(GL_TRIANGLES, 0, (int)triangles.size()*3);
		
		glDisableClientState(GL_VERTEX_ARRAY);
		
		if(useNormal)
			glDisableClientState(GL_NORMAL_ARRAY);
		if(useTex)	{
			glDisable(GL_TEXTURE_2D);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}
	else	{
		
	}
	
	glPopAttrib();
	glPopAttrib();
	glPopMatrix();
}
