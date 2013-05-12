	/*
	*This file is part of FREG.
	*
	*FREG is free software: you can redistribute it and/or modify
	*it under the terms of the GNU General Public License as published by
	*the Free Software Foundation, either version 3 of the License, or
	*(at your option) any later version.
	*
	*FREG is distributed in the hope that it will be useful,
	*but WITHOUT ANY WARRANTY; without even the implied warranty of
	*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	*GNU General Public License for more details.
	*
	*You should have received a copy of the GNU General Public License
	*along with FREG. If not, see <http://www.gnu.org/licenses/>.*/
#ifndef GLSL_PROGRAM_H
#define GLSL_PROGRAM_H
#include "ph.h"
#include "../freg_lib/vec.h"
#include "../freg_lib/matrix.h"

#define MAX_SHADER_SRC_LEN 8192
#define MAX_UNIFORM_NUMBER 16
#define MAX_ATTRIB_NUMBER  16
#define MAX_SHADER_DEFINES 8
#define MAX_DEFINE_LEN 32

#define MAX_UNIFORM_NAME_LEN	32
#define MAX_ATTRIB_NAME_LEN 32

#define HANDLE_NOT_CREATED 0xffffffff


/*���� ���������� ������*/
#define MAX_NUMBER_OF_ATTRIBS 12

#define ATTRIB_POSITION	    0x00
#define ATTRIB_TEX_COORD    0x01
#define ATTRIB_NORMAL	    0x02
#define ATTRIB_BINORMAL	    0x03
#define ATTRIB_TANGENT	    0x04
#define ATTRIB_COLOR		0x05

// ������ ���������, ����� � �����������
#define ATTRIB_USER0		0x06
#define ATTRIB_USER1		0x07
#define	ATTRIB_USER2		0x08
#define ATTRIB_USER3		0x09
#define ATTRIB_USER4		0x0a
#define ATTRIB_USER5		0x0b
/*���� ���������� ������*/
class r_GLSLProgram
{
public:
	int		Load( const char* frag_file, const char* vert_file, const char* geom_file = NULL );
	void	ShaderSource( const char* frag_src, const char* vert_src, const char* geom_src );

	int		MoveOnGPU();
	int		FindAttrib	( const char* name );
	int     SetAttribLocation( const char* name, unsigned int location );
	int		FindUniform	( const char* name );
    int		GetUniformId	( const char* name );

	int		Uniform( const char* name, m_Vec3& v) const;
	int		Uniform( const char* name, int i ) const;
	int		Uniform( const char* name, m_Mat4& m ) const;
	int		Uniform( const char* name, m_Mat3& m ) const;
	int		Uniform( const char* name, float f ) const;

	int		Uniform( int id, m_Vec3& v) const;
	int		Uniform( int id, int i ) const;
	int		Uniform( int id, m_Mat4& m ) const;
	int		Uniform( int id, m_Mat3& m ) const;

    int		Uniform( int id, float f ) const;

    int Define( const char* def );
    int UnDefine( const char* def );

	void	Bind() const;
	 r_GLSLProgram();
	~r_GLSLProgram();
private:
	int		GetUniform( const char* name ) const;
	static r_GLSLProgram* current_prog;

	const static char* attrib_names[ MAX_NUMBER_OF_ATTRIBS ];//����� ����������

	GLuint prog_handle;
	GLuint frag_handle, vert_handle, geom_handle;
	char frag_text[ MAX_SHADER_SRC_LEN ];
	char vert_text[ MAX_SHADER_SRC_LEN ];
	char geom_text[ MAX_SHADER_SRC_LEN ];

	char defines[ MAX_SHADER_DEFINES ][ MAX_DEFINE_LEN ];
    char user_attribs[ MAX_ATTRIB_NUMBER ][ MAX_ATTRIB_NAME_LEN ];
	int attribs[ MAX_ATTRIB_NUMBER ];
	char uniforms_name [ MAX_UNIFORM_NUMBER ][ MAX_UNIFORM_NAME_LEN ];
	int uniforms[ MAX_UNIFORM_NUMBER ];

	int attrib_num, uniform_num, define_num;

};

#endif//GLSL_PROGRAM_H
