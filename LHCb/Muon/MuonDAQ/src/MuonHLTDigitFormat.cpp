// $Id: MuonHLTDigitFormat.cpp,v 1.1.1.1 2004-02-05 16:26:03 cattanem Exp $
// Include files 



// local
#include "MuonHLTDigitFormat.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonHLTDigitFormat
//
// 2004-01-23 : Alessia Satta
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonHLTDigitFormat::MuonHLTDigitFormat(  ) {
m_data=0;
}
MuonHLTDigitFormat::MuonHLTDigitFormat(unsigned int num ){
m_data=num;
};

//=============================================================================
// Destructor
//=============================================================================
MuonHLTDigitFormat::~MuonHLTDigitFormat() {}; 

//=============================================================================
void MuonHLTDigitFormat::setAddress(unsigned int num)
{
 unsigned int tmp1 , tmp2 ;
 tmp1 = ( num << MuonHLTBase::ShiftAddress ) & MuonHLTBase::MaskAddress ;
 tmp2 = m_data & ~MuonHLTBase::MaskAddress ;
 m_data = ( tmp1 | tmp2 ) ;
};

void MuonHLTDigitFormat::setTime(unsigned int num)
{
 unsigned int tmp1 , tmp2 ;
 tmp1 = ( num << MuonHLTBase::ShiftTime ) & MuonHLTBase::MaskTime ;
 tmp2 = m_data & ~MuonHLTBase::MaskTime ;
 m_data = ( tmp1 | tmp2 ) ;
};

unsigned int MuonHLTDigitFormat::getTime()
{
 return  ( m_data & MuonHLTBase::MaskTime ) >> 
   MuonHLTBase::ShiftTime ;
};

unsigned int MuonHLTDigitFormat::getAddress()
{
 return  ( m_data & MuonHLTBase::MaskAddress ) >> 
   MuonHLTBase::ShiftAddress ;
};

unsigned int  MuonHLTDigitFormat::getWord()
{
  return m_data;
  
}
