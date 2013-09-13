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

#ifndef SCREEN_H
#define SCREEN_H

#include "header.h"
#include <QObject>
#include <QVBoxLayout>
#include <QEvent.h>
#include "VirtScreen.h"
#include "renderer/renderer.h"

#define CONSOLE_SCREEN 0 //panzerschrek
#include "freg_lib/vec.h"


#ifdef OGL21
#define FREG_GL_VERSION_MAJOR 2
#define FREG_GL_VERSION_MINOR 1
#else
#define FREG_GL_VERSION_MAJOR 3
#define FREG_GL_VERSION_MINOR 3
#endif

//Panzerschrk -  key scan codes
#define FREG_KEY_UP             ( ( Qt::Key_Up & 0xff ) | ( Qt::Key_Up >> 16 ) )
#define FREG_KEY_DOWN           ( ( Qt::Key_Down & 0xff ) | ( Qt::Key_Down >> 16 ) )
#define FREG_KEY_LEFT           ( ( Qt::Key_Left & 0xff ) | ( Qt::Key_Left >> 16 ) )
#define FREG_KEY_RIGHT          ( ( Qt::Key_Right & 0xff ) | ( Qt::Key_Right >> 16 ) )
#define FREG_KEY_FORWARD        Qt::Key_W
#define FREG_KEY_BACKWARD       Qt::Key_S
#define FREG_KEY_STRAFE_LEFT    Qt::Key_A
#define FREG_KEY_STRAFE_RIGHT   Qt::Key_D
#define FREG_KEY_JUMP           Qt::Key_Space
#define FREG_KEY_CROUCH         Qt::Key_C

#ifndef FREG_CONSOLE_BUFFER_LEN
#define FREG_CONSOLE_BUFFER_LEN 256
#define FREG_CONSOLE_MEMORY_LINES 4
#endif//aslo defined in renderer.h

class World;
class Player;
class Block;
class Inventory;
class QString;
class QTimer;

class Screen;
class FREGGLWidget : public QGLWidget
{
    Q_OBJECT

public:
    FREGGLWidget(QWidget *parent=0);
    FREGGLWidget( Screen* s, r_Renderer* screen_renderer, QGLFormat format, QWidget* parent );
    ~FREGGLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

protected:
    void initializeGL();
    void resizeGL(int, int);
    void paintGL();
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void keyPressEvent(QKeyEvent* e);
    void focusOutEvent( QFocusEvent *);
    void focusInEvent(QFocusEvent *);
    void keyReleaseEvent(QKeyEvent* e);
    void closeEvent(QCloseEvent* e);

public slots:
    void setXRotation(int) {}
    void setYRotation(int) {}
    void setZRotation(int) {}

private:
    void draw();

    r_Renderer* renderer;
    Screen* screen;

    QColor qtPurple;
};


class Screen : public VirtScreen
{
    Q_OBJECT

    bool updated;
    bool cleaned;


private slots:
    void Print();

public slots:
    void PlayerDestroyed();
	void PlayerMoved( long x, long y, ushort z );
    void Notify(const QString&) const;
    void CleanAll();
    QString PassString(QString &) const;
    void Update(
        const unsigned short,
        const unsigned short,
        const unsigned short);
    void UpdateAll();
    void Move(const int dir)
    {
        renderer->MoveMap(dir);
    }
    void UpdatePlayer();
    void UpdateAround() {}
    void UpdateAround(
        const ushort,
        const ushort,
        const ushort,
        const ushort range);
    void Flushinp()
    {
        /*flushinp();*/
    }
    void RePrint()
    {

    }
    void UpdatesEnd(){}

signals:
    void ExitReceived();
    void InputReceived(int, int) const;
public:

    Screen(World * const, Player * const);
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);
    void keyPressEvent( QKeyEvent* e );
    void keyReleaseEvent( QKeyEvent* e );
    void closeEvent(QCloseEvent* e);
    void focusOutEvent( QFocusEvent *e );
    void focusInEvent(QFocusEvent * e);
    ~Screen();

private:
    r_Renderer* renderer;


    void InventoryFunc(QMouseEvent * e);
    void BlockMenuFunc(QMouseEvent * e);

private:
    QTime startup_time;
    int screen_width, screen_height;
    QVBoxLayout *layout;
    FREGGLWidget* gl_widget;
    QWidget* window;
    QCursor cursor;

    bool keys[ 512 ];//для хранения состояния клавиш
    bool shift_pressed;
    bool use_mouse, is_focus;
    bool console_opened;
    unsigned int current_console_line;
    char console_buffer[ FREG_CONSOLE_MEMORY_LINES + 1 ][ FREG_CONSOLE_BUFFER_LEN ];
    unsigned int console_buffer_pos;

    void GetBuildCoord();

public:
    void InputTick();
private:

    m_Vec3 cam_pos, cam_ang, build_pos, player_global_pos, using_block_pos;
    short build_x, build_y, build_z;
    short using_block_x, using_block_y, using_block_z;
    bool free_look;
    bool player_invalid;
    float cam_lag;


    static Screen* current_screen;
    r_UniversalThread screenshot_thread;
    QMutex screenshot_thread_mutex;
    bool need_save_screenshot;
    unsigned char* screenshot_data;
    unsigned int screenshot_number;
    static void sSaveScreenshot();
public:
    void SaveScreenshot();

    private:

    int movable_inv_slot_number;
    bool inventory_drag;
    bool movable_in_player_inventory;
    int active_hand;


    void LoadBlockList();
    unsigned short block_list[64][2];
    bool block_list_opened;

    public:
     static char block_names[64][32];
     static unsigned int block_list_size;

};

/*
inline void Screen::sInputTick()
{
    current_screen->InputTick();
}
*/
#endif
