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

#ifndef SCREEN_FUNC_H
#define SCREEN_FUNC_H

#include <QString>
#include <QTimer>

#include "screen.h"
#include "world.h"
#include "blocks.h"
#include "Player.h"
#include <QApplication>
#include <QDesktopWidget>
enum Qt::Key;


Screen* Screen::current_screen= NULL;

FREGGLWidget::FREGGLWidget( Screen* s, r_Renderer* screen_renderer, QGLFormat format, QWidget* parent ):
    QGLWidget( format, parent )
{
    renderer=screen_renderer;
    screen= s;
    QGLFormat f= context()->format();

    if( FREG_GL_VERSION_MAJOR > format.majorVersion() )
        printf( "error. OpenGL %d.%d does no supported\n", FREG_GL_VERSION_MAJOR, FREG_GL_VERSION_MINOR );
    else if( FREG_GL_VERSION_MAJOR == format.majorVersion() && FREG_GL_VERSION_MINOR > format.minorVersion() )
        printf( "error. OpenGL %d.%d does no supported\n", FREG_GL_VERSION_MAJOR, FREG_GL_VERSION_MINOR );
}

FREGGLWidget::~FREGGLWidget() {}


QSize FREGGLWidget::minimumSizeHint() const
{
    return QSize(renderer->ViewportWidth(), renderer->ViewportHeight() );
}

QSize FREGGLWidget::sizeHint() const
{
    return QSize(renderer->ViewportWidth(), renderer->ViewportHeight() );
}

void FREGGLWidget::initializeGL()
{
    renderer->Initialize();
}

void FREGGLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height );
}

void FREGGLWidget::paintGL()
{
    screen->InputTick();
    renderer->Draw();
    screen->SaveScreenshot();
    update();
}

void FREGGLWidget::mousePressEvent(QMouseEvent* e)
{
    screen->mousePressEvent(e);
}
void FREGGLWidget::mouseMoveEvent(QMouseEvent* e)
{
    screen->mouseMoveEvent(e);
}
void FREGGLWidget::keyPressEvent(QKeyEvent* e)
{
    screen->keyPressEvent(e);
}
void FREGGLWidget::focusOutEvent( QFocusEvent* e)
{
    screen->focusOutEvent( e );
}
void FREGGLWidget::keyReleaseEvent(QKeyEvent* e)
{
    screen->keyReleaseEvent(e);
}
void FREGGLWidget::closeEvent(QCloseEvent* e)
{
    screen->closeEvent(e);
}

void FREGGLWidget::focusInEvent( QFocusEvent* e )
{
    screen->focusInEvent( e );
}


QString& Screen::PassString(QString & str) const
{
    return str;
}

void Screen::Print()
{
}


void Screen::Notify(const QString& str)
{
    //printf( "notify: %s\n", str.toLocal8Bit().constData() );
    renderer->AddNotifyString( str );
}

void Screen::UpdateAround(
    const ushort x ,
    const ushort y,
    const ushort z,
    const ushort range)
{

    if( ! renderer->Initialized() )
        return;

    short x0, y0, z0, x1, y1, z1;
    x0= max( 0, x - range );
    x1= min( x + range, w->NumShreds() * R_SHRED_WIDTH - 1 );

    y0= max( 0, y - range );
    y1= min( y + range, w->NumShreds() * R_SHRED_WIDTH - 1 );

    z0= max( 0, z - range );
    z1= min( z + range, R_SHRED_HEIGHT - 2 );

    renderer->UpdateCube( x0, y0, z0, x1, y1, z1 );
    //printf( "update cube: %d %d %d\n", x, y, z );

}
void Screen::Update(
    const unsigned short x,
    const unsigned short y,
    const unsigned short z)
{
    if( renderer->Initialized() )
    {
        renderer->UpdateBlock( x, y, z );
        //printf( "update\n" );
    }
}
void Screen::UpdateAll()
{
    if( renderer->Initialized() )
    {
        renderer->UpdateAll();
        printf( "update all\n" );
    }
}

