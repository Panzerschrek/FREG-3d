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
#ifndef SHRED_INFO_CPP
#define SHRED_INFO_CPP
#include "ph.h"
#include "renderer.h"
#include "texture_manager.h"
#include "../BlockManager.h"
#include "../blocks.h"

void r_ShredInfo::UpdateCube( short x0, short y0, short z0, short x1, short y1, short z1 )
{
    unsigned char* inf;
    short x, y, z;
    for( x= x0; x<= x1; x++ )
        for( y= y0; y<= y1; y++ )
        {
            inf= &visibly_information[ (x<<11) | (y<<7) | z0 ];
            for( z= z0; z<= z1; z++ )
            {
                //b= shred->GetBlock( x, y, z );
                // if( b!= NULL )
                inf[0]= shred->GetBlock( x, y, z )->Block::Transparent();
                // if( inf[0] == 6 )
                //  inf[0]= 2;
                // else
                //     inf[0]= 2;
                inf++;
            }
        }

    updated= true;
    if( x0 == 0 && west_shred != NULL )
        west_shred->updated= true;
    if( y0 == 0 && north_shred != NULL )
        north_shred->updated= true;
}

void r_Renderer::UpdateCube( short x0, short y0, short z0, short x1, short y1, short z1 )
{
    //  while( ! renderer_initialized  )
    //  usleep( 1000 );
    host_data_mutex.lock();

    short X0, Y0, X1, Y1;

    X0= x0>>4;
    X1= x1>>4;
    Y0= y0>>4;
    Y1= y1>>4;
    short x, y, i0, j0, i1, j1;

    r_ShredInfo* shred;
    for( x= X0, i0= x0&15, i1= 15; x<= X1; x++ )
    {
        for( y= Y0, j0= y0&15, j1= 15; y<= Y1; y++ )
        {
            if( x == X1 )
                i1= x1&15;
            if( y == Y1 )
                j1= y1&15;
            shred= &shreds[ x + y * visibly_world_size[0] ];
            shred->UpdateCube( i0, j0, z0, i1, j1, z1 );
            j0 = 0;
        }
        i0= 0;
    }

    host_data_mutex.unlock();
}

void r_Renderer::UpdateShred( short x, short y )
{
    r_ShredInfo* shred= &shreds[ x + y * visibly_world_size[0] ];
    shred->UpdateShred();
}

void r_Renderer::UpdateBlock( short x, short y, short z )
{
    // while( ! renderer_initialized )
    //     usleep( 1000 );

    if( z == ( R_SHRED_HEIGHT - 1 ) )
        return;//ignore sky

    host_data_mutex.lock();

    short X, Y;

    X= x>>4;
    Y= y>>4;
    x&=15;
    y&=15;

    register r_ShredInfo* shred= &shreds[ X + Y * visibly_world_size[0] ];

    shred->updated= true;

    if( x == 0 && shred->west_shred != NULL )
        shred->west_shred->updated= true;

    if( y == 0 && shred->north_shred != NULL )
        shred->north_shred->updated= true;

    Block* b= shred->shred->GetBlock( x, y, z );
    unsigned char inf;
    // if( b != NULL )
    inf= b->Block::Transparent();
    //if( inf == 6 )
    //   inf= 2;
    //else
    //  inf = 2;

    shred->visibly_information[ (x<<11) | (y<<7) | z ]= inf;

    host_data_mutex.unlock();
}

void r_ShredInfo::GetVisiblyInformation()
{
    //false видим
    //true невидим
    Block* b;
    unsigned int x, y, z;

    unsigned char* inf= visibly_information;
    for( x= 0; x< R_SHRED_WIDTH; x++ )
    {
        for( y= 0; y< R_SHRED_WIDTH; y++ )
        {
            for( z= 0; z< R_SHRED_HEIGHT; z++ )
            {
                b= shred->GetBlock( x, y, z );
                // if( b != NULL )
                inf[0]= b->Block::Transparent();
                //if( inf[0] == 6 )
                //    inf[0]= 2;


                inf++;
            }
            inf +=( 128 - R_SHRED_HEIGHT );//компенсация различий высоты (реальной и в массиве)
        }
    }
}



