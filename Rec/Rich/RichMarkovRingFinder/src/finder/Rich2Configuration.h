#ifndef LESTER_RICH2_CONFIGURATION_H
#define LESTER_RICH2_CONFIGURATION_H

#include "RichConfiguration.h"

namespace Lester {
  
  class Rich2Configuration : public RichConfiguration {

  public:

    typedef enum { leftPane, rightPane } Pane;

    Rich2Configuration( const Pane pane ) :
      m_pane(pane) {
    };
    double geometricXYAcceptanceBottomBound() const { return -730; }; // guessed extension of RICH acceptance
    double geometricXYAcceptanceTopBound   () const { return +730; }; // and here too
    double geometricXYAcceptanceLeftBound  () const { return m_pane != leftPane ? -700 :  -30 ; };
    double geometricXYAcceptanceRightBound () const { return m_pane != leftPane ?  30  :   700 ; };

  private:
    const Pane m_pane;
    
  };

  extern Rich2Configuration globalRich2LConfiguration; //declaration
  extern Rich2Configuration globalRich2RConfiguration; //declaration

}

#endif