Screen::Screen(
    World * const wor,
    Player * const pl):
    updated(true),
    cleaned(false),
    cam_ang( 0.0, 0.0, 2.18 ),
    cam_pos( 0.0, 0.0, 70.0 ),
    renderer( NULL ),
    screenshot_thread( sSaveScreenshot, NULL, true ),
    screenshot_thread_mutex( QMutex::NonRecursive ),
    VirtScreen( wor, pl ),
    screen_width(1024), screen_height(768),
    startup_time( 0, 0 ),
    free_look( true ),
    cam_lag( 0.8f ),
    cursor( Qt::BlankCursor )
{
    renderer= new r_Renderer( wor,pl, screen_width, screen_height );
    QGLFormat format;
#if OGL21
    format.setVersion( 2, 1 );
#else
    format.setVersion( FREG_GL_VERSION_MAJOR, FREG_GL_VERSION_MINOR );//format.setVersion( 3, 2 );
#endif
    format.setProfile( QGLFormat::CoreProfile );


    renderer->LoadConfig();
    screen_width= renderer->Config()->value( "screen/screen_x", 1024 ).toInt();
    screen_height= renderer->Config()->value( "screen/screen_y", 768 ).toInt();
    cam_lag= renderer->Config()->value( "screen/cam_lag", 0.8f ).toFloat();
    cam_lag= min( 0.99f, max( cam_lag, 0.5f ) );

    cam_pos.x= renderer->Config()->value( "screen/cam_pos_x", 1.0f ).toFloat();
    cam_pos.y= renderer->Config()->value( "screen/cam_pos_y", 1.0f ).toFloat();
    cam_pos.z= renderer->Config()->value( "screen/cam_pos_z", 1.0f ).toFloat();

    bool full_screen= renderer->Config()->value( "screen/fullscreen", false).toBool()  &&
                      QApplication::desktop()->width() == screen_width &&
                      QApplication::desktop()->height() == screen_height;

    format.setSwapInterval( renderer->Config()->value( "screen/vsync", 1).toBool() );
    gl_widget= new FREGGLWidget( this, renderer, format, NULL );
    layout=new QVBoxLayout();
    if( full_screen )
    {
        window= new QWidget( NULL, Qt::FramelessWindowHint );
        window->setWindowState( Qt::WindowFullScreen );
        window->showFullScreen();
    }
    else
        window= new QWidget( NULL, 0);


    window->setLayout(layout);
    window->move( 0, 0 );
    layout->addWidget( gl_widget );
    layout->setMargin(0);
    gl_widget->setFixedSize( screen_width, screen_height );

    window->setFocusPolicy( Qt::ClickFocus );
    gl_widget->setFocusPolicy( Qt::ClickFocus );
    gl_widget->setFocus();

    window->show();
    window->setFixedSize( window->size() );

    gl_widget->setCursor( cursor );

    for( unsigned int i= 0; i< 512; i++ )
        keys[i]= false;
    use_mouse= false;

    //current_screen= this;

    /*  connect(this, SIGNAL(InputReceived(int, int)),
              player, SLOT(Act(int, int)),
              Qt::DirectConnection);*/


    screenshot_number= 0;
    current_screen= this;
    need_save_screenshot= false;
    screenshot_data= new unsigned char[ screen_height * screen_width * 3 ];
    screenshot_thread_mutex.lock();
    screenshot_thread.start();
}

Screen::~Screen()
{
    //CleanAll();
}

void Screen::CleanAll()
{
    renderer->Config()->setValue( "screen/cam_pos_x", cam_pos.x );
    renderer->Config()->setValue( "screen/cam_pos_y", cam_pos.y );
    renderer->Config()->setValue( "screen/cam_pos_z", cam_pos.z );
    renderer->ShutDown();
    ExitReceived();
}


