/***************************************************************************
 *   Copyright (C) 2008 by Mononofu   *
 *   mononofu@mononofu-laptop   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <taskengine/taskengine.h>
#include <boost/shared_ptr.hpp>


class GraphicsImpl;

class Graphics : public Task, public DataProvider
{
	public:
		Graphics();
		~Graphics() { }
		bool doStep();
		void threadWillStart();
		void threadWillStop();
		DataContainer getData(const DataIdentifier& id);
	private:
		boost::shared_ptr<GraphicsImpl> impl;
};

#endif