void r_ShredInfo::GetQuadCount()
{
    quint16 x, y, z;
    unsigned int result= 0;

    unsigned char x1= false , y1= false , z1= false;
    unsigned char current= 0;
    //false видим
    //true невидим
    max_geometry_height= 0;
    min_geometry_height= 128;
    unsigned int model_count= 0;

    for( x= 0; x< R_SHRED_WIDTH  - 1 ; x++ )
    {
        for( y= 0; y< R_SHRED_WIDTH  - 1; y++ )
        {
            for( z= 0; z< R_SHRED_HEIGHT  - 2; z++ )
            {
                current= z1;

                x1= visibly_information[ ((x+1)<<11) | (y<<7) | z ] &3;
                y1= visibly_information[ (x<<11) | ((y+1)<<7) | z ] &3;
                z1= visibly_information[ (x<<11) | (y<<7) | (z+1) ];

                if( z1 & 4 )
                {
                    model_count++;
                    max_geometry_height= max( z, max_geometry_height );
                    min_geometry_height= min( z, min_geometry_height );
                }
                z1&= 3;

                if( current != x1 )
                {
                    max_geometry_height= max( z, max_geometry_height );
                    min_geometry_height= min( z, min_geometry_height );
                    result ++;
                }
                if( current != y1 )
                {
                    max_geometry_height= max( z, max_geometry_height );
                    min_geometry_height= min( z, min_geometry_height );
                    result ++;
                }
                if( current != z1 )
                {
                    max_geometry_height= max( z, max_geometry_height );
                    min_geometry_height= min( z, min_geometry_height );
                    result ++;
                }
            }
            z1= false;
        }
    }


    /*обход южной границы*/
    y= R_SHRED_WIDTH  - 1;
    for( x= 0; x< R_SHRED_WIDTH  - 1; x++ )
    {
        z1= false;
        for( z=0; z< R_SHRED_HEIGHT  - 2; z++ )
        {
            current= z1;

            x1= visibly_information[ ((x+1)<<11) | (y<<7) | z ] &3;
            if( south_shred != NULL )
                y1= south_shred->visibly_information[ (x<<11) | z ] &3;
            else
                y1= current;
            z1= visibly_information[ (x<<11) | (y<<7) | (z+1) ];

            if( z1 & 4 )
            {
            	max_geometry_height= max( z, max_geometry_height );
				min_geometry_height= min( z, min_geometry_height );
                model_count++;
            }
            z1&= 3;

            if( current != x1 )
            {
                max_geometry_height= max( z, max_geometry_height );
                min_geometry_height= min( z, min_geometry_height );
                result ++;
            }
            if( current != y1 )
            {
                max_geometry_height= max( z, max_geometry_height );
                min_geometry_height= min( z, min_geometry_height );
                result ++;
            }
            if( current != z1 )
            {
                max_geometry_height= max( z, max_geometry_height );
                min_geometry_height= min( z, min_geometry_height );
                result ++;
            }

        }

    }

    /*обход восточной границы*/
    x= R_SHRED_WIDTH  - 1;
    for( y= 0; y< R_SHRED_WIDTH  - 1; y++ )
    {
        z1= false;
        for( z=0; z< R_SHRED_HEIGHT  - 2; z++ )
        {
            current= z1;
            if( east_shred != NULL )
                x1= east_shred->visibly_information[ (y<<7) | z ] &3;
            else
                x1= current;
            y1= visibly_information[ (x<<11) | ((y+1)<<7) | z ] &3;
            z1= visibly_information[ (x<<11) | (y<<7) | (z+1) ];

            if( z1 & 4 )
            {
            	max_geometry_height= max( z, max_geometry_height );
                    min_geometry_height= min( z, min_geometry_height );
                model_count++;
            }
            z1&= 3;

            if( current != x1 )
            {
                max_geometry_height= max( z, max_geometry_height );
                min_geometry_height= min( z, min_geometry_height );
                result ++;
            }
            if( current != y1 )
            {
                max_geometry_height= max( z, max_geometry_height );
                min_geometry_height= min( z, min_geometry_height );
                result ++;
            }
            if( current != z1 )
            {
                max_geometry_height= max( z, max_geometry_height );
                min_geometry_height= min( z, min_geometry_height );
                result ++;
            }

        }

    }
    /*обход юговосточного угла*/

    z1= false;
    y = x= R_SHRED_WIDTH  - 1;
    for( z=0; z< R_SHRED_HEIGHT  - 2; z++ )
    {
        current= z1;
        if( east_shred != NULL )
            x1= east_shred->visibly_information[ (y<<7) | z ] &3;
        else
            x1= current;
        if( south_shred != NULL )
            y1= south_shred->visibly_information[ (x<<11) | z ] &3;
        else
            y1= current;
        z1= visibly_information[ (x<<11) | (y<<7) | (z+1) ];

        if( z1 & 4 )
        {
        	max_geometry_height= max( z, max_geometry_height );
                    min_geometry_height= min( z, min_geometry_height );
            model_count++;
        }
        z1&= 3;

        if( current != x1 )
        {
            max_geometry_height= max( z, max_geometry_height );
            min_geometry_height= min( z, min_geometry_height );
            result ++;
        }
        if( current != y1 )
        {
            max_geometry_height= max( z, max_geometry_height );
            min_geometry_height= min( z, min_geometry_height );
            result ++;
        }
        if( current != z1 )
        {
            max_geometry_height= max( z, max_geometry_height );
            min_geometry_height= min( z, min_geometry_height );
            result ++;
        }

    }

    //if( max_geometry_height != R_SHRED_HEIGHT - 3 )
    //	max_geometry_height++;

    quad_count= result;
    model_count_to_draw= model_count;
   // model_list.Clear();
    //model_list.SetModelCount( model_count );
    // if( model_count != 0 )
    //   printf( "models: %d\n", model_count );
}


