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

#ifndef RENDERER_CPP
#define RENDERER_CPP

#include <fstream>
#include "fmd_format.h"
#include "renderer.h"
#include "../screen.h"
#include "../Player.h"
#include "rendering_constants.h"
r_Renderer* r_Renderer::current_renderer= NULL;

void r_Renderer::CalculateFPS()
{
    unsigned int new_time = -QTime::currentTime().msecsTo( startup_time );

    if( new_time - last_fps_time > 1000 )
    {
        fps= 1000 * (  frame_count - last_frame_number  ) / ( new_time - last_fps_time );
        last_fps_time= new_time;
        last_frame_number= frame_count;

        max_fps_to_draw= max_fps;
        min_fps_to_draw= min_fps;
        max_fps= 0.0;
        min_fps= 2000.0;
        shreds_update_per_second_to_draw= shreds_update_per_second;
        shreds_update_per_second= 0;
    }

    if( new_time - last_frame_time  > 0 )
    {
        max_fps= max( 1000.0f / float( new_time - last_frame_time ), max_fps );
        //max_fps= max( max_fps, float( new_time - last_frame_time ) );
        min_fps= min( 1000.0f / float( new_time - last_frame_time ), min_fps );
        //min_fps= min( min_fps, float( new_time - last_frame_time ) );
    }
    last_frame_time= new_time;
    frame_count ++;
}

void r_Renderer::SetupVertexBuffers()
{
    r_WorldVertex vert;

    /*temp_buffer.VertexData( NULL, 48, sizeof( r_WorldVertex ), 0 );
    temp_buffer.IndexData( NULL, 64, GL_UNSIGNED_INT, GL_TRIANGLES );

    int t= (int)(((char*)vert.coord) - ((char*)&vert));
    temp_buffer.VertexAttribPointer( ATTRIB_POSITION, 3, GL_SHORT, false, t  );
    t= (int)(((char*)vert.tex_coord) - ((char*)&vert));
    temp_buffer.VertexAttribPointer( ATTRIB_TEX_COORD, 2, GL_UNSIGNED_BYTE, false, t );
    t= (int)(((char*)&vert.normal_id) - ((char*)&vert));
    temp_buffer.VertexAttribPointer( ATTRIB_NORMAL, 1, GL_UNSIGNED_BYTE, false, t );
    t= (int)(((char*)&vert.light) - ((char*)&vert));
    temp_buffer.VertexAttribPointer(  , 1, GL_UNSIGNED_BYTE, false, t );*/



    world_buffer.VertexData( (float*)vertex_buffer, sizeof(r_WorldVertex) * vertex_buffer_size ,sizeof(r_WorldVertex) , 0 );
    world_buffer.IndexData( (unsigned int)indeces, sizeof(quint16) * index_buffer_size, GL_UNSIGNED_SHORT, GL_TRIANGLES );


    int t= (int)(((char*)vert.coord) - ((char*)&vert));
    world_buffer.VertexAttribPointer( ATTRIB_POSITION, 3, GL_SHORT, false, t  );
    t= (int)(((char*)vert.tex_coord) - ((char*)&vert));
    world_buffer.VertexAttribPointer( ATTRIB_TEX_COORD, 3, GL_BYTE, false, t );
    //in ogl 2.1 intexer vertex attribs not supported
#ifdef OGL21
    t= (int)(((char*)&vert.normal_id) - ((char*)&vert));
    world_buffer.VertexAttribPointer( ATTRIB_NORMAL, 1, GL_UNSIGNED_BYTE, false, t );
#else
    t= (int)(((char*)&vert.normal_id) - ((char*)&vert));
    world_buffer.VertexAttribPointerInt( ATTRIB_NORMAL, 1, GL_UNSIGNED_BYTE, t );
#endif
    t= (int)(((char*)vert.light) - ((char*)&vert));
    world_buffer.VertexAttribPointer( /*ATTRIB_USER0*/3/*light*/ , 2, GL_UNSIGNED_BYTE, false, t );


    float sky_vertices[]= { 256.0, 256.0, 256.0, -256.0, 256.0, 256.0,
                            256.0, -256.0, 256.0, -256.0, -256.0, 256.0,
                            256.0, 256.0, -256.0, -256.0, 256.0, -256.0,
                            256.0, -256.0, -256.0, -256.0, -256.0, -256.0
                          };
    quint16 sky_indeces[]= { 0, 1, 5,  0, 5, 4,
                             0, 4, 6,  0, 6, 2,
                             //4, 5, 7,  4, 7, 6,//bottom
                             0, 3, 1,  0, 2, 3, //top
                             2, 7, 3,  2, 6, 7,
                             1, 3, 7,  1, 7, 5
                           };
    sky_buffer.VertexData( sky_vertices, sizeof(float) * 8 * 3, sizeof(float) * 3, 0 );
    sky_buffer.IndexData( (quint32*)sky_indeces, sizeof(quint16) * 30, GL_UNSIGNED_SHORT, GL_TRIANGLES );
    sky_buffer.VertexAttribPointer( 0, 3, GL_FLOAT, false, 0 );


    text_buffer.VertexData( NULL, R_LETTER_BUFFER_LEN * 4 * sizeof(r_FontVertex),
                            sizeof(r_FontVertex), 0 );
    text_buffer.IndexData( (quint32*) font_indeces,
                           R_LETTER_BUFFER_LEN * 6 * sizeof(quint16),
                           GL_UNSIGNED_SHORT, GL_TRIANGLES );
    r_FontVertex fv;
    t= (int)(((char*)fv.coord) - ((char*)&fv));
    text_buffer.VertexAttribPointer( ATTRIB_POSITION,   2, GL_FLOAT, false, t );
    t= (int)(((char*)fv.tex_coord) - ((char*)&fv));
    text_buffer.VertexAttribPointer( ATTRIB_TEX_COORD,  2, GL_UNSIGNED_SHORT, false, t );
    t= (int)(((char*)fv.color) - ((char*)&fv));
    text_buffer.VertexAttribPointer( /*color*/2,        3, GL_UNSIGNED_BYTE, true , t );
    t= (int)(((char*)&fv.tex_id) - ((char*)&fv));
    text_buffer.VertexAttribPointer( /*tex_id*/3,       1, GL_UNSIGNED_BYTE, false , t );


    float fq[]= { 	1.0, 1.0,  1.0, -1.0,
                    -1.0, -1.0, -1.0, 1.0
                };
    quint16 fq_ind[]= { 0, 1, 2, 0, 2, 3 };
    fullscreen_quad.VertexData( fq, sizeof( float ) * 8, sizeof( float ) * 2,  0 );
    fullscreen_quad.IndexData( (unsigned int*) fq_ind, sizeof( quint16) * 6, GL_UNSIGNED_SHORT, GL_TRIANGLES );
    fullscreen_quad.VertexAttribPointer( 0, 2, GL_FLOAT, false, 0 );


    float cube_vertices[]= { 0.0, 0.0, 0.0, 	0.0, 0.0, -1.0,
                             -1.0, 0.0, 0.0, 	-1.0, 0.0, -1.0,
                             -1.0, -1.0, 0.0, 	-1.0, -1.0, -1.0,
                             0.0, -1.0, 0.0, 	0.0, -1.0, -1.0
                           };
    quint16 cube_indeces[]= { 0,1, 2,3, 4,5, 6,7,
                              0,2, 1,3, 2,4, 3,5,
                              4,6, 5,7, 6,0, 7,1
                            };
    cube_buffer.VertexData( cube_vertices, 3 * 8 * sizeof( float ), sizeof(float) * 3, 0 );
    cube_buffer.IndexData( (unsigned int*) cube_indeces, 2 * 4 * 4 * sizeof(quint16), GL_UNSIGNED_SHORT, GL_LINES );
    cube_buffer.VertexAttribPointer( 0, 3, GL_FLOAT, false, 0 );

    model_manager.InitVertexBuffer();
    rain.InitVertexBuffer();
}

void r_Renderer::DrawWorld()
{
    frame_cam_angle= cam_angle;
    m_Mat4 rotX, rotZ, per, tr, chang;
    m_Vec3 tr_vec= - frame_cam_position;
    rotX.Identity();
    rotX.RotateX( frame_cam_angle.x - m_Math::FM_PI );
    rotZ.Identity();
    rotZ.RotateZ( -frame_cam_angle.z );
    tr.Identity();
    tr.Translate( tr_vec );
    chang.Identity();
    chang[10]= chang[5]= 0.0;
    chang[6]= 1.0;
    chang[9]= -1.0;

    per.MakePerspective( (float) viewport_x/ (float) viewport_y, fov, z_near, z_far );
    view_matrix= tr * rotZ * rotX *chang* per;


#ifdef OGL21
    texture_manager.TextureAtlas()->BindTexture(0);
#else
    texture_manager.TextureArray()->Bind(0);
#endif

    world_shader.Bind();
    world_shader.Uniform( "proj_mat", view_matrix );
    world_shader.Uniform( "tex", 0 );

    if( deferred_shading )
    {

        m_Mat4 nm4= rotZ * rotX * chang;
        normal_matrix= m_Mat3( nm4 );

        //calculate matrix view space to world space
        rotX.Identity();
        rotX.RotateX( - frame_cam_angle.x + m_Math::FM_PI );
        rotZ.Identity();
        rotZ.RotateZ( frame_cam_angle.z );
        tr.Identity();
        tr.Translate( frame_cam_position );
        chang.Identity();
        chang[10]= chang[5]= 0.0;
        chang[6]= -1.0;
        chang[9]= 1.0;

        view_space_shadow_matrix= chang * rotX * rotZ * tr;
        view_space_shadow_matrix*= shadow_matrix;//result shadow matrix for deferred shading

        //world_shader.Uniform( "normal_mat", normal_matrix );// normal in g-buffer stored int world space
    }
    else
    {
        sun_shadow_map[ front_shadowmap ].BindDepthTexture(1);
        texture_manager.BindMaterialPropertyTexture(3);

        world_shader.Bind();

        world_shader.Uniform( "sun_vector", sun_vector );
        world_shader.Uniform( "shadow_map", 1 );
        world_shader.Uniform( "material_property", 3 );
        world_shader.Uniform( "proj_mat", view_matrix );
        world_shader.Uniform( "shadow_mat", shadow_matrix );
        world_shader.Uniform( "cam_pos", frame_cam_position );
        if( world->PartOfDay() == NIGHT )
            world_shader.Uniform( "fog_color",(m_Vec3&)night_fog );
        else
            world_shader.Uniform( "fog_color",(m_Vec3&)day_fog );


        world_shader.Uniform( "direct_sun_light", direct_sun_light );
        world_shader.Uniform( "sky_ambient_light", sky_ambient_light );
        world_shader.Uniform( "fire_ambient_light", fire_ambient_light );


        float max_view= 1.0 / ( float( world->NumShreds() - 1 ) * 0.5f * 16.0f );
        world_shader.Uniform( "max_view2", max_view * max_view );
    }
    /*   for( unsigned int i= 0; i< shreds_to_draw_count; i++ )
       {

           glDrawElements( GL_TRIANGLES,
                           shreds_to_draw_list[i]->quad_count * 6,
                           GL_UNSIGNED_INT,
                           (void*)(shreds_to_draw_list[i]->index_buffer_offset * 6 * 4) );
       }*/
    /* glMultiDrawElements( GL_TRIANGLES, shreds_to_draw_quad_count,
                          GL_UNSIGNED_INT, (GLvoid* const*)shreds_to_draw_indeces,
                          shreds_to_draw_count );*/
    world_buffer.Bind();
#ifdef OGL21
    glMultiDrawArrays( GL_QUADS, shreds_to_draw_base_vertices,
                       shreds_to_draw_quad_count, shreds_to_draw_count );
#else
    glMultiDrawElementsBaseVertex( GL_TRIANGLES, shreds_to_draw_quad_count,
                                   GL_UNSIGNED_SHORT, (GLvoid* const*)shreds_to_draw_indeces,
                                   shreds_to_draw_count, shreds_to_draw_base_vertices );
#endif
    models_shader.Bind();
    models_shader.Uniform( "tex", 0 );
    models_shader.Uniform( "proj_mat", view_matrix );

    if( deferred_shading )
    {
    }
    else
    {
        models_shader.Uniform( "sun_vector", sun_vector );
        models_shader.Uniform( "shadow_map", 1 );
        models_shader.Uniform( "material_property", 3 );
        models_shader.Uniform( "shadow_mat", shadow_matrix );
        models_shader.Uniform( "cam_pos", frame_cam_position );
        models_shader.Uniform( "fog_color",(m_Vec3&)day_fog );

        models_shader.Uniform( "direct_sun_light", direct_sun_light );
        models_shader.Uniform( "sky_ambient_light", sky_ambient_light );
        models_shader.Uniform( "fire_ambient_light", fire_ambient_light );
    }

    model_manager.DrawModels( &models_shader );

    r_Texture::ResetBinding();
}

void r_Renderer::DrawSky()
{

    m_Vec3 sv= sun_vector * sky_light;

    sky_cubemap.Bind(1);
    sky_shader.Bind();
    sky_shader.Uniform( "cu", 1 );
    sky_shader.Uniform( "proj_mat", view_matrix );
    sky_shader.Uniform( "cam_pos", frame_cam_position );
    sky_shader.Uniform( "sun_vector", sv );
    sky_buffer.Show();
}

void r_Renderer::DrawSun()
{
    if( ( world->Time() % SECONDS_IN_DAY ) < ( END_OF_NIGHT + 1 * SECONDS_IN_HOUR ) ||
            ( world->Time() % SECONDS_IN_DAY ) > ( SECONDS_IN_DAY - 1 * SECONDS_IN_HOUR ) )
        return;
    sun_shader.Bind();
    sun_texture.BindTexture(0);
    sun_shader.Uniform( "sun_tex", 0 );
    m_Vec3 sun_pos= frame_cam_position + sun_vector * 200.0;
    sun_shader.Uniform( "sun_pos", sun_pos );
    sun_shader.Uniform( "proj_mat", view_matrix );
    sun_shader.Uniform( "aspect", float( float(viewport_y) / float(viewport_x) ) );
    fullscreen_quad.Show();
}

void r_Renderer::DrawRain()
{
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    float time= float(world->Time() % SECONDS_IN_DAY) / float(SECONDS_IN_HOUR);
    if( !(time > 12.0f && time < 15.0f ) )
        return;

    m_Vec3 particle_size;
    particle_size.y= 1.0f / m_Math::Tan( fov * 0.5f );
    particle_size.x=  particle_size.y * float( viewport_y) / float(viewport_x);
    particle_size*= 0.1f;
#ifdef OGL21// particle size in pixels
    particle_size*= m_Vec3( float( viewport_x ), float( viewport_y ), 0.0 );
#endif

    rain.SetSunShadowmap( sun_shadow_map + front_shadowmap, shadow_matrix );
    rain.SetParticleSize( particle_size );
    rain.SetCamPosition( cam_position );
    rain.SetViewMatrix( view_matrix );

    if( ! use_geometry_particles )
        fullscreen_quad.Bind();
    rain.ShowWeatherParticles( time > 13.5f ? rain.SNOW : rain.RAIN );
}

void r_Renderer::CopyDepthBuffer()
{
    glBindTexture( GL_TEXTURE_2D, scene_depth_buffer );

#ifdef OGL21
    if( underwater )
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, viewport_x/2, viewport_y/2 );
    else
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, viewport_x, viewport_y );
#else
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, viewport_x, viewport_y );
#endif
    r_Texture::ResetBinding();
}

