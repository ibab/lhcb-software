// $Id: L0DUElementaryCondition.cpp,v 1.2 2006-07-31 13:57:07 cattanem Exp $
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


bool LHCb::L0DUElementaryCondition::comparison(double a, const std::string& comparator, double b){
    if( "<" == comparator ){return (a < b);}
    if( ">" == comparator ){return (a > b);}
    if( "<=" == comparator ){return (a <= b);}
    if( ">=" == comparator ){return (a >= b);}
    if( "==" == comparator ){return (a = b);}
    if( "!=" == comparator){return (a != b);}
    return false;}

std::string LHCb::L0DUElementaryCondition::summary(){

  std::stringstream s(" ");
  s << "Condition ( " << m_name << " ) : "
    << "("   << m_data->summary()
    << " "   << m_comparator  
    << " "   << m_threshold 
    << ")?  == " << 
    comparison( m_data->value() ,m_comparator , m_threshold);
  return s.str();

}

