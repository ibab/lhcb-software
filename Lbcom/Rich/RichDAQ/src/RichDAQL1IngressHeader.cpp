
//=============================================================================================
/** @file RichDAQL1IngressHeader.cpp
 *
 *  Implementation file for RICH DAQ utility class : Rich::DAQ::L1IngressHeader
 *
 *  CVS Log :-
 *  $Id: RichDAQL1IngressHeader.cpp,v 1.1 2007-02-01 17:42:29 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   619/01/2007
 */
//=============================================================================================

#include "RichDAQL1IngressHeader.h"

using namespace Rich::DAQ;

L1IngressHeader::L1IngressHeader( const HPDHashMap & hpdMap,
                                  const LHCb::ODIN * odin )
  : m_data(0)
{
  // set the event and bxID bits
  setEventID ( EventID ( odin ? odin->eventNumber() : 0 ) );
  setBXID    ( BXID    ( odin ? odin->orbitNumber() : 0 ) );
  // set the bits for each active HPD
  for ( HPDHashMap::const_iterator iMap = hpdMap.begin();
        iMap != hpdMap.end(); ++iMap )
  {
    setHPDActive( iMap->first.second.l1InputWithinIngress() );
  }
}

void L1IngressHeader::setHPDsActive( const L1IngressInputs & inputs )
{
  for ( L1IngressInputs::const_iterator input = inputs.begin();
        input != inputs.end(); ++input )
  {
    setHPDActive( *input );
  }
}

ShortType L1IngressHeader::numActiveHPDs() const
{
  const ShortType hpdBits = activeHPDbits();
  ShortType numHPDs(0);
  for ( ShortType i =0; i < NumL1InputsPerIngress; ++i )
  {
    if ( isBitOn(hpdBits,i) ) ++numHPDs;
  }
  return numHPDs;
}

std::ostream& L1IngressHeader::fillStream( std::ostream& os ) const
{
  os << "[ EvtID=" << eventID() << " BXID=" << bxID()
     << " ActiveHPDs=";
  L1InputWithinIngress in(0);
  for ( ; in < L1InputWithinIngress(NumL1InputsPerIngress); ++in )
  {
    os << isHPDActive(in);
  }
  os << " Truncated=" << hpdsSuppressed();
  return os << " ]";
}
