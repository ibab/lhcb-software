// $Id: SortDigitInODE.h,v 1.1 2006-01-31 15:22:33 asatta Exp $
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
  return (( first & MuonHLTBase::MaskAddress ) >> MuonHLTBase::ShiftAddress) 
    <(( second & MuonHLTBase::MaskAddress ) >>MuonHLTBase::ShiftAddress);
  };



#endif // SORTDIGITINODE_H
