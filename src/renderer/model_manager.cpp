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
#ifndef MODEL_MANAGER_CPP
#define MODEL_MANAGER_CPP

#include "model_manager.h"
#include "../header.h"
#include "../BlockManager.h"
#include <fstream>

unsigned char r_ModelManager::model_table[ (LAST_KIND<<8)  * 8 ];

unsigned char r_ModelManager::GetModelId( unsigned short block_id, unsigned char direction )
{
    return model_table[ ( block_id << 3 ) | direction ];

}

unsigned char r_ModelManager::InitModelTable()
{
    for( int i= 0; i< (LAST_KIND<<8) * 8; i++ )
        model_table[i]= 0;

}

void r_ModelManager::LoadModels()
{

    LoadModelsFiles();


    //������� ���������� ������, ��������, ������� �������� � ����� ������
    unsigned int v_p= 0, i_p= 0, i;
    for( i= 0; i< model_count; i++ )
    {
        index_pointer[i]= i_p;
        vertex_pointer[i]= v_p;
        vertex_count[i]= models[i].vertex_count;
        index_count[i]=  models[i].index_count;
        v_p+= vertex_count[i];
        i_p+= index_count[i];
    }
    //����� �������� �������
    for( i= 1; i< model_count; i++ )
        rShiftModelIndeces( &models[i], vertex_pointer[i] );


    index_buffer_size= index_pointer[ model_count - 1 ] + index_count[ model_count - 1 ];
    vertex_buffer_size= vertex_pointer[ model_count - 1 ] + vertex_count[ model_count  - 1 ];

    model_buffer.VertexData( NULL, vertex_buffer_size * sizeof( fmd_Vertex ), sizeof( fmd_Vertex ), 0 );
    model_buffer.IndexData( NULL, index_buffer_size * sizeof(quint16), GL_UNSIGNED_SHORT, GL_TRIANGLES );
    for( i= 0; i< model_count; i++ )
    {
        model_buffer.VertexSubData( (float*) models[i].vertices,
                                    models[i].vertex_count * sizeof( fmd_Vertex ),
                                    vertex_pointer[i] * sizeof( fmd_Vertex ) );
        model_buffer.IndexSubData( (unsigned int*)models[i].indeces,
                                   models[i].index_count * sizeof( quint16 ),
                                   index_pointer[i] * sizeof( quint16 ) );
    }

#ifndef OGL21
    short swiborg_data[]= { 0,0,64,10, 5,5,70,10, 6,5,70,10, 23,14,63,10 };
    glGenBuffers( 1, &texture_buffer );
    glBindBuffer( GL_TEXTURE_BUFFER, texture_buffer );
    glBufferData( GL_TEXTURE_BUFFER, 16384, NULL, GL_STATIC_DRAW );

    glGenTextures( 1, &tex );
    glBindTexture( GL_TEXTURE_BUFFER, tex );
    //glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );
    glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA16I, texture_buffer );
#endif
    //model_instance_data= NULL;
    // LoadModels2();
}



