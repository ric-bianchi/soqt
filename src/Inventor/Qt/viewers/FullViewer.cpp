/**************************************************************************\
 *
 *  This file is part of the Coin GUI toolkit libraries.
 *  Copyright (C) 1998-2002 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  version 2.1 as published by the Free Software Foundation.  See the
 *  file LICENSE.LGPL at the root directory of this source distribution
 *  for more details.
 *
 *  If you want to use this library with software that is incompatible
 *  licensewise with the LGPL, and / or you would like to take
 *  advantage of the additional benefits with regard to our support
 *  services, please contact Systems in Motion about acquiring a Coin
 *  Professional Edition License.  See <URL:http://www.coin3d.org> for
 *  more information.
 *
 *  Systems in Motion, Prof Brochs gate 6, 7030 Trondheim, NORWAY
 *  <URL:http://www.sim.no>, <mailto:support@sim.no>
 *
\**************************************************************************/

/*!
  \class SoQtFullViewer SoQtFullViewer.h Inventor/Qt/viewers/SoQtFullViewer.h
  \brief The SoQtFullViewer class adds decorations to the simple viewer of the parent.
  \ingroup soqtviewers

  FIXME: proper class documentation
  ...overview of what this class provides over parent class...
  ...keyboard combinations...mousebutton combinations...explain pan,
  rotate, zoom, dolly, seek...explain the fact that we're moving the
  camera, not the scene...
  ...viewer modes (examine vs interact) w/ explanation of what they are
  useful for...

  \sa SoQtConstrainedViewer, SoQtExaminerViewer, SoQtPlaneViewer
*/

// FIXME: this sourcefile is way too large -- this reeks of bad
// design. Should try to factor out lots of stuff. 20010821 mortene.

// FIXME: can hide more stuff from the public header file into
// SoQtFullViewerP -- most or all of the private methods, for
// instance. 20010821 mortene.

//    FIXME, low priority:
//     * add tooltips to the widgets?
//     * trim length of edit fields

#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qframe.h>
#include <qslider.h>
#include <qcheckbox.h>
#include <qmetaobject.h>
#include <moc_SoQtFullViewerP.cpp>

#include <SoQtFullViewerP.h>

#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/errors/SoDebugError.h>

#include <soqtdefs.h>
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/widgets/SoQtThumbWheel.h>
#include <Inventor/Qt/widgets/SoQtPopupMenu.h>
#include <Inventor/Qt/viewers/SoQtFullViewer.h>

// Button icons.
#include <Inventor/Qt/common/pixmaps/pick.xpm>
#include <Inventor/Qt/common/pixmaps/view.xpm>
#include <Inventor/Qt/common/pixmaps/home.xpm>
#include <Inventor/Qt/common/pixmaps/set_home.xpm>
#include <Inventor/Qt/common/pixmaps/view_all.xpm>
#include <Inventor/Qt/common/pixmaps/seek.xpm>

// *************************************************************************

SOQT_OBJECT_ABSTRACT_SOURCE(SoQtFullViewer);

// *************************************************************************

/*!
  \enum SoQtFullViewer::BuildFlag
  FIXME: write documentation for enum
*/
/*!
  \var SoQtFullViewer::BuildFlag SoQtFullViewer::BUILD_NONE
  FIXME: write documentation for enum definition
*/
/*!
  \var SoQtFullViewer::BuildFlag SoQtFullViewer::BUILD_DECORATION
  FIXME: write documentation for enum definition
*/
/*!
  \var SoQtFullViewer::BuildFlag SoQtFullViewer::BUILD_POPUP
  FIXME: write documentation for enum definition
*/
/*!
  \var SoQtFullViewer::BuildFlag SoQtFullViewer::BUILD_ALL
  FIXME: write documentation for enum definition
*/

/*!
  \var QWidget * SoQtFullViewer::leftDecoration
  FIXME: write doc
*/

/*!
  \var QWidget * SoQtFullViewer::leftWheel
  FIXME: write doc
*/

/*!
  \var QWidget * SoQtFullViewer::leftWheelLabel
  FIXME: write doc
*/

/*!
  \var char * SoQtFullViewer::leftWheelStr
  FIXME: write doc
*/

/*!
  \var float SoQtFullViewer::leftWheelVal
  FIXME: write doc
*/

/*!
  \var QWidget * SoQtFullViewer::bottomDecoration
  FIXME: write doc
*/

/*!
  \var QWidget * SoQtFullViewer::bottomWheel
  FIXME: write doc
*/

/*!
  \var QWidget * SoQtFullViewer::bottomWheelLabel
  FIXME: write doc
*/

/*!
  \var char * SoQtFullViewer::bottomWheelStr
  FIXME: write doc
*/

/*!
  \var float SoQtFullViewer::bottomWheelVal
  FIXME: write doc
*/

