/***************************************************************************
 *   Copyright (C) 2009 by Mononofu   *
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
#ifndef DESTROYER_H
#define DESTROYER_H

template <class DOOMED>
class Destroyer
{
	public:
		Destroyer ();
		~Destroyer();

		void SetDoomed ( DOOMED* );
	private:
		// Prevent users from making copies of a
		// Destroyer to avoid double deletion:
		Destroyer ( const Destroyer<DOOMED>& );
		void operator= ( const Destroyer<DOOMED>& );
	private:
		DOOMED* _doomed;
};

template <class DOOMED>
		Destroyer<DOOMED>::Destroyer () : _doomed ( 0 )
{
}

template <class DOOMED>
		Destroyer<DOOMED>::~Destroyer ()
{
	delete _doomed;
}

template <class DOOMED>
		void Destroyer<DOOMED>::SetDoomed ( DOOMED* d )
{
	_doomed = d;
}

#endif