///ACHTUNG!!!
//нижеприведённые макросы существуют для оптимизации процесса построения меша по лоскуту
//а так-же для отсутствия дублирования кода. Макрос прибит к методу r_ShredInfo::BuildShred()

#define BUILD_QUAD_X \
tex_id= r_TextureManager::GetBlockTexture( b->GetId(), normal_id, b->Block::GetDir() );\
tex_scale= r_TextureManager::GetBlockTextureScale( tex_id );\
r_TextureManager::GetBlockTextureBasis( normal_id, b->Block::GetDir(), texture_basis );\
tc_x= ( ( -y * texture_basis[0] + z * texture_basis[1] ) * tex_scale )&63;\
tc_y= ( ( z * texture_basis[3]  - y * texture_basis[2] ) * tex_scale )&63;\
\
if( b->GetId() == BlockManager::MakeId( LIQUID, WATER ) )\
{\
	tmp_vert_p= shred_vertices;\
	shred_vertices= water_vertices;\
}\
shred_vertices[ v13[1] ].coord[0]= x + X;\
shred_vertices[ v13[1] ].coord[1]= y + Y;\
shred_vertices[ v13[1] ].coord[2]= z;\
shred_vertices[ v13[1] ].tex_coord[2]= tex_id;\
shred_vertices[ v13[1] ].tex_coord[1  ]= tc_y + tex_scale * texture_basis[3];\
shred_vertices[ v13[1] ].tex_coord[0]= tc_x + tex_scale * texture_basis[1];\
\
shred_vertices[2].coord[0]= x + X;\
shred_vertices[2].coord[1]= y - 1 + Y;\
shred_vertices[2].coord[2]= z;\
shred_vertices[2].tex_coord[2]= tex_id;\
shred_vertices[2].tex_coord[1  ]= tc_y + tex_scale * ( texture_basis[2] + texture_basis[3] );\
shred_vertices[2].tex_coord[0  ]= tc_x + tex_scale * ( texture_basis[0] + texture_basis[1] );\
\
shred_vertices[ v13[0] ].coord[0]= x + X;\
shred_vertices[ v13[0] ].coord[1]= y - 1 + Y;\
shred_vertices[ v13[0] ].coord[2]= z - 1;\
shred_vertices[ v13[0] ].tex_coord[2]= tex_id;\
shred_vertices[ v13[0] ].tex_coord[1]= tc_y + tex_scale * texture_basis[2];\
shred_vertices[ v13[0] ].tex_coord[0  ]= tc_x + tex_scale * texture_basis[0];\
\
shred_vertices[0].coord[0]= x + X;\
shred_vertices[0].coord[1]= y + Y;\
shred_vertices[0].coord[2]= z - 1;\
shred_vertices[0].tex_coord[2]= tex_id;\
shred_vertices[0].tex_coord[1]= tc_y;\
shred_vertices[0].tex_coord[0]= tc_x;\
\
shred_vertices[3].normal_id=\
    shred_vertices[2].normal_id=\
        shred_vertices[1].normal_id=\
			shred_vertices[0].normal_id= normal_id;\
\
shred_vertices[3].light[0]=\
    shred_vertices[2].light[0]=\
        shred_vertices[1].light[0]=\
            shred_vertices[0].light[0]= light & 15;\