/*!
  \var QWidget * SoQtFullViewer::rightDecoration
  FIXME: write doc
*/

/*!
  \var QWidget * SoQtFullViewer::rightWheel
  FIXME: write doc
*/

/*!
  \var QWidget * SoQtFullViewer::rightWheelLabel
  FIXME: write doc
*/

/*!
  \var char * SoQtFullViewer::rightWheelStr
  FIXME: write doc
*/

/*!
  \var float SoQtFullViewer::rightWheelVal
  FIXME: write doc
*/

/*!
  \var SoQtPopupMenu * SoQtFullViewer::prefmenu
  FIXME: write doc
*/

// *************************************************************************

static const int VIEWERBORDER = 2;
static const int ZOOMSLIDERRESOLUTION = 200;


///////// FIXME start //////////////////////////////////////////////////
// Do something clever about this Qt layout assistant code..
// 990222 mortene.

enum LayoutOrientation { LayoutVertical, LayoutHorizontal };

void
expandSize(QSize & result, const QSize & addend, LayoutOrientation o)
{
  if (o == LayoutHorizontal) {
    result.rwidth() += addend.width();
    if (result.height() < addend.height()) result.setHeight(addend.height());
  }
  else {
    result.rheight() += addend.height();
    if (result.width() < addend.width()) result.setWidth(addend.width());
  }
}

///////// FIXME end ////////////////////////////////////////////////////


// *************************************************************************

// Button index values.
enum {
  INTERACT_BUTTON = 0,
  EXAMINE_BUTTON,
  HOME_BUTTON,
  SET_HOME_BUTTON,
  VIEW_ALL_BUTTON,
  SEEK_BUTTON
};

// *************************************************************************

#define PUBLIC(o) (o->pub)
#define PRIVATE(o) (o->pimpl)

// *************************************************************************

/*!
  Constructor. See parent class for explanation of arguments.

  Subclasses will probably usually call with the \a buildNow flag
  set to \c FALSE to be able to do delayed building of the OpenGL
  canvas after other setup tasks has been performed.
*/

SoQtFullViewer::SoQtFullViewer(QWidget * parent,
                               const char * name,
                               SbBool embed,
                               SoQtFullViewer::BuildFlag buildFlag,
                               SoQtViewer::Type type,
                               SbBool build)
  : inherited(parent, name, embed, type, FALSE)
{
  PRIVATE(this) = new SoQtFullViewerP(this);

  PRIVATE(this)->viewerwidget = NULL;
  PRIVATE(this)->canvas = NULL;
//  PRIVATE(this)->canvasparent = NULL;

  PRIVATE(this)->viewbutton = NULL;
  PRIVATE(this)->interactbutton = NULL;

  this->leftDecoration = NULL;
  this->bottomDecoration = NULL;
  this->rightDecoration = NULL;

  this->leftWheel = NULL;
  this->leftWheelLabel = NULL;
  this->leftWheelStr = NULL;
  this->leftWheelVal = 0.0f;

  this->bottomWheel = NULL;
  this->bottomWheelLabel = NULL;
  this->bottomWheelStr = NULL;
  this->bottomWheelVal = 0.0f;

  this->rightWheel = NULL;
  this->rightWheelLabel = NULL;
  this->rightWheelStr = NULL;
  this->rightWheelVal = 0.0f;

  this->setLeftWheelString("Motion X");
  this->setBottomWheelString("Motion Y");
  this->setRightWheelString("Motion Z");

  PRIVATE(this)->mainlayout = NULL;
  PRIVATE(this)->appbuttonlayout = NULL;

  PRIVATE(this)->menuenabled = buildFlag & SoQtFullViewer::BUILD_POPUP;
  PRIVATE(this)->decorations =
    (buildFlag & SoQtFullViewer::BUILD_DECORATION) ? TRUE : FALSE;

  this->prefmenu = NULL;
  PRIVATE(this)->menutitle = "Viewer Menu";

  PRIVATE(this)->viewerbuttons = new SbPList;
  PRIVATE(this)->appbuttonlist = new SbPList;
  PRIVATE(this)->appbuttonform = NULL;

  this->setSize(SbVec2s(500, 390));

  if (! build) return;

  this->setClassName("SoQtFullViewer");
  QWidget * viewer = this->buildWidget(this->getParentWidget());
  this->setBaseWidget(viewer);
}

// *************************************************************************

/*!
  Destructor.
*/

SoQtFullViewer::~SoQtFullViewer()
{
  delete PRIVATE(this)->viewerbuttons;
  delete PRIVATE(this)->appbuttonlist;

  delete [] this->leftWheelStr;
  delete [] this->rightWheelStr;
  delete [] this->bottomWheelStr;

  delete PRIVATE(this);
}

// *************************************************************************

