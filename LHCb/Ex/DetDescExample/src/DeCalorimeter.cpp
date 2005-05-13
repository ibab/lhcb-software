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
  setCodingBit (param<int>("CodingBit"));
  setEtInCenter (param<double>("EtInCenter"));
  setEtSlope (param<double>("EtSlope"));
  setAdcMax (param<int>("AdcMax"));
  setActiveToTotal (param<double>("ActiveToTotal"));
  setZShowerMax (param<double>("ZShowerMax"));
  return StatusCode::SUCCESS;
}
