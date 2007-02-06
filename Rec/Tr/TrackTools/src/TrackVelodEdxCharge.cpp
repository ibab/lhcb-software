
//-----------------------------------------------------------------------------
/** @file TrackVelodEdxCharge.cpp
 *
 * Implementation file for tool TrackVelodEdxCharge
 *
 * CVS Log :-
 * $Id: TrackVelodEdxCharge.cpp,v 1.2 2007-02-06 13:15:36 cattanem Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 18/07/2006
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"
#include "TrackVelodEdxCharge.h"

// namespaces
using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrackVelodEdxCharge );

//-----------------------------------------------------------------------------

TrackVelodEdxCharge::TrackVelodEdxCharge(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent) :
  GaudiTool ( type, name, parent )
{
  // tool interface
  declareInterface<ITrackVelodEdxCharge>(this);
  // job options
  declareProperty( "Normalisation", m_Normalisation = 22.4 );
  declareProperty( "Ratio",         m_Ratio         = 0.6  );
}

TrackVelodEdxCharge::~TrackVelodEdxCharge(){
}

StatusCode TrackVelodEdxCharge::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  info() << "VELO dE/dx charge parameters :"
         << " normalisation = " << m_Normalisation
         << " ratio = " << m_Ratio <<endreq;

  return sc;
}

StatusCode TrackVelodEdxCharge::finalize()
{
  // finalise base class and return
  return GaudiTool::finalize();
}

StatusCode TrackVelodEdxCharge::nTracks( const LHCb::Track * track,
                                         double & nTks ) const
{
  // initialise the charge value
  nTks = 0;
  // is track OK
  if ( !track ) return Warning( "Null track pointer passed" );

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Trying Track " << track->key() << endreq;

  // get the measurements for the track
  const TkMeas & measurements = track->measurements();
  if ( measurements.empty() ) return Warning( "Track has no Measurements" );

  if ( msgLevel(MSG::DEBUG) )
    debug() << " -> Found " << measurements.size() << " Measurements" << endreq;

  // Fill a temp vector with the charges
  std::vector<double> totalSignals;

  // loop over measurements
  for ( TkMeas::const_iterator iM = measurements.begin();
        iM != measurements.end(); ++iM )
  {
    // pointer to the final velo cluster
    const VeloCluster * clus ( NULL );

    // is this a velo phi measurement
    if       ( const VeloPhiMeasurement * mPhi =
               dynamic_cast<VeloPhiMeasurement*>(*iM) )
    {
      clus = mPhi->cluster();
    }
    // or, is it a velo r measurement
    else if ( const VeloRMeasurement * mR =
              dynamic_cast<VeloRMeasurement*>(*iM) )
    {
      clus = mR->cluster();
    }

    // did we find a cluster ?
    if ( clus )
    {
      // get total charge for this cluster
      totalSignals.push_back( clus->totalCharge() );
    }

  } // loop over measurements

  // how many charges where found
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "  -> Found " << totalSignals.size() << " charge values : "
            << totalSignals << endreq;
  }
  if ( !totalSignals.empty() )
  {
    const unsigned int reducedSize =
      (unsigned int)ceil(m_Ratio*(double)totalSignals.size());
    std::nth_element( totalSignals.begin(), totalSignals.begin()+reducedSize, totalSignals.end() );
    nTks = ( std::accumulate( totalSignals.begin(), totalSignals.begin()+reducedSize, (double)0.0 )
             / ((double)reducedSize*m_Normalisation) );
    if ( msgLevel(MSG::DEBUG) )
      debug() << "   -> Number of tracks deduced = " << nTks << endreq;
  }
  else
  {
    // no velo clusters found
    if ( msgLevel(MSG::DEBUG) )
      debug() << "   -> No VELO clusters found -> no dE/dx charge measured" << endreq;

    return StatusCode::FAILURE;
  }

  // if we get here, all is OK
  return StatusCode::SUCCESS;
}
