// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/DeCalorimeter.cpp,v 1.3 2001-06-28 09:50:07 sponce Exp $
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
DeCalorimeter::DeCalorimeter() : m_coding(0) {}
  
// Retrieve Pointer to class defininition structure
const CLID& DeCalorimeter::classID() {
  return CLID_DECalorimeter;
}

// Get/Set size
int DeCalorimeter::coding() {
  return m_coding;
}

void DeCalorimeter::setCoding (int coding) {
  m_coding = coding;
}

StatusCode DeCalorimeter::initialize() {
  setCoding (userParameterAsInt("CodingBit"));
  return StatusCode::SUCCESS;
}