/*!
  Turn the viewer decorations on or off.

  \sa isDecoration()
*/

void
SoQtFullViewer::setDecoration(const SbBool enable)
{
#if SOQT_DEBUG
  if ((enable  && this->isDecoration()) ||
       (!enable && !this->isDecoration())) {
    SoDebugError::postWarning("SoQtFullViewer::setDecoration",
                              "decorations already turned %s",
                              enable ? "on" : "off");
    return;
  }
#endif // SOQT_DEBUG

  PRIVATE(this)->decorations = enable;
  if (PRIVATE(this)->viewerwidget)
    PRIVATE(this)->showDecorationWidgets(enable);
}

// *************************************************************************

/*!
  Return \c TRUE if the viewer decorations are on, otherwise \c FALSE.

  \sa setDecoration()
*/

SbBool
SoQtFullViewer::isDecoration(void) const
{
  return PRIVATE(this)->decorations;
}

// *************************************************************************

/*!
  Decide whether or not if clicking with the right mouse button on
  the OpenGL canvas should reveal a preferences popup menu when in
  viewing mode.

  \sa isPopupMenuEnabled()
*/
void
SoQtFullViewer::setPopupMenuEnabled(const SbBool enable)
{
#if SOQT_DEBUG
  if ((enable && this->isPopupMenuEnabled()) ||
       (!enable && !this->isPopupMenuEnabled())) {
    SoDebugError::postWarning("SoQtFullViewer::setPopupMenuEnabled",
                              "popup menu already turned %s",
                              enable ? "on" : "off");
    return;
  }
#endif // SOQT_DEBUG
  PRIVATE(this)->menuenabled = enable;
}

// *************************************************************************

/*!
  Return \c TRUE if the popup preferences menu is enabled,
  otherwise \c FALSE.

  \sa setPopupMenuEnabled()
*/
SbBool
SoQtFullViewer::isPopupMenuEnabled(void) const
{
  return PRIVATE(this)->menuenabled;
}

// *************************************************************************

/*!
  Returns the widget which is used as the parent of application
  specified buttons. The row of application buttons (if any) will be
  placed in the upper left corner.

  \sa addAppPushButton(), insertAppPushButton(), removeAppPushButton()
*/

QWidget *
SoQtFullViewer::getAppPushButtonParent(void) const
{
  return PRIVATE(this)->appbuttonform;
}

// *************************************************************************

/*!
  Add an application specific push button to the viewer decorations.
  Application buttons will be laid out in a vertical row from the
  upper left corner.

  The button will be added bottom-most.

  \sa insertAppPushButton(), removeAppPushButton(), getAppPushButtonParent()
*/

void
SoQtFullViewer::addAppPushButton(QWidget * newButton)
{
  PRIVATE(this)->appbuttonlist->append(newButton);
  PRIVATE(this)->layoutAppButtons(this->getAppPushButtonParent());
}

// *************************************************************************

/*!
  Insert an application specific push button to the viewer decorations
  at the given \c index.

  \sa addAppPushButton(), removeAppPushButton(), getAppPushButtonParent()
*/

void
SoQtFullViewer::insertAppPushButton(QWidget * newButton, int index)
{
#if SOQT_DEBUG
  if ((index < 0) || (index > this->lengthAppPushButton())) {
    SoDebugError::postWarning("SoQtFullViewer::insertAppPushButton",
                              "index %d out of bounds", index);
    return;
  }
#endif // SOQT_DEBUG
  PRIVATE(this)->appbuttonlist->insert(newButton, index);
  PRIVATE(this)->layoutAppButtons(this->getAppPushButtonParent());
}

// *************************************************************************

/*!
  Remove one of the application specific buttons.

  \sa addAppPushButton(), insertAppPushButton()
*/

void
SoQtFullViewer::removeAppPushButton(QWidget * oldButton)
{
  int idx = PRIVATE(this)->appbuttonlist->find(oldButton);

#if SOQT_DEBUG
  if (idx == -1) {
    SoDebugError::postWarning("SoQtFullViewer::removeAppPushButton",
                              "tried to remove non-existant button");
    return;
  }
#endif // SOQT_DEBUG

  PRIVATE(this)->appbuttonlist->remove(idx);
  PRIVATE(this)->layoutAppButtons(this->getAppPushButtonParent());
}

// *************************************************************************

/*!
  Return the index of a particular button that has been specified by
  the application, or -1 of the button has not been added.

  \sa addAppPushButton()
*/

int
SoQtFullViewer::findAppPushButton(QWidget * oldButton) const
{
  return PRIVATE(this)->appbuttonlist->find(oldButton);
}

// *************************************************************************

/*!
  Return number of application specific buttons added.

  \sa addAppPushButton(), insertAddAppPushButton()
*/