void Screen::GetBuildCoord()
{
    short local_world_x, local_world_y, z;
    local_world_x= -short( float( ( w->Latitude() - w->NumShreds()/2 ) * 16.0f ) - cam_pos.x  - 1.0f );
    local_world_y= -short( float( ( w->Longitude() - w->NumShreds()/2 ) * 16.0f ) - cam_pos.y  - 1.0f );
    z= short( cam_pos.z + 1.0f );

    m_Vec3 cam_vec;
    cam_vec.z= sin( cam_ang.x );
    cam_vec.y= cam_vec.x= cos( cam_ang.x );
    cam_vec.x*= cos( -cam_ang.z + m_Math::FM_PI2 );
    cam_vec.y*= - sin( -cam_ang.z + m_Math::FM_PI2 );

    short dx, dy, dz;

    if( cam_vec.z > 0.832f )
    {
        dz= 1;
        dx= 0;
        dy= 0;
    }
    else if( cam_vec.z < -0.832f )
    {
        dz= -1;
        dx= 0;
        dy= 0;
    }
    else if( cam_vec.z > 0.307776f )
    {
        dz= 1;
        if( cam_vec.x > 0.307776f )
            dx= 1;
        else if( cam_vec.x < -0.307776f )
            dx= -1;
        else dx= 0;

        if( cam_vec.y > 0.307776f )
            dy= 1;
        else if( cam_vec.y < -0.307776f )
            dy= -1;
        else dy= 0;
    }
    else if( cam_vec.z < - 0.307776f )
    {
        dz= -1;
        if( cam_vec.x > 0.307776f )
            dx= 1;
        else if( cam_vec.x < -0.307776f )
            dx= -1;
        else dx= 0;

        if( cam_vec.y > 0.307776f )
            dy= 1;
        else if( cam_vec.y < -0.307776f )
            dy= -1;
        else dy= 0;
    }
    else
    {
        dz= 0;
        if( cam_vec.x > 0.307776f )
            dx= 1;
        else if( cam_vec.x < -0.307776f )
            dx= -1;
        else dx= 0;

        if( cam_vec.y > 0.307776f )
            dy= 1;
        else if( cam_vec.y < -0.307776f )
            dy= -1;
        else dy= 0;
    }

    build_x= local_world_x + dx;
    build_y= local_world_y + dy;
    build_z= z + dz;


    build_pos.x= float(build_x + (w->Latitude() - w->NumShreds()/2 ) * 16  );
    build_pos.y= float( build_y + (w->Longitude() - w->NumShreds()/2 ) * 16  );
    build_pos.z= float( build_z );
}


//Screen* Screen::current_screen= NULL;


void Screen::focusOutEvent( QFocusEvent* e )
{
    for( int i= 0; i< 512; i++ )
        keys[i]= false;
    is_focus= false;
}


void Screen::focusInEvent(QFocusEvent * e)
{
    is_focus= true;
}

