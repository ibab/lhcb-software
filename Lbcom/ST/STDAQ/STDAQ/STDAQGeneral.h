#ifndef _STDAQGeneral_H_
#define _STDAQGeneral_H_

/** @class STDAQGeneral  STDAQGeneral.h STDAQ/STDAQGeneral.h
 *
 *  Dump everything that does not fit and is caused by sebd
 *
 *  @author M.Needham
 *  @date   10/1/2004
 */

#include "Event/RawBank.h"

namespace STDAQGeneral{

inline LHCb::RawBank::BankType stringToRawBankType(const std::string& type){

  // string to bank type
  LHCb::RawBank::BankType bankType = LHCb::RawBank::Velo;

  if (type == "TT"){
    bankType = LHCb::RawBank::TT;
  }
  else if (type == "IT"){
    bankType = LHCb::RawBank::IT;
  }
  else {
    // use the Velo as NULL
  }
  return  bankType;
}




};

#endif // _STDAQGeneral_H_
