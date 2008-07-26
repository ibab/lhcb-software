// $Id: GaussGenUtil.cpp,v 1.5 2008-07-26 18:01:29 robbep Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/Transform4DTypes.h"

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
    if ( thePart -> momentum().m2() < 0 ) return -1.0 ;
    
    if ( thePart->end_vertex() && thePart->production_vertex() ) {   

      Gaudi::LorentzVector thePosition( thePart->end_vertex()->position().x() -
                                        thePart->production_vertex()->position().x(),
                                        thePart->end_vertex()->position().y() -
                                        thePart->production_vertex()->position().y(),
                                        thePart->end_vertex()->position().z() -
                                        thePart->production_vertex()->position().z(),
                                        thePart->end_vertex()->position().t() -
                                        thePart->production_vertex()->position().t());
      // switch units for time to distance (mm)
      thePosition.SetE( thePosition.t() * Gaudi::Units::c_light ) ;
      
      Gaudi::LorentzVector mom( thePart->momentum() ) ;
      ROOT::Math::Boost theBoost( -mom.BoostToCM() ) ;
      
      // switch again to time from distance (ns)
      Gaudi::LorentzVector newPosition = theBoost( thePosition ) ;
      return newPosition.t();  // in mm
    }
    return -1.0 ;

  }
  
}

//=============================================================================
// const double EvtConst::c            =  2.99792458E11;       // mm/sec
// static const double c_light         =  2.99792458e+8 * m/s;
  
 