int
SoQtFullViewer::lengthAppPushButton(void) const
{
  return PRIVATE(this)->appbuttonlist->getLength();
}

// *************************************************************************

/*!
  Returns the render area OpenGL canvas widget.
*/

QWidget *
SoQtFullViewer::getRenderAreaWidget(void) const
{
  return PRIVATE(this)->canvas;
}

// *************************************************************************

// Documented in superclass. Overridden from parent class to correctly
// set the user interface indicators on the current state, i.e. to
// flip the examine and interact pushbuttons and to grey out the seek
// mode activation button iff in interact mode.
void
SoQtFullViewer::setViewing(SbBool enable)
{
  if ((enable && this->isViewing()) ||
       (!enable && !this->isViewing())) {
#if SOQT_DEBUG && 0 // debug
    SoDebugError::postWarning("SoQtFullViewer::setViewing",
                              "view mode already %s", on ? "on" : "off");
#endif // debug
    return;
  }

  inherited::setViewing(enable);

  // Must check that buttons have been built, in case this viewer
  // component was made without decorations.
  if (PRIVATE(this)->viewerbuttons->getLength() > 0) {
    PRIVATE(this)->getViewerbutton(EXAMINE_BUTTON)->setOn(enable);
    PRIVATE(this)->getViewerbutton(INTERACT_BUTTON)->setOn(enable ? FALSE : TRUE);
    PRIVATE(this)->getViewerbutton(SEEK_BUTTON)->setEnabled(enable);
  }
}

// *************************************************************************

// Documented in superclass.
QWidget *
SoQtFullViewer::buildWidget(QWidget * parent)
{
  // This will build the main view widgets, along with the decorations
  // widgets and popup menu if they are enabled.

#if SOQT_DEBUG && 0
  SoDebugError::postInfo("SoQtFullViewer::buildWidget", "[invoked]");
#endif // SOQT_DEBUG
  PRIVATE(this)->viewerwidget = new QWidget(parent);
  this->registerWidget(PRIVATE(this)->viewerwidget);
//  PRIVATE(this)->viewerwidget->installEventFilter(this);

  PRIVATE(this)->viewerwidget->move(0, 0);
#if SOQT_DEBUG && 0
  PRIVATE(this)->viewerwidget->setBackgroundColor(QColor(250, 0, 0));
#endif // SOQT_DEBUG

  // Build and layout the widget components of the viewer window on
  // top of the manager widget.

//  PRIVATE(this)->canvasparent = new QWidget(PRIVATE(this)->viewerwidget);
//  PRIVATE(this)->canvasparent->move(0, 0);

  PRIVATE(this)->canvas = inherited::buildWidget(PRIVATE(this)->viewerwidget);
  if (PRIVATE(this)->decorations) {
    PRIVATE(this)->canvas->move(30, 0);
    PRIVATE(this)->canvas->resize(
      QSize(PRIVATE(this)->viewerwidget->size().width() - 60,
             PRIVATE(this)->viewerwidget->size().height() - 30));
  } else {
    PRIVATE(this)->canvas->move(0, 0);
    PRIVATE(this)->canvas->resize(PRIVATE(this)->viewerwidget->size());
  }

  if (PRIVATE(this)->decorations) {
    this->buildDecoration(PRIVATE(this)->viewerwidget);
    PRIVATE(this)->showDecorationWidgets(TRUE);
  }

  if (PRIVATE(this)->menuenabled)
    this->buildPopupMenu();

  return PRIVATE(this)->viewerwidget;
}

// *************************************************************************

/*!
  Build viewer decorations.
*/

void
SoQtFullViewer::buildDecoration(QWidget * parent)
{
  this->leftDecoration = this->buildLeftTrim(parent);
  this->bottomDecoration = this->buildBottomTrim(parent);
  this->rightDecoration = this->buildRightTrim(parent);
}

// *************************************************************************

/*!
  Build decorations on the left of the render canvas.  Override this
  method in subclasses if you want your own decorations on the viewer
  window.

  The decoration will be 30 pixels wide.
*/

QWidget *
SoQtFullViewer::buildLeftTrim(QWidget * parent)
{
  QWidget * w = new QWidget(parent);
  w->setFixedWidth(30);

  QGridLayout * gl = new QGridLayout(w, 3, 1, 2, -1);
  gl->addWidget(this->buildAppButtons(w), 0, 0);

  SoQtThumbWheel * t = new SoQtThumbWheel(SoQtThumbWheel::Vertical, w);
  this->leftWheel = t;
  t->setRangeBoundaryHandling(SoQtThumbWheel::ACCUMULATE);
  this->leftWheelVal = t->value();

  QObject::connect(t, SIGNAL(wheelMoved(float)), PRIVATE(this), SLOT(leftWheelChanged(float)));
  QObject::connect(t, SIGNAL(wheelPressed()), PRIVATE(this), SLOT(leftWheelPressed()));
  QObject::connect(t, SIGNAL(wheelReleased()), PRIVATE(this), SLOT(leftWheelReleased()));

  gl->addWidget(t, 2, 0, AlignBottom | AlignHCenter);
  gl->activate();

  return w;
}

