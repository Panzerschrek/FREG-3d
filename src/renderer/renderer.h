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

#ifndef RENDERER_H
#define RENDERER_H
#include "ph.h"

#include <QSettings>
#include "fmd_format.h"
#include "texture.h"
#include "polygon_buffer.h"
#include "glsl_program.h"
#include "cubemap.h"
#include "font.h"
#include "frame_buffer.h"
#include "texture_array.h"
#include "../thread.h"
#include "../world.h"
#include "../Shred.h"
#include "model_manager.h"
#include "texture_manager.h"
//#include "../Player.h"
#include "weather_effects_particle_manager.h"
class Player;
#define MAX_GL_EXTENSIONS 512
#define R_SHRED_WIDTH ::SHRED_WIDTH
#define R_SHRED_HEIGHT ::HEIGHT

#define MIN_SUN_HEIGHT 0.2618f

#define FREG_GL_MULTI_THREAD 0

#define WORLD_NORMAL_X      0
#define WORLD_NORMAL_MIN_X  1
#define WORLD_NORMAL_Y      2
#define WORLD_NORMAL_MIN_Y  3
#define WORLD_NORMAL_Z      4
#define WORLD_NORMAL_MIN_Z  5

#define R_MAX_INDECES_PER_SHRED 24576 * 2

const m_Vec3 day_fog( 2.4, 2.4, 3.0 );
const m_Vec3 night_fog( 0.15, 0.15, 0.1875 );
struct r_WorldVertex
{
    qint16 coord[3];
    quint8 tex_coord[3];
    quint8 light[2];
    quint8  normal_id;
    quint8 reserved[4];
};//16b struct


struct r_FontVertex
{
    float coord[2];
    quint16 tex_coord[2];
    quint8 color[3];
    quint8 tex_id;
};//16b struct


struct r_ShredInfo
{
    short longitude;
    short latitude;
    unsigned int quad_count;
    unsigned int quad_buffer_count;//размер, выделенный под Лоскут в вершинном буффере
    unsigned int index_buffer_offset;//смещение в индексном буффере (в квадах )
    unsigned int flags;
    unsigned short  max_geometry_height,
    min_geometry_height;
    r_WorldVertex* vertex_buffer;//указатель на вершинный буффер( внутри глобального вершинного буффера )
    unsigned char* visibly_information;//указатель на массив, в котором хранится информация о видимости блоков лосута
    Shred* shred;//pointer valid in 1 update

    r_ShredInfo *east_shred, *south_shred,
    *north_shred, *west_shred;

    bool updated, rebuilded, immediately_update;
    bool quads_updated;
    bool visibly_information_is_valid;

    r_LocalModelList model_list;
    unsigned int model_count_to_draw;

    void GetQuadCount();
    void GetVisiblyInformation( );
    void BuildShred( r_WorldVertex* shred_vertices );
    void UpdateShred();
    void UpdateCube( short x0, short y0, short z0, short x1, short y1, short z1 );
    bool IsOnOtherSideOfPlane( m_Vec3 point, m_Vec3 normal );

    //возвращает максимальную проекцую расстаяния на оси от лоскута до точки x,y
    unsigned short GetShredDistance( short x, short y );

    //unsigned int model_count;

    unsigned int water_quad_count;
    r_WorldVertex* water_vertices;

};


#define R_NUMBER_OF_NOTIFY_LINES 8
#define R_NOTIFY_LINE_LENGTH 64

#define R_LETTER_BUFFER_LEN 8192

class r_Renderer
{
public:

    inline void SetCamAngle( m_Vec3 a );
    inline void SetCamPosition( m_Vec3 p );
    inline void SetBuildPosition( m_Vec3 p );
    inline void SetActiveInventorySlot( int n, bool player_inventory );
    inline void RotateCam( m_Vec3& r);

    inline int ViewportWidth()
    {
        return viewport_x;
    }
    inline int ViewportHeight()
    {
        return viewport_y;
    }


    r_Renderer( World* w,Player* p, int width, int height );
    void Initialize();
    void ShutDown();

