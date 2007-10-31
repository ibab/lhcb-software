// $Id: L0DUElementaryCondition.cpp,v 1.3 2007-10-31 14:31:15 odescham Exp $
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


bool LHCb::L0DUElementaryCondition::comparison(unsigned long a, const std::string& comparator, unsigned long b){
    if( "<" == comparator ){return (a < b);}
    if( ">" == comparator ){return (a > b);}
    if( "==" == comparator){return (a == b);}
    if( "!=" == comparator){return (a != b);}
    return false;}

std::string LHCb::L0DUElementaryCondition::summary(){

  std::stringstream s(" ");
  std::stringstream ss(" ");
  if(m_data->scale() != 1.)ss<< " ( => " << m_data->scale() * (double) m_threshold << " MeV )" ;
  
  s << "Condition [ " << m_name << " : "
    << m_data->summary()
    << " "   << m_comparator  
    << " "   << m_threshold << ss.str() <<"] ?  ==> " << value() ;
  return s.str(); 

}

std::string LHCb::L0DUElementaryCondition::description(){

  std::stringstream s(" ");
  
  s << "Condition [ " << m_name << " : "
    << m_data->description() << " "   << m_comparator  << " "   << m_threshold <<"] ?";
  return s.str(); 

}



