#ifndef LESTER_RICH1_CONFIGURATION_H
#define LESTER_RICH1_CONFIGURATION_H

#include <iostream>
#include <string>

#include "RichConfiguration.h"
#include "FinderExternalException.h"

namespace Lester {
  
  class Rich1Configuration : public RichConfiguration {

  public:

  typedef enum { upperPane, lowerPane } Pane;

    Rich1Configuration( const Pane pane ) :
      m_pane(pane) {
    }

    double geometricXYAcceptanceBottomBound() const { return m_pane != upperPane ? -600 :  -30 ; };
    double geometricXYAcceptanceTopBound   () const { return m_pane != upperPane ?  30  :   600 ; };
    double geometricXYAcceptanceLeftBound  () const { return -730; };
    double geometricXYAcceptanceRightBound () const { return  730; };

  private:

    const Pane m_pane;

  };

  extern Rich1Configuration globalRich1LConfiguration; //declaration
  extern Rich1Configuration globalRich1UConfiguration; //declaration

}

#endif