void r_Renderer::DrawWater()
{

    glBlendFunc( GL_ONE, GL_SRC_ALPHA );

    sun_shadow_map[ front_shadowmap ].BindDepthTexture(0);

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_3D, water_texture );

    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, scene_depth_buffer );
    //scene_hdr_buffer.BindDepthTexture(2);

    water_shader.Bind();
    water_shader.Uniform( "proj_mat", view_matrix );
    // water_shader.Uniform( "tex", 0 );


    water_shader.Uniform( "sun_vector", sun_vector );
    water_shader.Uniform( "shadow_map", 0 );
    water_shader.Uniform( "depth_buffer", 2 );
    water_shader.Uniform( "normal_map", 1 );
    water_shader.Uniform( "proj_mat", view_matrix );
    water_shader.Uniform( "shadow_mat", shadow_matrix );
    water_shader.Uniform( "cam_pos", frame_cam_position );
    if( world->PartOfDay() == NIGHT )
        water_shader.Uniform( "fog_color",(m_Vec3&)night_fog );
    else
        water_shader.Uniform( "fog_color",(m_Vec3&)day_fog );


    water_shader.Uniform( "direct_sun_light", direct_sun_light );
    water_shader.Uniform( "sky_ambient_light", sky_ambient_light );
    water_shader.Uniform( "fire_ambient_light", fire_ambient_light );
    water_shader.Uniform( "time", float( last_frame_time ) / 1000.0f );
    water_shader.Uniform( "inv_max_view_distance", 0.03125f*( underwater ? 0.0f : 1.0f ) );

    m_Vec3 inv_screen_size( 1.0f / float( viewport_x ), 1.0f / float( viewport_y ), 0.0f );
    water_shader.Uniform( "inv_screen_size", inv_screen_size );

    m_Vec3 depth_convert_k;
    depth_convert_k.x= - z_far * z_near / ( z_far - z_near );
    depth_convert_k.y= - z_far / ( z_far - z_near );
    water_shader.Uniform( "depth_convert_k", depth_convert_k );

    float max_view= 1.0f / ( float( world->NumShreds() - 1 ) * 0.5f * 16.0f );
    water_shader.Uniform( "max_view2", max_view * max_view );

    m_Vec3 viewport_scale;
    viewport_scale.y= m_Math::Tan( fov * 0.5f );
    viewport_scale.x= viewport_scale.y * float( viewport_x ) / float( viewport_y );
    water_shader.Uniform( "viewport_scale", viewport_scale );


    world_buffer.Bind();

#ifdef OGL21
    glMultiDrawArrays( GL_QUADS, water_quads_base_vertices,
                       water_quads_to_draw_count, water_shreds_to_draw_count );
#else
    glMultiDrawElementsBaseVertex( GL_TRIANGLES, water_quads_to_draw_count,
                                   GL_UNSIGNED_SHORT, (GLvoid* const*)shreds_to_draw_indeces,
                                   water_shreds_to_draw_count, water_quads_base_vertices );
#endif

}
void r_Renderer::DrawHUD()
{
    m_Vec3 p;

    //technical information
    p.y=  1.0f - float( 2 * DEFAULT_FONT_HEIGHT ) * 2.0f / float( viewport_y );
    m_Vec3 text_color( 1.0f, 1.0f, 0.3f );
    p= AddText( float( DEFAULT_FONT_WIDTH ) * 2.0f / float( viewport_x ) - 1.0f,
                p.y, &text_color, 1.0, "fps: %d\n", fps );
    p= AddText( p.x, p.y, &text_color, 1.0, "max fps: %2.1f\n", max_fps_to_draw );
    p= AddText( p.x, p.y, &text_color, 1.0, "min fps: %2.1f\n", min_fps_to_draw );
    p= AddText( p.x, p.y, &text_color, 1.0, "time: %dh %dm\n",
                ( world->Time() / SECONDS_IN_HOUR ) % 24,
                world->Time() % SECONDS_IN_HOUR );

    p= AddText( p.x, p.y, &text_color, 1.0, "shreds: %d/%d\n", shreds_to_draw_count, visibly_world_size[0] * visibly_world_size[1] );
    p= AddText( p.x, p.y, &text_color, 1.0, "shreds updated: %d\n", shreds_update_per_second_to_draw );
    p= AddText( p.x, p.y, &text_color, 1.0, deferred_shading ? "deferred shading\n" : "forward rendering\n");

    quint16 scale_signs[]= { ' ', 0, '#', 0 };
    //hp string
    text_color.x= text_color.y= text_color.z= 0.8;
    char hp_string[32];
    sprintf( hp_string, "HP: %3d\%[", player->HP() );
    p.x= 1.0f - float( 22 * DEFAULT_FONT_WIDTH ) * 2.0f / float( viewport_x );
    p.y=  float( 6 * DEFAULT_FONT_HEIGHT ) * 2.0f / float( viewport_y ) - 1.0f;
    p= AddText( p.x, p.y, &text_color, 1.0, hp_string );
    text_color.x= 0.8, text_color.y= 0.1, text_color.z= 0.1;
    unsigned int hp= player->HP()/10;
    for( int i=0; i< 10; i++ )
        p= AddTextUnicode( p.x, p.y, &text_color, 1.0, ( hp <= i ) ? scale_signs : scale_signs + 2  );

    text_color.x= text_color.y= text_color.z= 0.8;
    p= AddText( p.x, p.y, &text_color, 1, "]\n" );

    //br string
    sprintf( hp_string, "BR: %3d\%[", player->BreathPercent() );
    p.x= 1.0f - float( 22 * DEFAULT_FONT_WIDTH ) * 2.0f / float( viewport_x );
    p= AddText( p.x, p.y, &text_color, 1.0, hp_string );
    text_color.x= 0.1, text_color.y= 0.1, text_color.z= 0.8;
    unsigned int br= player->BreathPercent()/10;
    for( int i=0; i< 10; i++ )
        p= AddTextUnicode( p.x, p.y, &text_color, 1, ( br <= i ) ? scale_signs : scale_signs + 2   );

    text_color.x= text_color.y= text_color.z= 0.8;
    p= AddText( p.x, p.y, &text_color, 1.0, "]\n" );

    //satiation string
    p.x= 1.0f - float( 22 * DEFAULT_FONT_WIDTH ) * 2.0f / float( viewport_x );
    unsigned int sat= player->SatiationPercent();
    sprintf(  hp_string, "SA: %3d%[", sat );
    sat/=10;
    p= AddText( p.x, p.y, &text_color, 1.0, hp_string );
    text_color.x= 0.1, text_color.y= 0.8, text_color.z= 0.1;
     for( int i=0; i< 10; i++ )
        p= AddTextUnicode( p.x, p.y, &text_color, 1, ( sat <= i ) ? scale_signs : scale_signs + 2   );
    text_color.x= text_color.y= text_color.z= 0.8;
     p= AddText( p.x, p.y, &text_color, 1.0, "]\n" );



    //notify log and inventory
    p.x= float( DEFAULT_FONT_WIDTH ) * 2.0f / float( viewport_x ) - 1.0f;
    p.y= float( ( 2 + R_NUMBER_OF_NOTIFY_LINES ) * DEFAULT_FONT_HEIGHT ) * 2.0f / float( viewport_y ) - 1.0f;

    Inventory* inv= player->GetP()->HasInventory();
    char* inv_hand="";
    int inv_count= 0;
    int slot;
    if( inv != 0 )
    {
        slot= 1 + int(!player->IsRightActiveHand());
        inv_hand= inv->InvFullName( slot ).toLocal8Bit().constData();
        inv_count= inv->Number( slot );
    }
    text_color.x= text_color.x= 0.8f;
    text_color.z= 0.1f;
    p.y= AddText( p.x, p.y, &text_color, 1.0f, "%s hand: %s %d\n\n", player->IsRightActiveHand() ? "right" : "left",
                 inv_hand, inv_count ).y;
     //notify log
    text_color.x= text_color.y= text_color.z= 0.8f;
    for( int i= R_NUMBER_OF_NOTIFY_LINES - 1, n; i>= 0; i-- )
    {
        n= ( notify_log_last_line - 1 - i ) % R_NUMBER_OF_NOTIFY_LINES;
        if(  notify_log[n][0] != 0 )
            p=AddTextUnicode( p.x, p.y, &text_color, 1.0, (const quint16*) notify_log[n] );
    }

}

void r_Renderer::DrawBlockMenu()
{
    if( !show_block_list )
        return;
    m_Vec3 c( 1.0, 1.0, 1.0 );

    m_Vec3 p( 1.0f - 2.0f * float( DEFAULT_FONT_WIDTH * 42 ) / viewport_x,
              1.0f - 2.0f * float( DEFAULT_FONT_HEIGHT * 2 ) / viewport_y, 0.0 );
    int i;
    for( i= 0; i< Screen::block_list_size/2; i++ )
        p.y= AddText( p.x, p.y, &c, 1.0f, "%s\n",Screen::block_names[i] ).y;

    p.y= 1.0f - 2.0f * float( DEFAULT_FONT_HEIGHT * 2 ) / viewport_y;
    p.x= 1.0f - 2.0f * float( DEFAULT_FONT_WIDTH * 21 ) / viewport_x;
         for( ; i< Screen::block_list_size; i++ )
    p.y= AddText( p.x, p.y, &c, 1.0f, "%s\n",Screen::block_names[i] ).y;
}

void r_Renderer::DrawConsole()
{
    if( console_string[0] == 0 )
        return;
    m_Vec3 p;
    p.x= -1.0f;
    m_Vec3 c( 0.8f, 0.8f, 0.8f );
    p.y= 1.0f - float( DEFAULT_FONT_HEIGHT ) * 2.0f / float( viewport_y );
    p.x= AddTextUnicode( p.x, p.y, &c, 1.0f, console_string ).x;

    if( (last_frame_time / 300 ) & 1 )
    {
    quint16 space[]= { '_', 0 };
    AddTextUnicode( p.x, p.y, &c, 1.0f, space );
    }
}
void r_Renderer::DrawMap()
{
    if( ! show_map )
        return;
    m_Vec3 p( 0.0f, 0.0f, 0.0f );
    m_Vec3 color;
    float symbol_y;
    Shred* s;
    char c;
    unsigned char color_id;
    unsigned short str[3]= {0,0,0};
    int x, y;

    static m_Vec3 map_colors[]=
    {
        MAP_SHRED_COLOR_DEFAULT,
        MAP_SHRED_COLOR_WATER,
        MAP_SHRED_COLOR_PLAIN,
        MAP_SHRED_COLOR_FOREST,
        MAP_SHRED_COLOR_HILL,
        MAP_SHRED_COLOR_MOUNTAIN,
        MAP_SHRED_COLOR_DESERT,
        MAP_SHRED_COLOR_TEST,
        MAP_SHRED_COLOR_PYRAMID,
        MAP_PLAYER_COLOR
    };

    char player_arrow;
    if( cam_angle.z >	m_Math::FM_PI4 && cam_angle.z <= 3.0f * m_Math::FM_PI4 )
        player_arrow= '>';
    else if( cam_angle.z > 3.0f * m_Math::FM_PI4 && cam_angle.z <= 5.0f * m_Math::FM_PI4 )
        player_arrow= 'V';
    else if( cam_angle.z > 5.0f * m_Math::FM_PI4 && cam_angle.z <= 7.0f * m_Math::FM_PI4 )
        player_arrow= '<';
    else if( cam_angle.z <= m_Math::FM_2PI || cam_angle.z <= m_Math::FM_PI4 )
        player_arrow= '^';


    p.y= float( ( 0 + visibly_world_size[1] ) * DEFAULT_FONT_WIDTH*2 ) * 2.0f / float( viewport_y ) - 1.0f;
    for( y=0; y< visibly_world_size[1]; y++ )
    {
        p.x= - float( visibly_world_size[0] * DEFAULT_FONT_WIDTH ) * 2.0f / float( viewport_x );
        for( x= 0; x< visibly_world_size[0]; x++ )
        {
            c= world_map_to_draw[ y * visibly_world_size[0] + x ];
            color_id= world_map_colors_to_draw[ y * visibly_world_size[0] + x ];
            str[0]= c;
            if( c == '@' )
                str[1]= ushort( player_arrow );
            else
                str[1]= ' ';
            p= AddTextUnicode( p.x, p.y, &map_colors[ color_id ], 1.0f, str );
        }
        p.y-= float( DEFAULT_FONT_WIDTH*2 ) * 2.0f / float( viewport_y );
    }
}
void r_Renderer::DrawCursor()
{
    m_Vec3 text_color( 1.0f, 1.0f, 1.0f );
    unsigned short str[]= { 0xFE, '\n', 0x00 };
    float cursor_height= 2.0f * float( font.LetterHeight() )/ float( viewport_y );
    cursor_pos.z= cursor_height;
    AddTextUnicode( cursor_pos.x, cursor_pos.y - cursor_height,  &text_color, 1.0f, str );
}

