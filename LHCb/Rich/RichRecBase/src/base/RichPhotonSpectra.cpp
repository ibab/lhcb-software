// $Id: RichPhotonSpectra.cpp,v 1.2 2004-02-02 14:23:05 jonesc Exp $
// Include files 

// local
#include "RichRecBase/RichPhotonSpectra.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPhotonSpectra
//
// 2003-07-12 : Chris Jones
//-----------------------------------------------------------------------------

double RichPhotonSpectra::integral( const Rich::ParticleIDType id ) const {

  double sum = 0;
  for ( RichPhotonSpectra::PhotonData::const_iterator i = energyDist(id).begin();
        i != energyDist(id).end(); ++i ) { sum += *i; }
  
  return sum;
}

bool RichPhotonSpectra::multiply( const Rich::ParticleIDType id,
                                  const RichPhotonSpectra::PhotonData & data ) {
  
  if ( this->energyBins() != data.size() ) return false;
  RichPhotonSpectra::PhotonData::const_iterator j = data.begin();
  for ( RichPhotonSpectra::PhotonData::iterator i = energyDist(id).begin();
        i != energyDist(id).end(); ++i ) { *i *= *j; ++j; }
  
  return true;
}
