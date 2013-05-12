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

#ifndef THREAD_H
#define THREAD_H

#include <QThread>

class World;

class WorldThread : public QThread {
	Q_OBJECT

	public:
		WorldThread(World *);
		void Stop();

	protected:
		void run();

	private:
		World * w;
		volatile bool stopped;
};


class r_UniversalThread : public QThread {
	Q_OBJECT

	public:
		r_UniversalThread( void (*f)(), void* this_p= NULL, unsigned int in_loop = false);
		void Stop();

	protected:
		void run();

	private:
		void (*func_pointer)();
		void* this_pointer;
		unsigned int loop_call;
		volatile bool stopped;
};
#endif