void r_Renderer::DrawBuildCube()
{
    /*m_Vec3 screen_pos= ( build_position * view_matrix );
    float w= build_position.x * view_matrix[3]
     + build_position.y * view_matrix[7]
      + build_position.z * view_matrix[11]
       + view_matrix[15];
    screen_pos/=w;
    m_Vec3 color( 1.0f, 1.0f, 1.0f );
    if( screen_pos.z > -1.0f && screen_pos.z < 1.0f )
    	AddText( screen_pos.x, screen_pos.y, &color, screen_pos.z * w * 2.0f, "Build" );*/

    if( frame_count == 0 )
    {
        glLineWidth( 1.0f );
        glDisable( GL_LINE_SMOOTH );
    }
    cube_shader.Bind();
    cube_shader.Uniform( "cube_pos", build_position );
    cube_shader.Uniform( "proj_mat", view_matrix );
    cube_buffer.Show();

}
void r_Renderer::DrawInventory()
{
    //current menu width: player - 33 symbols, inventory - 27 stmbols
    if( player->UsingSelfType() != USAGE_TYPE_OPEN )
        return;

    bool is_one_empty_slot= false;
    bool is_cursor;

    unsigned int i, num;
    m_Vec3 p;
    m_Vec3 text_color( 0.8f, 0.8f, 0.8f );
    const m_Vec3 primary_text_color( 0.8f, 0.8f, 0.8f );
    int selected_inventoy_slot;


    QString str;
    Inventory* inv= player->GetP()->HasInventory();


    selected_inventoy_slot= int( ( (0.5f + 3.0f * float(DEFAULT_FONT_HEIGHT ) * 2.0f
                                    / float( viewport_y ) - cursor_pos.y ) * float(viewport_y) * 0.5f - 0.5f )
                                 / float(DEFAULT_FONT_HEIGHT) ) - 1;
    if( cursor_pos.x < 2.0f * float( DEFAULT_FONT_WIDTH ) / float(viewport_x) - 1.0f ||
            cursor_pos.x > 2.0f * ( PLAYER_INVENTORY_WIDTH + 1 ) * float( DEFAULT_FONT_WIDTH )/ float(viewport_x) - 1.0f )
        selected_inventoy_slot= -1;

    static const char* player_gibs_format_str[]=
    {
        "Head : %15-s %1d  %0*.*d\n",
        "Rhand: %15-s %1d  %0*.*d\n",
        "Lhand: %15-s %1d  %0*.*d\n",
        "Body : %15-s %1d  %0*.*d\n",
        "Legs : %15-s %1d  %0*.*d\n",
    };
    static const char* format_str= "       %15-s %1d  %0*.*d\n";
    static const char* top_format_str= "%23-snum weight\n";
    static const char* bottom_format_str= "Total:%20-s%0*.*d";

    static const char* right_format_str= " %15-s %1d  %0*.*d\n";
    static const char* right_top_format_str= "%17-snum weight\n";
    static const char* right_bottom_format_str= "Total:%14-s%0*.*d";

    //inventory name
    // p= AddText( 2.0f * 10.0f / float(viewport_x) - 1.0f , 0.5f + 84.0f / float( viewport_y ), &primary_text_color, 1, "Your Inventory:\n" );
    p= AddText( 2.0f * float( DEFAULT_FONT_WIDTH )/ float(viewport_x) - 1.0f ,
                0.5f + 3.0f * float(DEFAULT_FONT_HEIGHT ) * 2.0f / float( viewport_y ),
                &primary_text_color, 1, "Your Inventory:\n" );
    //inventory head
    p= AddText( p.x, p.y, &primary_text_color, 1, top_format_str, ""  );

    //inventory body - player gibs
    for( i=0; i< 5; i++ )
    {
        if( i == active_inventory_slot && active_inventory == 0  )
            text_color= m_Vec3( 1.0f, 0.1f, 0.1f );
        else if( i == selected_inventoy_slot )
            text_color= m_Vec3( 1.0f, 1.0f, 0.1f );
        else
            text_color= primary_text_color;

        p= AddText( p.x, p.y, &text_color, 1, player_gibs_format_str[i],
                    inv->InvFullName(i).toLocal8Bit().constData(),
                    inv->Number(i), 7, 1, inv->GetInvWeight(i) );

    }

    //inventory body
    for ( i= 5; i< inv->Size(); i++ )
    {

        if( i == active_inventory_slot && active_inventory == 0  )
            text_color= m_Vec3( 1.0f, 0.1f, 0.1f );
        else if( i == selected_inventoy_slot )
            text_color= m_Vec3( 1.0f, 1.0f, 0.1f );
        else
            text_color= primary_text_color;

        num= inv->Number( i );

        p= AddText( p.x, p.y, &text_color, 1, format_str,
                    inv->InvFullName(i).toLocal8Bit().constData(),
                    num, 7, 1, inv->GetInvWeight(i) );

    }
    //weight string
    p= AddText( p.x, p.y, &text_color, 1, bottom_format_str, "",  7, 1, inv->Weight() );


show_inventory_block:

    Block* inv_block;
    if( using_position.z >= 0 )
    inv= (inv_block= world->GetBlock( ushort( using_position.x ),
                         ushort( using_position.y ),
                         ushort( using_position.z ) )
                         )->HasInventory(); //player->UsingBlock()->HasInventory();HACK!!!
    else
        inv= NULL;
    if( inv == NULL )
        return;


    selected_inventoy_slot= int( ( (0.5f + 3.0f * float(DEFAULT_FONT_HEIGHT ) * 2.0f/
                                    float( viewport_y ) - cursor_pos.y ) * float(viewport_y) * 0.5f - 0.5f ) /
                                 float( DEFAULT_FONT_HEIGHT ) ) - 1;
    if( cursor_pos.x < 2.0f * float( DEFAULT_FONT_WIDTH ) * float( 2 + PLAYER_INVENTORY_WIDTH ) / float(viewport_x) - 1.0f ||
            cursor_pos.x > 2.0f * float( DEFAULT_FONT_WIDTH ) * float( 2 + PLAYER_INVENTORY_WIDTH + INVENTORY_WIDTH ) / float(viewport_x) - 1.0f )
        selected_inventoy_slot= -1;

    //inventory name
    p= AddText( ( 2 + PLAYER_INVENTORY_WIDTH ) * 2.0f * float( DEFAULT_FONT_WIDTH )/ float(viewport_x) - 1.0f,
                0.5f + 3.0f * float(DEFAULT_FONT_HEIGHT ) * 2.0f / float( viewport_y ),
                &primary_text_color, 1,"%s\n", inv_block->FullName().toLocal8Bit().constData() );
    //inventory head
    p= AddText( p.x, p.y, &primary_text_color, 1, right_top_format_str, "" );
    //inventory body
    for ( i= 0; i< inv->Size(); i++ )
    {

        if( i == active_inventory_slot && active_inventory == 1  )
            text_color= m_Vec3( 1.0f, 0.1f, 0.1f );
        else if( i == selected_inventoy_slot )
            text_color= m_Vec3( 1.0f, 1.0f, 0.1f );
        else
            text_color= primary_text_color;

        num= inv->Number(i);

        p= AddText( p.x, p.y, &text_color, 1, right_format_str,
                    inv->InvFullName(i).toLocal8Bit().constData(), num, 7, 1, inv->GetInvWeight(i) );

    }
    //weight string
    p= AddText( p.x, p.y, &primary_text_color, 1, right_bottom_format_str, "", 7, 1, inv->Weight() );
}

void r_Renderer::UpdateGPUData()
{
    if( world_map_updated  )
    {
        memcpy( world_map_to_draw, world_map, visibly_world_size[0] * visibly_world_size[1] );
        memcpy( world_map_colors_to_draw, world_map_colors, visibly_world_size[0] * visibly_world_size[1] );
        world_map_updated= false;

    }
    if( frame_count == 0 )
    {
        SetupVertexBuffers();

        model_manager.ClearModelList();

        for( unsigned int x= 0; x< visibly_world_size[0]; x++ )
            for( unsigned int y= 0; y< visibly_world_size[1]; y++ )
                model_manager.ReserveData( &shreds[ x + y * visibly_world_size[0] ].model_list );

        model_manager.AllocateMemory();

        for( unsigned int x= 0; x< visibly_world_size[0]; x++ )
            for( unsigned int y= 0; y< visibly_world_size[1]; y++ )
                model_manager.AddModels( &shreds[ x + y * visibly_world_size[0] ].model_list );

        model_manager.GenerateInstanceDataBuffer();
    }

#if  !FREG_GL_MULTI_THREAD
    if( vertex_buffer_updated )
    {
        host_data_mutex.lock();
        if( out_of_vertex_buffer )
        {
            world_buffer.VertexData( (float*)vertex_buffer, sizeof(r_WorldVertex) * vertex_buffer_size ,sizeof(r_WorldVertex) , 0 );
            if( out_of_index_buffer )
            {
                // world_buffer.IndexData( indeces, sizeof(quint32) * index_buffer_size, GL_UNSIGNED_INT, GL_TRIANGLES );
                //out_of_index_buffer= false;
            }
            out_of_vertex_buffer= false;
            full_update= false;

            memcpy( draw_shreds, shreds
                    , sizeof(r_ShredInfo) * visibly_world_size[0] * visibly_world_size[1] );
            need_full_update_shred_list= false;
        }
        else
        {
            r_ShredInfo* shred;
            for( unsigned int x= 0; x< visibly_world_size[0]; x++ )
            {
                for( unsigned int y= 0; y< visibly_world_size[1]; y++ )
                {
                    shred= &shreds[ x + y * visibly_world_size[0] ];
                    if( shred->rebuilded )
                    {
                        world_buffer.VertexSubData( (float*)shred->vertex_buffer,
                                                    shred->quad_count * 4 * sizeof( r_WorldVertex ),
                                                    (shred->vertex_buffer - vertex_buffer) * sizeof(r_WorldVertex) );

                        memcpy( &draw_shreds[ x + y * visibly_world_size[0] ], shred, sizeof( r_ShredInfo ) );
                        shred->rebuilded= false;
                        shreds_update_per_second++;
                    }
                }
            }
        }
        if( need_full_update_shred_list )
        {
            memcpy( draw_shreds, shreds,
                    sizeof(r_ShredInfo) * visibly_world_size[0] * visibly_world_size[1] );
            need_full_update_shred_list= false;
        }


        model_manager.ClearModelList();

        for( unsigned int x= 0; x< visibly_world_size[0]; x++ )
        {
            for( unsigned int y= 0; y< visibly_world_size[1]; y++ )
            {
                model_manager.ReserveData( &( shreds[ x + y * visibly_world_size[0] ].model_list ) );
            }
        }
        model_manager.AllocateMemory();

        for( unsigned int x= 0; x< visibly_world_size[0]; x++ )
        {
            for( unsigned int y= 0; y< visibly_world_size[1]; y++ )
            {
                model_manager.AddModels( &(shreds[ x + y * visibly_world_size[0] ].model_list ) );
            }
        }
        model_manager.GenerateInstanceDataBuffer();
        vertex_buffer_updated= false;
        host_data_mutex.unlock();


    }
#else
#endif//FREG_GL_MULTI_THREAD

}
void r_Renderer::Draw()
{

    if( frame_count == 0 )
        BuildWorld();

    gpu_data_mutex.lock();

    UpdateGPUData();

    frame_cam_position= cam_position;

#ifdef OGL21
    glClear( GL_DEPTH_BUFFER_BIT );
    if( underwater )
        glViewport( 0, 0, viewport_x/2, viewport_y/2 );
#endif

#ifndef OGL21
    glDisable( GL_CULL_FACE );
    RenderShadows();
    glEnable( GL_CULL_FACE );
#else
    RenderShadows();//hack for computing of light vector
#endif

    CalculateLightPower();
    BuildShredList();

#ifndef OGL21
    if( deferred_shading )
    {
        if( underwater )
        {
            underwater_g_buffer.Bind();
            underwater_g_buffer.ClearBuffer( true, false );
            //underwater_g_buffer.SetColorTextureFiltration( 0, GL_NEAREST, GL_NEAREST );
        }
        else
        {
            g_buffer.Bind();
            g_buffer.ClearBuffer( true, false );
        }
    }
    else
    {
        if( underwater )
        {
            scene_hdr_underwater_buffer.Bind();
            scene_hdr_underwater_buffer.ClearBuffer( true, false );
            scene_hdr_underwater_buffer.SetColorTextureFiltration( 0, GL_NEAREST, GL_NEAREST );
        }
        else
        {
            scene_hdr_buffer.Bind();
            scene_hdr_buffer.ClearBuffer( true, false );
        }
    }//if deferred_shading
#endif
    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    DrawWorld();
    DrawBuildCube();
    // glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    if( deferred_shading )
    {
        if( underwater )
        {
            scene_hdr_underwater_buffer.Bind();
            scene_hdr_underwater_buffer.SetColorTextureFiltration( 0, GL_NEAREST, GL_NEAREST );
        }
        else
        {
            scene_hdr_buffer.Bind();
        }

        MakeDeferredShading();

    }


    DrawSky();

#if OGL21
	if( !underwater )
        CopyDepthBuffer();
#else
    if( !underwater )
        CopyDepthBuffer();
#endif

    glEnable( GL_BLEND );
    DrawSun();

    if( underwater )
    {
        DrawRain();

        glDisable( GL_CULL_FACE );
        DrawWater();
        glEnable( GL_CULL_FACE );
    }
    else
    {
        glDisable( GL_CULL_FACE );
        DrawWater();
        glEnable( GL_CULL_FACE );

        DrawRain();
    }


#if OGL21
	if( underwater )
        CopyDepthBuffer();
#endif

    glDisable( GL_BLEND );

    glDisable( GL_DEPTH_TEST );

#ifdef OGL21
    MakePostprocessingGL21();
#else
    MakePostProcessing();
#endif
    DrawHUD();
    DrawInventory();
    DrawBlockMenu();
    DrawMap();
    DrawCursor();
    DrawConsole();
    glEnable( GL_BLEND );
    DrawText();
    glDisable( GL_BLEND );

    /*-------frame-end--------*/
    glFlush();
    /*------------------------*/

    glEnable( GL_DEPTH_TEST );
    gpu_data_mutex.unlock();
    CalculateFPS();
}

void r_Renderer::CalculateLightPower()
{
    float time= (float(world->Time() % SECONDS_IN_DAY) + float( world->MiniTime() ) * 0.1f )/ float(SECONDS_IN_HOUR);

    float k, k1;
    fire_ambient_light= AMBIENT_FIRE_LIGHT;
    if( time < 6.0f )//night
    {
        sky_ambient_light= AMBIENT_SKY_NIGHT_LIGHT;
        direct_sun_light= 0.0f;
        sky_light= 0.0f;
    }
    else if( time < 7.0f )
    {
        k= 7.0f - time;
        k1= 1.0f - k;
        direct_sun_light= 0.0f;
        sky_ambient_light= AMBIENT_SKY_DAY_LIGHT * k1 + k * AMBIENT_SKY_NIGHT_LIGHT;
        sky_light= k1;
    }
    else if( time < 23.0f )
    {
        sky_ambient_light= AMBIENT_SKY_DAY_LIGHT;
        direct_sun_light= DIRECT_SUN_LIGHT;
        sky_light= 1.0f;
    }
    else
    {
        k= time - 23.0f;
        k1= 1.0f - k;
        direct_sun_light= 0;
        sky_ambient_light= AMBIENT_SKY_DAY_LIGHT * k1 + k * AMBIENT_SKY_NIGHT_LIGHT;
        sky_light= k1;

    }


}

void r_Renderer::MakeDeferredShading()
{

    glDisable( GL_DEPTH_TEST );
    //scene_hdr_buffer.DeattachDepthTexture();

    //glDepthMask( GL_FALSE );

    if( underwater )
    {
        underwater_g_buffer.BindColorTexture( 0, 0 );
        underwater_g_buffer.BindColorTexture( 1, 1 );
        underwater_g_buffer.BindDepthTexture( 2 );
    }
    else
    {
        g_buffer.BindColorTexture( 0, 0 );
        g_buffer.BindColorTexture( 1, 1 );
        g_buffer.BindDepthTexture( 2 );
    }
    sun_shadow_map[ front_shadowmap ].BindDepthTexture( 3 );
    texture_manager.BindMaterialPropertyTexture( 4 );


    ds_base_shader.Bind();
    ds_base_shader.Uniform( "albedo_material_id", 0 );
    ds_base_shader.Uniform( "normal_light", 1 );
    ds_base_shader.Uniform( "depth_buffer", 2 );
    ds_base_shader.Uniform( "shadow_map", 3 );
    ds_base_shader.Uniform( "material_property", 4 );

    m_Vec3 depth_convert_k;
    depth_convert_k.x= - z_far * z_near / ( z_far - z_near );
    depth_convert_k.y= - z_far / ( z_far - z_near );
    ds_base_shader.Uniform( "depth_convert_k", depth_convert_k );

    m_Vec3 viewport_scale;
    viewport_scale.y= m_Math::Tan( fov * 0.5 );
    viewport_scale.x= viewport_scale.y * float( viewport_x ) / float( viewport_y );
    ds_base_shader.Uniform( "viewport_scale", viewport_scale );


    m_Vec3 viewspace_sun_vector= sun_vector * normal_matrix;
    ds_base_shader.Uniform( "sun_vector", viewspace_sun_vector );

    ds_base_shader.Uniform( "shadow_mat", view_space_shadow_matrix );
    ds_base_shader.Uniform( "normal_mat", normal_matrix );


    ds_base_shader.Uniform( "direct_sun_light", direct_sun_light );
    ds_base_shader.Uniform( "sky_ambient_light", sky_ambient_light );
    ds_base_shader.Uniform( "fire_ambient_light", fire_ambient_light );

    fullscreen_quad.Bind();
    fullscreen_quad.Show();

    //glDepthMask( GL_TRUE );
    //scene_hdr_buffer.ReattachDepthTexture();
    glEnable( GL_DEPTH_TEST );
}


