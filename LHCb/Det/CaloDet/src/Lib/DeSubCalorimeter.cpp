#define DDEXAMPLE_DESUBCALORIMETER_CPP

// Include files
#include "CaloDet/DeSubCalorimeter.h"



//------------------------------------------------------------------------------
//
// Implementation of class :  DeSubCalorimeter
//
//------------------------------------------------------------------------------

// Standard Constructors
DeSubCalorimeter::DeSubCalorimeter( const std::string& name ) 
  : DetectorElement     ( name )
  , m_size( 0.0 )
{};
  
// Standard Destructor
DeSubCalorimeter::~DeSubCalorimeter()
{};
  
