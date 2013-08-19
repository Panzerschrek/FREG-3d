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
#ifndef TEXTURE_MANAGER_CPP
#define TEXTURE_MANAGER_CPP

#include "texture_manager.h"
#include "texture.h"
#include <fstream>


//таблица соответствий текстур разным сторонам разных блоков
unsigned char r_TextureManager::texture_table[ ( LAST_KIND << 11 ) ];
//таблица масштабов текстур
unsigned char r_TextureManager::texture_scale_table[ 256 ];
//таблица текстурных базисов блока для каждой его стороны и для каждой его ориентации.
char r_TextureManager::texture_basis_table[ 8*8*4 ];
/*
V
^
|
|
|
+------> U
порядок: u.x, u.y, v.x, v.y
*/
//таблица нормалей для каждой нормали и направления
unsigned char r_TextureManager::normal_rotation_table[ 8*8 ];


r_TextureManager::r_TextureManager()
{
    InitTextureTable();
    InitNormalRotationTable();
    InitTextureBasisTable();
    texture_size= 256;
}

#ifndef OGL21
const r_TextureArray* r_TextureManager::TextureArray() const
{
    return &texture_array;
}
#endif

#ifdef OGL21
const r_Texture* r_TextureManager::TextureAtlas() const
{
    return &texture_atlas;
}
#endif