void r_Renderer::MakePostprocessingGL21()
{

    if( underwater )
    {

        underwater_buffer_gl21.BindTexture(0);
        glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 0, 0, viewport_x/2, viewport_y/2, 0 );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );


        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, scene_depth_buffer );


        glViewport( 0, 0, viewport_x, viewport_y );

        underwater_postprocess_shader.Bind();
        underwater_postprocess_shader.Uniform( "scene_buffer", 0 );
        underwater_postprocess_shader.Uniform( "depth_buffer", 1 );
        underwater_postprocess_shader.Uniform( "time", float( last_frame_time )/ 1000.0f );

        m_Vec3 inv_screen_size( 1.0f/float(viewport_x), 1.0f/float(viewport_y), 0.0f );
        underwater_postprocess_shader.Uniform( "inv_screen_size", inv_screen_size );

        m_Vec3 depth_convert_k;
        depth_convert_k.x= - z_far * z_near / ( z_far - z_near );
        depth_convert_k.y= - z_far / ( z_far - z_near );
        underwater_postprocess_shader.Uniform( "depth_convert_k", depth_convert_k );

        m_Vec3 viewport_scale;
        viewport_scale.y= m_Math::Tan( fov * 0.5 );
        viewport_scale.x= viewport_scale.y * float( viewport_x ) / float( viewport_y );
        underwater_postprocess_shader.Uniform( "viewport_scale", viewport_scale );

         m_Vec3 underwater_fog_color= UNDERWATER_FOG_COLOR;
        underwater_fog_color *=
            ( float( player_lighting[0] ) * sky_ambient_light );
        underwater_postprocess_shader.Uniform( "underwater_fog_color", underwater_fog_color );

        fullscreen_quad.Show();
          r_Texture::ResetBinding();
    }
}
void r_Renderer::MakePostProcessing()
{
    //\EF\EE\EB\F3\F7\E5\ED\E8\E5 \EE\E1\F9\E5\E9 \FF\F0\EA\EE\F1\F2\E8 \F1\F6\E5\ED\FB
    brightness_info_buffer.Bind();
    brightness_shader.Bind();
    if( !underwater )
        scene_hdr_buffer.BindColorTexture( 0, 0 );
    else
        scene_hdr_underwater_buffer.BindColorTexture( 0, 0 );

    brightness_shader.Uniform( "scene_buffer", 0 );
    fullscreen_quad.Show();

    float l[2];
    brightness_info_buffer.BindColorTexture( 0, 0 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glGetTexImage( GL_TEXTURE_2D, 7, GL_RED, GL_FLOAT, &l[0] );

    //l[1]= l[0];
    l[0]/= 3.0f;
    l[0]= 0.69f / l[0];

    if( scene_brightness == 0.0f )
        scene_brightness= l[0];
    else
        scene_brightness= scene_brightness * 0.96f + 0.04f * l[0];

    if( scene_brightness > 6.0f )
        scene_brightness= 6.0f;
    else if( scene_brightness < 0.08f )
        scene_brightness= 0.08f;

    //scene_brightness= 1.0;
    //\EF\EE\EB\F3\F7\E5\ED\E8\E5 \EE\E1\F9\E5\E9 \FF\F0\EA\EE\F1\F2\E8 \F1\F6\E5\ED\FB

    m_Vec3 inv_screen_size( 2.0f/float(viewport_x), 2.0f/float(viewport_y), 0.0f );
    if( !underwater && bloom )//\EF\EE\E4 \E2\EE\E4\EE\E9 \E1\EB\F3\EC \ED\E8\ED\F3\E6\E5\ED
    {
        //\F1\EE\E7\E4\E0\ED\E8\E5 \F2\E5\EA\F1\F2\F3\F0\FB \E1\EB\F3\EC\E0 - \E2 2 \FD\F2\E0\EF\E0
        bloom_buffer[0].Bind();
        bloom_shader[0].Bind();
        scene_hdr_buffer.BindColorTexture( 0, 0 );
        bloom_shader[0].Uniform( "scene_buffer", 0 );
        bloom_shader[0].Uniform( "pass_edge", 1.0f );
        bloom_shader[0].Uniform( "inv_screen_size", inv_screen_size );
        fullscreen_quad.Show();

        bloom_buffer[1].Bind();
        bloom_shader[1].Bind();
        bloom_buffer[0].BindColorTexture( 0, 0 );
        bloom_shader[1].Uniform( "bloom_0_buffer", 0 );
        bloom_shader[1].Uniform( "inv_screen_size", inv_screen_size );
        fullscreen_quad.Show();
    }

    bloom_buffer[1].BindNull();
    glViewport( 0.0, 0.0, viewport_x, viewport_y );

    //\EA\EE\ED\E5\F7\ED\FB\E9 \FD\F2\E0\EF

    m_Vec3 depth_convert_k;
    depth_convert_k.x= - z_far * z_near / ( z_far - z_near );
    depth_convert_k.y= - z_far / ( z_far - z_near );

    if( underwater )
    {
        //bloom_buffer[1].BindColorTexture( 0, 1 );
        scene_hdr_underwater_buffer.SetColorTextureFiltration( 0, GL_LINEAR, GL_LINEAR );
        scene_hdr_underwater_buffer.BindColorTexture( 0, 0 );
        scene_hdr_underwater_buffer.BindDepthTexture(2);

        underwater_postprocess_shader.Bind();
        underwater_postprocess_shader.Uniform( "time", float( last_frame_time) / 1000.0f );
        underwater_postprocess_shader.Uniform( "scene_buffer", 0 );
        underwater_postprocess_shader.Uniform( "bloom_buffer", 1 );
        underwater_postprocess_shader.Uniform( "depth_buffer", 2 );
        underwater_postprocess_shader.Uniform ( "adapted_brightness", scene_brightness );
        underwater_postprocess_shader.Uniform( "depth_convert_k", depth_convert_k );

        m_Vec3 underwater_fog_color= UNDERWATER_FOG_COLOR;
        underwater_fog_color *=
            ( float( player_lighting[0] ) * sky_ambient_light );
        underwater_postprocess_shader.Uniform( "underwater_fog_color", underwater_fog_color );

        inv_screen_size= m_Vec3( 1.0f/float(viewport_x/2), 1.0f/float(viewport_y/2), 0.0f );
        underwater_postprocess_shader.Uniform( "inv_screen_size", inv_screen_size );

        m_Vec3 viewport_scale;
        viewport_scale.y= m_Math::Tan( fov * 0.5 );
        viewport_scale.x= viewport_scale.y * float( viewport_x ) / float( viewport_y );
        underwater_postprocess_shader.Uniform( "viewport_scale", viewport_scale );
    }
    else
    {
        bloom_buffer[1].BindColorTexture( 0, 1 );
        scene_hdr_buffer.BindColorTexture( 0, 0 );
        scene_hdr_buffer.BindDepthTexture(2);
        //glActiveTexture( GL_TEXTURE2 );
        //glBindTexture( GL_TEXTURE_2D, scene_depth_buffer );

        postprocess_shader.Bind();
        postprocess_shader.Uniform( "scene_buffer", 0 );
        postprocess_shader.Uniform( "bloom_buffer", 1 );
        postprocess_shader.Uniform( "depth_buffer", 2 );
        postprocess_shader.Uniform ( "adapted_brightness", scene_brightness );
        postprocess_shader.Uniform( "depth_convert_k", depth_convert_k );

        inv_screen_size= m_Vec3( 1.0f/float(viewport_x), 1.0f/float(viewport_y), 0.0f );
        postprocess_shader.Uniform( "inv_screen_size", inv_screen_size );
    }
    fullscreen_quad.Show();
}

void r_Renderer::RenderShadows()
{
    const float sun_max_horison_angle= m_Math::FM_TORAD * 60.0;
    m_Vec3 sun_pos;
    m_Mat4 translate, result_mat, projection, rotateX,rotateY, rotateZ,rotateZ2, viewport, yzchange, sun_vec_rot_X, sun_vec_rot_Z;

    //\F0\E0\F1\F7\B8\F2 \F3\E3\EB\E0 \F1\EE\EB\ED\F6\E0 \EE\F2\ED\EE\F1\E8\F2\E5\EB\FC\E3\EE \E2\F0\E5\EC\E5\ED\E8 \E4\ED\FF
    float angle= m_Math::FM_PI * float( ( world->Time() % SECONDS_IN_DAY / 60 ) * 60 - END_OF_NIGHT - 1 * SECONDS_IN_HOUR )/float ( SECONDS_IN_DAY - END_OF_NIGHT - 2 * SECONDS_IN_HOUR );
    if( angle < MIN_SUN_HEIGHT )  angle= 0.17;
    else if ( angle > ( m_Math::FM_PI - MIN_SUN_HEIGHT ) ) angle= 2.96;

    //\F0\E0\F1\F7\B8\F2 \EF\EE\E7\E8\F6\E8\E8 \F6\E5\ED\F2\F0\E0 \EA\E0\F0\F2\FB \F2\E5\ED\E5\E9
    Shred* player_shred= world->GetShred( player->X(), player->Y() );
    sun_pos.x= - float( player_shred->Latitude() ) * float( R_SHRED_WIDTH ) - float( R_SHRED_WIDTH/2 );
    sun_pos.y= - float( player_shred->Longitude() ) * float( R_SHRED_WIDTH ) - float( R_SHRED_WIDTH/2 );
    sun_pos.z= - float(R_SHRED_HEIGHT/2 + R_SHRED_HEIGHT/4);
    //sun_pos= -cam_position;

    //\F0\E0\F1\F7\B8\F2 \E2\E5\EA\F2\EE\F0\E0 \F1\EE\EB\ED\F6\E0
    sun_vector= m_Vec3( 0.0f, 1.0f, 0.0f );
    sun_vec_rot_Z.Identity(), sun_vec_rot_Z.RotateZ( angle - m_Math::FM_PI2 );
    sun_vec_rot_X.Identity(), sun_vec_rot_X.RotateX( sun_max_horison_angle );
    sun_vec_rot_Z= sun_vec_rot_Z * sun_vec_rot_X;
    sun_vector= sun_vector * sun_vec_rot_Z;
    /* if( ( world->Time() % SECONDS_IN_DAY ) < ( END_OF_NIGHT + 1 * SECONDS_IN_HOUR ) ||
    	( world->Time() % SECONDS_IN_DAY ) > ( SECONDS_IN_DAY - 1 * SECONDS_IN_HOUR ) )
         sun_vector= m_Vec3( 0.0, 0.0, 0.0 );*/

#ifdef OGL21
    return;
#endif

    //\F0\E0\F1\F7\B8\F2 \EC\E0\F2\F0\E8\F6\FB \E4\EB\FF \F0\E5\ED\E4\E5\F0\E8\ED\E3\E0 \F2\E5\ED\E5\E9
    translate.Identity();
    translate.Translate( sun_pos );
    rotateX.Identity();
    rotateX.RotateX( m_Math::FM_PI - angle );
    rotateZ.Identity();
    rotateZ.RotateZ( m_Math::FM_PI2 );
    rotateY.Identity();
    rotateY.RotateY( m_Math::FM_PI2 - sun_max_horison_angle );

    yzchange.Identity();
    yzchange[10]= yzchange[5]= 0.0;
    yzchange[6]= 1.0;
    yzchange[9]= -1.0;

    //\F0\E0\F1\F7\B8\F2 \EC\E0\F2\ED\F0\E8\F6\FB \EF\F0\EE\E5\EA\F6\E8\E8
    projection.Identity();
    float l= visibly_world_size[0] * visibly_world_size[1] * 2.0 *
             float( R_SHRED_WIDTH * R_SHRED_WIDTH ) + float( R_SHRED_HEIGHT/2 * R_SHRED_HEIGHT/2 );
    l= sqrt(l) + 16.0;//\E4\E8\E0\E3\E0\ED\E0\EB\FC \EF\E0\F0\E0\EB\EB\E5\EB\E5\EF\E8\EF\E5\E4\E0 \EC\E8\F0\E0
    projection.MakeProjection( 2.0 / l, 2.0 / l, -0.5 * l, 0.5 * l );

    result_mat= translate * rotateZ * rotateY * rotateX * yzchange * projection;

    if( (frame_count&15) == 0 )
        back_shadow_matrix= result_mat;
    //\E2\F1\E5 \F0\E5\ED\E4\E5\F0\E8\F2\F1\FF \EE\F2\ED\EE\F1\E8\F2\E5\EB\FC\ED\EE \E7\E0\F0\E0\ED\E5\E5 \EF\EE\E4\F1\F7\E8\F2\E0\ED\ED\EE\E9 \E7\E0 1 \F2\E0\EA \EE\E1\ED\EE\E2\EB\E5\ED\E8\FF( 16 \EA\E0\E4\F0\EE\E2 ) \EC\E0\F2\F0\E8\F6\FB

    r_ShredInfo* shred;
    shreds_to_draw_count= 0;
    for( unsigned int i= 0; i< visibly_world_size[0]; i++ )
    {
        for( unsigned int j=0; j< visibly_world_size[1]; j++ )
        {
            shred= &draw_shreds[ i + j * visibly_world_size[0] ];
            if( ( ( shred->longitude + shred->latitude ) &15 ) == ( frame_count&15 ) )
            {
                shreds_to_draw_quad_count[ shreds_to_draw_count ] = ( shred->quad_count - shred->water_quad_count * 0 )* 6;
                //shreds_to_draw_indeces[ shreds_to_draw_count ]= shred->index_buffer_offset * 6 * sizeof(quint32);
                shreds_to_draw_indeces[ shreds_to_draw_count ]= 0;
                shreds_to_draw_base_vertices[ shreds_to_draw_count ]= shred->vertex_buffer - vertex_buffer;
                shreds_to_draw_count++;
            }
        }
    }
    sun_shadow_map[ back_shadowmap ].Bind();
    if( ( frame_count & 15 ) == 0 )
        sun_shadow_map[ back_shadowmap ].ClearBuffer( true, true );
    shadow_shader.Bind();
    shadow_shader.Uniform( "proj_mat", back_shadow_matrix );

    world_buffer.Bind();
    /* glMultiDrawElements( GL_TRIANGLES, shreds_to_draw_quad_count,
                          GL_UNSIGNED_INT, (GLvoid* const*)shreds_to_draw_indeces,
                          shreds_to_draw_count );*/
    glMultiDrawElementsBaseVertex( GL_TRIANGLES, shreds_to_draw_quad_count,
                                   GL_UNSIGNED_SHORT,(GLvoid* const*)shreds_to_draw_indeces,
                                   shreds_to_draw_count, shreds_to_draw_base_vertices );

    if( (frame_count & 15 ) == 15 )
    {
        unsigned int tmp= front_shadowmap;
        front_shadowmap= back_shadowmap;
        back_shadowmap= tmp;

        shadow_matrix= back_shadow_matrix;
    }
    r_FrameBuffer::BindNull();


    glViewport( 0, 0, viewport_x, viewport_y );
}
void r_Renderer::BuildWorld()
{
    host_data_mutex.lock();
    unsigned int x, y;
    Shred* shred;
    r_ShredInfo* cur;

    int t1= clock(), t2, t3, t4, t5;
    /*\EF\EE\EB\F3\F7\E5\ED\E8\E5 \E8\ED\F4\EE\F0\EC\E0\F6\E8\E8 \EE \E2\E8\E4\E8\EC\EE\F1\F2\E8 \E1\EB\EE\EA\EE\E2*/
    vertices_in_buffer= 0;
    for( x= 0; x< visibly_world_size[0]; x++ )
    {
        for( y= 0; y< visibly_world_size[1]; y++ )
        {
            shred= (Shred*) world->GetShred( x * 16, y * 16 );
            cur= &shreds[ y * visibly_world_size[0] + x ];

            cur->visibly_information= visibly_information + 16 * 16 * 128 * ( y * visibly_world_size[0] + x );
            cur->latitude= shred->Latitude();//x;
            cur->longitude= shred->Longitude();//y;
            cur->shred = shred;

            if( x < ( visibly_world_size[0] - 1 ) )
                cur->east_shred= &shreds[ y * visibly_world_size[0] + x + 1 ];
            else
                cur->east_shred= NULL;

            if( y < ( visibly_world_size[1] - 1 ) )
                cur->south_shred= &shreds[ (y + 1)* visibly_world_size[0] + x ];
            else
                cur->south_shred= NULL;

            if( x >= 1 )
                cur->west_shred= &shreds[ y * visibly_world_size[0] + x - 1 ];
            else
                cur->west_shred= NULL;

            if( y >= 1 )
                cur->north_shred= &shreds[ (y - 1)* visibly_world_size[0] + x ];
            else
                cur->north_shred= NULL;

            cur->GetVisiblyInformation();
            cur->visibly_information_is_valid= true;
        }
    }
    t2= clock();
    /*\EF\EE\E4\F1\F7\B8\F2 \F7\E8\F1\EB\E0 \EA\E2\E0\E4\F0\E0\F2\EE\E2*/
    for( x= 0; x< visibly_world_size[0]; x++ )
    {
        for( y= 0; y< visibly_world_size[1]; y++ )
        {
            cur= &shreds[ y * visibly_world_size[0] + x ];

            // cur->model_list.Clear();
            cur->GetQuadCount();
            cur->updated= false;
            cur->rebuilded= false;
            cur->quads_updated= false;

            cur->quad_buffer_count= cur->quad_count + ( cur->quad_count>>2 );
            vertices_in_buffer+= cur->quad_count * 4;
            vertex_buffer_size+= cur->quad_buffer_count * 4;
        }
    }
    t3= clock();
    vertex_buffer= new  r_WorldVertex[ vertex_buffer_size ];
    r_WorldVertex* v= vertex_buffer;
    unsigned int i_offset= 0;

    // \EF\EE\F1\F2\F0\EE\E5\ED\E8\E5 \E3\E5\EE\EC\E5\F2\F0\E8\E8
    for( x= 0; x< visibly_world_size[0]; x++ )
    {
        for( y= 0; y< visibly_world_size[1]; y++ )
        {
            cur= &shreds[ y * visibly_world_size[0] + x ];

            cur->index_buffer_offset= i_offset;
            cur->vertex_buffer= v;

            //cur->model_list.AllocateMemory();
            cur->BuildShred( v );

            v+= cur->quad_buffer_count * 4;
            i_offset+= cur->quad_buffer_count;

        }
    }
    t4= clock();
    //\E7\E0\EF\EE\EB\ED\E5\ED\E8\E5 \E8\ED\E4\E5\EA\F1\ED\EE\E3\EE \E1\F3\F4\F4\E5\F0\E0
    index_buffer_size= R_MAX_INDECES_PER_SHRED;
    //index_buffer_size= 6 * vertex_buffer_size / 4;
    indeces= new quint16[ index_buffer_size ];

    for( x= 0, y=0; x< index_buffer_size; x+=6, y+=4 )
    {
        indeces[x] = y;
        indeces[x + 1] = y + 1;
        indeces[x + 2] = y + 2;

        indeces[x + 3] = y;
        indeces[x + 4] = y + 2;
        indeces[x + 5] = y + 3;
    }
    t5= clock();
    shred= world->GetShred( 16 * visibly_world_size[0]/2, 16 * visibly_world_size[1]/2 );
    center_shred_latitude= shred->Latitude();
    center_shred_longitude= shred->Longitude();

    memcpy( draw_shreds, shreds
            , sizeof(r_ShredInfo) * visibly_world_size[0] * visibly_world_size[1] );

    printf( "vis: %d\n", t2 - t1 );
    printf( "quads: %d\n", t3 - t2 );
    printf( "meshes: %d\n", t4 - t3 );
    printf( "indeces: %d\n", t5 - t4 );
    renderer_initialized= true;
    vertex_buffer_updated= false;

    UpdateWorldMap();
    host_data_mutex.unlock();
    world->Unlock();

}

void r_Renderer::UpdateData()
{
    host_data_mutex.lock();
    r_ShredInfo* shred;
    Shred* world_shred;
    unsigned int x, y;
    unsigned int new_quad_count= 0, new_index_buffer_size;
    //unsigned int* new_index_buffer;
    r_WorldVertex* new_vertex_buffer, *v;
    unsigned int i_offset;
    // unsigned char tmp[ sizeof( r_PolygonBuffer ) ];
    bool full_shred_list_update= false;
    if( full_update )
        goto out_point;


    for( x= 0; x< visibly_world_size[0]; x++ )
    {
        for( y= 0; y< visibly_world_size[1]; y++ )
        {
            shred= &shreds[ x + y * visibly_world_size[0] ];
            if( shred->updated )
            {
                //shred->model_list.Clear();
                shred->GetQuadCount();
                shred->quads_updated = false;
                if( shred->quad_buffer_count < shred->quad_count )
                {
                    out_of_vertex_buffer= true;
                    goto out_point;
                }
            }
        }
    }

out_point:
    /*\E5\F1\EB\E8 \E1\F3\F4\F4\E5\F0 \ED\E0\E4\EE \EF\E5\F0\E5\F1\EE\E7\E4\E0\F2\FC*/
    if( full_update || out_of_vertex_buffer )
    {
        printf( "out of vertex buffer\n" );
        if( full_update )
        {
            for( x= 0; x< visibly_world_size[0]; x++ )
            {
                for( y= 0; y< visibly_world_size[1]; y++ )
                {

                    world_shred= (Shred*) world->GetShred( x * 16, y * 16 );
                    shred= &shreds[ x + y * visibly_world_size[0] ];

                    shred->latitude= world_shred->Latitude();//x;
                    shred->longitude= world_shred->Longitude();//y;
                    shred->shred = world_shred;


                    if( x < ( visibly_world_size[0] - 1 ) )
                        shred->east_shred= &shreds[ y * visibly_world_size[0] + x + 1 ];
                    else
                        shred->east_shred= NULL;

                    if( y < ( visibly_world_size[1] - 1 ) )
                        shred->south_shred= &shreds[ (y + 1)* visibly_world_size[0] + x ];
                    else
                        shred->south_shred= NULL;

                    if( x >= 1 )
                        shred->west_shred= &shreds[ y * visibly_world_size[0] + x - 1 ];
                    else
                        shred->west_shred= NULL;

                    if( y >= 1 )
                        shred->north_shred= &shreds[ (y - 1)* visibly_world_size[0] + x ];
                    else
                        shred->north_shred= NULL;

                    shred->visibly_information= visibly_information + 16 * 16 * 128 * ( y * visibly_world_size[0] + x );
                    shred->GetVisiblyInformation();
                    shred->visibly_information_is_valid= true;
                }
            }
        }
        for( x= 0; x< visibly_world_size[0]; x++ )
        {
            for( y= 0; y< visibly_world_size[1]; y++ )
            {
                shred= &shreds[ x + y * visibly_world_size[0] ];
                //shred->model_list.Clear();
                shred->GetQuadCount();
                shred->quad_buffer_count= shred->quad_count + (shred->quad_count>>2);
                new_quad_count+= shred->quad_buffer_count;
            }
        }

        new_index_buffer_size= new_quad_count * 6;
        new_vertex_buffer= new r_WorldVertex[ new_quad_count * 4 ];
        v= new_vertex_buffer;
        i_offset= 0;
        for( x= 0; x< visibly_world_size[0]; x++ )
        {
            for( y= 0; y< visibly_world_size[1]; y++ )
            {
                shred= &shreds[ x + y * visibly_world_size[0] ];

                //shred->model_list.AllocateMemory();
                shred->BuildShred( v );

                shred->vertex_buffer= v;
                shred->index_buffer_offset= i_offset;
                v+= shred->quad_buffer_count * 4;
                i_offset+= shred->quad_buffer_count;
            }
        }

        /* if( new_index_buffer_size > index_buffer_size )
         {
             delete[] indeces;
             index_buffer_size= new_index_buffer_size;
             indeces= new quint32[ index_buffer_size ];
             out_of_index_buffer= true;
             for( x= 0, y=0; x< index_buffer_size; x+=6, y+=4 )
             {
                 indeces[x] = y;
                 indeces[x + 1] = y + 1;
                 indeces[x + 2] = y + 2;

                 indeces[x + 3] = y;
                 indeces[x + 4] = y + 2;
                 indeces[x + 5] = y + 3;
             }
         }*/
        delete[] vertex_buffer;
        vertex_buffer= new_vertex_buffer;
        vertex_buffer_size= new_quad_count * 4;

        vertex_buffer_updated= true;
        full_update= true;
        out_of_vertex_buffer= true;
    }

    else// \E5\F1\EB\E8 \F2\EE\EB\FC\EA\EE \ED\E0\E4\EE \EE\E1\ED\EE\E2\E8\F2\FC \E1\F3\F4\F4\E5\F0
    {
        for( x= 0; x< visibly_world_size[0]; x++ )
        {
            for( y= 0; y< visibly_world_size[1]; y++ )
            {
                shred= &shreds[ x + y * visibly_world_size[0] ];
                if( shred->updated )
                {
                    bool need_rebuild = shred->immediately_update;
                    if( shred->immediately_update )
                        full_shred_list_update= true;
                    shred->immediately_update= false;

                    unsigned short shred_distance= shred->GetShredDistance( center_shred_latitude, center_shred_longitude );
                    if( shred_distance < r_Config.double_update_interval_radius )
                        need_rebuild= true;

                    else if( shred_distance < r_Config.quad_update_intrval_radius )
                    {
                        if( ( ( shred->latitude + shred->latitude + update_count ) & 1 ) == 0 )
                            need_rebuild= true;
                    }
                    else if( shred_distance < r_Config.octal_update_interval_radius )
                    {
                        if( ( ( shred->latitude + shred->latitude + update_count ) & 3 ) == 0 )
                            need_rebuild= true;
                    }
                    else
                    {
                        if( ( ( shred->latitude + shred->latitude + update_count ) & 7 ) == 0 )
                            need_rebuild= true;
                    }
                    // need_rebuild= true;
                    if( need_rebuild )
                    {
                        //shred->model_list.AllocateMemory();
                        shred->BuildShred( shred->vertex_buffer );
                        shred->rebuilded= true;
                        shred->updated= false;
                        vertex_buffer_updated= true;
                    }

                }
            }
        }
    }
    if( full_shred_list_update )
        need_full_update_shred_list= true;
    host_data_mutex.unlock();
}



m_Vec3 r_Renderer::AddText( float x, float y, const m_Vec3* color, float size,  const char* text, ... )
{
    if( letter_count >= R_LETTER_BUFFER_LEN )
        return m_Vec3( x, y, 0.0 );

    static char str[ 2048 ];
    va_list ap;
    va_start( ap, text );
    vsprintf( str, text, ap );
    va_end( ap );//\E4\EE\EF\E8\F1\FB\E2\E0\E5\EC \E2 \F1\F2\F0\EE\EA\F3 \EF\EE\F1\EB\E5\E4\ED\E8\E5 \ED\E5\E8\E7\E0\E2\E5\F1\F2\ED\FB\E5 \EF\E0\F0\E0\EC\E5\F2\F0\FB

    unsigned int len= strlen( str );

    if( len + letter_count >= R_LETTER_BUFFER_LEN )
        len-= ( R_LETTER_BUFFER_LEN - letter_count -1 );
    unsigned int non_type_symbols= 0;
    unsigned int i, j;
    unsigned short l;
    float pos_x= x, pos_y= y, h, dx;

    unsigned char c_color[3];
    unsigned short tex_id;
    c_color[0]= (unsigned char)(color->x * 255.0f);
    c_color[1]= (unsigned char)(color->y * 255.0f);
    c_color[2]= (unsigned char)(color->z * 255.0f);

    h= size * float( font.LetterHeight() ) /  float( viewport_y>>1 );//\E2\FB\F1\EE\F2\E0 \E1\F3\EA\E2\FB \E2 \FD\EA\F0\E0\ED\ED\FB\F5 \EA\EE\EE\F0\E4\E8\ED\E0\F2\E0\F5
    dx= h * float(viewport_y) / float(viewport_x);//\FD\F2\EE\F2\F2 \E6\E5 \F0\E0\E7\EC\E5\F0, \ED\EE \EF\EE \EE\F1\E8 y
    for( i= 0, j= letter_count * 4; i< len; i++, j+=4 )
    {
        l= (unsigned short) str[i];
        if( l == '\n' )
        {
            pos_x= x;
            non_type_symbols++;
            pos_y-= h;
            j-=4;
            continue;
        }
        tex_id= font.TextureNum( l );

        font_vertices[j].coord[0]= pos_x;
        font_vertices[j].coord[1]= pos_y;
        font_vertices[j].tex_coord[0]= font.TexCoordLeft( l );
        font_vertices[j].tex_coord[1]= font.TexCoordBottom( l );

        font_vertices[j+1].coord[0]= pos_x;
        font_vertices[j+1].coord[1]= pos_y + h;
        font_vertices[j+1].tex_coord[0]= font.TexCoordLeft( l );
        font_vertices[j+1].tex_coord[1]= font.TexCoordTop( l );

        font_vertices[j+2].coord[0]= pos_x + font.LetterWidth( l ) * dx;
        font_vertices[j+2].coord[1]= pos_y + h;
        font_vertices[j+2].tex_coord[0]= font.TexCoordRight( l );
        font_vertices[j+2].tex_coord[1]= font.TexCoordTop( l );

        font_vertices[j+3].coord[0]= pos_x + font.LetterWidth( l ) * dx;
        font_vertices[j+3].coord[1]= pos_y;
        font_vertices[j+3].tex_coord[0]= font.TexCoordRight( l );
        font_vertices[j+3].tex_coord[1]= font.TexCoordBottom( l );

        font_vertices[j].tex_id=
            font_vertices[j+1].tex_id=
                font_vertices[j+2].tex_id=
                    font_vertices[j+3].tex_id= tex_id;

        font_vertices[j].color[0]=
            font_vertices[j+1].color[0]=
                font_vertices[j+2].color[0]=
                    font_vertices[j+3].color[0]= c_color[0];

        font_vertices[j].color[1]=
            font_vertices[j+1].color[1]=
                font_vertices[j+2].color[1]=
                    font_vertices[j+3].color[1]= c_color[1];

        font_vertices[j].color[2]=
            font_vertices[j+1].color[2]=
                font_vertices[j+2].color[2]=
                    font_vertices[j+3].color[2]= c_color[2];

        pos_x+= font.LetterWidth( l ) * dx;

    }
    letter_count+= len - non_type_symbols;

    return m_Vec3( pos_x, pos_y, 0.0 );
}


m_Vec3 r_Renderer::AddTextUnicode( float x, float y, const m_Vec3* color, float size,  const quint16* text )
{
    if( letter_count >= R_LETTER_BUFFER_LEN )
        return m_Vec3( x, y, 0.0 );

    unsigned int len= 0;
    while( text[ len ] != 0 )
        len++;

    if( len + letter_count >= R_LETTER_BUFFER_LEN )
        len-= ( R_LETTER_BUFFER_LEN - letter_count - 1 );

    unsigned int non_type_symbols= 0;
    unsigned int i, j;
    unsigned short l;
    float pos_x= x, pos_y= y, h, dx;

    unsigned char c_color[3];
    unsigned short tex_id;
    c_color[0]= (unsigned char)(color->x * 255.0f);
    c_color[1]= (unsigned char)(color->y * 255.0f);
    c_color[2]= (unsigned char)(color->z * 255.0f);

    h= size * float( font.LetterHeight() ) /  float( viewport_y>>1 );//\E2\FB\F1\EE\F2\E0 \E1\F3\EA\E2\FB \E2 \FD\EA\F0\E0\ED\ED\FB\F5 \EA\EE\EE\F0\E4\E8\ED\E0\F2\E0\F5
    dx= h * float(viewport_y) / float(viewport_x);//\FD\F2\EE\F2\F2 \E6\E5 \F0\E0\E7\EC\E5\F0, \ED\EE \EF\EE \EE\F1\E8 y
    for( i= 0, j= letter_count * 4; i< len; i++, j+=4 )
    {
        l= (unsigned short) text[i];
        if( l == '\n' )
        {
            pos_x= x;
            non_type_symbols++;
            pos_y-= h;
            j-=4;
            continue;
        }
        tex_id= font.TextureNum( l );

        font_vertices[j].coord[0]= pos_x;
        font_vertices[j].coord[1]= pos_y;
        font_vertices[j].tex_coord[0]= font.TexCoordLeft( l );
        font_vertices[j].tex_coord[1]= font.TexCoordBottom( l );

        font_vertices[j+1].coord[0]= pos_x;
        font_vertices[j+1].coord[1]= pos_y + h;
        font_vertices[j+1].tex_coord[0]= font.TexCoordLeft( l );
        font_vertices[j+1].tex_coord[1]= font.TexCoordTop( l );

        font_vertices[j+2].coord[0]= pos_x + font.LetterWidth( l ) * dx;
        font_vertices[j+2].coord[1]= pos_y + h;
        font_vertices[j+2].tex_coord[0]= font.TexCoordRight( l );
        font_vertices[j+2].tex_coord[1]= font.TexCoordTop( l );

        font_vertices[j+3].coord[0]= pos_x + font.LetterWidth( l ) * dx;
        font_vertices[j+3].coord[1]= pos_y;
        font_vertices[j+3].tex_coord[0]= font.TexCoordRight( l );
        font_vertices[j+3].tex_coord[1]= font.TexCoordBottom( l );

        font_vertices[j].tex_id=
            font_vertices[j+1].tex_id=
                font_vertices[j+2].tex_id=
                    font_vertices[j+3].tex_id= tex_id;

        font_vertices[j].color[0]=
            font_vertices[j+1].color[0]=
                font_vertices[j+2].color[0]=
                    font_vertices[j+3].color[0]= c_color[0];

        font_vertices[j].color[1]=
            font_vertices[j+1].color[1]=
                font_vertices[j+2].color[1]=
                    font_vertices[j+3].color[1]= c_color[1];

        font_vertices[j].color[2]=
            font_vertices[j+1].color[2]=
                font_vertices[j+2].color[2]=
                    font_vertices[j+3].color[2]= c_color[2];

        pos_x+= font.LetterWidth( l ) * dx;

    }
    letter_count+= len - non_type_symbols;

    return m_Vec3( pos_x, pos_y, 0.0 );
}


void r_Renderer::DrawText()
{
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    text_buffer.VertexSubData( (float*) font_vertices,
                               letter_count * 4 * sizeof( r_FontVertex ), 0 );


    font.FontTexture( R_FONT_PAGE_ASCII )->BindTexture();
    text_shader.Bind();
    text_shader.Uniform( "tex", 0 );
    text_buffer.Bind();
    glDrawElements( GL_TRIANGLES, letter_count * 6, GL_UNSIGNED_SHORT, 0 );
    letter_count= 0;

}

void r_Renderer::StartUpText()
{
#ifdef OGL21
    if( text_shader.Load( "shaders/glsl_120/text_frag.glsl", "shaders/glsl_120/text_vert.glsl" ) )
        printf( "error, text shader not found\n" );
#else
    if( text_shader.Load( "shaders/text_frag.glsl", "shaders/text_vert.glsl" ) )
        printf( "error, text shader not found\n" );
#endif
    text_shader.SetAttribLocation( "coord", ATTRIB_POSITION );
    text_shader.SetAttribLocation( "tex_coord", ATTRIB_TEX_COORD );
    text_shader.SetAttribLocation( "color", 2 );
    text_shader.SetAttribLocation( "tex_id", 3 );
    text_shader.MoveOnGPU();

    //text_shader.FindUniform( "tex" );
    //text_shader.FindAttrib( "coord" );
    //text_shader.FindAttrib( "tex_coord" );
    //text_shader.FindAttrib( "color" );


    font.LoadFontPage( R_FONT_PAGE_ASCII, "fonts/Courier_New_12.txt", "fonts/cn.bmp" );

#ifdef OGL21
    m_Vec3 inv_tex_size( 1.0f/ float( font.FontTexture( R_FONT_PAGE_ASCII )->GetWidth() ),
                         1.0f / float( font.FontTexture( R_FONT_PAGE_ASCII )->GetHeight() ), 0.0f );
    text_shader.Uniform( "inv_tex_size", inv_tex_size );
    //no textureSize glsl function
#endif

    font_vertices= new r_FontVertex[ R_LETTER_BUFFER_LEN * 4 ];
    font_indeces= new quint16[ R_LETTER_BUFFER_LEN * 6 ];


    for( unsigned short i=0, j=0; j< R_LETTER_BUFFER_LEN * 6; i+=4, j+=6 )
    {
        font_indeces[j  ]= i    ;
        font_indeces[j+1]= i + 1;
        font_indeces[j+2]= i + 2;
        font_indeces[j+3]= i    ;
        font_indeces[j+4]= i + 2;
        font_indeces[j+5]= i + 3;
    }
    letter_count= 0;
}
void r_Renderer::UpdateTick()
{
    int t0= -QTime::currentTime().msecsTo( startup_time ), dt;
    if( renderer_initialized )
    {
        world->ReadLock();

        short player_coord[3];
        player_coord[0]= player->X();
        player_coord[1]= player->Y();
        player_coord[2]= player->Z();

        player_lighting[0]= world->LightMap( player_coord[0], player_coord[1], player_coord[2] );
        player_lighting[1]= player_lighting[0] >>4;
        player_lighting[0]&= 15;

        UpdateWorldMap();
        UpdateData();
        world->Unlock();
        update_count++;
    }
    dt= -QTime::currentTime().msecsTo( startup_time ) - t0;

    if( dt > r_Config.update_interval )
        return;

    usleep( ( r_Config.update_interval - dt ) * 1000 );
}

void r_Renderer::LoadTextures()
{
    texture_manager.SetTextureFilter( filter_world_texture );
    texture_manager.LoadTextures();

    sun_texture.Load( "textures/new_sun.tga" );

    sky_cubemap.SetFiltration( GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR );
    sky_cubemap.Load( "textures/sky/0.bmp" );
    sky_cubemap.DeleteFromRAM();

    //rInitTextureTable();


    glGenTextures( 1, &water_texture );
    glBindTexture( GL_TEXTURE_3D, water_texture );
    glTexImage3D( GL_TEXTURE_3D, 0, GL_RGB8, 256,256, 16, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );


    r_TextureFile tf;
    char tex_name_0[]= "textures/water/WaterH%d%d.bmp";
    char tex_name[256];
    unsigned char* tmp_tex_data= new unsigned char[256 * 256 * 3 ];
    for( int i=0; i<16; i++ )
    {
        sprintf( tex_name, tex_name_0, i/10, i%10 );
        rLoadTextureBMP( &tf, tex_name );
        rRGB_RChanenl2NormalMap( tf.data, tmp_tex_data, 256, 256, 1.0f );
        delete[] tf.data;
        glTexSubImage3D( GL_TEXTURE_3D,0, 0,0,i, 256,256,1, GL_RGB, GL_UNSIGNED_BYTE, tmp_tex_data );
    }
    delete[] tmp_tex_data;
    glGenerateMipmap( GL_TEXTURE_3D );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
}


void r_Renderer::LoadShaders()
{
#ifdef OGL21
    if( world_shader.Load( "shaders/glsl_120/world_frag.glsl",
                           "shaders/glsl_120/world_vert.glsl", NULL ) )
        printf( "error, world shader not found\n" );
#else
    if( deferred_shading )
    {
        if( world_shader.Load( "shaders/DS/world_frag.glsl",
                               "shaders/DS/world_vert.glsl", NULL ) )
            printf( "error, world shader not found\n" );
    }
    else
    {
        if( world_shader.Load( "shaders/world_frag.glsl",
                               "shaders/world_vert.glsl", NULL ) )
            printf( "error, world shader not found\n" );
    }
#endif

    world_shader.SetAttribLocation( "coord", ATTRIB_POSITION );
    world_shader.SetAttribLocation( "tex_coord", ATTRIB_TEX_COORD );
    world_shader.SetAttribLocation( "normal", ATTRIB_NORMAL );
    world_shader.SetAttribLocation( "light", /*ATTRIB_USER0*/3 );

    world_shader.MoveOnGPU();

    if( deferred_shading )
    {
        world_shader.FindUniform( "tex" );
        world_shader.FindUniform( "proj_mat" );
        world_shader.FindUniform( "normal_mat" );

    }
    else
    {
        world_shader.FindUniform( "tex" );
        world_shader.FindUniform( "shadow_map" );
        world_shader.FindUniform( "shadow_mat" );
        world_shader.FindUniform( "proj_mat" );
        world_shader.FindUniform( "normal_mat" );
        world_shader.FindUniform( "cam_pos" );
        world_shader.FindUniform( "fog_color" );
        world_shader.FindUniform( "max_view2" );
        world_shader.FindUniform( "sun_vector" );
        world_shader.FindUniform( "material_property" );

        world_shader.FindUniform( "direct_sun_light" );
        world_shader.FindUniform( "sky_ambient_light" );
        world_shader.FindUniform( "fire_ambient_light" );
    }


#ifdef OGL21
    if( water_shader.Load( "shaders/glsl_120/water_frag.glsl",
                           "shaders/glsl_120/water_vert.glsl", NULL ) )
        printf( "error, water shader not found\n" );
#else
    if( water_shader.Load( "shaders/water_frag.glsl",
                           "shaders/water_vert.glsl", NULL ) )
        printf( "error, water shader not found\n" );
#endif
    water_shader.SetAttribLocation( "coord", ATTRIB_POSITION );
    water_shader.SetAttribLocation( "tex_coord", ATTRIB_TEX_COORD );
    water_shader.SetAttribLocation( "normal", ATTRIB_NORMAL );
    water_shader.SetAttribLocation( "light", /*ATTRIB_USER0*/3 );

    water_shader.MoveOnGPU();

    water_shader.FindUniform( "depth_buffer" );
    water_shader.FindUniform( "depth_convert_k" );
    water_shader.FindUniform( "inv_max_view_distance" );
    water_shader.FindUniform( "inv_screen_size" );
    water_shader.FindUniform( "viewport_scale" );

    //water_shader.FindUniform( "tex" );
    water_shader.FindUniform( "shadow_map" );
    water_shader.FindUniform( "normal_map" );
    water_shader.FindUniform( "shadow_mat" );
    water_shader.FindUniform( "proj_mat" );
    water_shader.FindUniform( "normal_mat" );
    water_shader.FindUniform( "cam_pos" );
    water_shader.FindUniform( "fog_color" );
    water_shader.FindUniform( "max_view2" );
    water_shader.FindUniform( "sun_vector" );
    //water_shader.FindUniform( "material_property" );
    water_shader.FindUniform( "time" );

    water_shader.FindUniform( "direct_sun_light" );
    water_shader.FindUniform( "sky_ambient_light" );
    water_shader.FindUniform( "fire_ambient_light" );


#ifdef OGL21
    if( cube_shader.Load( "shaders/glsl_120/cube_frag.glsl", "shaders/glsl_120/cube_vert.glsl", NULL ) )
        printf( "error, cube shader not found\n" );
#else
    if( deferred_shading )
    {
        if( cube_shader.Load( "shaders/DS/cube_frag.glsl", "shaders/DS/cube_vert.glsl", NULL ) )
            printf( "error, cube shader not found\n" );
    }
    else
    {
        if( cube_shader.Load( "shaders/cube_frag.glsl", "shaders/cube_vert.glsl", NULL ) )
            printf( "error, cube shader not found\n" );
    }
#endif

    cube_shader.SetAttribLocation( "pos", 0 );
    cube_shader.MoveOnGPU();
    cube_shader.FindUniform( "cube_pos" );
    cube_shader.FindUniform( "proj_mat" );

#ifdef OGL21
    if( sky_shader.Load( "shaders/glsl_120/sky_frag.glsl", "shaders/glsl_120/sky_vert.glsl", NULL ) )
        printf( "error, sky shader not found\n" );
#else
    if( sky_shader.Load( "shaders/sky_frag.glsl", "shaders/sky_vert.glsl", NULL ) )
        printf( "error, sky shader not found\n" );
#endif

    sky_shader.SetAttribLocation( "coord", 0 );
    sky_shader.MoveOnGPU();
    sky_shader.FindUniform( "cu" );
    sky_shader.FindUniform( "cam_pos" );
    sky_shader.FindUniform( "proj_mat" );
    sky_shader.FindUniform( "sun_vector" );

    //sky_shader.FindAttrib( "coord" );


#ifndef OGL21
    if( shadow_shader.Load( NULL, "shaders/shadow_vert.glsl", NULL ) )
        printf( "error, shadow shader not found\n" );

    shadow_shader.SetAttribLocation( "coord", 0 );
    shadow_shader.MoveOnGPU();
    shadow_shader.FindUniform( "proj_mat" );

    //shadow_shader.FindAttrib( "coord" );
#endif


#ifdef OGL21
    if( sun_shader.Load( "shaders/glsl_120/sun_frag.glsl", "shaders/glsl_120/sun_vert.glsl", NULL ) )
        printf( "error, sun shader not found\n" );
#else
    if( sun_shader.Load( "shaders/sun_frag.glsl", "shaders/sun_vert.glsl", NULL ) )
        printf( "error, sun shader not found\n" );
#endif

    sun_shader.SetAttribLocation( "coord", 0 );
    sun_shader.MoveOnGPU();

    sun_shader.FindUniform( "sun_tex" );
    sun_shader.FindUniform( "sun_pos" );
    sun_shader.FindUniform( "proj_mat" );
    sun_shader.FindUniform( "aspect" );

#ifndef OGL21
    if( postprocess_shader.Load( "shaders/postprocess_frag.glsl", "shaders/fullscreen_quad_vert.glsl", NULL ) )
        printf( "error, postprocess shader not found\n" );

    if( bloom )
        postprocess_shader.Define( "#define BLOOM\n" );
    if( antialiasing )
        postprocess_shader.Define( "#define ANTIALIASING\n" );
    if( edge_detect )
        postprocess_shader.Define( "#define EDGE_DETECT\n" );

    postprocess_shader.SetAttribLocation( "coord", 0 );
    postprocess_shader.MoveOnGPU();
    //postprocess_shader.FindAttrib( "coord" );
    postprocess_shader.FindUniform( "depth_buffer" );
    postprocess_shader.FindUniform( "scene_buffer" );
    postprocess_shader.FindUniform( "adapted_brightness" );
    postprocess_shader.FindUniform( "bloom_buffer" );
    postprocess_shader.FindUniform( "inv_screen_size" );
    postprocess_shader.FindUniform( "depth_convert_k" );

#endif



#ifdef OGL21
    if( underwater_postprocess_shader.Load( "shaders/glsl_120/underwater_postprocess_frag.glsl", "shaders/glsl_120/fullscreen_quad_vert.glsl", NULL ) )
        printf( "error, underwater postprocess shader not found\n" );

    underwater_postprocess_shader.SetAttribLocation( "coord", 0 );
    underwater_postprocess_shader.MoveOnGPU();
    underwater_postprocess_shader.FindUniform( "scene_buffer" );
    underwater_postprocess_shader.FindUniform( "depth_buffer" );
    underwater_postprocess_shader.FindUniform( "inv_screen_size" );
    underwater_postprocess_shader.FindUniform( "time" );
    underwater_postprocess_shader.FindUniform( "viewport_scale" );
    underwater_postprocess_shader.FindUniform( "inv_max_view_distance" );
    underwater_postprocess_shader.FindUniform( "depth_convert_k" );
    underwater_postprocess_shader.FindUniform( "viewport_scale" );
    underwater_postprocess_shader.FindUniform( "underwater_fog_color" );

#else
    if( underwater_postprocess_shader.Load( "shaders/postprocess_frag.glsl", "shaders/fullscreen_quad_vert.glsl", NULL ) )
        printf( "error, underwater postprocess shader not found\n" );

    underwater_postprocess_shader.Define( "#define UNDERWATER\n" );

    underwater_postprocess_shader.SetAttribLocation( "coord", 0 );
    underwater_postprocess_shader.MoveOnGPU();
    //underwater_postprocess_shader.FindAttrib( "coord" );
    underwater_postprocess_shader.FindUniform( "depth_buffer" );
    underwater_postprocess_shader.FindUniform( "scene_buffer" );
    underwater_postprocess_shader.FindUniform( "adapted_brightness" );
    underwater_postprocess_shader.FindUniform( "bloom_buffer" );
    underwater_postprocess_shader.FindUniform( "inv_screen_size" );
    underwater_postprocess_shader.FindUniform( "time" );
    underwater_postprocess_shader.FindUniform( "depth_convert_k" );
    underwater_postprocess_shader.FindUniform( "viewport_scale" );
    underwater_postprocess_shader.FindUniform( "underwater_fog_color" );
#endif

#ifndef OGL21
    if( brightness_shader.Load( "shaders/scene_brightness_frag.glsl", "shaders/fullscreen_quad_vert.glsl", NULL ) )
        printf( "error, brightness shader not found\n" );

    brightness_shader.SetAttribLocation( "coord", 0 );
    brightness_shader.MoveOnGPU();
    brightness_shader.FindUniform( "scene_buffer" );
#endif

#ifndef OGL21
    if( bloom_shader[0].Load( "shaders/bloom_0_frag.glsl", "shaders/fullscreen_quad_vert.glsl", NULL ) )
        printf( "error, bloom shader 0 not found\n" );

    bloom_shader[0].SetAttribLocation( "coord", 0 );
    bloom_shader[0].MoveOnGPU();
    bloom_shader[0].SetAttribLocation( "coord", 0 );
    bloom_shader[0].FindUniform( "scene_buffer" );
    bloom_shader[0].FindUniform( "inv_screen_size" );
    bloom_shader[0].FindUniform( "pass_edge" );
#endif

#ifndef OGL21
    if( bloom_shader[1].Load( "shaders/bloom_1_frag.glsl", "shaders/fullscreen_quad_vert.glsl", NULL ) )
        printf( "error, bloom shader 1 not found\n" );

    bloom_shader[1].SetAttribLocation( "coord", 0 );
    bloom_shader[1].MoveOnGPU();
    bloom_shader[1].FindUniform( "bloom_0_buffer" );
    bloom_shader[1].FindUniform( "inv_screen_size" );
#endif


#ifndef OGL21
    if(deferred_shading )
    {
        if( ds_base_shader.Load( "shaders/DS/ds_main_frag.glsl", "shaders/DS/ds_main_vert.glsl", NULL ) )
            printf( "error, base deferred shader not found\n" );

        ds_base_shader.SetAttribLocation( "pos", 0 );
        ds_base_shader.MoveOnGPU();

        ds_base_shader.FindUniform( "albedo_material_id" );
        ds_base_shader.FindUniform( "normal_light" );
        ds_base_shader.FindUniform( "depth_buffer" );
        ds_base_shader.FindUniform( "viewport_scale" );
        ds_base_shader.FindUniform( "depth_convert_k" );
        ds_base_shader.FindUniform( "sun_vector" );
        ds_base_shader.FindUniform( "shadow_mat" );
        ds_base_shader.FindUniform( "shadow_map" );
        ds_base_shader.FindUniform( "normal_mat" );
        ds_base_shader.FindUniform( "material_property" );

        ds_base_shader.FindUniform( "sky_ambient_light" );
        ds_base_shader.FindUniform( "fire_ambient_light" );
        ds_base_shader.FindUniform( "direct_sun_light" );

    }
#endif




#ifdef OGL21
    if( models_shader.Load( "shaders/glsl_120/world_frag.glsl", "shaders/glsl_120/model_vert.glsl", NULL ) )
        printf( "error. Model shader not found.\n" );

    models_shader.Define( "#define NORMAL_INTERPOLATION\n" );
    models_shader.SetAttribLocation( "coord", ATTRIB_POSITION );
    models_shader.SetAttribLocation( "tex_coord", ATTRIB_TEX_COORD );
    models_shader.SetAttribLocation( "normal", ATTRIB_NORMAL );
    models_shader.MoveOnGPU();

    models_shader.FindUniform( "tex" );
    models_shader.FindUniform( "proj_mat" );
    models_shader.FindUniform( "pos" );
    models_shader.FindUniform( "light" );

    models_shader.FindUniform( "sky_ambient_light" );
    models_shader.FindUniform( "fire_ambient_light" );

#else//if !OGL21
    if( deferred_shading )
    {
        if( models_shader.Load( "shaders/DS/world_frag.glsl", "shaders/DS/model_vert.glsl", NULL ) )
            printf( "error. Model shader not found.\n" );

        models_shader.Define( "#define NORMAL_INTERPOLATION\n" );
        models_shader.SetAttribLocation( "coord", ATTRIB_POSITION );
        models_shader.SetAttribLocation( "tex_coord", ATTRIB_TEX_COORD );
        models_shader.SetAttribLocation( "normal", ATTRIB_NORMAL );
        models_shader.MoveOnGPU();

        models_shader.FindUniform( "tex" );
        models_shader.FindUniform( "model_buffer" );
        models_shader.FindUniform( "texture_buffer_shift" );
        models_shader.FindUniform( "proj_mat" );
        models_shader.FindUniform( "normal_mat" );
        models_shader.FindUniform( "cam_pos" );
    }
    else// if ! deferred_shading
    {
        if( models_shader.Load( "shaders/world_frag.glsl", "shaders/model_vert.glsl", NULL ) )
            printf( "error. Model shader not found.\n" );

        models_shader.Define( "#define NORMAL_INTERPOLATION\n" );
        models_shader.SetAttribLocation( "coord", ATTRIB_POSITION );
        models_shader.SetAttribLocation( "tex_coord", ATTRIB_TEX_COORD );
        models_shader.SetAttribLocation( "normal", ATTRIB_NORMAL );
        models_shader.MoveOnGPU();

        models_shader.FindUniform( "tex" );
        models_shader.FindUniform( "shadow_map" );
        models_shader.FindUniform( "model_buffer" );
        models_shader.FindUniform( "texture_buffer_shift" );
        models_shader.FindUniform( "shadow_mat" );
        models_shader.FindUniform( "proj_mat" );
        models_shader.FindUniform( "normal_mat" );
        models_shader.FindUniform( "cam_pos" );
        models_shader.FindUniform( "fog_color" );
        models_shader.FindUniform( "max_view2" );
        models_shader.FindUniform( "sun_vector" );
        models_shader.FindUniform( "material_property" );

        models_shader.FindUniform( "direct_sun_light" );
        models_shader.FindUniform( "sky_ambient_light" );
        models_shader.FindUniform( "fire_ambient_light" );
    }
#endif//if !OGL21

}


void r_Renderer::SetupFrameBuffers()
{
#ifdef OGL21
    underwater_buffer_gl21.SetFiltration( GL_NEAREST, GL_LINEAR );
    underwater_buffer_gl21.Create();
    underwater_buffer_gl21.TextureData( viewport_x/2, viewport_y/2, GL_UNSIGNED_INT, GL_RGB, 24, NULL );
    underwater_buffer_gl21.MoveOnGPU();
#else
    GLenum hdr_tex_type= GL_HALF_FLOAT;
    int hdr_tex_components= 3;
    scene_hdr_buffer.Create( 24, 0, 1, &hdr_tex_components, &hdr_tex_type, viewport_x, viewport_y );
    scene_hdr_buffer.DesableDepthTextureCompareMode();

    scene_hdr_underwater_buffer.Create( 24, 0, 1, &hdr_tex_components, &hdr_tex_type, viewport_x/2, viewport_y/2 );
    scene_hdr_underwater_buffer.DesableDepthTextureCompareMode();

    hdr_tex_components= 1;
    brightness_info_buffer.Create( 0, 0, 1, &hdr_tex_components, &hdr_tex_type, 128, 128 );


    hdr_tex_components=3;
    hdr_tex_type= GL_HALF_FLOAT;
    bloom_buffer[0].Create( 0, 0, 1, &hdr_tex_components, &hdr_tex_type, viewport_x/2, viewport_y/2 );
    bloom_buffer[1].Create( 0, 0, 1, &hdr_tex_components, &hdr_tex_type, viewport_x/2, viewport_y/2 );
    bloom_buffer[1].SetColorTextureFiltration( 0, GL_NEAREST, GL_LINEAR );

    sun_shadow_map[0].Create( 16, 0, 0, NULL, NULL, shadowmap_size, shadowmap_size );
    if( filter_shadowmap_texture )
        sun_shadow_map[0].SetDepthTextureFiltration( GL_NEAREST, GL_LINEAR );
    sun_shadow_map[1].Create( 16, 0, 0, NULL, NULL, shadowmap_size, shadowmap_size );
    if( filter_shadowmap_texture )
        sun_shadow_map[1].SetDepthTextureFiltration( GL_NEAREST, GL_LINEAR );
    sun_shadow_map[1].BindNull();


    if( deferred_shading )
    {
        GLuint tex_components[]= { 4, 4 };
        GLuint tex_data_types[]= { GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE };

        g_buffer.Create( 0, 0, 2, tex_components, tex_data_types, viewport_x, viewport_y );
        g_buffer.SetDepthBuffer( &scene_hdr_buffer );
        g_buffer.DesableDepthTextureCompareMode();
        g_buffer.ClearBuffer( true, true );

        underwater_g_buffer.Create( 0, 0, 2, tex_components, tex_data_types, viewport_x/2, viewport_y/2 );
        underwater_g_buffer.SetDepthBuffer( &scene_hdr_underwater_buffer );
        underwater_g_buffer.DesableDepthTextureCompareMode();
        underwater_g_buffer.ClearBuffer( true, true );
    }


#endif

    glGenTextures( 1, &scene_depth_buffer );
    glBindTexture( GL_TEXTURE_2D, scene_depth_buffer );


    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,
                  viewport_x, viewport_y, 0, GL_DEPTH_COMPONENT,
                  GL_UNSIGNED_BYTE, NULL );


    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

}