// *************************************************************************

/*!
  Build decorations on the bottom of the render canvas. Override this
  method in subclasses if you want your own decorations on the viewer window.
*/

QWidget *
SoQtFullViewer::buildBottomTrim(QWidget * parent)
{
  QWidget * w = new QWidget(parent);
  w->setFixedHeight(30);

  QLabel * label = new QLabel(this->leftWheelStr, w);
  label->adjustSize();
  label->setAlignment(AlignLeft | AlignTop);
  label->setMargin(2);
  this->leftWheelLabel = label;

  label = new QLabel(this->bottomWheelStr, w);
  label->adjustSize();
  label->setAlignment(AlignRight | AlignVCenter);
  label->setMargin(2);
  this->bottomWheelLabel = label;

  label = new QLabel(this->rightWheelStr, w);
  label->adjustSize();
  label->setAlignment(AlignRight | AlignTop);
  label->setMargin(2);
  this->rightWheelLabel = label;

  SoQtThumbWheel * t = new SoQtThumbWheel(SoQtThumbWheel::Horizontal, w);
  this->bottomWheel = t;
  t->setRangeBoundaryHandling(SoQtThumbWheel::ACCUMULATE);

  QObject::connect(t, SIGNAL(wheelMoved(float)),
                   PRIVATE(this), SLOT(bottomWheelChanged(float)));
  QObject::connect(t, SIGNAL(wheelPressed()),
                   PRIVATE(this), SLOT(bottomWheelPressed()));
  QObject::connect(t, SIGNAL(wheelReleased()),
                   PRIVATE(this), SLOT(bottomWheelReleased()));

  this->bottomWheelVal = t->value();

  QGridLayout * gl = new QGridLayout(w, 1, 5);
  gl->setColStretch(0, 0);
  gl->setColStretch(1, 0);
  gl->setColStretch(2, 0);
  gl->setColStretch(3, 1);
  gl->setColStretch(4, 0);

  gl->addWidget(this->leftWheelLabel, 0, 0, AlignVCenter | AlignHCenter);
  gl->addWidget(this->bottomWheelLabel, 0, 1, AlignVCenter | AlignRight);
  gl->addWidget(t, 0, 2, AlignVCenter | AlignLeft);
  gl->addWidget(this->rightWheelLabel, 0, 4, AlignVCenter | AlignRight);

  gl->activate();

  return w;
}

// *************************************************************************

/*!
  Build decorations on the right side of the render canvas. Override this
  method in subclasses if you want your own decorations on the viewer window.
*/

QWidget *
SoQtFullViewer::buildRightTrim(QWidget * parent)
{
  QWidget * w = new QWidget(parent);
  // FIXME: should be set according to width of viewer
  // buttons. 20000424 mortene.
  w->setFixedWidth(30);
  // FIXME: nope, trims are actually guaranteed to be 30 pixels wide

  SoQtThumbWheel * t = new SoQtThumbWheel(SoQtThumbWheel::Vertical, w);
  this->rightWheel = t;
  t->setRangeBoundaryHandling(SoQtThumbWheel::ACCUMULATE);
  this->rightWheelVal = t->value();

  QObject::connect(t, SIGNAL(wheelMoved(float)),
                   PRIVATE(this), SLOT(rightWheelChanged(float)));
  QObject::connect(t, SIGNAL(wheelPressed()),
                   PRIVATE(this), SLOT(rightWheelPressed()));
  QObject::connect(t, SIGNAL(wheelReleased()),
                   PRIVATE(this), SLOT(rightWheelReleased()));

  QGridLayout * l = new QGridLayout(w, 3, 1, 2, -1);
  l->setMargin(0);
  l->addWidget(this->buildViewerButtons(w), 0, 0);
  l->addWidget(t, 2, 0, AlignBottom|AlignHCenter);
  l->activate();

  return w;
}

// *************************************************************************

/*!
  Build the application specified button row (if any buttons were
  set up).
*/

QWidget *
SoQtFullViewer::buildAppButtons(QWidget * parent)
{
  PRIVATE(this)->appbuttonform = new QWidget(parent);

  if (this->lengthAppPushButton() > 0)
    PRIVATE(this)->layoutAppButtons(PRIVATE(this)->appbuttonform);

  return PRIVATE(this)->appbuttonform;
}

// *************************************************************************

/*!
  Build and layout viewer specified button row.
*/

