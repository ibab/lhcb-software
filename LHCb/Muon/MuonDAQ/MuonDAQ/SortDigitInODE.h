// $Id: SortDigitInODE.h,v 1.2 2008-04-02 11:52:05 asatta Exp $
#ifndef SORTDIGITINODE_H 
#define SORTDIGITINODE_H 1



#include <iostream>
#include <functional>
#include "MuonDAQ/MuonHLTDigitFormat.h"
#include "MuonHLTBase.h"

class SortDigitInODE :
  public std::binary_function<unsigned int,unsigned int,bool>{
public :
  bool operator()(  const unsigned int first, 
                    const unsigned int second )
    const;
};


inline bool SortDigitInODE::operator()(const unsigned int first,
const unsigned int second )const
{
  return (( first & MuonHLTBaseDC06::MaskAddress ) >> 
MuonHLTBaseDC06::ShiftAddress) 
    <(( second & MuonHLTBaseDC06::MaskAddress ) 
>>MuonHLTBaseDC06::ShiftAddress);
  }



#endif // SORTDIGITINODE_H
