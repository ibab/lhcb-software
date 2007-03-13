// $Id: GaussGenUtil.cpp,v 1.3 2007-03-13 19:13:26 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"

// From HepMC
#include "Event/HepMCEvent.h"

// local
#include "GaussGenUtil.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaussGenUtil
//
// 2007-02-16 : Gloria CORTI
//-----------------------------------------------------------------------------

//=============================================================================
//  Lifetime of a stable particle (as c*tau)
//=============================================================================
namespace GaussGenUtil {
  
  double lifetime( const HepMC::GenParticle* thePart ) {

    // Exit for off-shell particles
    if ( thePart -> momentum().restMass2() < 0 ) return -1.0 ;
    
    if ( thePart->end_vertex() ) {   
      HepLorentzVector thePosition = thePart->end_vertex()->position() -
        thePart->production_vertex()->position() ;
      HepLorentzVector theNewPosition ;
      theNewPosition.setX( thePosition.x() ) ;
      theNewPosition.setY( thePosition.y() ) ;
      theNewPosition.setZ( thePosition.z() ) ;
      theNewPosition.setT( thePosition.t() * Gaudi::Units::c_light ) ;

      Hep3Vector theBoost = thePart->momentum().boostVector() ;
      HepLorentzVector thePositionBoost = theNewPosition.boost( -theBoost );
      return thePositionBoost.t();  // in mm
    }
    return -1.0 ;

  }
  
}

//=============================================================================
// const double EvtConst::c            =  2.99792458E11;       // mm/sec
// static const double c_light         =  2.99792458e+8 * m/s;
  
 