QWidget *
SoQtFullViewer::buildViewerButtons(QWidget * parent)
{
  QWidget * w = new QWidget(parent);
  this->createViewerButtons(w, PRIVATE(this)->viewerbuttons);
  w->move(0, 0);

  // assert(PRIVATE(this)->viewerbuttons->getLength() != 0);
  QGridLayout * l =
    new QGridLayout(w, PRIVATE(this)->viewerbuttons->getLength(), 1);
  l->setMargin(0);
  l->setSpacing(0);

  const int numViewerButtons = PRIVATE(this)->viewerbuttons->getLength();
  for (int i = 0; i < numViewerButtons; i++) {
    QButton * b = PRIVATE(this)->getViewerbutton(i);
    b->setFixedSize(30, 30);
    b->setFocusPolicy(QWidget::NoFocus);
    l->addWidget(b, i, 0);
  }

  l->activate();
  return w;
}

// *************************************************************************

// Documented in common/viewers/SoGuiFullViewer.cpp.in.
void
SoQtFullViewer::createViewerButtons(QWidget * parent, SbPList * buttonlist)
{
  for (int i=0; i <= SEEK_BUTTON; i++) {
    QPushButton * p = new QPushButton(parent);
    // Button focus doesn't really make sense in the way we're using
    // the pushbuttons.
    p->setFocusPolicy(QWidget::NoFocus);

    // FIXME: its wrong to cast xpm data to (const char **), it
    // _should_ be (const char * const []), but this is not valid ANSI
    // C++. Not sure what to do about this (and the wrong cast can
    // (and has) in fact lead to bugs, so the situation should be
    // resolved). 20000505 mortene.

    switch (i) {
    case INTERACT_BUTTON:
      PRIVATE(this)->interactbutton = p;
      p->setToggleButton(TRUE);
      p->setPixmap(QPixmap((const char **)pick_xpm));
      p->setOn(this->isViewing() ? FALSE : TRUE);
      QObject::connect(p, SIGNAL(clicked()),
                       PRIVATE(this), SLOT(interactbuttonClicked()));
      break;
    case EXAMINE_BUTTON:
      PRIVATE(this)->viewbutton = p;
      p->setToggleButton(TRUE);
      p->setPixmap(QPixmap((const char **)view_xpm));
      p->setOn(this->isViewing());
      QObject::connect(p, SIGNAL(clicked()),
                       PRIVATE(this), SLOT(viewbuttonClicked()));
      break;
    case HOME_BUTTON:
      QObject::connect(p, SIGNAL(clicked()), PRIVATE(this), SLOT(homebuttonClicked()));
      p->setPixmap(QPixmap((const char **)home_xpm));
      break;
    case SET_HOME_BUTTON:
      QObject::connect(p, SIGNAL(clicked()),
                       PRIVATE(this), SLOT(sethomebuttonClicked()));
      p->setPixmap(QPixmap((const char **)set_home_xpm));
      break;
    case VIEW_ALL_BUTTON:
      QObject::connect(p, SIGNAL(clicked()),
                       PRIVATE(this), SLOT(viewallbuttonClicked()));
      p->setPixmap(QPixmap((const char **)view_all_xpm));
      break;
    case SEEK_BUTTON:
      QObject::connect(p, SIGNAL(clicked()), PRIVATE(this), SLOT(seekbuttonClicked()));
      p->setPixmap(QPixmap((const char **)seek_xpm));
      break;
    default:
      assert(0);
      break;
    }

    p->adjustSize();
    buttonlist->append(p);
  }
}

// *************************************************************************

/*!
  Make a popup menu with preferences settings.
*/

void
SoQtFullViewer::buildPopupMenu(void)
{
  this->prefmenu = PRIVATE(this)->setupStandardPopupMenu();
}

// *************************************************************************

/*!
  NOTE: This method is not part of the original InventorXt API.
*/

void
SoQtFullViewer::openPopupMenu(const SbVec2s position)
{
  if (! this->isPopupMenuEnabled()) return;
  if (this->prefmenu == NULL)
    this->buildPopupMenu();
  int x = 2 + position[0];
  int y = 2 + this->getGLSize()[1] - position[1] - 1;

  PRIVATE(this)->prepareMenu(this->prefmenu);
  this->prefmenu->popUp(this->getGLWidget(), x, y);
}

// *************************************************************************

/*!
  Set label of the left thumbwheel.
*/

void
SoQtFullViewer::setLeftWheelString(const char * const string)
{
  delete [] this->leftWheelStr;
  this->leftWheelStr = NULL;

  if (string)
    this->leftWheelStr = strcpy(new char [strlen(string)+1], string);
  if (this->leftWheelLabel)
    ((QLabel *)this->leftWheelLabel)->setText(string ? string : "");
}

// *************************************************************************

/*!
  Set label of the bottom thumbwheel.
*/