void r_Renderer::Initialize()
{
    gpu_data_mutex.lock();


    GetGLFunctions();
    printf( "functions ready\n" );
    current_renderer= this;

    GetExtensionsStrings();
#if OGL21
    //if( ! GLExtensionSupported( "GL_ARB_draw_elements_base_vertex" ) )
    //   printf( "error, extension \"GL_ARB_draw_elements_base_vertex\" not supported.\n" );
#endif

    printf( "\n\n#video information:\n" );
    char* vendorname= glGetString( GL_VENDOR );
    if( !strcmp( vendorname, "NVIDIA Corporation" ) )
        printf( "vendor:            %s\n", vendorname );
    else
        printf( "warning, \"%s\" is not a graphics card.\n", vendorname );
    printf( "gl version:        %s\n", glGetString( GL_VERSION ) );

    int i_tmp;
    glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &i_tmp ) ;
    printf( "texture units:     %d\n", i_tmp );
    glGetIntegerv( GL_MAX_TEXTURE_SIZE, &i_tmp );
    printf( "max texture size: %d\n", i_tmp );
    glGetIntegerv( GL_MAX_ARRAY_TEXTURE_LAYERS, &i_tmp );
    printf( "max texture layers in array: %d\n", i_tmp );



    LoadShaders();
    printf( "shaders compiled\n" );


    SetupFrameBuffers();

    LoadTextures();
    printf( "textures ready\n" );


    rain.UseGeometryShader( use_geometry_particles );
    rain.Initialize();


    vertex_buffer_size= 0;
    vertex_buffer= NULL;


    visibly_world_size[0]= world->NumShreds();//R_SHRED_NUM;
    visibly_world_size[1]= world->NumShreds();//R_SHRED_NUM;

    //world map
    world_map= new char [ visibly_world_size[0] * visibly_world_size[1] ];
    world_map_colors= new unsigned char [ visibly_world_size[0] * visibly_world_size[1] ];
    world_map_to_draw= new unsigned char [ visibly_world_size[0] * visibly_world_size[1] ];
    world_map_colors_to_draw= new unsigned char [ visibly_world_size[0] * visibly_world_size[1] ];

    shreds= new r_ShredInfo[ visibly_world_size[0] * visibly_world_size[1] ];
    draw_shreds=  new r_ShredInfo[ visibly_world_size[0] * visibly_world_size[1] ];
    /*setup of vertex buffer parametrs*/

    visibly_information= new unsigned char[ visibly_world_size[0] * visibly_world_size[1] * 16 * 16 * 128 ];
    shreds_to_draw_list= new r_ShredInfo*[ visibly_world_size[0] * visibly_world_size[1] ];
    shreds_to_draw_indeces= new quint32[ visibly_world_size[0] * visibly_world_size[1] ];
    shreds_to_draw_quad_count= new quint32[ visibly_world_size[0] * visibly_world_size[1] ];
    shreds_to_draw_base_vertices= new quint32[ visibly_world_size[0] * visibly_world_size[1] ];

    water_quads_base_vertices= new quint32[ visibly_world_size[0] * visibly_world_size[1] ];
    water_quads_to_draw_count= new quint32[ visibly_world_size[0] * visibly_world_size[1] ];

    printf( "vertex attribs initialized\n" );

    model_manager.InitModelTable();
    model_manager.LoadModels();

    StartUpText();
    printf( "text subsystem initialized" );

    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClearDepth( 1.0f );
    glClearStencil(0);
    glEnable( GL_DEPTH_TEST );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glDepthFunc( GL_LEQUAL );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_FRONT );


    //for( int i=0; i< number_of_extension_string; i++ )
    //   printf( "%s\n", extension_strings[i] );
    update_thread.start();
    gpu_data_mutex.unlock();
    vertex_buffer_updated= false;

    AddNotifyString( QString("game started") );
}

