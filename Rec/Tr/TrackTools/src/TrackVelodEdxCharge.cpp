
//-----------------------------------------------------------------------------
/** @file TrackVelodEdxCharge.cpp
 *
 * Implementation file for tool TrackVelodEdxCharge
 *
 * CVS Log :-
 * $Id: TrackVelodEdxCharge.cpp,v 1.4 2008-12-01 16:43:59 mneedham Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 18/07/2006
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"
#include "TrackVelodEdxCharge.h"
#include "Event/VeloCluster.h"
#include "Kernel/SiChargeFun.h"

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

  // Fill a temp vector with the velo clusters
  std::vector<const LHCb::VeloMeasurement*> veloMeas; veloMeas.reserve(32);

  // loop over measurements
  for ( TkMeas::const_iterator iM = measurements.begin();
        iM != measurements.end(); ++iM )
  {
  
    // is this a velo phi measurement
    if       ( const VeloMeasurement* mVelo =
               dynamic_cast<VeloMeasurement*>(*iM) )
    {
       veloMeas.push_back(mVelo);
    }
  } // loop over measurements

  // how many charges where found
  if ( msgLevel(MSG::DEBUG) ){
    debug() << "  -> Found " << veloMeas.size() << endreq;
  }

  if ( !veloMeas.empty() ){
    nTks = SiChargeFun::truncatedMean(veloMeas.begin(),veloMeas.end(), m_Ratio )/m_Normalisation;
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
