// $Id : $

// Include files
#include "Event/PhysSelDecay.h"

//-------------------------------------------------------------------
// Implementation file for class: PhysSelDecay
//
// 10/06/2001 : Gloria Corti
//-------------------------------------------------------------------

//===================================================================
// Retrieve all results of selection routine for a decay channel
//===================================================================
void PhysSelDecay::results( bool& lMCFlag, bool& lTKRFlag,  
                          bool& lPQUALFlag, bool& lSELFlag, 
                          bool& lAGRFlag, bool& lTAGFlag ) const {
  lMCFlag = decayIsInMCTree();
  lTKRFlag = decayHasTrkRecon();
  lPQUALFlag = decayHasTrkPQual();
  lSELFlag = decayIsSelected();
  lAGRFlag = decaySelectedIsMC();
  lTAGFlag = decayIsFlavourTagged();
  return;
}

//===================================================================
// Set all results of selection routine for a decay channel
//===================================================================
void PhysSelDecay::setResults ( bool lMCFlag, bool lTKRFlag, 
                                bool lPQUALFlag, bool lSELFlag,
                                bool lAGRFlag, bool lTAGFlag ) {
  if( lMCFlag ) {
    setDecayIsInMCTree();
  }
  if( lTKRFlag ) {
    setDecayHasTrkRecon();
  }
  if( lPQUALFlag ) {
    setDecayHasTrkPQual();
  }
  if( lSELFlag ) {
    setDecayIsSelected();
  }
  if( lAGRFlag ) {
    setDecaySelectedIsMC();
  }
  if( lTAGFlag ) {
    setDecayIsFlavourTagged();
  }
  return;
}

