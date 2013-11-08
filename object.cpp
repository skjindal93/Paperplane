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
	Ke = glm::vec3(0.0f);
	name = new char[32];
}

Material::~Material()	{
	// TODO
	//delete[] name;
}

void Material::apply()	{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(glm::vec4(Ka, 1.0f)));
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(glm::vec4(Kd, 1.0f)));
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(glm::vec4(Ks, 1.0f)));
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, s);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, glm::value_ptr(glm::vec4(Ke, 1.0f)));
}

vector<Material>* readMTL(string file)	{
	vector<Material>* materials;
	materials = new vector<Material>();
	Material *cur = NULL;
	
	// read file
	FILE *mtl = fopen(file.c_str(), "r");
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
				fscanf(mtl, "%*[ ]%31[^\n]", cur->name);
				
			} else if (!strcmp(type, "Ns"))	{
				fscanf(mtl, "%f", &cur->s);
			} else if (!strcmp(type, "Ka"))	{
				fscanf(mtl, "%f %f %f", &cur->Ka.x, &cur->Ka.y, &cur->Ka.z);
			} else if (!strcmp(type, "Kd"))	{
				fscanf(mtl, "%f %f %f", &cur->Kd.x, &cur->Kd.y, &cur->Kd.z);
			} else if (!strcmp(type, "Ks"))	{
				fscanf(mtl, "%f %f %f", &cur->Ks.x, &cur->Ks.y, &cur->Ks.z);
			} else if (!strcmp(type, "Ke"))	{
				fscanf(mtl, "%f %f %f", &cur->Ke.x, &cur->Ke.y, &cur->Ke.z);
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
	loaded = save = false;
	modelView = NULL;
	texture = vertexVBO = textureVBO = normalVBO = 0;
	normals.clear();
	vertices.clear();
	uvs.clear();
	triangles.clear();
	mtl = NULL;
	name = new char[32];
	timesUsed = 0;
}

Object::~Object()	{
	if(loaded)
		unload();
	if(texture)
		glDeleteTextures( 1, &texture );
	//delete[] name;
}

