#ifndef _STRawBankMap_H_
#define _STRawBankMap_H_

/** @namespace STRawBankMap STRawBankMap.h SKernel/STRawBank
 *
 *  Map string to corresponding bank type enum
 *  @author M.Needham
 *  @date   06/09/2007
 */

#include "Event/RawBank.h"

namespace STRawBankMap{

inline LHCb::RawBank::BankType stringToType(const std::string& type){

  // string to bank type
  LHCb::RawBank::BankType bankType = LHCb::RawBank::Velo;

  if (type == "TT"){
    bankType = LHCb::RawBank::TT;
  }
  else if (type == "IT"){
    bankType = LHCb::RawBank::IT;
  }
  else if (type == "UT"){
    bankType = LHCb::RawBank::UT;
  }
  else if (type == "TTFull"){
    bankType = LHCb::RawBank::TTFull;
  }
  else if (type == "ITFull"){
    bankType = LHCb::RawBank::ITFull;
  }
  else if (type == "UTFull"){
    bankType = LHCb::RawBank::UTFull;
  }
  else if (type == "TTError" ) {
    bankType = LHCb::RawBank::TTError;
  }
  else if (type == "ITError" ){
    bankType = LHCb::RawBank::ITError;
  }
  else if (type == "UTError" ){
    bankType = LHCb::RawBank::UTError;
  }
  else if (type == "TTPedestal" ){
    bankType = LHCb::RawBank::TTPedestal;
  }
  else if (type == "ITPedestal" ){
    bankType = LHCb::RawBank::ITPedestal;
  }
  else if (type == "UTPedestal" ){
    bankType = LHCb::RawBank::UTPedestal;
  }
  else {
    // use the Velo as NULL
  }
  return  bankType;
}




}

#endif // _STRawBankMap_H_
