/**************************************************************************\
 *
 *  This file is part of a set of demo programs for the SoQt library.
 *  Copyright (C) 1998-2003 by Systems in Motion.  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using this code with software that can not be combined with
 *  the GNU GPL, and for taking advantage of the additional benefits
 *  of our support services, please contact Systems in Motion about
 *  acquiring a Coin Professional Edition License.
 *
 *  See <URL:http://www.coin3d.org> for more information.
 *
 *  Systems in Motion, Teknobyen, Abels Gate 5, 7030 Trondheim, NORWAY.
 *  <URL:http://www.sim.no>.
 *
\**************************************************************************/

/*
  This is just a simple test application to try out the functionality
  of the SoQtThumbWheel widget.

  FIXME: this doesn't work at the moment (the SoQtThumbWheel API is
  not exported in the SoQt library). 20001125 mortene.
*/

#include <qapplication.h>
#include <Inventor/Qt/widgets/SoQtThumbWheel.h>

int
main( int argc, char **argv )
{
  QApplication a(argc,argv);

  SoQtThumbWheel wheel_h(SoQtThumbWheel::Horizontal);
  wheel_h.resize(200, 30);

  SoQtThumbWheel wheel_v(SoQtThumbWheel::Vertical);
  wheel_v.resize(30, 200);

  a.setMainWidget(&wheel_h);
  wheel_h.show();
  wheel_v.show();
  return a.exec();
}