void
SoQtFullViewer::setBottomWheelString(const char * const string)
{
  delete [] this->bottomWheelStr;
  this->bottomWheelStr = NULL;

  if (string)
    this->bottomWheelStr = strcpy(new char [strlen(string)+1], string);
  if (this->bottomWheelLabel)
    ((QLabel *)this->bottomWheelLabel)->setText(string ? string : "");
}

// *************************************************************************

/*!
  Set label of the right thumbwheel.
*/

void
SoQtFullViewer::setRightWheelString(const char * const string)
{
  delete [] this->rightWheelStr;
  this->rightWheelStr = NULL;

  if (string)
    this->rightWheelStr = strcpy(new char [strlen(string)+1], string);
  if (this->rightWheelLabel)
    ((QLabel *)this->rightWheelLabel)->setText(string ? string : "");
}

// *************************************************************************

// doc in super
void
SoQtFullViewer::sizeChanged(const SbVec2s & size)
{
#if SOQT_DEBUG && 0
  SoDebugError::postInfo("SoQtFullViewer::sizeChanged", "[invoked (%d, %d)]",
                         size[0], size[1]);
#endif // SOQT_DEBUG

  if (PRIVATE(this)->decorations) {
    if (size[0] <= 60 || size[1] <= 30) return; // bogus
    if (PRIVATE(this)->viewerwidget) {
      // SoDebugError::postInfo("SoQtFullViewer::sizeChanged", "[resizing]");
      PRIVATE(this)->viewerwidget->setGeometry(0, 0, size[0], size[1]);
      PRIVATE(this)->canvas->setGeometry(30, 0, size[0] - 60, size[1] - 30);
      this->leftDecoration->resize(30, size[1] - 30);
      this->rightDecoration->setGeometry(size[0]-30, 0, size[0], size[1] - 30);
      this->bottomDecoration->setGeometry(0, size[1]-30, size[0], size[1]);
    }
    const SbVec2s rasize = SbVec2s(size[0] - 60, size[1] - 30);
    inherited::sizeChanged(rasize);
  } else {
    if (size[0] <= 0 || size[1] <= 0) return;
    if (PRIVATE(this)->viewerwidget && PRIVATE(this)->canvas) {
      // SoDebugError::postInfo("SoQtFullViewer::sizeChanged", "[resizing]");
      PRIVATE(this)->viewerwidget->setGeometry(0, 0, size[0], size[1]);
      PRIVATE(this)->canvas->setGeometry(0, 0, size[0], size[1]);
    }
    inherited::sizeChanged(size);
  }
}

// *************************************************************************

#ifndef DOXYGEN_SKIP_THIS

// Show or hide decorations. Will make and activate a Qt layout grid
// if we're turning the decorations on.
void
SoQtFullViewerP::showDecorationWidgets(SbBool onOff)
{
#if SOQT_DEBUG && 0
  SoDebugError::postInfo("SoQtFullViewerP::showDecorationWidgets", "[invoked]");
#endif // SOQT_DEBUG

  if (this->mainlayout) delete this->mainlayout;

  assert(this->viewerwidget);
//  assert(this->canvasparent);

  assert(PUBLIC(this)->leftDecoration && PUBLIC(this)->bottomDecoration && PUBLIC(this)->rightDecoration);
  if (onOff) {
    PUBLIC(this)->leftDecoration->show();
    PUBLIC(this)->bottomDecoration->show();
    PUBLIC(this)->rightDecoration->show();

    QGridLayout * g = new QGridLayout(this->viewerwidget, 2, 1, 0, -1); // VIEWERBORDER);

    g->addWidget(PUBLIC(this)->bottomDecoration, 1, 0);

    QGridLayout * subLayout = new QGridLayout(1, 3, 0);
    g->addLayout(subLayout, 0, 0);

    subLayout->addWidget(PUBLIC(this)->leftDecoration, 0, 0);
    subLayout->addWidget(this->canvas, 0, 1);
    subLayout->addWidget(PUBLIC(this)->rightDecoration, 0, 2);

//     subLayout->setColStretch(1, 1);
//     g->setRowStretch(0, 1);

    this->mainlayout = g;
  } else {
    QGridLayout * g = new QGridLayout(this->viewerwidget, 1, 1, 0, -1);
    g->addWidget(this->canvas, 0, 0);
    this->mainlayout = g;

    PUBLIC(this)->leftDecoration->hide();
    PUBLIC(this)->bottomDecoration->hide();
    PUBLIC(this)->rightDecoration->hide();
  }

  this->mainlayout->activate();
  QSize size = this->viewerwidget->size();
  SbVec2s rasize = SbVec2s(size.width(), size.height());
  PUBLIC(this)->sizeChanged(rasize);
}

// *************************************************************************

