#ifndef TRACKEVENT_TRACKUNITSCONVERTERS_H
#define TRACKEVENT_TRACKUNITSCONVERTERS_H_H 1

// Include files
// -------------
// from LHCbKernel
#include "Kernel/PhysicalConstants.h"
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
    C(0,0) /= cm2;
    C(0,1) /= cm2; 
    C(0,2) /= cm;
    C(0,3) /= cm;
    C(0,4) /= cm*GeV;
    
    C(1,1) /= cm2; 
    C(1,2) /= cm;
    C(1,3) /= cm;
    C(1,4) /= cm*GeV;
    
    C(2,4) /= GeV;
    
    C(3,4) /= GeV;
    
    C(4,4) /= GeV*GeV;
  };

  //===========================================================================
  // Change some units for better matrix invertability
  //===========================================================================
  void convertToG3( Gaudi::SymMatrix4x4& C )
  {
    C(0,0) /= cm2;
    C(0,1) /= cm2; 
    C(0,2) /= cm;
    C(0,3) /= cm;
    
    C(1,1) /= cm2; 
    C(1,2) /= cm;
    C(1,3) /= cm;
  };
  
  //===========================================================================
  // Restore units after inversion. same as convertToG3 !
  //===========================================================================
  void convertToG4( Gaudi::TrackSymMatrix& invC )
  {
    invC(0,0) /= cm2;
    invC(0,1) /= cm2;
    invC(0,2) /= cm;
    invC(0,3) /= cm;
    invC(0,4) /= cm*GeV;

    invC(1,1) /= cm2;
    invC(1,2) /= cm;
    invC(1,3) /= cm;
    invC(1,4) /= cm*GeV;

    invC(2,4) /= GeV;

    invC(3,4) /= GeV;

    invC(4,4) /= GeV*GeV;
  };
  
  //===========================================================================
  // Restore units after inversion. same as convertToG3 !
  //===========================================================================
  void convertToG4( Gaudi::SymMatrix4x4& invC )
  {
    invC(0,0) /= cm2;
    invC(0,1) /= cm2; 
    invC(0,2) /= cm;
    invC(0,3) /= cm;
    
    invC(1,1) /= cm2; 
    invC(1,2) /= cm;
    invC(1,3) /= cm;
  };
  
  //===========================================================================
}

#endif /// TRACKEVENT_TRACKUNITSCONVERTERS_H