    void UpdateBlock( short x, short y, short z );
    void MoveMap( int dir );
    void UpdateAll();
    void UpdateShred( short x, short y );
    void UpdateCube( short x0, short y0, short z0, short x1, short y1, short z1 );

public:
    void AddNotifyString( const QString& str );
    void SetUnderwaterMode( bool );
    void Draw();
    bool Initialized()
    {
        return renderer_initialized;
    }
private:

    void BuildWorld();
    void UpdateData();
    void UpdateGPUData();
    // функции работы с лоскутами
    void BuildShredList();
    //void UpdateShredGPUData( r_ShredInfo* shred );

    //hack for multithreading
public:
    static r_Renderer* current_renderer;
private:
    static void SUpdateTick();
    void UpdateTick();
    void SetupVertexBuffers();
    void DrawWorld();
    void DrawSky();
    void DrawHUD();
    void DrawMap();
    void DrawSun();
    void DrawWater();
    void DrawBuildCube();
    void DrawInventory();
    void LoadTextures();
    void LoadModels();
    void UpdateWorldMap();

    QMutex gpu_data_mutex, host_data_mutex;

    struct
    {
        int update_interval;//in ms
        unsigned short double_update_interval_radius,
        quad_update_intrval_radius,
        octal_update_interval_radius;
    } r_Config;

    const World* world;
    const Player* player;
    r_UniversalThread update_thread;
    unsigned int update_count;

    short center_shred_longitude, center_shred_latitude;


    r_Texture sun_texture;
    r_TextureArray texture_array;
    r_TextureManager texture_manager;
    r_CubeMap sky_cubemap;
    r_GLSLProgram world_shader, sky_shader, sun_shader, cube_shader, water_shader;
    r_PolygonBuffer world_buffer, temp_buffer, sky_buffer, cube_buffer;
    r_WorldVertex* vertex_buffer;
    quint16* indeces;

    unsigned char* visibly_information;
    unsigned short visibly_world_size[2];

    unsigned int vertex_buffer_size;//size of buffer
    unsigned int vertices_in_buffer;//real count of vertices in buffer
    unsigned int index_buffer_size;


	//map
	char* world_map, *world_map_to_draw;
	unsigned char* world_map_colors, *world_map_colors_to_draw;
	bool world_map_updated;


    //shred list
    r_ShredInfo* shreds, *draw_shreds;
    r_ShredInfo** shreds_to_draw_list;
    quint32 *shreds_to_draw_indeces, *shreds_to_draw_quad_count, *shreds_to_draw_base_vertices;
    unsigned int shreds_to_draw_count;

    //water shred list
    quint32 *water_quads_base_vertices, *water_quads_to_draw_count;
    unsigned int water_shreds_to_draw_count;


    int active_inventory_slot, active_inventory;

    m_Mat4 view_matrix;
    m_Vec3 cam_position, build_position, frame_cam_position, cam_angle, frame_cam_angle;
    float fov, z_near, z_far;
    unsigned short viewport_x, viewport_y;


    bool gl_texture_array_ext_enabled;
    unsigned int max_texture_units;

    void GetExtensionsStrings();
    bool GLExtensionSupported( char* ext_name );
    static char* extension_strings[ MAX_GL_EXTENSIONS ];
    static unsigned char extension_strings_hash[ MAX_GL_EXTENSIONS ];
    static unsigned int number_of_extension_string;


    bool renderer_initialized;
    bool out_of_vertex_buffer, out_of_index_buffer;

    unsigned int last_update_time;
    bool vertex_buffer_updated;
    bool full_update, need_full_update_shred_list;

#if FREG_GL_MULTI_THREAD
    GLsync sync_object;
    unsigned int transmition_frame;
#endif

    //font section
    m_Vec3 cursor_pos;
    r_FontVertex* font_vertices;
    quint16* font_indeces;
    unsigned int letter_count;
    r_GLSLProgram text_shader;
    r_Font font;
    r_PolygonBuffer text_buffer;
    QMutex text_data_mutex;
    void StartUpText();
public:
    //вывод текста. возвращает позицию, где текст закончился
    m_Vec3 AddText( float x, float y, const m_Vec3* color, float size,  const char* text, ... );
    m_Vec3 AddTextUnicode( float x, float y, const m_Vec3* color, float size,  const quint16* text );
    void SetCursorPos( float x, float y );
private:
    void DrawText();
    void DrawCursor();


