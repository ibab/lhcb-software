#ifndef LESTER_RICH_CONFIGURATION_H
#define LESTER_RICH_CONFIGURATION_H

#include "Configuration.h"
#include "Constants.h"

namespace Lester {
  
  class RichConfiguration : public Configuration { 
  public:
    virtual double geometricXYAcceptanceLeftBound() const = 0;
    virtual double geometricXYAcceptanceRightBound() const = 0;
    virtual double geometricXYAcceptanceBottomBound() const = 0;
    virtual double geometricXYAcceptanceTopBound() const = 0;
    double geometricThetaAcceptanceLeftBound() const {
      return geometricXYAcceptanceLeftBound()  *Constants::realXYDataInputScaleFactor;
    };
    double geometricThetaAcceptanceRightBound() const {
      return geometricXYAcceptanceRightBound() *Constants::realXYDataInputScaleFactor;
    };
    double geometricThetaAcceptanceBottomBound() const {
      return geometricXYAcceptanceBottomBound()*Constants::realXYDataInputScaleFactor;
    };
    double geometricThetaAcceptanceTopBound() const {
      return geometricXYAcceptanceTopBound()   *Constants::realXYDataInputScaleFactor;
    };
  protected:
    RichConfiguration() {};
  };

}

#endif
