#ifndef TRACKEVENT_TRACKUNITSCONVERTERS_H
#define TRACKEVENT_TRACKUNITSCONVERTERS_H_H 1

// Include files
// -------------
// from LHCbKernel
#include "GaudiKernel/PhysicalConstants.h"
#include "Kernel/TrackTypes.h"

/** @namespace TrackUnitsConverters
 *
 * This namespace defines some useful units converters
 *
 * @author Eduardo Rodrigues
 * created 2006-02-10
 * 
 */

namespace TrackUnitsConverters
{
  
  //===========================================================================
  // Change some units for better matrix invertability
  //===========================================================================
  void convertToG3( Gaudi::TrackSymMatrix& C )
  {
    C(0,0) /= Gaudi::Units::cm2;
    C(0,1) /= Gaudi::Units::cm2; 
    C(0,2) /= Gaudi::Units::cm;
    C(0,3) /= Gaudi::Units::cm;
    C(0,4) *= Gaudi::Units::GeV/ Gaudi::Units::cm;
    
    C(1,1) /= Gaudi::Units::cm2; 
    C(1,2) /= Gaudi::Units::cm;
    C(1,3) /= Gaudi::Units::cm;
    C(1,4) *= Gaudi::Units::GeV/ Gaudi::Units::cm;
    
    C(2,4) *= Gaudi::Units::GeV;
    
    C(3,4) *= Gaudi::Units::GeV;
    
    C(4,4) *= Gaudi::Units::GeV*Gaudi::Units::GeV;
  };

  //===========================================================================
  // Change some units for better matrix invertability
  //===========================================================================
  void convertToG3( Gaudi::SymMatrix4x4& C )
  {
    C(0,0) /= Gaudi::Units::cm2;
    C(0,1) /= Gaudi::Units::cm2; 
    C(0,2) /= Gaudi::Units::cm;
    C(0,3) /= Gaudi::Units::cm;
    
    C(1,1) /= Gaudi::Units::cm2; 
    C(1,2) /= Gaudi::Units::cm;
    C(1,3) /= Gaudi::Units::cm;
  };
  
  //===========================================================================
  // Restore units after inversion. same as convertToG3 !
  //===========================================================================
  void convertToG4( Gaudi::TrackSymMatrix& invC )
  {
    invC(0,0) /= Gaudi::Units::cm2;
    invC(0,1) /= Gaudi::Units::cm2;
    invC(0,2) /= Gaudi::Units::cm;
    invC(0,3) /= Gaudi::Units::cm;
    invC(0,4) *= Gaudi::Units::GeV/ Gaudi::Units::cm ;

    invC(1,1) /= Gaudi::Units::cm2;
    invC(1,2) /= Gaudi::Units::cm;
    invC(1,3) /= Gaudi::Units::cm;
    invC(1,4) *= Gaudi::Units::GeV/ Gaudi::Units::cm;

    invC(2,4) *= Gaudi::Units::GeV;

    invC(3,4) *= Gaudi::Units::GeV;

    invC(4,4) *= Gaudi::Units::GeV*Gaudi::Units::GeV;
  };
  
  //===========================================================================
  // Restore units after inversion. same as convertToG3 !
  //===========================================================================
  void convertToG4( Gaudi::SymMatrix4x4& invC )
  {
    invC(0,0) /= Gaudi::Units::cm2;
    invC(0,1) /= Gaudi::Units::cm2; 
    invC(0,2) /= Gaudi::Units::cm;
    invC(0,3) /= Gaudi::Units::cm;
    
    invC(1,1) /= Gaudi::Units::cm2; 
    invC(1,2) /= Gaudi::Units::cm;
    invC(1,3) /= Gaudi::Units::cm;
  };
  
  //===========================================================================
}

#endif /// TRACKEVENT_TRACKUNITSCONVERTERS_H