void r_ModelManager::LoadModelsFiles()
{
    std::ifstream f( "models/models.cfg" );
    if( f.fail() )
        return;

    char str[64];
    char model_name[128];
    unsigned int i= 0, source_model_id;
    unsigned short block_id;
    unsigned int model_rotation;// �����������, � ������� ������ �������� � �����
    unsigned int rotation_variants[4];
    unsigned int rotation_variants_num;
    unsigned short kind, sub;

    qint16 transform_matrices[]=
    {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1,
        0, 0, 0,// no rotation

        -1, 	0, 	  0,
        0, 		-1,   0,
        0, 		0, 	  1,
        -1024, 	-1024, 	 0, //180 deg

         0,    1, 0,
        -1,	  0, 0,
        0,    0,  1,
        -1024,	 0,  0,  // 90 deg

       0,   -1,  0,
        1,	  0,  0,
        0,    0,  1,
        0 , -1024,  0,  // 270 ( -90) deg


    };
    short* mat;
    while( !f.eof() )
    {
        f>>model_name;
        if( rLoadModel( &models[i], model_name ) )
        {
            if( !strcmp( "fend", model_name ) )
            {
                f.close();
                return;
            }
            else
                printf( "error. model \"%s\" not found.\n", model_name );
        }
        source_model_id= i;
        i++;


        f>>str;
        if( str[0] != '{' || str[1] != 0 )
        {
            printf( "parse error. model info.\n" );
            return;
        }

        while( !f.eof() ) //one model data
        {
            f>>str;
            if( str[0] == '}' && str[1] == 0 )
                break;
            if( !strcmp( str, "rotation_variants" ) )
            {
                f>>str;
                if( ! (str[0]== '{' && str[1] == 0 ) )
                {
                    printf( "model conig parsing error - rotation variants\n" );

                }
                rotation_variants_num= 1;
                rotation_variants[0]= NORTH;
                while(  !f.eof() )
                {

                    f>>str;
                    if( str[0] == '}' && str[1] == 0 )
                        break;

                    if( !strcmp( str, "east" ) ) rotation_variants[ rotation_variants_num ]= EAST;
                    else if( !strcmp( str, "west" ) ) rotation_variants[ rotation_variants_num ]= WEST;
                    else if( !strcmp( str, "south" ) ) rotation_variants[ rotation_variants_num ]= SOUTH;
                    else if( !strcmp( str, "north" ) ) rotation_variants[ rotation_variants_num ]= NORTH;
                    else if( !strcmp( str, "universal" ) ) rotation_variants[ rotation_variants_num ]= NORTH;

                    if( rotation_variants[ rotation_variants_num ] != NORTH )
                    {
                        models[i].index_count= models[ source_model_id ].index_count;
                        models[i].vertex_count= models[ source_model_id ].vertex_count;
                        models[i].indeces= new quint16[ models[i].index_count ];
                        models[i].vertices= new fmd_Vertex[ models[i].vertex_count ];
                        rTransformModel( &models[i], &models[ source_model_id ],
                                         transform_matrices +  (rotation_variants[ rotation_variants_num ] - 2)*12 );
                        memcpy( models[i].indeces, models[ source_model_id ].indeces, sizeof( qint16 ) * models[i].index_count );
                        i++;
                        rotation_variants_num++;
                    }


                }
            }
            else if( !strcmp( str, "blocks" ) )
            {
                f>>str;
                if( ! (str[0]== '{' && str[1] == 0 ) )
                {
                    printf( "model conig parsing error - blocks\n" );

                }
                while(  !f.eof() )
                {

                    f>>str;
                    if( str[0] == '}' && str[1] == 0 )
                        break;

                    QString qstr(str);
                    kind= BlockManager::StringToKind( qstr );
                    f>>str;
                    qstr= str;
                    sub= BlockManager::StringToSub( qstr );
                    if( kind < LAST_KIND && sub < LAST_SUB )
                    {
                        unsigned short block_id= BlockManager::MakeId( kind, sub );

                        for( int j= 0; j< 8; j++ )
                            model_table[ ( block_id << 3 ) | j ] = source_model_id;
                        for( int j= source_model_id; j< i; j++ )
                            model_table[ ( block_id << 3 ) | rotation_variants[ j - source_model_id ] ]= j;

                    }
                    else
                        printf( "model: %s unknown kind\sub\n", model_name );
                }
            }
        }

    }
    model_count= i;
    f.close();
}



void r_ModelManager::DrawModels( const r_GLSLProgram* shader )
{
    // return;


    glActiveTexture( GL_TEXTURE0 + 2 );
    glBindTexture( GL_TEXTURE_BUFFER, tex );
    shader->Uniform( "model_buffer", 2 );

    model_buffer.Bind();
    //unsigned int un_id= shader->GetUniformId( "texture_buffer_shift" );
    for( unsigned int i= 0; i< model_count; i++ )
    {
        if( instance_model_count[i] != 0 )
        {
            shader->Uniform( "texture_buffer_shift", (int)instance_data_pointer[i] );
            glDrawElementsInstanced( GL_TRIANGLES, index_count[i], GL_UNSIGNED_SHORT,
                                     (void*) (index_pointer[i] * sizeof(quint16)), instance_model_count[i] );
        }
    }
}

void r_ModelManager::InitVertexBuffer()
{
    //return;
    model_buffer.Bind();
    model_buffer.VertexAttribPointer( 0, 3, GL_SHORT, false, 0 );
    model_buffer.VertexAttribPointer( 1, 2, GL_SHORT, false, 6 );
    model_buffer.VertexAttribPointer( 2, 3, GL_BYTE, true, 10 );
}

void r_ModelManager::ClearModelList()
{
    //return;
    instance_total_model_count= 0;
    for( int i=0; i< model_count; i++ )
        instance_model_count[i]= 0;
}
void r_ModelManager::ReserveData( r_LocalModelList* model_list )
{
    //return;
    instance_total_model_count+= model_list->total_model_count;
    for( int i=0; i< model_count; i++ )
        instance_model_count[i]+= model_list->model_count[i];

}

void r_ModelManager::AllocateMemory()
{
    //return;
    unsigned int n;
    if( instance_total_model_count > model_buffer_size )
    {
        if( model_instance_data != NULL )
            delete[] model_instance_data;
        n= ( instance_total_model_count ) +  ( instance_total_model_count /4 );
        model_instance_data= new short[ n * 4 ];
        model_buffer_size= n;
        printf( "model memory: %u models\n", instance_total_model_count );
    }

    instance_data_pointer[0]= 0;
    for( unsigned int i= 1; i< model_count; i++ )
    {
        instance_data_pointer[i]= instance_data_pointer[ i - 1 ] + instance_model_count[ i - 1 ];
    }

    for( unsigned int i= 0; i< model_count; i++ )
    {
        instance_model_count[i]= 0;
    }
}

