#pragma once

#ifndef __SLSHADER_H__
#define __SLSHADER_H__

#include "glad/glad.h"
#include "GLFW/glfw3.h"

const char *SL_DEFAULT_VERTEX_SHADER = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec3 aColor;\n"
	"layout (location = 2) in vec2 aTexCoord;\n"
	"\n"
	"out vec3 ourColor;\n"
	"out vec2 TexCoord;\n"
	"\n"
	"void main()\n"
	"{\n"
		"gl_Position = vec4(aPos, 1.0);\n"
		"ourColor = aColor;\n"
		"TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
	"}\0";

const char *SL_DEFAULT_FRAGMENT_SHADER = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"\n"
	"in vec3 ourColor;\n"
	"in vec2 TexCoord;\n"
	// texture samplers
	"uniform sampler2D imageTexture;\n"
	"\n"
	"void main()\n"
	"{\n"
		"// linearly interpolate between both textures (80% container, 20% awesomeface)\n"
		"FragColor = texture(imageTexture, TexCoord);\n"
	"}\0";

GLuint LoadShaders(const char **vertexShader, const char **fragmentShader)
{
	vertexShader = vertexShader ? vertexShader : &SL_DEFAULT_VERTEX_SHADER;
	fragmentShader = fragmentShader ? fragmentShader : &SL_DEFAULT_FRAGMENT_SHADER;
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	glShaderSource(VertexShaderID, 1, vertexShader , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	
	// Compile Fragment Shader
	glShaderSource(FragmentShaderID, 1, fragmentShader , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	// Link the program
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

#endif /* __SLSHADER_H__ */
