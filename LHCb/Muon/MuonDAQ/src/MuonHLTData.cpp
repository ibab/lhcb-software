// $Id: MuonHLTData.cpp,v 1.2 2004-08-31 10:06:10 asatta Exp $
// Include files 



// local
#include "MuonDAQ/MuonHLTData.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonHLTData
//
// 2004-01-23 : Alessia Satta
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonHLTData::MuonHLTData(  ) {
  m_data=0;  
};
MuonHLTData::MuonHLTData(raw_int num  ) {
  m_data=num;

};
//=============================================================================
// Destructor
//=============================================================================
MuonHLTData::~MuonHLTData() {}; 

//=============================================================================

  void MuonHLTData::setFirstHalf(unsigned int num){
  unsigned int tmp1 , tmp2 ;
  tmp1 = ( num << MuonHLTBase::ShiftFirstHalf ) & MuonHLTBase::MaskFirstHalf ;
  tmp2 = m_data & ~MuonHLTBase::MaskFirstHalf ;
  m_data = ( tmp1 | tmp2 ) ;    
};
  void MuonHLTData::setSecondHalf(unsigned int num)
{
  unsigned int tmp1 , tmp2 ;
  tmp1 = ( num << MuonHLTBase::ShiftSecondHalf ) & 
    MuonHLTBase::MaskSecondHalf ;
  tmp2 = m_data & ~MuonHLTBase::MaskSecondHalf ;
  m_data = ( tmp1 | tmp2 ) ;

};
//  void MuonHLTData::setAddress(unsigned int num, unsigned int value );
//  void MuonHLTData::setTime(unsigned int num, unsigned int value );  
  unsigned int MuonHLTData::getFirstHalf(){
 return  ( m_data & MuonHLTBase::MaskFirstHalf ) >> MuonHLTBase::ShiftFirstHalf ;
};
  unsigned int MuonHLTData::getSecondHalf(){
 return  ( m_data & MuonHLTBase::MaskSecondHalf ) >> 
   MuonHLTBase::ShiftSecondHalf ;
};

  unsigned int MuonHLTData::getWord(){
    return m_data;
    
};

