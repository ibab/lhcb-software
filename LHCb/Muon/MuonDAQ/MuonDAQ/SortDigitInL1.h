// $Id: SortDigitInL1.h,v 1.2 2008-04-02 11:52:05 asatta Exp $
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
//std::cout<<(( first & MuonHLTBaseV1::MaskAddress ) >>
//MuonHLTBaseV1::ShiftAddress)<<std::endl;
  return (( first & MuonHLTBaseV1::MaskAddress ) >> 
MuonHLTBaseV1::ShiftAddress) 
    <(( second & MuonHLTBaseV1::MaskAddress ) 
>>MuonHLTBaseV1::ShiftAddress);
  };



#endif // SORTDIGITINL1_H
