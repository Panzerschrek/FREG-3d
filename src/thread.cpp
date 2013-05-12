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

#include "thread.h"
#include "header.h"
#include "world.h"
#include <QThread>

WorldThread::WorldThread(World * world) : w(world), stopped(false) {}

void WorldThread::run() {
	while ( !stopped ) {
		if ( NULL!=w )
			w->PhysEvents();
		msleep(1000/ w->TimeStepsInSec());
	}
}

void WorldThread::Stop() {
	stopped=true;
}


r_UniversalThread::r_UniversalThread( void (*f)(), void* this_p, unsigned int in_loop )
{
    this_pointer= this_p;
    func_pointer= f;
    loop_call= in_loop;
}


void r_UniversalThread::run()
{
    do
    {
        if( this_pointer == NULL )
            func_pointer();
        else
        {
           //сдесь предполагаются извращения с вызовом
           //метода неизвестного класса
        }
    }while(loop_call );
}

void r_UniversalThread::Stop()
{
    loop_call= 0;
}
