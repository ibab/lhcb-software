// $ID: $
#define DDEXAMPLE_DECALORIMETER_CPP

// Include files
#include "DeCalorimeter.h"

//------------------------------------------------------------------------------
//
// Implementation of class :  DeCalorimeter
//
//------------------------------------------------------------------------------

const CLID& CLID_DECalorimeter = 8900;  // User defined

// Standard Constructors
DeCalorimeter::DeCalorimeter() :
  m_codingBit(0),
  m_etInCenter(0),
  m_etSlope(0),
  m_adcMax(0),
  m_activeToTotal(0),
  m_zShowerMax(0)
{}


// Retrieve Pointer to class defininition structure
const CLID& DeCalorimeter::classID() {
  return CLID_DECalorimeter;
}


StatusCode DeCalorimeter::initialize() {
  setCodingBit (userParameterAsInt("CodingBit"));
  setEtInCenter (userParameterAsDouble("EtInCenter"));
  setEtSlope (userParameterAsDouble("EtSlope"));
  setAdcMax (userParameterAsInt("AdcMax"));
  setActiveToTotal (userParameterAsDouble("ActiveToTotal"));
  setZShowerMax (userParameterAsDouble("ZShowerMax"));
  return StatusCode::SUCCESS;
}
