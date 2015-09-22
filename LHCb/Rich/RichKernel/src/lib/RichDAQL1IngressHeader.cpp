
//=============================================================================================
/** @file RichDAQL1IngressHeader.cpp
 *
 *  Implementation file for RICH DAQ utility class : Rich::DAQ::L1IngressHeader
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   619/01/2007
 */
//=============================================================================================

#include "RichKernel/RichDAQL1IngressHeader.h"

using namespace Rich::DAQ;

void L1IngressHeader::setHPDsActive( const L1IngressInputs & inputs )
{
  for ( const auto& input : inputs ) { setHPDActive(input); }
}

void L1IngressHeader::activeHPDInputs( L1IngressInputs & inputs ) const
{
  inputs.clear();
  inputs.reserve(numActiveHPDs());
  const ShortType hpdBits = activeHPDbits();
  for ( ShortType i = 0; i < NumL1InputsPerIngress; ++i )
  {
    if ( isBitOn(hpdBits,i) ) inputs.push_back( L1InputWithinIngress(i) );
  }
}

void L1IngressHeader::inactiveHPDInputs( L1IngressInputs & inputs ) const
{
  inputs.clear();
  const ShortType hpdBits = activeHPDbits();
  for ( ShortType i = 0; i < NumL1InputsPerIngress; ++i )
  {
    if ( !isBitOn(hpdBits,i) ) inputs.push_back( L1InputWithinIngress(i) );
  }
}

std::ostream& L1IngressHeader::fillStream( std::ostream& os ) const
{
  os << "[ ID=" << ingressID() << " EvtID=" << eventID() 
     << " BXID=" << bxID() << " ActiveHPDs=";
  L1InputWithinIngress in(0);
  for ( ; in < L1InputWithinIngress(NumL1InputsPerIngress); ++in )
  {
    os << isHPDActive(in);
  }
  os << " Truncated=" << hpdsSuppressed();
  return os << " ]";
}