r_Renderer::r_Renderer( World* w,Player* p, int width, int height ):
    world(w),
    player(p),
    frame_count(0),
    update_count(0),
    fps(0), last_fps_time(0), last_frame_number(0), last_frame_time(0),
    max_fps(0), min_fps(0), max_fps_to_draw(0), min_fps_to_draw(0),
    shreds_update_per_second(0), shreds_update_per_second_to_draw(0),
    startup_time( 0, 0 ),
    active_inventory_slot(-1),
    gpu_data_mutex(QMutex::NonRecursive),
    host_data_mutex( QMutex::NonRecursive ),
    text_data_mutex( QMutex::NonRecursive ),
    update_thread( SUpdateTick, NULL, true ),
    cam_angle( 0.0f, 0.0f, 0.0f ),
    fov( 70.0f * m_Math::FM_TORAD ), z_near( 0.125f ), z_far( 512.0f ),
    viewport_x(width), viewport_y(height),
    renderer_initialized( false ), full_update(false), need_full_update_shred_list(false),
    front_shadowmap(0), back_shadowmap(1),
    scene_brightness(0.0f),
    config( QDir::currentPath()+"/freg.ini", QSettings::IniFormat ),
    rain( 16384 ),
    show_map(false),
    show_block_list(false )
{
    world->WriteLock();
    r_Config.double_update_interval_radius= 2;
    r_Config.quad_update_intrval_radius= 3;
    r_Config.octal_update_interval_radius = 4;
    r_Config.update_interval= 50;
    //z_far= float( world->NumShreds() ) * 0.5f * 16.0f;

    notify_log_last_line= R_NUMBER_OF_NOTIFY_LINES+ 1;
    for( int i=0; i< R_NUMBER_OF_NOTIFY_LINES; i++ )
        notify_log[i][0]= 0;
}

