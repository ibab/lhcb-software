
#include "Event/STTELL1BoardErrorBank.h"
#include <iostream>

//-----------------------------------------------------------------------------
// Implementation file for class : STChannelID
//
// 2008-06-07 : M Needham
//-----------------------------------------------------------------------------

double LHCb::STTELL1BoardErrorBank::fractionOK(const unsigned int pcn) const{

  double frac = 0.0;
  LHCb::STTELL1BoardErrorBank::ErrorVector::const_iterator iter = m_errorInfo.begin();
  for(; iter != m_errorInfo.end(); ++iter){
    frac += (*iter)->fractionOK(pcn);
  }
  return frac/m_errorInfo.size();
}

std::ostream& LHCb::STTELL1BoardErrorBank::fillStream(std::ostream& s) const
{
  unsigned int pp = 0;
  s << "Error bank for TELL1 board source "  << key() << std::endl;
  LHCb::STTELL1BoardErrorBank::ErrorVector::const_iterator iter = m_errorInfo.begin();
  for(; iter != m_errorInfo.end(); ++iter, ++pp){
    s << "pp "  << pp<< std::endl;
    s << **iter << std::endl;
  } // iter 
 
  return s << std::endl;
}
