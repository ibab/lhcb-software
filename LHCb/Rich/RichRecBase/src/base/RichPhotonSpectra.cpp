
//-----------------------------------------------------------------------------
/** @file RichPhotonSpectra.cpp
 *
 *  Implementation file for class : RichPhotonSpectra
 *
 *  CVS Log :-
 *  $Id: RichPhotonSpectra.cpp,v 1.6 2007-02-01 17:26:23 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.5  2004/07/26 18:00:58  jonrob
 *  Various improvements to the doxygen comments
 *
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-12
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichPhotonSpectra.h"

template <class TYPE>
TYPE Rich::PhotonSpectra<TYPE>::integral( const Rich::ParticleIDType id ) const
{
  TYPE sum = 0;
  for ( typename Rich::PhotonSpectra<TYPE>::PhotonData::const_iterator i = energyDist(id).begin();
        i != energyDist(id).end(); ++i ) { sum += *i; }
  return sum;
}

template <class TYPE>
bool Rich::PhotonSpectra<TYPE>::multiply( const Rich::ParticleIDType id,
                                          const typename Rich::PhotonSpectra<TYPE>::PhotonData & data )
{
  if ( this->energyBins() != data.size() ) return false;
  typename Rich::PhotonSpectra<TYPE>::PhotonData::const_iterator j = data.begin();
  for ( typename Rich::PhotonSpectra<TYPE>::PhotonData::iterator i = energyDist(id).begin();
        i != energyDist(id).end(); ++i ) { *i *= *j; ++j; }
  return true;
}
