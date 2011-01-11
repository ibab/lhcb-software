// $Id: L0DUElementaryCondition.cpp,v 1.6 2010-01-20 15:59:06 odescham Exp $
// Include files 
#include <utility>
#include <string>
#include <map>
#include <sstream>

#include "Event/L0DUElementaryCondition.h"

//-----------------------------------------------------------------------------
// Auxilliary Implementation file for L0DUElementaryCondition Event classes
//
// 09/02/2006 : Olivier Deschamps
//-----------------------------------------------------------------------------


bool LHCb::L0DUElementaryCondition::comparison( unsigned int a, const std::string& comparator,  unsigned int b){
    if( "<" == comparator ){return (a < b);}
    if( ">" == comparator ){return (a > b);}
    if( "==" == comparator){return (a == b);}
    if( "!=" == comparator){return (a != b);}
    return false;}

std::string LHCb::L0DUElementaryCondition::summary(){

  std::ostringstream s(" ");
  std::ostringstream ss(" ");
  #ifdef __INTEL_COMPILER         // Disable ICC remark
    #pragma warning(disable:1572) // Floating-point equality and inequality comparisons are unreliable
  #endif
  if(m_data->scale() != 1.)ss<< " ( => " << m_data->scale() * (double) m_threshold << " MeV )" ;
  
  s << "Condition [ " << m_name << " : "
    << m_data->summary()
    << " "   << m_comparator  
    << " "   << m_threshold << ss.str() <<"] ?  ==> " << emulatedValue() ;
  return s.str(); 

}

std::string LHCb::L0DUElementaryCondition::description(){

  std::ostringstream s(" ");
  
  s << "Condition [ " << m_name << " : "
    << m_data->description() << " "   << m_comparator  << " "   << m_threshold <<"] ?";
  return s.str(); 

}



