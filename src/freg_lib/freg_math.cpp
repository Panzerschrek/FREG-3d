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
#ifndef MATH_CPP
#define MATH_CPP

#include "freg_math.h"


const float m_Math:: FM_PI=	m_Math::Pi();
const float m_Math:: FM_2PI= FM_PI * 2.0;
const float m_Math:: FM_PI2= FM_PI * 0.5;
const float m_Math:: FM_PI4= FM_PI * 0.25;
const float m_Math:: FM_PI8= FM_PI * 0.125;

const float m_Math:: FM_TODEG=	180.0 / FM_PI;
const float m_Math:: FM_TORAD=	FM_PI / 180.0;


/*float m_Math::F16_to_f32( unsigned short x )
{
    unsigned int sign;
    unsigned int exponent;
    unsigned int fraction;
    unsigned int result;

    sign= 		( x & 0x8000 ) >> 15;
    exponent=  (( x & 0x7C00 ) >> 7 ) | 7;
    fraction= 	( x & 0x03FF );

    result= 	(sign << 31);
    result|= 	(exponent << 23 );
    result|=	(fraction << 13 );
//result= 0x3f800000;
	return *( (float*)&result );
}*/




// -15 stored using a single precision bias of 127
const unsigned int  HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP = 0x38000000;
// max exponent value in single precision that will be converted
// to Inf or Nan when stored as a half-float
const unsigned int  HALF_FLOAT_MAX_BIASED_EXP_AS_SINGLE_FP_EXP = 0x47800000;
// 255 is the max exponent biased value
const unsigned int  FLOAT_MAX_BIASED_EXP = (0xFF << 23);
const unsigned int  HALF_FLOAT_MAX_BIASED_EXP = (0x1F << 10);

typedef unsigned short hfloat;

float m_Math::F16_to_f32( unsigned short hf )
{
    unsigned int    sign = (unsigned int)(hf >> 15);
    unsigned int    mantissa = (unsigned int)(hf & ((1 << 10) - 1));
    unsigned int    exp = (unsigned int)(hf & HALF_FLOAT_MAX_BIASED_EXP);
    unsigned int    f;

    if (exp == HALF_FLOAT_MAX_BIASED_EXP)
    {
        // we have a half-float NaN or Inf
        // half-float NaNs will be converted to a single precision NaN
        // half-float Infs will be converted to a single precision Inf
        exp = FLOAT_MAX_BIASED_EXP;
        if (mantissa)
            mantissa = (1 << 23) - 1;    // set all bits to indicate a NaN
    }
    else if (exp == 0x0)
    {
        // convert half-float zero/denorm to single precision value
        if (mantissa)
        {
           mantissa <<= 1;
           exp = HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP;
           // check for leading 1 in denorm mantissa
           while ((mantissa & (1 << 10)) == 0)
           {
               // for every leading 0, decrement single precision exponent by 1
               // and shift half-float mantissa value to the left
               mantissa <<= 1;
               exp -= (1 << 23);
            }
            // clamp the mantissa to 10-bits
            mantissa &= ((1 << 10) - 1);
            // shift left to generate single-precision mantissa of 23-bits
            mantissa <<= 13;
        }
    }
    else
    {
        // shift left to generate single-precision mantissa of 23-bits
        mantissa <<= 13;
        // generate single precision biased exponent value
        exp = (exp << 13) + HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP;
    }

    f = (sign << 31) | exp | mantissa;
    return *((float *)&f);
}
#endif//MATH_CPP
