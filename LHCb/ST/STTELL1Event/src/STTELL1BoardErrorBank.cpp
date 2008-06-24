
#include "Event/STTELL1BoardErrorBank.h"
#include <bitset>

//-----------------------------------------------------------------------------
// Implementation file for class : STChannelID
//
// 2008-06-07 : M Needham
//-----------------------------------------------------------------------------

std::ostream& LHCb::STTELL1BoardErrorBank::fillStream(std::ostream& s) const
{
  s  << "################################################" << std::endl;
  s << "Error bank for TELL1 board source "  << key();
  LHCb::STTELL1BoardErrorBank::Errors::const_iterator iter = m_errorInfo.begin();
  for(; iter != m_errorInfo.end(); ++iter){
    s << *iter << std::endl;
  } // iter 
 
  return s << "################################################" << std::endl;
}
