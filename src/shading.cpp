//
//  shading.cpp
//  PaperPlane
//
//  Created by Shivanker Goel and Shubham Jindal on 09/11/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#include "paperplane.h"

Shader::Shader()	{
	pID = vID = fID = 0;
}

Shader::Shader(string vert, string frag)	{
	pID = vID = fID = 0;
	initFromFile(vert, frag);
}

char* readFile(string file)	{
	ifstream fin(file);
	string line;
	string file_contents;
	
	while (getline(fin, line))	{
		file_contents += line;
		file_contents.push_back('\n');
	}
	
	char* array = new char[file_contents.length()];
	strcpy(array, file_contents.c_str());
	return array;
}

char* getShaderLog(unsigned int shader)	{
	char *buffer = new char[BUFFER_SIZE];
	int length;
	glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);
	buffer[length] = '\0';
	return buffer;
}

void Shader::init(char* vShader, char* fShader)	{
	char* log;
	pID = glCreateProgram();

	vID = glCreateShader(GL_VERTEX_SHADER);
	int vLen = (int)strlen(vShader);

	glShaderSource(vID, 1, (const char**)&vShader, &vLen);
	glCompileShader(vID);

	log = getShaderLog(vID);
	if(strlen(log))	{
		cerr << "Compilation error in vertex shader:\n"
		<< log << endl;
	}
	delete[] log;

	glAttachShader(pID, vID);

	glDeleteShader(vID);

	fID = glCreateShader(GL_FRAGMENT_SHADER);
	int fLen = (int)strlen(fShader);

	glShaderSource(fID, 1, (const char**)&fShader, &fLen);
	glCompileShader(fID);

	log = getShaderLog(fID);
	if(strlen(log))	{
		cerr << "Compilation error in fragment shader:\n"
		<< log << endl;
	}
	delete[] log;
	glAttachShader(pID, fID);

	glDeleteShader(fID);

	glLinkProgram(pID);

	log = getShaderLog(pID);
	if(strlen(log))	{
		cerr << "Link error in shader program:\n"
		<< log << endl;
	}
	delete[] log;

	glValidateProgram(pID);
    GLint status;
	glGetProgramiv(pID, GL_VALIDATE_STATUS, &status);

	if (status == GL_FALSE)	{
		cerr << "Error validating shader program." << endl;
		exit(9);
	}
}

void Shader::initFromFile(string vertFile, string fragFile)	{
	char* vShader = readFile(vertFile);
	char* fShader = readFile(fragFile);
	init(vShader, fShader);
	delete[] vShader;
	delete[] fShader;
}

void Shader::bind()	{
	glUseProgram(pID);
}

void Shader::unbind()	{
	glUseProgram(0);
}

Shader::~Shader()	{
	glDeleteShader(vID);
	glDetachShader(pID, fID);
	glDeleteProgram(pID);
}