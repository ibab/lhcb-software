#ifndef LESTER_RICH1_CONFIGURATION_H
#define LESTER_RICH1_CONFIGURATION_H

#include <iostream>
#include <string>

#include "RichConfiguration.h"
#include "FinderExternalException.h"

namespace Lester {
  
  class Rich1Configuration : public RichConfiguration {
  private:

    class FatalThrow : public FinderExternalException {
    public:
      FatalThrow(const std::string & s) : FinderExternalException(s) {
      };
    };

    void moan() const {
      throw FatalThrow(std::string("Abstract functions are not yet implemented in Rich1 (as opposed to Rich2) Configuration objects."));
    };
  public:
    double geometricXYAcceptanceBottomBound() const { moan(); return 0; };
    double geometricXYAcceptanceTopBound   () const { moan(); return 0; };
    double geometricXYAcceptanceLeftBound  () const { moan(); return 0; };
    double geometricXYAcceptanceRightBound () const { moan(); return 0; };
  };

  extern Rich1Configuration globalRich1LConfiguration; //declaration
  extern Rich1Configuration globalRich1UConfiguration; //declaration

}

#endif