// Layout application specified buttons.
void
SoQtFullViewerP::layoutAppButtons(QWidget * form)
{
  delete this->appbuttonlayout; this->appbuttonlayout = NULL;

  int nrbuttons = this->appbuttonlist->getLength();
  if (nrbuttons == 0) return;

  this->appbuttonlayout = new QGridLayout(form, nrbuttons, 1);

  for (int i=0; i < nrbuttons; i++) {
    QWidget * button = (QWidget *)((*(this->appbuttonlist))[i]);
    button->show();
    ((QGridLayout *)this->appbuttonlayout)->addWidget(button, i, 0);
  }

  this->appbuttonlayout->activate();
}

// *************************************************************************

// Qt slot.
void
SoQtFullViewerP::interactbuttonClicked(void)
{
  if (this->interactbutton)
    ((QPushButton *)this->interactbutton)->setOn(TRUE);
  if (this->viewbutton)
    ((QPushButton *)this->viewbutton)->setOn(FALSE);
  if (PUBLIC(this)->isViewing())
    PUBLIC(this)->setViewing(FALSE);
}

// *************************************************************************

// Qt slot.
void
SoQtFullViewerP::viewbuttonClicked(void)
{
  if (this->interactbutton)
    ((QPushButton *)this->interactbutton)->setOn(FALSE);
  if (this->viewbutton)
    ((QPushButton *)this->viewbutton)->setOn(TRUE);
  if (!PUBLIC(this)->isViewing())
    PUBLIC(this)->setViewing(TRUE);
}

// *************************************************************************

// Qt slot.
void
SoQtFullViewerP::homebuttonClicked()
{
  PUBLIC(this)->resetToHomePosition();
}

// *************************************************************************

// Qt slot.
void
SoQtFullViewerP::sethomebuttonClicked()
{
  PUBLIC(this)->saveHomePosition();
}

// *************************************************************************

// Qt slot.
void
SoQtFullViewerP::viewallbuttonClicked()
{
  PUBLIC(this)->viewAll();
}

// *************************************************************************

// Qt slot.
void
SoQtFullViewerP::seekbuttonClicked()
{
  PUBLIC(this)->setSeekMode(PUBLIC(this)->isSeekMode() ? FALSE : TRUE);
}

// *************************************************************************

// Qt slot.
void
SoQtFullViewerP::selectedViewing()
{
  PUBLIC(this)->setViewing(PUBLIC(this)->isViewing() ? FALSE : TRUE);
}

// *************************************************************************

// Qt slot.
void
SoQtFullViewerP::selectedDecoration()
{
  PUBLIC(this)->setDecoration(PUBLIC(this)->isDecoration() ? FALSE : TRUE);
}

// *************************************************************************

// Qt slot.
void
SoQtFullViewerP::selectedHeadlight()
{
  PUBLIC(this)->setHeadlight(PUBLIC(this)->isHeadlight() ? FALSE : TRUE);
}

// *************************************************************************

// Qt slot.
void
SoQtFullViewerP::increaseInteractiveCount()
{
  PUBLIC(this)->interactiveCountInc();
}

// *************************************************************************

// Qt slot.
void
SoQtFullViewerP::decreaseInteractiveCount()
{
  PUBLIC(this)->interactiveCountDec();
}

// *************************************************************************

SoQtFullViewerP::SoQtFullViewerP(SoQtFullViewer * publ)
  : SoGuiFullViewerP(publ)
{
}

SoQtFullViewerP::~SoQtFullViewerP()
{
}

void
SoQtFullViewerP::setThumbWheelValue(void * wheel, float val)
{
  ((SoQtThumbWheel *)wheel)->setValue(val);
}

// *************************************************************************

// These are all private slots for catching Qt events.
void SoQtFullViewerP::leftWheelPressed(void) { PUBLIC(this)->leftWheelStart(); }
void SoQtFullViewerP::leftWheelChanged(float v) { PUBLIC(this)->leftWheelMotion(v); }
void SoQtFullViewerP::leftWheelReleased(void) { PUBLIC(this)->leftWheelFinish(); }
void SoQtFullViewerP::bottomWheelPressed(void) { PUBLIC(this)->bottomWheelStart(); }
void SoQtFullViewerP::bottomWheelChanged(float v) { PUBLIC(this)->bottomWheelMotion(v);}
void SoQtFullViewerP::bottomWheelReleased(void) { PUBLIC(this)->bottomWheelFinish(); }
void SoQtFullViewerP::rightWheelPressed(void) { PUBLIC(this)->rightWheelStart(); }
void SoQtFullViewerP::rightWheelChanged(float v) { PUBLIC(this)->rightWheelMotion(v); }
void SoQtFullViewerP::rightWheelReleased(void) { PUBLIC(this)->rightWheelFinish(); }

// *************************************************************************

#endif // DOXYGEN_SKIP_THIS