shred_vertices[3].light[1]=\
    shred_vertices[2].light[1]=\
        shred_vertices[1].light[1]=\
            shred_vertices[0].light[1]= light >> 4;\
shred_vertices+= 4;\
if( b->GetId() == BlockManager::MakeId( LIQUID, WATER ) )\
{\
	water_vertices= shred_vertices-8;\
	shred_vertices= tmp_vert_p;\
	water_quad_count++;\
}



#define BUILD_QUAD_Y \
 \
tex_id= r_TextureManager::GetBlockTexture( b->GetId(), normal_id, b->Block::GetDir() );\
tex_scale= r_TextureManager::GetBlockTextureScale( tex_id );\
r_TextureManager::GetBlockTextureBasis( normal_id, b->Block::GetDir(), texture_basis );\
tc_x= ( ( x * texture_basis[0] + z * texture_basis[1] ) * tex_scale )&63;\
tc_y= ( ( z * texture_basis[3] + x * texture_basis[2] ) * tex_scale )&63;\
\
if( b->GetId() == BlockManager::MakeId( LIQUID, WATER ) )\
{\
	tmp_vert_p= shred_vertices;\
	shred_vertices= water_vertices;\
}\
shred_vertices[0].coord[0]= x + X;\
shred_vertices[0].coord[1]= y + Y;\
shred_vertices[0].coord[2]= z;\
shred_vertices[0].tex_coord[2]= tex_id;\
shred_vertices[0].tex_coord[1]= tc_y + tex_scale * ( texture_basis[2] + texture_basis[3] );\
shred_vertices[0].tex_coord[0]= tc_x + tex_scale * ( texture_basis[0] + texture_basis[1] );\
\
\
shred_vertices[ v13[0] ].coord[0]= x - 1 + X;\
shred_vertices[ v13[0] ].coord[1]= y + Y;\
shred_vertices[ v13[0] ].coord[2]= z;\
shred_vertices[ v13[0] ].tex_coord[2]= tex_id;\
shred_vertices[ v13[0] ].tex_coord[1]= tc_y + tex_scale * texture_basis[3];\
shred_vertices[ v13[0] ].tex_coord[0]= tc_x + tex_scale * texture_basis[1];\
\
shred_vertices[2].coord[0]= x - 1 + X;\
shred_vertices[2].coord[1]= y + Y;\
shred_vertices[2].coord[2]= z - 1;\
shred_vertices[2].tex_coord[2]= tex_id;\
shred_vertices[2].tex_coord[1]= tc_y;\
shred_vertices[2].tex_coord[0]= tc_x;\
\
shred_vertices[ v13[1] ].coord[0]= x + X;\
shred_vertices[ v13[1] ].coord[1]= y + Y;\
shred_vertices[ v13[1] ].coord[2]= z - 1;\
shred_vertices[ v13[1] ].tex_coord[2]= tex_id;\
shred_vertices[ v13[1] ].tex_coord[1]= tc_y + tex_scale * texture_basis[2];\
shred_vertices[ v13[1] ].tex_coord[0]= tc_x + tex_scale * texture_basis[0];\
\
\
shred_vertices[3].normal_id=\
    shred_vertices[2].normal_id=\
        shred_vertices[1].normal_id=\
            shred_vertices[0].normal_id= normal_id;\
\
shred_vertices[3].light[0]=\
    shred_vertices[2].light[0]=\
        shred_vertices[1].light[0]=\
            shred_vertices[0].light[0]= light & 15;\
shred_vertices[3].light[1]=\
    shred_vertices[2].light[1]=\
        shred_vertices[1].light[1]=\
            shred_vertices[0].light[1]= light >> 4;\
shred_vertices+= 4;\
if( b->GetId() == BlockManager::MakeId( LIQUID, WATER ) )\
{\
	water_vertices= shred_vertices-8;\
	shred_vertices= tmp_vert_p;\
	water_quad_count++;\
}


