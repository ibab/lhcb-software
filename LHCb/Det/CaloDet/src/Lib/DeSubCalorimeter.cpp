
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

std::ostream& DeSubCalorimeter::printOut( std::ostream& os ) const 
{
  os << "\tDeSubCalorimeter" 
     << " fullname ='" << fullpath() 
     << std::endl; 
  os << "\t\t\tCellSize=" << std::setw(12) << m_size
     << std::endl;    
  return os;
};

MsgStream&    DeSubCalorimeter::printOut( MsgStream&    os ) const 
{
  os << "\tDeSubCalorimeter" 
     << " fullname ='" << fullpath() 
     << endreq   ; 
  os << "\t\t\tCellSize=" << std::setw(12) << m_size
     << endreq   ;    
  return os;
};
