// $Id: RichPhotonSpectra.cpp,v 1.4 2004-07-23 08:47:39 cattanem Exp $
// Include files 

// local
#include "RichRecBase/RichPhotonSpectra.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPhotonSpectra
//
// 2003-07-12 : Chris Jones
//-----------------------------------------------------------------------------

template <class TYPE>
TYPE RichPhotonSpectra<TYPE>::integral( const Rich::ParticleIDType id ) const 
{  
  TYPE sum = 0;
  for ( typename RichPhotonSpectra<TYPE>::PhotonData::const_iterator i = energyDist(id).begin();
        i != energyDist(id).end(); ++i ) { sum += *i; }
  return sum;
}

template <class TYPE>
bool RichPhotonSpectra<TYPE>::multiply( const Rich::ParticleIDType id,
                                        const typename RichPhotonSpectra<TYPE>::PhotonData & data ) 
{  
  if ( this->energyBins() != data.size() ) return false;
  typename RichPhotonSpectra<TYPE>::PhotonData::const_iterator j = data.begin();
  for ( typename RichPhotonSpectra<TYPE>::PhotonData::iterator i = energyDist(id).begin();
        i != energyDist(id).end(); ++i ) { *i *= *j; ++j; }
  return true;
}
