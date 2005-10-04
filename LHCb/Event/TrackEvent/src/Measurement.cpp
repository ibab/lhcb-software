// local
#include"Event/Measurement.h"

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
  unsigned int val = (unsigned int)value;
  return 0 != ( m_mtype & typeMask & val );
}

//=============================================================================