int r_TextureManager::LoadTextures()
{
    unsigned int max_tex_size;
    float max_lod;

    glGetIntegerv( GL_MAX_TEXTURE_SIZE, &max_tex_size );
    #ifdef OGL21
    if( max_tex_size < 512 )
    	printf( "fatal error, hardware not support 512x512 textures\n" );
    #endif

    texture_size= min( max_tex_size>>4/*16 textures x 16 textures in atlas*/,
                       texture_size );
    texture_size= min( (int)texture_size, 256 );
    texture_size= max( (int)texture_size, 32 );// "Drake" texture quality

    unsigned int atlas_size;

    if( texture_size>= 256 )
        atlas_size= 4096, max_lod= 8.0f;
    else if( texture_size>= 128 )
        atlas_size= 2048, max_lod= 7.0f;
    else if( texture_size>= 64 )
        atlas_size= 1024, max_lod= 6.0f;
    else if( texture_size>= 32 )//Drake mode
        atlas_size= 512, max_lod= 5.0f;

#ifdef OGL21
 	texture_atlas.Create();
    texture_atlas.TextureData( atlas_size, atlas_size, GL_UNSIGNED_BYTE, GL_RGBA, 32, NULL );
    texture_atlas.SetFiltration( GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST );
    texture_atlas.MoveOnGPU();
    texture_atlas.SetMaxLod( max_lod );

#else// if ogl 3.3
    texture_array.TextureData( atlas_size>>4, atlas_size>>4, 32, GL_UNSIGNED_BYTE, GL_RGBA, 32, NULL );
    texture_array.MoveOnGPU();
    if( !filter_textures )
        texture_array.SetFiltration( GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST );
#endif



    r_TextureFile tf1, tf2;
    tf1.data= new unsigned char[ 128 * 128 * 4];
    tf2.data= new unsigned char[ 64 * 64 * 4];



    std::ifstream f( "textures/textures.cfg" );
    if( f.fail() )
    {
        printf( "error. texture config file nor found.\n" );
        return 1;
    }

    unsigned int kind, sub, normal;
    unsigned char tex_id= 0;
    r_TextureFile tf;
    char str[256];

    for( int i=0; i< 256; i++ )
    {
        material_property[ i * 3 ]= 255;
        material_property[ i * 3 + 1 ]= 0;
        material_property[ i * 3 + 2 ]= 1;
    }


    if( ! rLoadTextureTGA( &tf, "textures/null.tga" ) )
    {
#ifdef OGL21
        if( texture_size>= 256 )
            texture_atlas.TexSubData2GPU( (31%16)*256, (31/16)*256, 256, 256, tf.data );
        else if( texture_size>= 128 )
        {
            rRGBAGetMip( &tf, &tf1 );//256->128
            texture_atlas.TexSubData2GPU( (31%16)*128, (31/16)*128, 128, 128, tf1.data );
        }
        else if( texture_size>= 64 )
        {
            rRGBAGetMip( &tf, &tf1 );//256->128
            rRGBAGetMip( &tf1, &tf2 );//128->64
            texture_atlas.TexSubData2GPU( (31%16)*64, (31/16)*64, 64, 64, tf2.data );
        }
        else//if( texture_size>= 32 )
        {
            rRGBAGetMip( &tf, &tf1 );//256->128
            rRGBAGetMip( &tf1, &tf2 );//128->64
            rRGBAGetMip( &tf2, &tf1 );//64->32
            texture_atlas.TexSubData2GPU( (31%16)*32, (31/16)*32, 32, 32, tf1.data );
        }
#else
        if( texture_size>= 256 )
            texture_array.TextureLayer( 31, tf.data );
        else if( texture_size>= 128 )
        {
            rRGBAGetMip( &tf, &tf1 );//256->128
            texture_array.TextureLayer( 31, tf1.data );
        }
        else if( texture_size>= 64 )
        {
            rRGBAGetMip( &tf, &tf1 );//256->128
            rRGBAGetMip( &tf1, &tf2 );//128->64
            texture_array.TextureLayer( 31, tf2.data );
        }
        else//if( texture_size>= 32 )
        {
            rRGBAGetMip( &tf, &tf1 );//256->128
            rRGBAGetMip( &tf1, &tf2 );//128->64
            rRGBAGetMip( &tf2, &tf1 );//64->32
            texture_array.TextureLayer( 31, tf1.data );
        }
        delete[] tf.data;
#endif
    }

    while( ! f.eof() )
    {
        f>>str;
        if( !rLoadTextureTGA( &tf, str ) )
        {
#ifdef OGL21
            if( texture_size>= 256  )
                texture_atlas.TexSubData2GPU( (tex_id%16)*256, (tex_id/16)*256, 256, 256, tf.data );
            else if( texture_size>= 128 )
            {
                rRGBAGetMip( &tf, &tf1 );//256->128
                texture_atlas.TexSubData2GPU( (tex_id%16)*128, (tex_id/16)*128, 128, 128, tf1.data );
            }
            else if( texture_size>= 64 )
            {
                rRGBAGetMip( &tf, &tf1 );//256->128
                rRGBAGetMip( &tf1, &tf2 );//128->64
                texture_atlas.TexSubData2GPU( (tex_id%16)*64, (tex_id/16)*64, 64, 64, tf2.data );
            }
            else//if( texture_size>= 32 )
            {
                rRGBAGetMip( &tf, &tf1 );//256->128
                rRGBAGetMip( &tf1, &tf2 );//128->64
                rRGBAGetMip( &tf2, &tf1 );//64->32
                texture_atlas.TexSubData2GPU( (tex_id%16)*32, (tex_id/16)*32, 32, 32, tf1.data );
            }
#else
            if( texture_size >= 256 )
                texture_array.TextureLayer( tex_id, tf.data );
            else if( texture_size>= 128 )
            {
                rRGBAGetMip( &tf, &tf1 );//256->128
                texture_array.TextureLayer( tex_id, tf1.data );
            }
            else if( texture_size>= 64 )
            {
                rRGBAGetMip( &tf, &tf1 );//256->128
                rRGBAGetMip( &tf1, &tf2 );//128->64
                texture_array.TextureLayer( tex_id, tf2.data );
            }
            else//if( texture_size>= 32 )
            {
                rRGBAGetMip( &tf, &tf1 );//256->128
                rRGBAGetMip( &tf1, &tf2 );//128->64
                rRGBAGetMip( &tf2, &tf1 );//64->32
                texture_array.TextureLayer( tex_id, tf1.data );
            }
            delete[] tf.data;
#endif
        }
        else
            printf( "error. texture \"%s\" not found.\n", str );

        f>>str;
        if( str[0] != '{' || str[1] != 0 )
        {
            printf( "parse error. texture info.\n" );
            return 1;
        }

        f>>str;
        do//обход того, что внутри скобок после имени текстуры
        {
            if( !strcmp( str, "scale" ) )
            {
                f>>str;
                texture_scale_table[ tex_id ]= atoi( str );
            }
            if( !strcmp( str, "diffuse" ) )
            {
                f>>str;
                material_property[ tex_id * 3 ]= (unsigned char)( atof( str ) * 255.0f );
            }
            else if( !strcmp( str, "specular" ) )
            {
                f>>str;
                material_property[ tex_id * 3  + 1 ]= (unsigned char)( atof( str ) * 255.0f );
            }
            else if( !strcmp( str, "specular_pow" ) )
            {
                f>>str;
                material_property[ tex_id * 3  + 2 ]= max( atoi( str ), 1 );
            }
            else if( !strcmp( str, "blocks"  ) )//обход списка блоков тестур
            {
                f>>str;
                if( str[0] != '{' || str[1] != 0 )
                {
                    printf( "parse error. block info.\n" );
                    return 1;
                }
                while ( !( str[0] == '}' && str[1] == 0 ) )
                {
                    f>>str;
                    if( str[0] == '}' && str[1] == 0 )
                        break;
                    kind=atoi(str);
                    f>>sub;
                    f>>normal;
                  //  texture_table[ normal |
                      //             ( sub << 3 ) |
                       //            ( kind << ( R_MAX_SUB_LOG2 + 3 ) )  ]= tex_id;
                       texture_table[ normal | ( kind << 11 ) | (sub<<3 ) ]= tex_id;
                }
            }
            f>>str;
        }
        while ( !( str[0] == '}' && str[1] == 0 )  );
        tex_id++;
    }

#ifndef OGL21
    texture_array.GenerateMipmap();
#endif


    delete[] tf1.data;
    delete[] tf2.data;

    //glActiveTexture( GL_TEXTURE3 );
    glGenTextures( 1, &material_porperty_texture );
    glBindTexture( GL_TEXTURE_1D, material_porperty_texture );
    glTexImage1D( GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, material_property );
    glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    //glGenerateMipmap( GL_TEXTURE_1D );
}



