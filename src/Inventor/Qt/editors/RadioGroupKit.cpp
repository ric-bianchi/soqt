

#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoTranslation.h> 
#include <Inventor/nodes/SoSeparator.h> 
#include <Inventor/nodes/SoTransformSeparator.h> 
#include <Inventor/nodes/SoMaterial.h> 
#include <Inventor/nodes/SoText2.h> 
#include <Inventor/nodes/SoBaseColor.h> 
#include <Inventor/nodes/SoSphere.h> 
#include <Inventor/nodes/SoSelection.h> 
#include <Inventor/nodes/SoShapeHints.h> 
#include <Inventor/nodes/SoSwitch.h> 
#include <Inventor/nodes/SoFont.h> 
#include <Inventor/nodes/SoEventCallback.h> 
#include <Inventor/events/SoEvent.h> 
#include <Inventor/events/SoMouseButtonEvent.h> 
#include <Inventor/nodes/SoTransformation.h> 
#include <Inventor/lists/SbPList.h> 
#include <Inventor/nodekits/SoShapeKit.h>
#include <Inventor/nodes/SoMarkerSet.h> 



#include "RadioGroupKit.h"

SO_KIT_SOURCE(RadioGroupKit);

class RadioGroupKitP {
public:
  RadioGroupKitP(RadioGroupKit * master) {
    this->master = master;
  }

  RadioGroupKit * master;
  int buttonCounter;
  SbPList *buttonList;
  SoSeparator *root;

  static void buttonClickedCallback(void *userData, SoPath *node);

};

#undef THIS
#define THIS this->pimpl

RadioGroupKit::RadioGroupKit(void)
{
  SO_KIT_CONSTRUCTOR(RadioGroupKit);
   
  THIS = new RadioGroupKitP(this);

  if (SO_KIT_IS_FIRST_INSTANCE()) {
    SoInput input;
    input.setBuffer((void *) RADIOBULLET_radiobulletgeometry, strlen(RADIOBULLET_radiobulletgeometry));
    SoDB::readAll(&input);
  }

  SO_KIT_ADD_CATALOG_ENTRY(radioGroupRoot,SoSeparator,TRUE,this, "",TRUE);

  SO_KIT_ADD_CATALOG_ENTRY(RadioBulletActive, SoTransformSeparator, TRUE, radioGroupRoot, , TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(RadioBullet, SoTransformSeparator, TRUE, radioGroupRoot, , TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(BulletColorActive, SoBaseColor, TRUE, radioGroupRoot, , TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(BulletColor, SoBaseColor, TRUE, radioGroupRoot, , TRUE);

  // FIXME: Are these needed? (handegar)
  //SO_KIT_ADD_FIELD(selected, -1);
  //SO_KIT_ADD_FIELD(labels, "");

  SO_KIT_INIT_INSTANCE();

  // initialize default parts
  this->setPartAsDefault("RadioBullet", "RadioBullet");
  this->setPartAsDefault("RadioBulletActive", "RadioBulletActive");
  this->setPartAsDefault("BulletColor", "BulletColor");
  this->setPartAsDefault("BulletColorActive", "BulletColorActive");

  THIS->buttonCounter = 0;
  labels = new SoMFString;
  THIS->buttonList = new SbPList(1);


  // Default Spacing between buttons
  buttonSpacingX = new SoTranslation;
  buttonSpacingX->translation.setValue(RADIO_BUTTON_SIZE*1.5,-RADIO_BUTTON_SIZE/2,0);
  buttonSpacingY = new SoTranslation;
  buttonSpacingY->translation.setValue(0,-RADIO_BUTTON_SIZE*2.5,0);

  THIS->root = new SoSeparator;
  setPart("radioGroupRoot",THIS->root);
  
}

RadioGroupKit::~RadioGroupKit()
{
}

void
RadioGroupKit::initClass(void)
{
  SO_KIT_INIT_CLASS(RadioGroupKit, SoInteractionKit, "InteractionKit");
}

SbBool 
RadioGroupKit::affectsState() const
{
  return(FALSE);
}

void 
RadioGroupKitP::buttonClickedCallback(void * userData, SoPath * node)
{

  paramPackage * pack = (paramPackage *) userData;
  RadioGroupKitP * radioButtonsP = (RadioGroupKitP *) pack->thisClass;  

  SoTransformSeparator * button = (SoTransformSeparator *) radioButtonsP->buttonList->get(pack->button);

  // Make all buttons unselected first
  for(int i=0;i<radioButtonsP->buttonList->getLength();++i){
    SoTransformSeparator *sep = (SoTransformSeparator *) radioButtonsP->buttonList->get(i);
    SoBaseColor *color = (SoBaseColor *) sep->getChild(0);
    SoBaseColor *bulletColor =  (SoBaseColor*) SO_GET_PART(radioButtonsP->master, "BulletColor",SoBaseColor);
    color->rgb = bulletColor->rgb;
  }
 
  // Highlight selected button
  SoBaseColor * color = (SoBaseColor *) button->getChild(0);
  SoBaseColor * bulletColor =  (SoBaseColor*) SO_GET_PART(radioButtonsP->master, "BulletColorActive",SoBaseColor);
  color->rgb = bulletColor->rgb;

  // Update selected field
  radioButtonsP->master->selected.setValue(pack->button);

}

void 
RadioGroupKit::addRadioButton(SbString label)
{

  // Expand label-array.
  labels->set1Value(THIS->buttonCounter,label);

  SoSeparator * buttonRoot = new SoSeparator;
  SoShapeHints * hints = new SoShapeHints;
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  hints->shapeType = SoShapeHints::SOLID;

  SoFont * font = new SoFont;
  font->name.setValue("Times-Roman");
  font->size.setValue(24.0);

  buttonRoot->addChild(hints);
  buttonRoot->addChild(font);

 
  SoText2 * buttonLabel = new SoText2;
  buttonLabel->string = label;
 
  SoSelection * selection = new SoSelection;

  paramPackage * pack = new paramPackage;
  pack->thisClass = THIS;
  pack->button = THIS->buttonCounter;
  selection->addSelectionCallback(&THIS->buttonClickedCallback, (void *)pack);
  SoTransformSeparator * sep;

  // Make first added button selected as default
  if(THIS->buttonCounter == 0){
    sep = (SoTransformSeparator*) SO_GET_PART(this, "RadioBulletActive",SoTransformSeparator)->copy();
    selected.setValue(0);
  } else {
    sep = (SoTransformSeparator*) SO_GET_PART(this, "RadioBullet",SoTransformSeparator)->copy();
  }

  THIS->buttonList->set(THIS->buttonCounter,sep);    // Save the separator node for later changes

  selection->addChild(sep);
  selection->addChild(buttonSpacingX);
  selection->addChild(buttonLabel);
  buttonRoot->addChild(selection);

  THIS->root->addChild(buttonRoot); 
  THIS->root->addChild(buttonSpacingY);

  ++THIS->buttonCounter; // Increase class-global counter

}