void Screen::mousePressEvent(QMouseEvent * e)
{
    if( e->button() == Qt::RightButton )
    {
        QString s("give 6 0 1");
        player->ProcessCommand( s );
        int i=0;
        while(  player->GetP()->HasInventory()->Number(i) == 0 && i < 10 )i++;
        player->Build( build_x, build_y, build_z, i );
    }
    else if( e->button() == Qt::LeftButton )
        // player->Damage( build_x, build_y, build_z );
        w->Damage( build_x, build_y, build_z, 10000, DIG );
}
void Screen::mouseMoveEvent(QMouseEvent * e )
{
}
void Screen::keyPressEvent( QKeyEvent* e )
{
    int key= e->key();
    key= ( key & 0xff ) | ( key >> 16 );
    if( key < 512 )
        keys[ key ]= true;
    switch(key)
    {

    case Qt::Key_Z:
        free_look= ! free_look;
        break;

    case Qt::Key_M:
        use_mouse= !use_mouse;
        if( use_mouse )
        {
            cursor.setShape( Qt::BlankCursor );
            renderer->AddNotifyString( "mouse on" );
        }
        else
        {
            cursor.setShape( Qt::ArrowCursor );
            renderer->AddNotifyString( "mouse off" );
        }
        break;


    case Qt::Key_Q:
        CleanAll();
        break;

    case Qt::Key_I:
        player->Backpack();
        break;

    case Qt::Key_BracketRight:
        need_save_screenshot= true;
        break;

    default:
        break;
    };
}
void Screen::closeEvent(QCloseEvent *e)
{
    CleanAll();
}
void Screen::keyReleaseEvent( QKeyEvent* e )
{
    int key= e->key();
    key= ( key & 0xff ) | ( key >> 16 );
    if( key < 512 )
        keys[ key ]= false;
}
void Screen::InputTick()
{
    // while( renderer == NULL )
    //    usleep( 1000 );
    //while( !renderer->Initialized() )
    //    usleep( 1000 );

    static int last_time= 0;
    static int last_move_event_time= 0;
    int dt_i= -QTime::currentTime().msecsTo( startup_time ) - last_time;
    float dt= float( dt_i );

    if( use_mouse && is_focus )
    {
        QPoint cur_local_pos= gl_widget->mapFromGlobal( cursor.pos() );
        cam_ang.z+= float( cur_local_pos.x() - screen_width/2 ) * 0.005f;
        cam_ang.x-= float( cur_local_pos.y() - screen_height/2 ) * 0.005f;
        cur_local_pos= gl_widget->mapFromGlobal( QPoint( 0, 0 ) );
        cursor.setPos( screen_width/2 - cur_local_pos.x(), screen_height/2 - cur_local_pos.y() );

        if( cam_ang.z < 0.0f ) cam_ang.z+= m_Math::FM_2PI;
        else if( cam_ang.z > m_Math::FM_2PI ) cam_ang.z-= m_Math::FM_2PI;

        if( cam_ang.x > m_Math::FM_PI2 ) cam_ang.x= m_Math::FM_PI2;
        else if( cam_ang.x < -m_Math::FM_PI2 ) cam_ang.x= -m_Math::FM_PI2;
    }

    if( keys[ FREG_KEY_FORWARD ] )
    {
        if( free_look )
        {
            cam_pos.y-= dt * 0.025 * m_Math::Cos( cam_ang.z );
            cam_pos.x+= dt * 0.025 * m_Math::Sin( cam_ang.z );
        }
        else if( last_time + dt_i - last_move_event_time > 1000 / w->TimeStepsInSec() )
        {
            if( cam_ang.z >	m_Math::FM_PI4 && cam_ang.z <= 3.0f * m_Math::FM_PI4 )
            {
                player->Move( EAST );
            }
            else if( cam_ang.z > 3.0f * m_Math::FM_PI4 && cam_ang.z <= 5.0f * m_Math::FM_PI4 )
                player->Move( SOUTH );
            else if( cam_ang.z > 5.0f * m_Math::FM_PI4 && cam_ang.z <= 7.0f * m_Math::FM_PI4 )
                player->Move( WEST );
            else if( cam_ang.z <= m_Math::FM_2PI || cam_ang.z <= m_Math::FM_PI4 )
                player->Move( NORTH );

            last_move_event_time= last_time + dt_i;
        }
    }
    if( keys[ FREG_KEY_BACKWARD ] )
    {
        if( free_look )
        {
            cam_pos.y+= dt * 0.025 * m_Math::Cos( cam_ang.z );
            cam_pos.x-= dt * 0.025 * m_Math::Sin( cam_ang.z );
        }
        else if( last_time + dt_i - last_move_event_time > 1000 / w->TimeStepsInSec() )
        {
            if( cam_ang.z >	m_Math::FM_PI4 && cam_ang.z <= 3.0f * m_Math::FM_PI4 )
                player->Move( WEST );
            else if( cam_ang.z > 3.0f * m_Math::FM_PI4 && cam_ang.z <= 5.0f * m_Math::FM_PI4 )
                player->Move( NORTH );
            else if( cam_ang.z > 5.0f * m_Math::FM_PI4 && cam_ang.z <= 7.0f * m_Math::FM_PI4 )
                player->Move( EAST );
            else if( cam_ang.z <= m_Math::FM_2PI || cam_ang.z <= m_Math::FM_PI4 )
                player->Move( SOUTH );

            last_move_event_time= last_time + dt_i;
        }
    }

    if( keys[ FREG_KEY_STRAFE_LEFT ] )
    {
        if( free_look )
        {
            cam_pos.y-= dt * 0.025 * m_Math::Cos( cam_ang.z - m_Math::FM_PI2 );
            cam_pos.x+= dt * 0.025 * m_Math::Sin( cam_ang.z - m_Math::FM_PI2);

        }
        else if( last_time + dt_i - last_move_event_time > 1000 / w->TimeStepsInSec() )
        {
            if( cam_ang.z >	m_Math::FM_PI4 && cam_ang.z <= 3.0f * m_Math::FM_PI4 )
                player->Move( NORTH );
            else if( cam_ang.z > 3.0f * m_Math::FM_PI4 && cam_ang.z <= 5.0f * m_Math::FM_PI4 )
                player->Move( EAST );
            else if( cam_ang.z > 5.0f * m_Math::FM_PI4 && cam_ang.z <= 7.0f * m_Math::FM_PI4 )
                player->Move( SOUTH );
            else if( cam_ang.z <= m_Math::FM_2PI || cam_ang.z <= m_Math::FM_PI4 )
                player->Move( WEST );

            last_move_event_time= last_time + dt_i;
        }
    }
    if( keys[ FREG_KEY_STRAFE_RIGHT ] )
    {
        if( free_look )
        {
            cam_pos.y-= dt * 0.025 * m_Math::Cos( cam_ang.z + m_Math::FM_PI2);
            cam_pos.x+= dt * 0.025 * m_Math::Sin( cam_ang.z + m_Math::FM_PI2);
        }
        else if( last_time + dt_i - last_move_event_time > 1000 / w->TimeStepsInSec() )
        {
            if( cam_ang.z >	m_Math::FM_PI4 && cam_ang.z <= 3.0f * m_Math::FM_PI4 )
                player->Move( SOUTH );
            else if( cam_ang.z > 3.0f * m_Math::FM_PI4 && cam_ang.z <= 5.0f * m_Math::FM_PI4 )
                player->Move( WEST );
            else if( cam_ang.z > 5.0f * m_Math::FM_PI4 && cam_ang.z <= 7.0f * m_Math::FM_PI4 )
                player->Move( NORTH );
            else if( cam_ang.z <= m_Math::FM_2PI || cam_ang.z <= m_Math::FM_PI4 )
                player->Move( EAST );

            last_move_event_time= last_time + dt_i;
        }
    }

    if( keys[ FREG_KEY_UP ] )
    {
        cam_ang.x+= dt * 0.18 * m_Math::FM_TORAD;
        if( cam_ang.x > m_Math::FM_PI2 ) cam_ang.x= m_Math::FM_PI2;
    }
    if( keys[ FREG_KEY_DOWN ] )
    {
        cam_ang.x-= dt * 0.18 * m_Math::FM_TORAD;
        if( cam_ang.x < -m_Math::FM_PI2 ) cam_ang.x= -m_Math::FM_PI2;
    }

    if( keys[ FREG_KEY_LEFT ] )
    {
        cam_ang.z-= dt * 0.18 * m_Math::FM_TORAD;
        if( cam_ang.z < 0.0f ) cam_ang.z+= m_Math::FM_2PI;
    }
    if( keys[ FREG_KEY_RIGHT ] )
    {
        cam_ang.z+= dt * 0.18 * m_Math::FM_TORAD;
        if( cam_ang.z > m_Math::FM_2PI ) cam_ang.z-= m_Math::FM_2PI;

    }

    if( keys[ FREG_KEY_JUMP ] )
    {
        if( free_look )
            cam_pos.z+= dt * 0.025;
        else if( last_time + dt_i - last_move_event_time > 1000 / w->TimeStepsInSec() )
        {
            if( cam_ang.z >	m_Math::FM_PI4 && cam_ang.z <= 3.0f * m_Math::FM_PI4 )
                player->Turn( EAST );
            else if( cam_ang.z > 3.0f * m_Math::FM_PI4 && cam_ang.z <= 5.0f * m_Math::FM_PI4 )
                player->Turn( SOUTH );
            else if( cam_ang.z > 5.0f * m_Math::FM_PI4 && cam_ang.z <= 7.0f * m_Math::FM_PI4 )
                player->Turn( WEST );
            else if( cam_ang.z <= m_Math::FM_2PI || cam_ang.z <= m_Math::FM_PI4 )
                player->Turn( NORTH );

            player->Jump();

            last_move_event_time= last_time + dt_i;
        }
    }
    if( keys[ FREG_KEY_CROUCH ] )
    {
        if( free_look )
            cam_pos.z-= dt * 0.025;
        else if( last_time + dt_i - last_move_event_time > 1000 / w->TimeStepsInSec() )
        {
            player->Turn( DOWN );
            player->Move( DOWN );
        }
    }
    renderer->SetCamAngle( cam_ang );

    if( ! free_look )
    {
        m_Vec3 new_cam_pos;
        new_cam_pos.x= float( player->X() + (w->Latitude() - w->NumShreds()/2 ) * 16  ) -0.5f;
        new_cam_pos.y= float( player->Y() + (w->Longitude() - w->NumShreds()/2 ) * 16  ) -0.5f;
        new_cam_pos.z= float( player->Z() ) + 0.5f/*- cos( cam_ang.x + m_Math::FM_PI2 ) + 1.0f*/;

        unsigned char upper_block_sub= w->GetBlock( player->X(), player->Y(), player->Z() + 1 )->Sub() ;
        if( !( upper_block_sub == AIR || upper_block_sub == WATER ) || keys[ Qt::Key_X ] )
            new_cam_pos.z-= 1.0f;

        cam_pos= cam_pos * cam_lag + ( 1.0f - cam_lag ) * new_cam_pos;
        if( ( cam_pos - new_cam_pos ).Length() > 2.0f )
        {
            cam_pos = new_cam_pos + ( ( cam_pos - new_cam_pos ).Normalize() ) * 2.0f;
        }

        if( !( upper_block_sub == AIR || upper_block_sub == WATER ) || keys[ Qt::Key_X ] )
            cam_pos.z= min( float( player->Z() ) - 0.5f, cam_pos.z );
    }
    if( w->GetBlock( player->X(), player->Y(), player->Z()+  1 )->Sub() == WATER )
        renderer->SetUnderwaterMode( true );
    else
        renderer->SetUnderwaterMode( false );

    renderer->SetCamPosition( cam_pos );
    GetBuildCoord();
    renderer->SetBuildPosition( build_pos );

    QPoint cur_window_pos= gl_widget->mapFromGlobal( cursor.pos() );
    if( !use_mouse )
        renderer->SetCursorPos( float( cur_window_pos.x() ) / float( screen_width ) * 2.0f - 1.0f,
                                float( cur_window_pos.y() ) / float( screen_height ) * (-2.0f) + 1.0f );
    else
        renderer->SetCursorPos( -2.0f, -2.0f );

    last_time+= dt_i;
}


void Screen::sSaveScreenshot()
{
    current_screen->screenshot_thread_mutex.lock();

    char file_name[32];
    sprintf( file_name, "screenshots/screenshot%d.tga", current_screen->screenshot_number );
    if( !rSaveTextureTGA( file_name , current_screen->screenshot_data,
                          current_screen->screen_width, current_screen->screen_height ) )
    {
        QString str( "write screenshot: " );
        str+= QString( file_name );
        current_screen->Notify( str );
        printf( "write screenshot: %s\n", file_name );
    }
    else
    {
        printf( "errror, screenshot \"%s\" not saved\n", file_name );

    }
    current_screen->screenshot_number++;

}


void Screen::SaveScreenshot()
{
    if( need_save_screenshot )
    {
        need_save_screenshot= false;
        glReadPixels( 0, 0, screen_width, screen_height, GL_RGB, GL_UNSIGNED_BYTE, screenshot_data );
        screenshot_thread_mutex.unlock();//unlock screenshot saving thread
    }
}

#endif