void r_TextureManager::InitNormalRotationTable()
{
    normal_rotation_table[ ( 0 << 3 ) | UP ]= 0;
    normal_rotation_table[ ( 1 << 3 ) | UP ]= 1;
    normal_rotation_table[ ( 2 << 3 ) | UP ]= 2;
    normal_rotation_table[ ( 3 << 3 ) | UP ]= 3;
    normal_rotation_table[ ( 4 << 3 ) | UP ]= 4;
    normal_rotation_table[ ( 5 << 3 ) | UP ]= 5;

    normal_rotation_table[ ( 0 << 3 ) | DOWN ]= 0;
    normal_rotation_table[ ( 1 << 3 ) | DOWN ]= 1;
    normal_rotation_table[ ( 2 << 3 ) | DOWN ]= 2;
    normal_rotation_table[ ( 3 << 3 ) | DOWN ]= 3;
    normal_rotation_table[ ( 4 << 3 ) | DOWN ]= 5;
    normal_rotation_table[ ( 5 << 3 ) | DOWN ]= 4;


    normal_rotation_table[ ( 0 << 3 ) | NORTH ]= 0;
    normal_rotation_table[ ( 1 << 3 ) | NORTH ]= 1;
    normal_rotation_table[ ( 2 << 3 ) | NORTH ]= 5;
    normal_rotation_table[ ( 3 << 3 ) | NORTH ]= 4;
    normal_rotation_table[ ( 4 << 3 ) | NORTH ]= 2;
    normal_rotation_table[ ( 5 << 3 ) | NORTH ]= 3;

    normal_rotation_table[ ( 0 << 3 ) | SOUTH ]= 0;
    normal_rotation_table[ ( 1 << 3 ) | SOUTH ]= 1;
    normal_rotation_table[ ( 2 << 3 ) | SOUTH ]= 4;
    normal_rotation_table[ ( 3 << 3 ) | SOUTH ]= 5;
    normal_rotation_table[ ( 4 << 3 ) | SOUTH ]= 2;
    normal_rotation_table[ ( 5 << 3 ) | SOUTH ]= 3;

    normal_rotation_table[ ( 0 << 3 ) | EAST ]= 4;
    normal_rotation_table[ ( 1 << 3 ) | EAST ]= 5;
    normal_rotation_table[ ( 2 << 3 ) | EAST ]= 0;
    normal_rotation_table[ ( 3 << 3 ) | EAST ]= 1;
    normal_rotation_table[ ( 4 << 3 ) | EAST ]= 2;
    normal_rotation_table[ ( 5 << 3 ) | EAST ]= 3;

    normal_rotation_table[ ( 0 << 3 ) | WEST ]= 5;
    normal_rotation_table[ ( 1 << 3 ) | WEST ]= 4;
    normal_rotation_table[ ( 2 << 3 ) | WEST ]= 0;
    normal_rotation_table[ ( 3 << 3 ) | WEST ]= 1;
    normal_rotation_table[ ( 4 << 3 ) | WEST ]= 2;
    normal_rotation_table[ ( 5 << 3 ) | WEST ]= 3;
}



