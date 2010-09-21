#include "GaudiUPI/DisplayFactory.h"
#include "GaudiUPI/DialogDisplayFactory.h"
#include "GaudiUPI/AbstractDisplay.h"
#include "GaudiUPI/UpiDialogMenu.h"
#include <string>

DisplayFactory::DisplayFactory (const std::string& type) {
  if ( AbstractDisplayFactory::instance() == 0 ) {
    static AbstractDisplayFactory* fac = 0;
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

AbstractDisplayFactory* DisplayFactory::factory() {
  return AbstractDisplayFactory::instance();
}

DialogDisplayFactory::DialogDisplayFactory() {
}

DialogDisplayFactory::~DialogDisplayFactory() {
}

