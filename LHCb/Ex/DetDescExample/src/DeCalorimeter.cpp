// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/DeCalorimeter.cpp,v 1.1.1.1 2001-03-13 15:11:14 cattaneb Exp $
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
DeCalorimeter::DeCalorimeter()
: m_size( 0.0 )
{
}
  
DeCalorimeter::DeCalorimeter( double s )
: m_size( s )
{
}

// Standard Destructor
DeCalorimeter::~DeCalorimeter()
{
}
  
// Retrieve Pointer to class defininition structure
const CLID& DeCalorimeter::classID()
{
  return CLID_DECalorimeter;
}

// Get/Set size
double DeCalorimeter::size()
{
  return m_size;
}

void   DeCalorimeter::setSize( double s )
{
  m_size = s;
}