#define BUILD_QUAD_Z \
tex_id= r_TextureManager::GetBlockTexture( b->GetId(), normal_id, b->Block::GetDir() );\
tex_scale= r_TextureManager::GetBlockTextureScale( tex_id );\
r_TextureManager::GetBlockTextureBasis( normal_id, b->Block::GetDir(), texture_basis );\
tc_x= ( ( x * texture_basis[0] - y * texture_basis[1] ) * tex_scale )&63;\
tc_y= ( ( -y * texture_basis[3] + x * texture_basis[2] ) * tex_scale )&63;\
\
if( b->GetId() == BlockManager::MakeId( LIQUID, WATER ) )\
{\
	tmp_vert_p= shred_vertices;\
	shred_vertices= water_vertices;\
}\
shred_vertices[0].coord[0]= x + X;\
shred_vertices[0].coord[1]= y + Y;\
shred_vertices[0].coord[2]= z;\
shred_vertices[0].tex_coord[2]= tex_id;\
shred_vertices[0].tex_coord[1]= tc_y + tex_scale * texture_basis[2];\
shred_vertices[0].tex_coord[0]= tc_x + tex_scale * texture_basis[0];\
\
shred_vertices[ v13[0] ].coord[0]= x + X;\
shred_vertices[ v13[0] ].coord[1]= y - 1 + Y;\
shred_vertices[ v13[0] ].coord[2]= z;\
shred_vertices[ v13[0] ].tex_coord[2]= tex_id;\
shred_vertices[ v13[0] ].tex_coord[1]= tc_y + tex_scale * ( texture_basis[2] + texture_basis[3] );\
shred_vertices[ v13[0] ].tex_coord[0]= tc_x + tex_scale * ( texture_basis[0] + texture_basis[1] );\
\
\
shred_vertices[2].coord[0]= x - 1 + X;\
shred_vertices[2].coord[1]= y - 1 + Y;\
shred_vertices[2].coord[2]= z;\
shred_vertices[2].tex_coord[2]= tex_id;\
shred_vertices[2].tex_coord[1]= tc_y + tex_scale * texture_basis[3];\
shred_vertices[2].tex_coord[0]= tc_x + tex_scale * texture_basis[1];\
\
shred_vertices[ v13[1] ].coord[0]= x - 1 + X;\
shred_vertices[ v13[1] ].coord[1]= y + Y;\
shred_vertices[ v13[1] ].coord[2]= z;\
shred_vertices[ v13[1] ].tex_coord[2]= tex_id;\
shred_vertices[ v13[1] ].tex_coord[1]= tc_y;\
shred_vertices[ v13[1] ].tex_coord[0]= tc_x;\
\
\
shred_vertices[3].normal_id=\
    shred_vertices[2].normal_id=\
        shred_vertices[1].normal_id=\
            shred_vertices[0].normal_id= normal_id;\
\
shred_vertices[3].light[0]=\
    shred_vertices[2].light[0]=\
        shred_vertices[1].light[0]=\
            shred_vertices[0].light[0]= light & 15;\
shred_vertices[3].light[1]=\
    shred_vertices[2].light[1]=\
        shred_vertices[1].light[1]=\
            shred_vertices[0].light[1]= light >> 4;\
shred_vertices+= 4;\
if( b->GetId() == BlockManager::MakeId( LIQUID, WATER ) )\
{\
	water_vertices= shred_vertices-8;\
	shred_vertices= tmp_vert_p;\
	water_quad_count++;\
}

