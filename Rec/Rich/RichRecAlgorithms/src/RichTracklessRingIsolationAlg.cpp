
//--------------------------------------------------------------------------
/** @file RichTracklessRingIsolationAlg.cpp
 *
 *  Implementation file for algorithm class : RichTracklessRingIsolationAlg
 *
 *  CVS Log :-
 *  $Id: RichTracklessRingIsolationAlg.cpp,v 1.2 2008-06-13 13:20:27 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichTracklessRingIsolationAlg.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TracklessRingIsolationAlg );

// Standard constructor, initializes variables
TracklessRingIsolationAlg::
TracklessRingIsolationAlg( const std::string& name,
                           ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name, pSvcLocator )
{
  using namespace boost::assign;
  declareProperty( "InputRings",
                   m_inputRings = LHCb::RichRecRingLocation::MarkovRings );
  declareProperty( "OutputRings",
                   m_outputRings = LHCb::RichRecRingLocation::MarkovRings+"Isolated" );
  declareProperty( "SizeSepCut", m_sizesepcut = list_of(260)(260)(260) );
}

// Destructor
TracklessRingIsolationAlg::~TracklessRingIsolationAlg() {}

StatusCode TracklessRingIsolationAlg::initialize()
{
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  info() << "Input Rings            : " << m_inputRings  << endreq;
  info() << "Output Rings           : " << m_outputRings << endreq;
  info() << "Ring centre cut        : " << m_sizesepcut  << endreq;

  return sc;
}

StatusCode TracklessRingIsolationAlg::execute()
{
  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Load the input rings
  const LHCb::RichRecRings * inrings = get<LHCb::RichRecRings>(m_inputRings);

  // Create a container for the output rings
  LHCb::RichRecRings * outrings = new LHCb::RichRecRings();
  put ( outrings, m_outputRings );

  // Loop over the input rings and select the 'best' ones
  for ( LHCb::RichRecRings::const_iterator iRing = inrings->begin();
        iRing != inrings->end(); ++iRing )
  {
    bool ringIsIsolated(true);

    // Ring2 centre
    const Gaudi::XYZPoint & RingCentreLocal = (*iRing)->centrePointLocal();

    // Loop over rings to compare ring centres
    for ( LHCb::RichRecRings::const_iterator iR2 = inrings->begin();
          iR2 != inrings->end(); ++iR2 )
    {
      if ( (*iRing) == (*iR2) ) continue; // check not comparing the ring with itself

      // Ring2 centre
      const Gaudi::XYZPoint & RingCentreLocal2 = (*iR2)->centrePointLocal();

      // Compare outer loop segment entry to all other segments and find separation
      const double Xdifsq      = std::pow( fabs(RingCentreLocal.x()-RingCentreLocal2.x()), 2 );
      const double Ydifsq      = std::pow( fabs(RingCentreLocal.y()-RingCentreLocal2.y()), 2 );
      const double centreXYdif = std::sqrt(Xdifsq+Ydifsq);

      if ( centreXYdif < m_sizesepcut[(*iRing)->radiator()] )
      {
        debug() << "Too close to another ring !" << endmsg;
        ringIsIsolated = false;
        break;
      }

    } // inner ring loop

    // is ring isolated
    if ( ringIsIsolated )
    {
      // clone and save
      outrings->insert( new LHCb::RichRecRing(**iRing), (*iRing)->key() );
    }

  } // outer ring loop

  return StatusCode::SUCCESS;
}
