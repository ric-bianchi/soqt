#ifndef SOQT_GRADIENTDIALOG_H
#define SOQT_GRADIENTDIALOG_H

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2003 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See <URL:http://www.coin3d.org> for more information.
 *
 *  Systems in Motion, Teknobyen, Abels Gate 5, 7030 Trondheim, NORWAY.
 *  <URL:http://www.sim.no>.
 *
\**************************************************************************/

#include <qdialog.h>
#include <Inventor/Qt/SoQtBasic.h>
#include <Inventor/Qt/widgets/Gradient.h>

class SOQT_DLL_API SoQtGradientDialog : public QDialog
{
  Q_OBJECT

public:
  SoQtGradientDialog(const Gradient & grad = Gradient(),
                     QWidget * parent=0,
                     bool modal = FALSE,
                     const char* name=0);

  ~SoQtGradientDialog();
  
  void addGradient(const Gradient & grad, QString description = "");
  void setDataLimits(float min, float max);
  const Gradient & getGradient(void) const;
  void setChangeCallback(changeCB * cb);

// FIXME: implement these slots in the 
// private implementation. frodo, 20030927
private slots:
  void loadGradient(void);
  void saveGradient(void);
  void chooseGradient(int i);

private:
  class SoQtGradientDialogP * pimpl;
};

#endif // SOQT_GRADIENTDIALOG_H