void r_ShredInfo::BuildShred( r_WorldVertex* shred_vertices )
{
	model_list.Clear();
    model_list.SetModelCount( model_count_to_draw );
	model_list.AllocateMemory();

    unsigned char x, y, z;
    Block* b;
    unsigned char x1= false , y1= false , z1= false;
    unsigned char current= false;
    //false видим
    //true невидим
    unsigned char normal_id;
    unsigned char light;
    unsigned char tex_id, tex_scale, tc_x, tc_y;
    char texture_basis[4];

    unsigned int v13[2];
    short X= latitude * 16, Y= longitude * 16;

    /*Поскольку количество квадов с водой становится известно только на этапе построения меша,
    размещать их приходится от конца буффера с данными вершин, и заполнять в обратном направлении*/
    water_vertices= shred_vertices + quad_count * 4 - 4;
    water_quad_count= 0;
    r_WorldVertex* tmp_vert_p;
    /*в конце алгоритма указатель как раз будет указывать на начало буффера с квадами воды*/

    for( x= 0; x< R_SHRED_WIDTH  - 1 ; x++ )
    {
        for( y= 0; y< R_SHRED_WIDTH  - 1; y++ )
        {
            z1= visibly_information[ (x<<11) | (y<<7) | min_geometry_height ];
            for( z= min_geometry_height; z<= max_geometry_height; z++ )
            {
                current= z1;

                x1= visibly_information[ ((x+1)<<11) | (y<<7) | z ] &3;
                y1= visibly_information[ (x<<11) | ((y+1)<<7) | z ] &3;
                z1= visibly_information[ (x<<11) | (y<<7) | (z+1) ];


                if( z1 & 4 )
                {
                    b= shred->GetBlock( x, y, z + 1 );
                    unsigned char s= b->Sub(), k= b->Kind(), d= b->GetDir();
                    model_list.AddModel( b->GetId(), d, r_TextureManager::GetBlockTexture( b->GetId(), 0, 0 ), shred->Lightmap( x, y, z + 1 ), x + X, y + Y, z );
				}
                z1&= 3;

                if( current != x1 )
                {
                    normal_id= WORLD_NORMAL_X;
                    if( current > x1 )
                    {
                        v13[0]= 1;
                        v13[1]= 3;
                        normal_id++;
                        b= shred->GetBlock( x + 1, y, z );
                        light= shred->Lightmap( x , y, z );
                    }
                    else
                    {
                        v13[0]= 3;
                        v13[1]= 1;
                        b= shred->GetBlock( x, y, z );
                        light= shred->Lightmap( x + 1, y, z );
                    }

                    BUILD_QUAD_X
                }
                if( current != y1 )
                {
                    normal_id= WORLD_NORMAL_Y;
                    if( current > y1 )
                    {
                        v13[0]= 1;
                        v13[1]= 3;
                        normal_id++;
                        b= shred->GetBlock( x, y + 1, z );
                        light= shred->Lightmap( x, y, z );
                    }
                    else
                    {
                        v13[0]= 3;
                        v13[1]= 1;
                        b= shred->GetBlock( x, y, z );
                        light= shred->Lightmap( x, y+1, z );
                    }

                    BUILD_QUAD_Y
                }
                if( current != z1 )
                {
                    normal_id= WORLD_NORMAL_Z;
                    if( current > z1 )
                    {
                        v13[0]= 1;
                        v13[1]= 3;
                        normal_id++;
                        b= shred->GetBlock( x, y, z + 1 );
                        light= shred->Lightmap( x , y, z );
                    }
                    else
                    {
                        v13[0]= 3;
                        v13[1]= 1;
                        b= shred->GetBlock( x, y, z );
                        light= shred->Lightmap( x, y, z + 1 );
                    }

                    BUILD_QUAD_Z
                }
            }
            z1= false;
        }
    }

    //обход южной границы
    y= R_SHRED_WIDTH - 1;
    for( x= 0; x< R_SHRED_WIDTH - 1; x++ )
    {
        z1= visibly_information[ (x<<11) | (y<<7) | min_geometry_height ];
        for( z= min_geometry_height; z<= max_geometry_height; z++ )
        {
            current= z1;
            x1= visibly_information[ ((x+1)<<11) | (y<<7) | z ] &3;
            if( south_shred != NULL )
                y1= south_shred->visibly_information[ (x<<11) | z ] &3;
            else
                y1= current;
            z1= visibly_information[ (x<<11) | (y<<7) | (z+1) ];

            if( z1 & 4 )
            {
                b= shred->GetBlock( x, y, z + 1 );
                unsigned char s= b->Sub(), k= b->Kind(), d= b->GetDir();
                model_list.AddModel( b->GetId(), d, r_TextureManager::GetBlockTexture( b->GetId(), 0, 0 ), shred->Lightmap( x, y, z + 1 ), x + X, y + Y, z );
            }
            z1&= 3;

            if( current != x1 )
            {
                normal_id= WORLD_NORMAL_X;
                if( current > x1 )
                {
                    v13[0]= 1;
                    v13[1]= 3;
                    normal_id++;
                    b= shred->GetBlock( x + 1, y, z );
                    light= shred->Lightmap( x , y, z );
                }
                else
                {
                    v13[0]= 3;
                    v13[1]= 1;
                    b= shred->GetBlock( x, y, z );
                    light= shred->Lightmap( x + 1, y, z );
                }

                BUILD_QUAD_X
            }
            if( current != y1 )
            {
                normal_id= WORLD_NORMAL_Y;
                if( current > y1 )
                {
                    v13[0]= 1;
                    v13[1]= 3;
                    normal_id++;
                    b= south_shred->shred->GetBlock( x, 0, z );
                    light= shred->Lightmap( x, y, z );
                }
                else
                {
                    v13[0]= 3;
                    v13[1]= 1;
                    b= shred->GetBlock( x, y, z );
                    light= south_shred->shred->Lightmap( x, 0, z );
                }

                BUILD_QUAD_Y
            }
            if( current != z1 )
            {
                normal_id= WORLD_NORMAL_Z;
                if( current > z1 )
                {
                    v13[0]= 1;
                    v13[1]= 3;
                    normal_id++;
                    b= shred->GetBlock( x, y, z + 1 );
                    light= shred->Lightmap( x, y, z );
                }
                else
                {
                    v13[0]= 3;
                    v13[1]= 1;
                    b= shred->GetBlock( x, y, z );
                    light= shred->Lightmap( x, y, z + 1 );
                }

                BUILD_QUAD_Z
            }
        }
    }


    //обход восточной границы
    x= R_SHRED_WIDTH - 1;
    for( y= 0; y< R_SHRED_WIDTH - 1; y++ )
    {
        z1= visibly_information[ (x<<11) | (y<<7) | min_geometry_height ];
        for( z= min_geometry_height; z<= max_geometry_height; z++ )
        {
            current= z1;
            if( east_shred != NULL )
                x1= east_shred->visibly_information[ (y<<7) | z ] &3;
            else
                x1= current;
            y1= visibly_information[ (x<<11) | ((y+1)<<7) | z ] &3;
            z1= visibly_information[ (x<<11) | (y<<7) | (z+1) ];

            if( z1 & 4 )
            {
                b= shred->GetBlock( x, y, z + 1 );
                unsigned char s= b->Sub(), k= b->Kind(), d= b->GetDir();
                model_list.AddModel( b->GetId(), d, r_TextureManager::GetBlockTexture( b->GetId(), 0, 0 ), shred->Lightmap( x, y, z + 1 ), x + X, y + Y, z );
            }
            z1&= 3;

            if( current != x1 )
            {
                normal_id= WORLD_NORMAL_X;
                if( current > x1 )
                {
                    v13[0]= 1;
                    v13[1]= 3;
                    normal_id++;
                    b= east_shred->shred->GetBlock( 0, y, z );
                    light= shred->Lightmap( x, y, z );
                }
                else
                {
                    v13[0]= 3;
                    v13[1]= 1;
                    b= shred->GetBlock( x, y, z );
                    light= east_shred->shred->Lightmap( 0, y, z );
                }

                BUILD_QUAD_X
            }
            if( current != y1 )
            {
                normal_id= WORLD_NORMAL_Y;
                if( current > y1 )
                {
                    v13[0]= 1;
                    v13[1]= 3;
                    normal_id++;
                    b= shred->GetBlock( x, y + 1, z );
                    light= shred->Lightmap( x, y, z );
                }
                else
                {
                    v13[0]= 3;
                    v13[1]= 1;
                    b= shred->GetBlock( x, y, z );
                    light= shred->Lightmap( x, y + 1, z );
                }

                BUILD_QUAD_Y
            }
            if( current != z1 )
            {
                normal_id= WORLD_NORMAL_Z;
                if( current > z1 )
                {
                    v13[0]= 1;
                    v13[1]= 3;
                    normal_id++;
                    b= shred->GetBlock( x, y, z + 1 );
                    light= shred->Lightmap( x, y, z );
                }
                else
                {
                    v13[0]= 3;
                    v13[1]= 1;
                    b= shred->GetBlock( x, y, z );
                    light= shred->Lightmap( x, y, z + 1 );
                }

                BUILD_QUAD_Z
            }
        }
    }

    //обход юговосточного угла
    z1= visibly_information[ (x<<11) | (y<<7) | min_geometry_height ];
    y= x= R_SHRED_WIDTH - 1;
    for( z= min_geometry_height; z<= max_geometry_height; z++ )
    {
        current= z1;
        if( east_shred != NULL )
            x1= east_shred->visibly_information[ (y<<7) | z ] &3;
        else
            x1= current;
        if( south_shred != NULL )
            y1= south_shred->visibly_information[ (x<<11) | z ] &3;
        else
            y1= current;
        z1= visibly_information[ (x<<11) | (y<<7) | (z+1) ];

        if( z1 & 4 )
        {
            b= shred->GetBlock( x, y, z + 1 );
            unsigned char s= b->Sub(), k= b->Kind(), d= b->GetDir();
            model_list.AddModel( b->GetId(), d, r_TextureManager::GetBlockTexture( b->GetId(), 0, 0 ), shred->Lightmap( x, y, z + 1 ), x + X, y + Y, z );
        }
        z1&= 3;

        if( current != x1 )
        {
            normal_id= WORLD_NORMAL_X;
            if( current > x1 )
            {
                v13[0]= 1;
                v13[1]= 3;
                normal_id++;
                b= east_shred->shred->GetBlock( 0, y, z );
                light= shred->Lightmap( x, y, z );
            }
            else
            {
                v13[0]= 3;
                v13[1]= 1;
                b= shred->GetBlock( x, y, z );
                light= east_shred->shred->Lightmap( 0, y, z );
            }

            BUILD_QUAD_X
        }
        if( current != y1 )
        {
            normal_id= WORLD_NORMAL_Y;
            if( current > y1 )
            {
                v13[0]= 1;
                v13[1]= 3;
                normal_id++;
                b= south_shred->shred->GetBlock( x, 0, z );
                light= shred->Lightmap( x, y, z );
            }
            else
            {
                v13[0]= 3;
                v13[1]= 1;
                b= shred->GetBlock( x, y, z );
                light= south_shred->shred->Lightmap( x, 0, z );
            }

            BUILD_QUAD_Y
        }
        if( current != z1 )
        {
            normal_id= WORLD_NORMAL_Z;
            if( current > z1 )
            {
                v13[0]= 1;
                v13[1]= 3;
                normal_id++;
                b= shred->GetBlock( x, y, z + 1 );
                light= shred->Lightmap( x, y, z );
            }
            else
            {
                v13[0]= 3;
                v13[1]= 1;
                b= shred->GetBlock( x, y, z );
                light= shred->Lightmap( x, y,  z + 1 );
            }

            BUILD_QUAD_Z
        }
    }

    water_vertices+=4;
}

