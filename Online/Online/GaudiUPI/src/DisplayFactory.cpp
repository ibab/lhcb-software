#include "Gaudi/DisplayFactory.h"
#include "Gaudi/DialogDisplayFactory.h"
#include "Gaudi/AbstractDisplay.h"
#include "Gaudi/UpiDialogMenu.h"
#include <string>

DisplayFactory::DisplayFactory (const std::string& type) {
  if ( AbstractDisplayFactory::instance() == 0 ) {
    AbstractDisplayFactory* fac = 0;
    if ( type.substr(0,2) == "UP" ) {
      UpiFactory::instance();
      fac = new DialogDisplayFactory();
    }
    else {
      printf("You suitable dialogfactory found.... better exit\n");
      exit(1);
    }
  }
}
static DialogDisplayFactory* s_instance = 0;

AbstractDisplayFactory* DisplayFactory::factory() {
  return AbstractDisplayFactory::instance();
}

DialogDisplayFactory::DialogDisplayFactory() {
  s_instance = this;
}
DialogDisplayFactory::~DialogDisplayFactory() {
  s_instance = 0;
}