bool r_Renderer::GLExtensionSupported( char* ext_name )
{
    unsigned int i;
    unsigned char hash= 0;
    while( ext_name[i] != 0 )
    {
        hash^= ext_name[i];
        i++;
    }
    for( i=0; i< number_of_extension_string; i++ )
    {
        if( hash == extension_strings_hash[i] )
        {
            if( !strcmp( ext_name, extension_strings[i] ) )
                return true;
        }
    }
    return false;
}

char* r_Renderer::extension_strings[ MAX_GL_EXTENSIONS ];
unsigned char r_Renderer::extension_strings_hash[ MAX_GL_EXTENSIONS ];
unsigned int r_Renderer::number_of_extension_string;

void r_Renderer::GetExtensionsStrings()
{
#ifdef OGL21
    char* ext_str= glGetString( GL_EXTENSIONS );
    extension_strings[0]= new unsigned char[ strlen( (const char*) ext_str ) + 1 ];
    strcpy( extension_strings[0], ext_str );
    unsigned int str_i= 0, i;
    while(1)
    {
        //find substring
        while( (*ext_str) != 0 && (*ext_str) != ' ' )
            ext_str++;

        if( *ext_str == 0 )
            break;
        extension_strings[ str_i + 1 ]= ext_str+1;

        *ext_str= 0;
        ext_str++;
        //calculate extension string hash
        i= 0;
        while( extension_strings[ str_i ][i] != 0  )
        {
            extension_strings_hash[str_i]^= extension_strings[ str_i ][i];
            i++;
        }
        str_i++;
    }
    number_of_extension_string= str_i;

#else
    unsigned int i= 0, j;
    char* str= NULL;
    do
    {
        str= (char*) glGetStringi( GL_EXTENSIONS, i );
        if( str != NULL )
        {
            extension_strings[i]= str;
            extension_strings_hash[i]= 0;
            j= 0;
            while( str[j] != 0 )
            {
                extension_strings_hash[i]^=str[j];
                j++;
            }
        }
        i++;
    }
    while ( str!= NULL && i < MAX_GL_EXTENSIONS );
    number_of_extension_string= i-1;
#endif
}

