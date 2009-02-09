
#include "Event/STTELL1BoardErrorBank.h"
#include <iostream>
#include <string>

#include "GaudiKernel/IRegistry.h"

#include "Kernel/STBoardMapping.h"

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

std::string LHCb::STTELL1BoardErrorBank::regName() const{

  // get the name of the container in the TES
  const ObjectContainerBase* parentCont = parent();
  const IRegistry* reg =  parentCont->registry();
  return reg->identifier();
}

bool LHCb::STTELL1BoardErrorBank::isIT() const {
  const std::string name = regName();
  return (name.find("IT") != std::string::npos ? true : false);
}


bool LHCb::STTELL1BoardErrorBank::isTT() const {
  const std::string name = regName();
  return (name.find("TT") != std::string::npos ? true : false);
}

std::ostream& LHCb::STTELL1BoardErrorBank::fillStream(std::ostream& s) const
{
  unsigned int pp = 0;
  s << "Error bank for TELL1 board source "  << key() << std::endl;
  if (isIT() == true) {
    s << STBoardMapping::find(key(), STBoardMapping::ITSourceIDToNumberMap()) << std::endl;
  } 
  else if (isTT() == true){
    s << STBoardMapping::find(key(), STBoardMapping::TTSourceIDToNumberMap()) << std::endl;
  }

  LHCb::STTELL1BoardErrorBank::ErrorVector::const_iterator iter = m_errorInfo.begin();
  for(; iter != m_errorInfo.end(); ++iter, ++pp){
    s << "pp "  << pp<< std::endl;
    s << **iter << std::endl;
  } // iter 
 
  return s << std::endl;
}
