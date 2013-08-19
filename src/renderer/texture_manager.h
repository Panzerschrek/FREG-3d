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
#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "ph.h"
#include "../header.h"
#include "texture_array.h"
#include "texture.h"

#define R_MAX_TEXTURE_SCALE 8
#define R_DEFAULT_TEXTURE_SCALE 2

class r_TextureManager
{
public:
    r_TextureManager();
    ~r_TextureManager() {}


    int LoadTextures();
    void SetTexturesSize( unsigned int s );
    #ifdef OGL21
    const r_Texture* TextureAtlas() const;
    #else
    const r_TextureArray* TextureArray() const;
    #endif
    void BindMaterialPropertyTexture( unsigned int unit = 0 ) const;


    static unsigned char GetBlockTexture( quint16 block_id, unsigned char normal_id, dirs dir );
    static unsigned char GetBlockTextureScale( unsigned char texture_id );
    static void GetBlockTextureBasis( unsigned char normal_id, dirs dir, char* result );

	void SetTextureFilter( bool );
private:
    static unsigned char RotateBlockNormal( unsigned char normal_id, dirs dir );

	bool filter_textures;

	void InitNormalRotationTable();
	void InitTextureBasisTable();
	void InitTextureTable();

	#ifdef OGL21
	r_Texture texture_atlas;
	#else
    r_TextureArray texture_array;
    #endif
    unsigned char material_property[ 3 * 256 ];//таблица свойст каждого материала
    GLuint material_porperty_texture;
    //содержит diffuse и specular составляющие материала, а так-же степень в specualr
	unsigned int texture_size;//quality of textures

    static unsigned char texture_table[];
    static unsigned char texture_scale_table[];
    static char texture_basis_table[];
    static unsigned char normal_rotation_table[];
};


inline void r_TextureManager::SetTextureFilter( bool filter )
{
	filter_textures= filter;
}

inline void r_TextureManager::SetTexturesSize( unsigned int s )
{
	texture_size= s;
}

 inline void r_TextureManager::BindMaterialPropertyTexture( unsigned int unit ) const
 {
 	glActiveTexture( GL_TEXTURE0 + unit );
 	glBindTexture( GL_TEXTURE_1D, material_porperty_texture );
 }

inline unsigned char r_TextureManager::GetBlockTexture( quint16 block_id, unsigned char normal_id, dirs dir )
{
    normal_id= RotateBlockNormal( normal_id, dir );
    return texture_table[ normal_id  |
                          ( block_id << 3 )  ];
}

inline unsigned char r_TextureManager::GetBlockTextureScale( unsigned char texture_id )
{
    return texture_scale_table[ texture_id ];
}

inline void r_TextureManager::GetBlockTextureBasis( unsigned char normal_id, dirs dir, char* result )
{
    //такие выпендривания нужны, чтобы переслать 4 байта за одну инструкцию процессора
    *((quint32*)result) =((quint32*)texture_basis_table)[ ( normal_id << 3 ) | dir ];
}

inline unsigned char r_TextureManager::RotateBlockNormal( unsigned char normal_id, dirs dir )
{
    return normal_rotation_table[ ( normal_id << 3 ) | dir ];
}


#endif//TEXTURE_MANAGER_H
