// local
#include"Event/Measurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : Measurement
//
// 2004-12-14 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

//=============================================================================
// Check the measurement type
//=============================================================================
bool Measurement::checkType( unsigned int value ) const
{
  return type() == value;
}

//=============================================================================
