#include "GaudiUPI/DisplayFactory.h"
#include "GaudiUPI/DialogDisplayFactory.h"
#include "GaudiUPI/AbstractDisplay.h"
#include "GaudiUPI/UpiDialogMenu.h"
#include <string>

static DialogDisplayFactory* fac = 0;

DisplayFactory::DisplayFactory (const std::string& type) {
  if ( AbstractDisplayFactory::instance() == 0 ) {
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

DialogDisplayFactory* DisplayFactory_factory() {
  return fac;
}

DialogDisplayFactory::DialogDisplayFactory() {
}

DialogDisplayFactory::~DialogDisplayFactory() {
}