void r_ModelManager::AddModels( r_LocalModelList* model_list )
{
    //return;
    unsigned int n;
    unsigned char m_id;
    for( unsigned int i= 0; i< model_list->total_model_count; i++ )
    {
        m_id= model_list->model_id[i];
        n= ( instance_data_pointer[ m_id ] + instance_model_count[ m_id ] ) * 4;
        instance_model_count[ m_id ]++;

        model_instance_data[n    ]= model_list->coord[ i * 3    ];
        model_instance_data[n + 1]= model_list->coord[ i * 3 + 1];
        model_instance_data[n + 2]= model_list->coord[ i * 3 + 2];
        model_instance_data[n + 3]= ( model_list->tex_id[i] << 8 ) | model_list->packed_light[i];
    }
}
void r_ModelManager::GenerateInstanceDataBuffer()
{
    //return;
#ifndef OGL21
    glBindBuffer( GL_TEXTURE_BUFFER, texture_buffer );
    glBufferSubData( GL_TEXTURE_BUFFER, 0, 2 * 4 * instance_total_model_count, model_instance_data );
#endif
}

void r_LocalModelList::AllocateMemory()
{
    //return;
    if( list_size < total_model_count )
    {
        if( tex_id != NULL )
        {
            delete[] tex_id;
            delete[] coord;
            delete[] packed_light;
            delete[] model_id;
        }

        unsigned int n= total_model_count + ( total_model_count  /4 ) ;
        tex_id= 		new quint8[n];
        coord= 			new qint16[n*3];
        packed_light=	new quint8[n];
        model_id= 		new quint8[n];

        list_size= n;
    }
}

void r_LocalModelList::Clear()
{
    //return;
    total_model_count= 0;
    pointer= 0;
    for( unsigned int i=0; i< r_ModelManager::MODEL_COUNT; i++ )
    {
        model_count[i]= 0;
    }
}

void r_LocalModelList::AddModel( unsigned short block_id, unsigned char direction, unsigned char packed_light, unsigned char tex_id, short x, short y, short z )
{
    //return;
    unsigned char m_id= r_ModelManager::GetModelId( block_id, direction );
    this->model_id[ pointer ]= m_id;
    this->model_count[ m_id ]++;

    this->tex_id[ pointer ] = tex_id;
    this->coord[ pointer * 3 	]= x;
    this->coord[ pointer * 3 +1 ]= y;
    this->coord[ pointer * 3 +2 ]= z;
    this->packed_light[ pointer ]= packed_light;

    pointer++;
}


int rLoadModel( r_Model* model, const char* file_name )
{
    fmd_FileHeader fh;
    FILE* f;
    unsigned short tmp;

    f= fopen( file_name, "rb" );
    if( f == NULL )
    {
        printf( "error. Model file \"%s\" not found\n", file_name );
        return 1;
    }
    fread( &fh, 1, sizeof( fmd_FileHeader ), f );

    fread( &tmp, 1, 2, f );
    fread( &tmp, 1, 2, f );

    fread( &tmp, 1, 2, f );
    model->vertex_count= tmp;
    fread( &tmp, 1, 2, f );
    model->index_count= tmp;

    model->vertices= new fmd_Vertex[ model->vertex_count ];
    model->indeces= new quint16[ model->index_count ];

    fread( model->vertices, 1, sizeof( fmd_Vertex ) * model->vertex_count, f );
    fread( model->indeces, 1, sizeof( quint16 ) * model->index_count, f );

    fclose( f );
    return 0;
}

//rotate matrix:
/*
			  | 0 1 2 |
( x, y, z ) * | 3 4 5 |
			  | 6 7 8 |
			  |9 10 11|
*/
void rTransformModel( r_Model* model_dst, const r_Model* model_src, const qint16* mat )
{
    unsigned int i;
    qint8 norm_mat[9];
    fmd_Vertex *v_src= model_src->vertices, *v_dst= model_dst->vertices;

    for( i= 0; i< 9; i++ )
        norm_mat[i]= qint8( mat[i] );

    for( i= 0; i< model_src->vertex_count; i++ , v_dst++, v_src++ )
    {
        v_dst->coord[0]= v_src->coord[0] * mat[0] +  v_src->coord[1] * mat[3] +  v_src->coord[2] * mat[6] + mat[9];
        v_dst->coord[1]= v_src->coord[0] * mat[1] +  v_src->coord[1] * mat[4] +  v_src->coord[2] * mat[7] + mat[10];
        v_dst->coord[2]= v_src->coord[0] * mat[2] +  v_src->coord[1] * mat[5] +  v_src->coord[2] * mat[8] + mat[11];

        v_dst->normal[0]= v_src->normal[0] * norm_mat[0] + v_src->normal[1] * norm_mat[3] + v_src->normal[2] * norm_mat[6];
        v_dst->normal[1]= v_src->normal[0] * norm_mat[1] + v_src->normal[1] * norm_mat[4] + v_src->normal[2] * norm_mat[7];
        v_dst->normal[2]= v_src->normal[0] * norm_mat[2] + v_src->normal[1] * norm_mat[5] + v_src->normal[2] * norm_mat[8];


        v_dst->tex_coord[0]= v_src->tex_coord[0];
        v_dst->tex_coord[1]= v_src->tex_coord[1];
    }
}

void rShiftModelIndeces( r_Model* model, unsigned short shift )
{
    for( unsigned int i= 0; i< model->index_count; i++ )
        model->indeces[i]+= shift;
}
#endif//MODEL_MANAGER_CPP
