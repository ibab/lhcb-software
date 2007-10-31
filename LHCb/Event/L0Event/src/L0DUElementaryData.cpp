// $Id: L0DUElementaryData.cpp,v 1.3 2007-10-31 14:31:15 odescham Exp $
// Include files 
#include <utility>
#include <string>
#include <map>
#include <sstream>

#include "Event/L0DUElementaryData.h"

//-----------------------------------------------------------------------------
// Auxilliary Implementation file for L0DUElementaryData Event classes
//
// 09/02/2006 : Olivier Deschamps
//-----------------------------------------------------------------------------

unsigned long LHCb::L0DUElementaryData::operation(const std::string& v_operator, const std::vector<unsigned long>& v_operands){
  unsigned long digit = 0;
  // 1 dimensional operator (Identity)
  if(1 == v_operands.size() ){
    if( "Id" == v_operator ){digit = v_operands[0] ;}
  }
  // 2 dimensional operator (+,-)
  if(2 == v_operands.size() ){
    if( "+" == v_operator ){digit = v_operands[0] + v_operands[1] ;}
    if( "-" == v_operator ){digit = v_operands[0] - v_operands[1] ;}
    if( "&" == v_operator ){digit = v_operands[0] & v_operands[1] ;}
    if( "^" == v_operator ){digit = v_operands[0] ^ v_operands[1] ;}
  }
  // Insert here other N-dimensional operators you need (LUT,...)
  // ...
  //  
  return digit;
}


std::string LHCb::L0DUElementaryData::summary(){
  std::stringstream s(" ");
  std::stringstream ss(" ");
  if(m_operator != "Id")
    ss << " = op[" << m_operator << "](" << operandsName() << ")";
  std::stringstream sss(" ");
  if( scale() != 1.)sss<< "  ( => " << value() << " MeV) " ;
  s << " [ " << m_name << ss.str() << "  : " <<  digit() << sss.str() << " ] ";
  return s.str();
}


std::string LHCb::L0DUElementaryData::description(){
  std::stringstream s(" ");
  std::stringstream ss(" ");
  if(m_operator != "Id")ss << " = [" << m_operator << "](" << m_operandsName << ")" ;
  s << "[ " << m_name << " " << ss.str() << "] ";
  return s.str();

}

