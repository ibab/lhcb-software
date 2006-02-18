// $Id: L0DUElementaryData.cpp,v 1.1 2006-02-18 11:20:15 ocallot Exp $
// Include files 
#include <utility>
#include <string>
#include <map>

#include "Event/L0DUElementaryData.h"

//-----------------------------------------------------------------------------
// Auxilliary Implementation file for L0DUElementaryData Event classes
//
// 09/02/2006 : Olivier Deschamps
//-----------------------------------------------------------------------------

double LHCb::L0DUElementaryData::operation(const std::string& v_operator, const std::vector<double>& v_operands){
double value = 0.;
    // 1 dimensional operator (Identity)
    if(1 == v_operands.size() ){
      if( "Id" == v_operator ){value = v_operands[0] ;}
    }
    // 2 dimensional operator (+,-)
    if(2 == v_operands.size() ){
      if( "+" == v_operator ){value = v_operands[0] + v_operands[1] ;}
      if( "-" == v_operator ){value = v_operands[0] - v_operands[1] ;}
    }
    // Insert here other N-dimensional operators you need (LUT,...)

    return value;}

std::string LHCb::L0DUElementaryData::summary(){

  std::stringstream s(" ");
  s << " [ " << m_name <<" , " <<  LHCb::L0DUElementaryData::operation(m_operator,m_operands) << " ] ";
  return s.str();

}

