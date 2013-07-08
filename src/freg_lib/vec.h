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
	*along with FREG. If not, see <http://www.gnu.org/licenses/>.
	*/
#ifndef VEC_H
#define VEC_H

#include "freg_math.h"

class m_Vec3
{

public:
	float x, y, z;

	m_Vec3(){}
	~m_Vec3(){}
	m_Vec3( float a, float b, float c);
	m_Vec3( double a, double b, double c);
	m_Vec3( int a, int b, int c );

	float	Length()	const;
	float	LengthSqr() const;
	float	InvLength() const;
	float	InvLengthSqr() const;
	float 	MaxComponent() const;
	m_Vec3&	Normalize();
	float*	ToArr();

	m_Vec3 operator-() const;

	m_Vec3  operator+ ( const m_Vec3& v ) const;
	m_Vec3& operator+=( const m_Vec3& v );

	m_Vec3  operator- ( const m_Vec3& v ) const;
	m_Vec3& operator-=( const m_Vec3& v );

	float operator* ( const m_Vec3& v ) const;//скалярное умножение векторов

	m_Vec3& operator*=( const m_Vec3& v);//покомпонентное умножение векторов

	m_Vec3  operator*( float a) const;//умножение вектора на число
	m_Vec3& operator*=( float a );//умножение вектора на число
	m_Vec3& operator/=( float a );//деление   вектора на число


	friend m_Vec3 operator*( float a, m_Vec3& v );

	char* ToGLSLConstant( char* str );

};


inline m_Vec3::m_Vec3( float a, float b, float c) :
x(a), y(b), z(c) {}

inline m_Vec3::m_Vec3( double a, double b, double c )
{
	x= float(a);
	y= float(b);
	z= float(c);
}

inline m_Vec3::m_Vec3( int a, int b, int c )
{
	x= float(a);
	y= float(b);
	z= float(c);
}
inline m_Vec3  m_Vec3::operator+ ( const m_Vec3& v ) const
{
	return m_Vec3( this->x + v.x, this->y + v.y, this->z + v.z );
}
inline m_Vec3& m_Vec3::operator+=( const m_Vec3& v )
{
	this->x+= v.x;
	this->y+= v.y;
	this->z+= v.z;
	return *this;
}

inline m_Vec3  m_Vec3::operator- ( const m_Vec3& v ) const
{
	return m_Vec3( this->x - v.x, this->y - v.y, this->z - v.z );
}
inline m_Vec3& m_Vec3::operator-=( const m_Vec3& v )
{
	this->x-= v.x;
	this->y-= v.y;
	this->z-= v.z;
	return *this;
}

inline float m_Vec3::operator* ( const m_Vec3& v ) const
{
 return this->x * v.x + this->y * v.y + this->z * v.z;
}
inline m_Vec3& m_Vec3::operator*=( float a )
{
	x*= a;
	y*= a;
	z*= a;
	return *this;
}

inline m_Vec3& m_Vec3::operator/=( float a )
{
	float r= 1.0f / a;
	x*= r;
	y*= r;
	z*= r;
	return *this;
}
inline m_Vec3 m_Vec3::operator-() const
{
	return m_Vec3( -x, -y, -z );
}

inline float m_Vec3::MaxComponent() const
{
	float m= 	x > y ? x : y;
	return 		m > z ? m : z;
}

inline m_Vec3  m_Vec3::operator*( float a) const
{
	return m_Vec3( x * a, y * a, z * a );
}

inline m_Vec3& m_Vec3::operator*=( const m_Vec3& v)
{
	this->x*= v.x;
	this->y*= v.y;
	this->z*= v.z;
	return *this;
}

inline m_Vec3& m_Vec3::Normalize()
{
	float r= m_Math::Sqrt( x * x + y * y + z * z );
	if( r != 0.0f )
		r= 1.0f / r;
	x*= r;
	y*= r;
	z*= r;
	return *this;
}

inline float* m_Vec3::ToArr()
{
	return &x;
}

inline float m_Vec3::Length() const
{
	return m_Math::Sqrt( x * x + y * y + z * z );

}
inline float m_Vec3::LengthSqr() const
{
	return  x * x + y * y + z * z;

}

inline float m_Vec3::InvLength() const
{
	return 1.0f / m_Math::Sqrt( x * x + y * y + z * z );
}

inline float m_Vec3::InvLengthSqr() const
{
	return 1.0f / ( x * x + y * y + z * z );
}


inline m_Vec3 operator*( float a, m_Vec3& v )
{
	return m_Vec3( a * v.x, a * v.y, a * v.z );
}

#endif