vector<Object>* readOBJ(string file)	{
	vector<Object>* objects;
	objects = new vector<Object>();
	Object *cur = NULL;
	vector<Material> *mtllib;

	// read file
	FILE *obj = fopen(file.c_str(), "r");
	if(obj == NULL)
		cerr << "Could not read file " << file << endl;
	else	{
		while(1)	{
			char type[32];
			int res = fscanf(obj, "%s", type);
			if(res == EOF)
				break;
			
			if (!strcmp(type, "mtllib"))	{
				
				char buffer[1000];
				fscanf(obj, "%*[ ]%255[^\n]", buffer);
				mtllib = readMTL(string(PATH) + buffer);
				
			} else if (!strcmp(type, "o"))	{
				
				if(cur)
					objects->push_back(*cur);
				cur = new Object();
				fscanf(obj, "%*[ ]%31[^\n]", cur->name);
				
			} else if (!strcmp(type, "usemtl"))	{
				
				fscanf(obj, "%*[ ]%31[^\n]", type);
				for(vector<Material>::iterator it = mtllib->begin(); it != mtllib->end(); ++it)
					if(!strcmp(it->name, type))	{
						cur->mtl = &(*it);
						break;
					}
				
			} else if (!strcmp(type, "usetex"))	{
				
				char buffer[1000];
				fscanf(obj, "%*[ ]%255[^\n]", buffer);
				cur->texture = loadTexture(buffer);
				
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
					
					tri.v -= 1;
					tri.n -= 1;
					tri.t -= 1;
					
					if(r == 9)
						cur->triangles.push_back(tri);
				} else if (cur->useNormal)	{
					int r = fscanf(obj, "%d//%d %d//%d %d//%d", &tri.v[0], &tri.n[0], &tri.v[1], &tri.n[1], &tri.v[2], &tri.n[2]);
					
					tri.v -= 1;
					tri.n -= 1;
					
					if(r == 6)
						cur->triangles.push_back(tri);
				} else if (cur->useTex)	{
					int r = fscanf(obj, "%d/%d/ %d/%d/ %d/%d/", &tri.v[0], &tri.t[0], &tri.v[1], &tri.t[1], &tri.v[2], &tri.t[2]);
					
					tri.v -= 1;
					tri.t -= 1;
					
					if(r == 6)
						cur->triangles.push_back(tri);
				} else	{
					int r = fscanf(obj, "%d// %d// %d//", &tri.v[0], &tri.v[1], &tri.v[2]);
					tri.v -= 1;
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
	if(vertexVBO)
		return;
	
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
	if(textureVBO)	{
		glDeleteBuffers(1, &textureVBO);
		textureVBO = 0;
	}
	if(vertexVBO)	{
		glDeleteBuffers(1, &vertexVBO);
		vertexVBO = 0;
	}
	if(normalVBO)	{
		glDeleteBuffers(1, &normalVBO);
		normalVBO = 0;
	}
	loaded = false;
}

GLfloat triangleArea(glm::vec3 a,glm::vec3 b,glm::vec3 c){
	glm::vec3 v1 (b[0]-a[0], b[1]-a[1], b[2]-a[2]);
	glm::vec3 v2 (c[0]-a[0], c[1]-a[1], c[2]-a[2]);
	glm::vec3 cross = glm::cross(v1,v2);

	return 0.5 * sqrt(abs(glm::dot(cross,cross)));
}

bool pointInTriangle(glm::vec3 triangle[3], glm::vec3 point){
	GLfloat A = triangleArea(triangle[0], triangle[1], triangle[2]);
	GLfloat A1 = triangleArea(point, triangle[1], triangle[2]);
	GLfloat A2 = triangleArea(triangle[0], point, triangle[2]);
	GLfloat A3 = triangleArea(triangle[0], triangle[1], point);
	
	return (A == (A1 + A2 + A3));
}

bool lineWithPlane(glm::vec3 plane[3], glm::vec3 x, glm::vec3 y){
	glm::vec3 normal(glm::cross(plane[1] - plane[0], plane[2] - plane[0]));
	GLfloat lambda = glm::dot(plane[0] - x, normal) / glm::dot(y-x, normal);
	
	if (lambda >= 0.0f && lambda <= 1.0f)
		return pointInTriangle(plane, x + lambda * (y - x));
	
	return false;
}

bool triangleIntersect(glm::vec4 a[3], glm::vec4 b[3]){
	GLfloat minXa,minYa,minZa,maxXa,maxYa,maxZa;
	GLfloat minXb,minYb,minZb,maxXb,maxYb,maxZb;
	glm::vec3 TriA[3], TriB[3];

	for (int i=0; i<3; i ++){
		a[i] /= a[i][3];
		b[i] /= b[i][3];
	}
	minXa = min( min( a[0][0],a[1][0] ),a[2][0] );
	minYa = min( min( a[0][1],a[1][1] ),a[2][1] );
	minZa = min( min( a[0][2],a[1][2] ),a[2][2] );
	
	maxXa = max( max( a[0][0],a[1][0] ),a[2][0] );
	maxYa = max( max( a[0][1],a[1][1] ),a[2][1] );
	maxZa = max( max( a[0][2],a[1][2] ),a[2][2] );
	
	minXb = min( min( b[0][0],b[1][0] ),b[2][0] );
	minYb = min( min( b[0][1],b[1][1] ),b[2][1] );
	minZb = min( min( b[0][2],b[1][2] ),b[2][2] );
	
	maxXb = max( max( b[0][0],b[1][0] ),b[2][0] );
	maxYb = max( max( b[0][1],b[1][1] ),b[2][1] );
	maxZb = max( max( b[0][2],b[1][2] ),b[2][2] );
	
	if ( (maxXb < minXa || maxXa < minXb)
		|| (maxYb < minYa || maxYa < minYb)
		|| (maxZb < minZa || maxZa < minZb) )
	{
		return false;
	}
	
	for (int k=0; k<3; k++)	{
		TriA[k] = glm::vec3(a[k][0],a[k][1],a[k][2]);
		TriB[k] = glm::vec3(b[k][0],b[k][1],b[k][2]);
	}

	for (int j=0; j<3; j++)
		if (lineWithPlane(TriA, TriB[j % 3], TriB[(j+1) % 3]))
			return true;

	for (int l=0; l<3; l++)
		if (lineWithPlane(TriB, TriA[l % 3], TriA[(l+1) % 3]))
			return true;

	return false;
}

bool Object::collision(Object *other){
	int ntriA = (int)this->triangles.size();
	int ntriB = (int)other->triangles.size();
	glm::vec4 a[3],b[3];
	
	for (int i=0; i < ntriA; i++)	{
		
		for (int k = 0; k < 3; k++)
			a[k] = *(this->modelView) * glm::vec4(this->vertices[this->triangles[i].v[k]], 1);
		
		for (int j = 0; j < ntriB; j++)	{

			for (int l = 0; l < 3; l++)
				b[l] = *(other->modelView) * glm::vec4(other->vertices[other->triangles[j].v[l]], 1);
			
			if(triangleIntersect(a,b))
				return true;
		}
	}
	return false;
}

void Object::render(GLfloat scale, bool load)	{
	render(scale, scale, scale, load);
}

void Object::render(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ, bool load)	{
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glPushAttrib(GL_CURRENT_BIT);
	
    glScalef(scaleX, scaleY, scaleZ);
	
	if (save) {
		if (modelView == NULL)
			modelView = new glm::mat4();

		glGetFloatv (GL_MODELVIEW_MATRIX, glm::value_ptr(*modelView));
	}

	
	if(mtl)
		mtl->apply();
	
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
		cout << name << " not loaded!\n";
		if(useTex)	{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture);
		}
		
		int ntri = (int)triangles.size();
		for(int i = 0; i < ntri; i++) {
			
			glBegin(GL_TRIANGLES);
			for(int j = 0; j < 3; j++) {
				int v = triangles[i].v[j], n = triangles[i].n[j], t = triangles[i].t[j];
				if(normals.size() > 0)
					glNormal3f(normals[n][0], normals[n][1], normals[n][2]);
				if(texture) glTexCoord2f(uvs[t][0], uvs[t][1]);
				glVertex3f(vertices[v][0], vertices[v][1], vertices[v][2]);
			}
			glEnd();
			
		}
		if(texture) glDisable(GL_TEXTURE_2D);
		
		if(load)
			this->load();
	}
	
	glPopAttrib();
	glPopAttrib();
	glPopMatrix();
}