    quint16 notify_log[ R_NUMBER_OF_NOTIFY_LINES ][ R_NOTIFY_LINE_LENGTH ];
    unsigned int notify_log_last_line;

    //shadows section
    r_FrameBuffer sun_shadow_map[2];// raw_sun_shadow_map;
    unsigned int front_shadowmap, back_shadowmap;
    bool back_shadowmap_is_invalid;
    r_GLSLProgram shadow_shader;// shadowmap_transform_shader;
    r_PolygonBuffer fullscreen_quad;
    m_Mat4 shadow_matrix, back_shadow_matrix;
    void RenderShadows();
    m_Vec3 sun_vector;

    //hdr
    r_FrameBuffer scene_hdr_buffer, brightness_info_buffer, scene_hdr_underwater_buffer;
    float scene_brightness;
    r_GLSLProgram postprocess_shader, brightness_shader;
    r_GLSLProgram underwater_postprocess_shader;
    bool underwater;

    r_FrameBuffer bloom_buffer[2];
    r_GLSLProgram bloom_shader[2];
    void MakePostProcessing();

    r_Texture underwater_buffer_gl21;
    void MakePostprocessingGL21();


    //models
    r_ModelManager model_manager;
    r_GLSLProgram models_shader;


    //perfomance counters section
    unsigned int last_fps_time;//время последнего замера fps
    unsigned int last_frame_time;//аремя последнего кадра
    unsigned int last_frame_number;//номер кадра,когда последний раз замерялся fps
    unsigned int frame_count;
    unsigned int fps;//количество кадров за секунду измерения
    unsigned int shreds_update_per_second, shreds_update_per_second_to_draw;
    float max_fps, min_fps, min_fps_to_draw, max_fps_to_draw;//максимальный и минимальный fps за это время
    QTime startup_time;
    void CalculateFPS();


    //water
    GLuint water_texture;

    //deferred shading
    m_Mat3 normal_matrix;//world2viewspace
    m_Mat4 view_space_shadow_matrix;
    r_FrameBuffer g_buffer, underwater_g_buffer;
    r_GLSLProgram ds_base_shader;
    void MakeDeferredShading();

    float sky_ambient_light, fire_ambient_light, direct_sun_light, sky_light;
    void CalculateLightPower();

    QSettings config;
    bool filter_world_texture, filter_shadowmap_texture;
    unsigned int shadowmap_size;

    bool bloom, antialiasing, edge_detect, deferred_shading;
public:
    const QSettings* Config() const
    {
        return &config;
    }
    void LoadConfig();


    r_WeatherEffectsParticleManager rain;
    void DrawRain();
    bool use_geometry_particles;
};


inline void r_ShredInfo::UpdateShred()
{
    updated= true;
    visibly_information_is_valid= false;
}

inline void r_Renderer::SUpdateTick()
{
    current_renderer->UpdateTick();
}
inline void r_Renderer::SetCamAngle( m_Vec3 a )
{
    cam_angle= a;
}
inline void r_Renderer::SetCamPosition( m_Vec3 p )
{
    cam_position= p;
}

inline void r_Renderer::RotateCam( m_Vec3& r)
{
    cam_angle+= r;
}
inline void r_Renderer::UpdateAll()
{
    full_update= true;
    unsigned short x, y;
    for( x= 0; x< visibly_world_size[0]; x++ )
    {
        for( y= 0; y< visibly_world_size[1]; y++ )
        {
            shreds[ x + visibly_world_size[0] * y ].visibly_information_is_valid= false;
        }
    }
   // UpdateWorldMap();
}



inline void r_Renderer::SetUnderwaterMode( bool underwater )
{
    this->underwater= underwater;
}

inline void r_Renderer::SetCursorPos( float x, float y )
{
    cursor_pos.x= x;
    cursor_pos.y= y;
}

inline void r_Renderer::SetBuildPosition( m_Vec3 p )
{
    build_position= p;
}

inline void r_Renderer::SetActiveInventorySlot( int n, bool player_inventory )
{
    active_inventory_slot= n;
    if( player_inventory )
    	active_inventory= 0;
	else
		active_inventory = 1;
}
#endif//RENDERER_H