unsigned short r_ShredInfo::GetShredDistance( short x, short y )
{
    short dx, dy;
    dx= abs( x - latitude );
    dy= abs( y - longitude );
    return max( dx, dy );
}
bool r_ShredInfo::IsOnOtherSideOfPlane( m_Vec3 point, m_Vec3 normal )
{
    m_Vec3 vec2point;
    float dot;

    vec2point= m_Vec3( latitude * R_SHRED_WIDTH - 1, longitude * R_SHRED_WIDTH - 1, min_geometry_height - 1 ) - point;
    dot= vec2point * normal;
    if( dot > 0.0 )
        return true;

    vec2point= m_Vec3( ( latitude + 1 ) * R_SHRED_WIDTH - 1, longitude * R_SHRED_WIDTH - 1, min_geometry_height - 1) - point;
    dot= vec2point * normal;
    if( dot > 0.0 )
        return true;

    vec2point= m_Vec3( ( latitude + 1 ) * R_SHRED_WIDTH - 1, ( longitude + 1 ) * R_SHRED_WIDTH - 1, min_geometry_height - 1 ) - point;
    dot= vec2point * normal;
    if( dot > 0.0 )
        return true;

    vec2point= m_Vec3( latitude * R_SHRED_WIDTH - 1, ( longitude + 1 ) * R_SHRED_WIDTH - 1, min_geometry_height - 1 ) - point;
    dot= vec2point * normal;
    if( dot > 0.0 )
        return true;

    //top side
    vec2point= m_Vec3( latitude * R_SHRED_WIDTH - 1, longitude * R_SHRED_WIDTH - 1, max_geometry_height + 1 ) - point;
    dot= vec2point * normal;
    if( dot > 0.0 )
        return true;

    vec2point= m_Vec3( ( latitude + 1 ) * R_SHRED_WIDTH - 1, longitude * R_SHRED_WIDTH - 1, max_geometry_height + 1 ) - point;
    dot= vec2point * normal;
    if( dot > 0.0 )
        return true;

    vec2point= m_Vec3( ( latitude + 1 ) * R_SHRED_WIDTH - 1, ( longitude + 1 ) * R_SHRED_WIDTH - 1, max_geometry_height + 1 ) - point;
    dot= vec2point * normal;
    if( dot > 0.0 )
        return true;

    vec2point= m_Vec3( latitude * R_SHRED_WIDTH - 1, ( longitude + 1 ) * R_SHRED_WIDTH - 1, max_geometry_height + 1 ) - point;
    dot= vec2point * normal;
    if( dot > 0.0 )
        return true;

    return false;
}

#endif// SHRED_INFO_CPP
