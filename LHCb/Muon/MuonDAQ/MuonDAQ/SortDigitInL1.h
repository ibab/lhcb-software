// $Id: SortDigitInL1.h,v 1.1 2004-08-31 10:06:10 asatta Exp $
#ifndef SORTDIGITINL1_H 
#define SORTDIGITINL1_H 1



#include <iostream>
#include <functional>
#include "MuonDAQ/MuonHLTDigitFormat.h"
#include "MuonHLTBase.h"

class SortDigitInL1 :
  public std::binary_function<unsigned int,unsigned int,bool>{
public :
  bool operator()(  const unsigned int first, 
                    const unsigned int second )
    const;
};


inline bool SortDigitInL1::operator()(const unsigned int first,
const unsigned int second )const
{
  return (( first & MuonHLTBase::MaskAddress ) >> MuonHLTBase::ShiftAddress) 
    <(( second & MuonHLTBase::MaskAddress ) >>MuonHLTBase::ShiftAddress);
  };



#endif // SORTDIGITINL1_H