void r_Renderer::BuildShredList()
{
    float fov_x= 2.0 * atan( tan( fov * 0.5 ) * viewport_y / viewport_x );
    unsigned int x, y;
    float a, b;
    a= cam_angle.z + m_Math::FM_PI2;
    b= -cam_angle.x;
    m_Vec3 cam_vec( -cos(a) * cos(b),
                    -sin(a) * cos(b),
                    -sin(b) );

    a= cam_angle.z + m_Math::FM_PI2;
    b= cam_angle.x + fov * 0.5;
    m_Vec3 top_plane_vec( -cos(a) * sin(b),
                          -sin(a) * sin(b),
                          -cos(b) );

    top_plane_vec.Normalize();
    cam_vec.Normalize();
    float tmp= cam_vec * top_plane_vec * 2.0;
    m_Vec3 bottom_plane_vec= cam_vec * tmp;
    bottom_plane_vec-= top_plane_vec;

    m_Vec3 left_plane_vec;
    left_plane_vec.x= cos( cam_angle.z + m_Math::FM_PI - fov_x );
    left_plane_vec.y= -sin( cam_angle.z + m_Math::FM_PI - fov_x );
    m_Vec3 right_plane_vec;
    // m_Vec3 vec_to_shred;
    r_ShredInfo* shred;
    float dot;
    shreds_to_draw_count= 0;
    water_shreds_to_draw_count= 0;

    for( x=0 ; x< visibly_world_size[0]; x++ )
    {
        for( y= 0; y< visibly_world_size[1]; y ++ )
        {
            //circle world form
            /*short dx, dy;
            dx= x - visibly_world_size[0]/2;
            dy= y - visibly_world_size[1]/2;
            if( dx * dx + dy * dy > visibly_world_size[0] * visibly_world_size[1] / 4 )
                continue;*/

            shred= &draw_shreds[ x + y * visibly_world_size[0] ];
            if( shred->IsOnOtherSideOfPlane( cam_position, cam_vec ) )
            {
                if( shred->IsOnOtherSideOfPlane( cam_position, top_plane_vec ) &&
                        shred->IsOnOtherSideOfPlane( cam_position, bottom_plane_vec ) )
                {

                    shreds_to_draw_list[ shreds_to_draw_count ] = shred;
                    shreds_to_draw_indeces[ shreds_to_draw_count ] =0;

#ifdef OGL21
                    shreds_to_draw_quad_count[ shreds_to_draw_count ]=  ( shred->quad_count - shred->water_quad_count ) * 4;
                    shreds_to_draw_base_vertices[ shreds_to_draw_count ] = shred->vertex_buffer - vertex_buffer;
#else
                    shreds_to_draw_quad_count[ shreds_to_draw_count ]=  ( shred->quad_count - shred->water_quad_count ) * 6;
                    shreds_to_draw_base_vertices[ shreds_to_draw_count ] = shred->vertex_buffer - vertex_buffer;
#endif

                    shreds_to_draw_count++;

                    if( shred->water_quad_count )
                    {
#ifdef OGL21
                        water_quads_base_vertices[ water_shreds_to_draw_count ]= shred->water_vertices - vertex_buffer;
                        water_quads_to_draw_count[ water_shreds_to_draw_count ]= shred->water_quad_count * 4;
#else
                        water_quads_base_vertices[ water_shreds_to_draw_count ]= shred->water_vertices - vertex_buffer;
                        water_quads_to_draw_count[ water_shreds_to_draw_count ]= shred->water_quad_count * 6;
#endif
                        water_shreds_to_draw_count++;
                    }
                }
            }
        }
    }
}

void r_Renderer::UpdateWorldMap()
{
    int x, y;
    char c;
    unsigned char color;
    Shred* s;

    int player_longi= player->GetLongitude() , player_lati= player->GetLatitude();
    for( x=0; x< visibly_world_size[0]; x++ )
    {

        for( y= 0; y< visibly_world_size[1]; y++ )
        {
            s= shreds[ y * visibly_world_size[0] + x ].shred;
            c= s->TypeOfShred( s->Longitude(), s->Latitude() );
            if( s->Latitude() ==  player_lati && s->Longitude() ==  player_longi )
                c= '@';

            if( c == '~' )
                color= MAP_SHRED_COLOR_ID_WATER;
            else if( c == '.' )
                color= MAP_SHRED_COLOR_ID_PLAIN;
            else if( c == '^' )
                color= MAP_SHRED_COLOR_ID_MOUNTAIN;
            else if( c == '%' )
                color= MAP_SHRED_COLOR_ID_FOREST;
            else if( c == '+' )
                color= MAP_SHRED_COLOR_ID_HILL;
            else if( c == 'p' )
                color= MAP_SHRED_COLOR_ID_PYRAMID;
            else if( c == '@' )
                color= MAP_PLAYER_COLOR_ID;
            else if( c == ':' )
                color= MAP_SHRED_COLOR_ID_DESERT;
            else
                color= MAP_SHRED_COLOR_ID_DEFAULT;

            world_map_colors[ y * visibly_world_size[0] + x ]= color;
            world_map[ y * visibly_world_size[0] + x ]= c;
        }
    }
    world_map_updated= true;
}

void r_Renderer::MoveMap( int dir )
{
    host_data_mutex.lock();


    short x, y, dx, dy, x0, y0, x1, y1, x_shift, y_shift;

    switch (dir)
    {
    case EAST://+x
        x0= 0;
        x1= visibly_world_size[0] - 1;
        y0= 0;
        y1= visibly_world_size[1];
        dx= 1;
        dy= 1;
        x_shift= 1;
        y_shift= 0;
        break;

    case WEST://-x
        x0= visibly_world_size[0] - 1;
        x1= 0;
        y0= 0;
        y1= visibly_world_size[1];
        dx= -1;
        dy= 1;
        x_shift= -1;
        y_shift= 0;
        break;

    case NORTH://+y
        x0= 0;
        x1= visibly_world_size[0];
        y0= visibly_world_size[1] - 1;
        y1= 0;
        dx= 1;
        dy= -1;
        x_shift= 0;
        y_shift= -1;
        break;

    case SOUTH://-y
        x0= 0;
        x1= visibly_world_size[0];
        y0= 0;
        y1= visibly_world_size[1] - 1;
        dx= 1;
        dy= 1;
        x_shift= 0;
        y_shift= 1;
        break;

    default:
        printf( "error. Unknown direction.\n" );
    };

    r_ShredInfo *shred, *shred1;

    ///hack
    r_ShredInfo new_shreds[ 40];
    if( visibly_world_size[0] > 40 || visibly_world_size[1] > 40 )
        printf( "error, shred temp buffer is small." );

    //\E7\E0\EF\EE\EB\ED\E5\ED\E8\E5 \E4\E0\ED\ED\FB\F5 \EE \ED\EE\E2\FB\F5 \EB\EE\F1\EA\F3\F2\E0\F5
    switch (dir)
    {
    case WEST:
    case EAST:
        for( y=0; y< visibly_world_size[1]; y++ )
        {
            shred= &new_shreds[y];//&shreds[ x1 + y * visibly_world_size[0] ];
            shred->shred= world->GetShred( x1 * 16, y * 16 );
            shred->longitude= shred->shred->Longitude();
            shred->latitude= shred->shred->Latitude();

            shred->updated= true;
            shred->immediately_update= true;
            shred1= &shreds[ x0 + y * visibly_world_size[0] ];
            shred->quad_buffer_count=   shred1->quad_buffer_count;
            shred->visibly_information= shred1->visibly_information;
            shred->vertex_buffer=       shred1->vertex_buffer;
            shred->index_buffer_offset= shred1->index_buffer_offset;
            shred->model_list= 			shred1->model_list;
            shred->GetVisiblyInformation();
            //\E7\E0\EA\EE\EB\FC\F6\EE\E2\FB\E2\E0\ED\E8\E5 - \E1\F3\F4\F4\E5\F0\FB \E2\FB\E4\E5\EB\E5\ED\ED\FB\E5 \EF\EE\E4 \F1\F2\E0\F0\FB\FB\E5 \EB\EE\F1\EA\F3\F2\FB \EF\F0\E8\E2\FF\E7\FB\E2\E0\FE\F2\F1\FF \EA \ED\EE\E2\FB\EC
        }
        break;

    case SOUTH:
    case NORTH:
        //printf( "y0=%d, y1=%d\n", y0, y1 );
        for( x=0; x< visibly_world_size[0]; x++ )
        {
            shred= &new_shreds[x];//&shreds[ x + y1 * visibly_world_size[0] ];
            shred->shred= world->GetShred( x * 16, y1 * 16 );
            shred->longitude= shred->shred->Longitude();
            shred->latitude= shred->shred->Latitude();

            shred->updated= true;
            shred->immediately_update= true;
            shred1= &shreds[ x + y0 * visibly_world_size[0] ];
            shred->quad_buffer_count=   shred1->quad_buffer_count;
            shred->visibly_information= shred1->visibly_information;
            shred->vertex_buffer=       shred1->vertex_buffer;
            shred->index_buffer_offset= shred1->index_buffer_offset;
            shred->model_list= 			shred1->model_list;
            shred->GetVisiblyInformation();
        }
        break;

    };

    //\EF\E5\F0\E5\EC\E5\F9\E5\ED\E8\E5 \EB\EE\F1\EA\F3\F2\EE\E2, \F3\E6\E5 \F1\F3\F9\E5\F1\F2\E2\F3\FE\F9\E8\F5
    for( x= x0; x != x1; x+= dx )
    {
        for( y= y0; y!= y1; y+=dy )
        {
            shred = &shreds[ x + y * visibly_world_size[0] ];
            shred1= &shreds[ x + x_shift + visibly_world_size[0] * ( y + y_shift ) ];
            memcpy( shred, shred1, sizeof( r_ShredInfo ) );
        }
    }

    //\E7\E0\EF\EE\EB\ED\E5\ED\E8\E5 \E4\E0\ED\ED\FB\F5 \EE \ED\EE\E2\FB\F5 \EB\EE\F1\EA\F3\F2\E0\F5
    switch (dir)
    {
    case WEST:
    case EAST:
        for( y=0; y< visibly_world_size[1]; y++ )
            shreds[ x1 + y * visibly_world_size[0] ]= new_shreds[y];
        break;

    case SOUTH:
    case NORTH:
        for( x=0; x< visibly_world_size[0]; x++ )
            shreds[ x + y1 * visibly_world_size[0] ]= new_shreds[x];
        break;

    };
    //\E2\FB\F1\F2\E0\E2\EB\E5\ED\E8\E5 \F3\EA\E0\E7\E0\F2\E5\E4\E5\E9 \ED\E0 \F1\EE\F1\E5\E4\ED\E8\E5 \EB\EE\F1\EA\F3\F2\FB
    for( x=0; x< visibly_world_size[0]; x++ )
    {
        for( y= 0; y< visibly_world_size[1]; y++ )
        {

            shred= &shreds[ x + y * visibly_world_size[0] ];
            //printf( "shred(%d,%d): %d %d\n", x, y, shred->longitude, shred->latitude );
            if( x != 0 )
                shred->west_shred= &shreds[ -1 + x + y * visibly_world_size[0] ];
            else
                shred->west_shred= NULL;

            if( y != 0 )
                shred->north_shred= &shreds[ x + ( y - 1 ) * visibly_world_size[0] ];
            else
                shred->north_shred= NULL;

            if( x != visibly_world_size[0] - 1 )
                shred->east_shred= &shreds[ 1 + x + y * visibly_world_size[0] ];
            else
                shred->east_shred= NULL;

            if( y != visibly_world_size[1] - 1 )
                shred->south_shred= &shreds[ x + ( y + 1 ) * visibly_world_size[0] ];
            else
                shred->south_shred= NULL;

        }
    }

    //\EE\E1\ED\EE\E2\EB\E5\ED\E8\E5 \F1\EE\F1\E5\E4\ED\E8\F5 \F1 \ED\EE\E2\FB\EC\E8 \EB\EE\F1\EA\F3\F2\EE\E2, \E2 \F1\EB\F3\F7\E0\E5 \E4\E2\E8\E6\E5\ED\E8\FF \ED\E0 \FE\E3 \E8\EB\E8 \ED\E0 \E2\EE\F1\F2\EE\EA
    if( dir == SOUTH )
    {
        for( x=0; x< visibly_world_size[0]; x++ )
            shreds[ x + ( visibly_world_size[1] - 2 ) * visibly_world_size[0] ].updated= true;
    }
    else if ( dir == EAST )
    {
        for( y=0; y< visibly_world_size[1]; y++ )
            shreds[ visibly_world_size[0] - 2 + y * visibly_world_size[0] ].updated= true;
    }

    Shred* world_shred= world->GetShred( 16 * visibly_world_size[0]/2, 16 * visibly_world_size[1]/2 );
    center_shred_latitude= world_shred->Latitude();
    center_shred_longitude= world_shred->Longitude();

    host_data_mutex.unlock();
}


void r_Renderer::SetConsoleComandString( char* str )
{
    if( str == NULL )
    {
        console_string[0]= 0;
        return;
    }

    console_string[0]= '>';
    unsigned int i=1;
    while( *str )
    {
        console_string[i++]= quint16( *str );
        str++;
    }
    console_string[i]= 0;
}

void r_Renderer::AddNotifyString( const QString& str )
{
    unsigned int l= min( str.length(), R_NOTIFY_LINE_LENGTH - 2 );
    unsigned int n= notify_log_last_line % R_NUMBER_OF_NOTIFY_LINES;

    memcpy( notify_log[n], str.unicode(),
            sizeof( quint16 ) * l );

    notify_log[n][ l ]= '\n';
    notify_log[n][ l + 1]= 0;
    notify_log_last_line++;
}


void r_Renderer::LoadConfig()
{

    config.beginGroup( "screen" );

    shadowmap_size= config.value( "shadowmap_size", 1024 ).toInt();
    filter_world_texture= config.value( "filter_world_texture", true ).toBool();
    filter_shadowmap_texture= config.value( "filter_shadowmap_texture", true ).toBool();

    bloom= config.value( "bloom", false ).toBool();
    antialiasing= config.value( "antialiasing", false ).toBool();
    edge_detect= config.value( "edge_detect", false ).toBool();

    viewport_x= config.value( "screen_x", 1024 ).toInt();
    viewport_y= config.value( "screen_y", 768 ).toInt();

    use_geometry_particles= config.value( "use_geometry_particles", true ).toBool();

    deferred_shading= config.value( "deferred_shading", false ).toBool();

    texture_manager.SetTexturesSize( config.value( "texture_size", 128 ).toInt() );


#ifdef OGL21//hack
    deferred_shading= false;
#endif

    config.endGroup();
}

void r_Renderer::ShutDown()
{
    update_thread.Stop();
}
#endif//RENDERER_CPP