void r_TextureManager::InitTextureBasisTable()
{
    for( unsigned int i=0; i< 6; i++ )
    {
        for( unsigned int k=0; k< 6; k++ )
        {
            texture_basis_table[ ( i << 5 ) | ( k << 2 ) | 0 ] = 1;
            texture_basis_table[ ( i << 5 ) | ( k << 2 ) | 1 ] = 0;
            texture_basis_table[ ( i << 5 ) | ( k << 2 ) | 2 ] = 0;
            texture_basis_table[ ( i << 5 ) | ( k << 2 ) | 3 ] = 1;
        }
    }
    //сеаерный поворот блока
    {
        texture_basis_table[ ( 0 << 5 ) | ( NORTH << 2 ) | 0 ] = 0;
        texture_basis_table[ ( 0 << 5 ) | ( NORTH << 2 ) | 1 ] = -1;
        texture_basis_table[ ( 0 << 5 ) | ( NORTH << 2 ) | 2 ] = 1;
        texture_basis_table[ ( 0 << 5 ) | ( NORTH << 2 ) | 3 ] = 0;

        texture_basis_table[ ( 1 << 5 ) | ( NORTH << 2 ) | 0 ] = 0;
        texture_basis_table[ ( 1 << 5 ) | ( NORTH << 2 ) | 1 ] = -1;
        texture_basis_table[ ( 1 << 5 ) | ( NORTH << 2 ) | 2 ] = 1;
        texture_basis_table[ ( 1 << 5 ) | ( NORTH << 2 ) | 3 ] = 0;
    }

    //южный поворот блока
    {
        texture_basis_table[ ( 0 << 5 ) | ( SOUTH << 2 ) | 0 ] = 0;
        texture_basis_table[ ( 0 << 5 ) | ( SOUTH << 2 ) | 1 ] = 1;
        texture_basis_table[ ( 0 << 5 ) | ( SOUTH << 2 ) | 2 ] = -1;
        texture_basis_table[ ( 0 << 5 ) | ( SOUTH << 2 ) | 3 ] = 0;

        texture_basis_table[ ( 1 << 5 ) | ( SOUTH << 2 ) | 0 ] = 0;
        texture_basis_table[ ( 1 << 5 ) | ( SOUTH << 2 ) | 1 ] = 1;
        texture_basis_table[ ( 1 << 5 ) | ( SOUTH << 2 ) | 2 ] = -1;
        texture_basis_table[ ( 1 << 5 ) | ( SOUTH << 2 ) | 3 ] = 0;

        texture_basis_table[ ( 2 << 5 ) | ( SOUTH << 2 ) | 0 ] = 1;
        texture_basis_table[ ( 2 << 5 ) | ( SOUTH << 2 ) | 1 ] = 0;
        texture_basis_table[ ( 2 << 5 ) | ( SOUTH << 2 ) | 2 ] = 0;
        texture_basis_table[ ( 2 << 5 ) | ( SOUTH << 2 ) | 3 ] = 1;

        texture_basis_table[ ( 3 << 5 ) | ( SOUTH << 2 ) | 0 ] = 1;
        texture_basis_table[ ( 3 << 5 ) | ( SOUTH << 2 ) | 1 ] = 0;
        texture_basis_table[ ( 3 << 5 ) | ( SOUTH << 2 ) | 2 ] = 0;
        texture_basis_table[ ( 3 << 5 ) | ( SOUTH << 2 ) | 3 ] = 1;

        texture_basis_table[ ( 4 << 5 ) | ( SOUTH << 2 ) | 0 ] = 1;
        texture_basis_table[ ( 4 << 5 ) | ( SOUTH << 2 ) | 1 ] = 0;
        texture_basis_table[ ( 4 << 5 ) | ( SOUTH << 2 ) | 2 ] = 0;
        texture_basis_table[ ( 4 << 5 ) | ( SOUTH << 2 ) | 3 ] = -1;

        texture_basis_table[ ( 5 << 5 ) | ( SOUTH << 2 ) | 0 ] = 1;
        texture_basis_table[ ( 5 << 5 ) | ( SOUTH << 2 ) | 1 ] = 0;
        texture_basis_table[ ( 5 << 5 ) | ( SOUTH << 2 ) | 2 ] = 0;
        texture_basis_table[ ( 5 << 5 ) | ( SOUTH << 2 ) | 3 ] = -1;
    }

    //блок смотрит вниз
    {
        texture_basis_table[ ( 0 << 5 ) | ( DOWN << 2 ) | 0 ] = 1;
        texture_basis_table[ ( 0 << 5 ) | ( DOWN << 2 ) | 1 ] = 0;
        texture_basis_table[ ( 0 << 5 ) | ( DOWN << 2 ) | 2 ] = 0;
        texture_basis_table[ ( 0 << 5 ) | ( DOWN << 2 ) | 3 ] = -1;

        texture_basis_table[ ( 1 << 5 ) | ( DOWN << 2 ) | 0 ] = 1;
        texture_basis_table[ ( 1 << 5 ) | ( DOWN << 2 ) | 1 ] = 0;
        texture_basis_table[ ( 1 << 5 ) | ( DOWN << 2 ) | 2 ] = 0;
        texture_basis_table[ ( 1 << 5 ) | ( DOWN << 2 ) | 3 ] = -1;

        texture_basis_table[ ( 2 << 5 ) | ( DOWN << 2 ) | 0 ] = 1;
        texture_basis_table[ ( 2 << 5 ) | ( DOWN << 2 ) | 1 ] = 0;
        texture_basis_table[ ( 2 << 5 ) | ( DOWN << 2 ) | 2 ] = 0;
        texture_basis_table[ ( 2 << 5 ) | ( DOWN << 2 ) | 3 ] = -1;

        texture_basis_table[ ( 3 << 5 ) | ( DOWN << 2 ) | 0 ] = 1;
        texture_basis_table[ ( 3 << 5 ) | ( DOWN << 2 ) | 1 ] = 0;
        texture_basis_table[ ( 3 << 5 ) | ( DOWN << 2 ) | 2 ] = 0;
        texture_basis_table[ ( 3 << 5 ) | ( DOWN << 2 ) | 3 ] = -1;

        texture_basis_table[ ( 4 << 5 ) | ( DOWN << 2 ) | 0 ] = 1;
        texture_basis_table[ ( 4 << 5 ) | ( DOWN << 2 ) | 1 ] = 0;
        texture_basis_table[ ( 4 << 5 ) | ( DOWN << 2 ) | 2 ] = 0;
        texture_basis_table[ ( 4 << 5 ) | ( DOWN << 2 ) | 3 ] = -1;

        texture_basis_table[ ( 5 << 5 ) | ( DOWN << 2 ) | 0 ] = 1;
        texture_basis_table[ ( 5 << 5 ) | ( DOWN << 2 ) | 1 ] = 0;
        texture_basis_table[ ( 5 << 5 ) | ( DOWN << 2 ) | 2 ] = 0;
        texture_basis_table[ ( 5 << 5 ) | ( DOWN << 2 ) | 3 ] = -1;
    }

    //блок смотрит на восток
    {

        texture_basis_table[ ( 2 << 5 ) | ( EAST << 2 ) | 0 ] = 0;
        texture_basis_table[ ( 2 << 5 ) | ( EAST << 2 ) | 1 ] = -1;
        texture_basis_table[ ( 2 << 5 ) | ( EAST << 2 ) | 2 ] = 1;
        texture_basis_table[ ( 2 << 5 ) | ( EAST << 2 ) | 3 ] = 0;

        texture_basis_table[ ( 3 << 5 ) | ( EAST << 2 ) | 0 ] = 0;
        texture_basis_table[ ( 3 << 5 ) | ( EAST << 2 ) | 1 ] = 1;
        texture_basis_table[ ( 3 << 5 ) | ( EAST << 2 ) | 2 ] = -1;
        texture_basis_table[ ( 3 << 5 ) | ( EAST << 2 ) | 3 ] = 0;

        texture_basis_table[ ( 4 << 5 ) | ( EAST << 2 ) | 0 ] = 0;
        texture_basis_table[ ( 4 << 5 ) | ( EAST << 2 ) | 1 ] = 1;
        texture_basis_table[ ( 4 << 5 ) | ( EAST << 2 ) | 2 ] = -1;
        texture_basis_table[ ( 4 << 5 ) | ( EAST << 2 ) | 3 ] = 0;

        texture_basis_table[ ( 5 << 5 ) | ( EAST << 2 ) | 0 ] = 0;
        texture_basis_table[ ( 5 << 5 ) | ( EAST << 2 ) | 1 ] = -1;
        texture_basis_table[ ( 5 << 5 ) | ( EAST << 2 ) | 2 ] = 1;
        texture_basis_table[ ( 5 << 5 ) | ( EAST << 2 ) | 3 ] = 0;
    }

    //блок смотрит на запад
    {

        texture_basis_table[ ( 2 << 5 ) | ( WEST << 2 ) | 0 ] = 0;
        texture_basis_table[ ( 2 << 5 ) | ( WEST << 2 ) | 1 ] = 1;
        texture_basis_table[ ( 2 << 5 ) | ( WEST << 2 ) | 2 ] = -1;
        texture_basis_table[ ( 2 << 5 ) | ( WEST << 2 ) | 3 ] = 0;

        texture_basis_table[ ( 3 << 5 ) | ( WEST << 2 ) | 0 ] = 0;
        texture_basis_table[ ( 3 << 5 ) | ( WEST << 2 ) | 1 ] = 1;
        texture_basis_table[ ( 3 << 5 ) | ( WEST << 2 ) | 2 ] = -1;
        texture_basis_table[ ( 3 << 5 ) | ( WEST << 2 ) | 3 ] = 0;

        texture_basis_table[ ( 4 << 5 ) | ( WEST << 2 ) | 0 ] = 0;
        texture_basis_table[ ( 4 << 5 ) | ( WEST << 2 ) | 1 ] = 1;
        texture_basis_table[ ( 4 << 5 ) | ( WEST << 2 ) | 2 ] = -1;
        texture_basis_table[ ( 4 << 5 ) | ( WEST << 2 ) | 3 ] = 0;

        texture_basis_table[ ( 5 << 5 ) | ( WEST << 2 ) | 0 ] = 0;
        texture_basis_table[ ( 5 << 5 ) | ( WEST << 2 ) | 1 ] = 1;
        texture_basis_table[ ( 5 << 5 ) | ( WEST << 2 ) | 2 ] = -1;
        texture_basis_table[ ( 5 << 5 ) | ( WEST << 2 ) | 3 ] = 0;
    }
}


void r_TextureManager::InitTextureTable()
{

    for( int i=0; i< ( LAST_KIND << 11 ); i++ )
    {
        texture_table[i]= 31;
    }

    for( int i= 0; i< 256; i++ )
        texture_scale_table[i]= R_DEFAULT_TEXTURE_SCALE;
}
#endif//TEXTURE_MANAGER_CPP
