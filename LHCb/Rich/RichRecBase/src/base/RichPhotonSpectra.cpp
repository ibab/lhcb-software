
//-----------------------------------------------------------------------------
/** @file RichPhotonSpectra.cpp
 *
 *  Implementation file for class : RichPhotonSpectra
 *
 *  CVS Log :-
 *  $Id: RichPhotonSpectra.cpp,v 1.5 2004-07-26 18:00:58 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-12
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichPhotonSpectra.h"

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
