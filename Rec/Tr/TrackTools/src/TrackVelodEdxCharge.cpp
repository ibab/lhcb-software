
//-----------------------------------------------------------------------------
/** @file TrackVelodEdxCharge.cpp
 *
 * Implementation file for tool TrackVelodEdxCharge
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 18/07/2006
 */
//-----------------------------------------------------------------------------

#include "TrackVelodEdxCharge.h"
#include "Event/VeloCluster.h"
#include "Kernel/SiChargeFun.h"
#include "Event/VeloMeasurement.h"
#include "Event/VeloLiteMeasurement.h"

// namespaces
using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrackVelodEdxCharge )

//-----------------------------------------------------------------------------

TrackVelodEdxCharge::TrackVelodEdxCharge(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent) :
GaudiTool ( type, name, parent ),
  m_totalTracks(0),
  m_veloTracks(0),
  m_sumEffective(0.)
{
  // tool interface
  declareInterface<ITrackVelodEdxCharge>(this);
  // job options
  declareProperty( "Normalisation", m_Normalisation = 47.1 );
  declareProperty( "Ratio",         m_Ratio         = 0.6  );
  declareProperty( "UseConditions", m_useConditions  = true );
}

TrackVelodEdxCharge::~TrackVelodEdxCharge(){
}

StatusCode TrackVelodEdxCharge::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  if( !existDet<DataObject>(detSvc(),"Conditions/ParticleID/Velo/VelodEdx") ){
    Warning("VELO dE/dx parameters not in conditions DB",
            StatusCode::SUCCESS).ignore();
    m_useConditions = false;
  }
  if( !m_useConditions ){
    Warning("Using VELO dE/dx parameters from options not conditions",
            StatusCode::SUCCESS).ignore();
  }else{
    registerCondition("Conditions/ParticleID/Velo/VelodEdx",m_dEdx,
                      &TrackVelodEdxCharge::i_cachedEdx);
    sc = runUpdate();
    if(!sc) return sc;
  }

  info() << "VELO dE/dx charge parameters :"
         << " normalisation = " << m_Normalisation
         << " ratio = " << m_Ratio <<endmsg;

  return StatusCode::SUCCESS;
}

StatusCode TrackVelodEdxCharge::nTracks( const LHCb::Track * track,
                                         double & nTks ) const
{
  ++m_totalTracks;
  // initialise the charge value
  nTks = 0;
  // is track OK
  if ( !track ) return Warning( "Null track pointer passed" );

  if ( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "Trying Track " << track->key() << endmsg;

  // get the measurements for the track
  typedef LHCb::Track::MeasurementContainer TkMeas ;
  const TkMeas measurements = track->measurements();

  bool useIDs = measurements.empty();
  if( !useIDs ){
    // Fill a temp vector with the velo clusters
    std::vector<const LHCb::VeloMeasurement*> veloMeas; veloMeas.reserve(32);
    // loop over measurements
    for ( TkMeas::const_iterator iM = measurements.begin();
          iM != measurements.end(); ++iM ){
      // is this a velo measurement
      if ( const VeloMeasurement* mVelo =
           dynamic_cast<const VeloMeasurement*>(*iM) ) {
        veloMeas.push_back(mVelo);
      }
      if ( dynamic_cast<const VeloLiteMeasurement*>(*iM) ) {
        // have measurements, but no pointers to clusters :(
        useIDs = true;
        break;
      }
    } // loop over measurements
    if( !useIDs ){
      // how many charges where found
      if ( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << " -> Found " << measurements.size()
                << " Measurements, used " << veloMeas.size()<< endmsg;
      if ( !veloMeas.empty() ){
        nTks = SiChargeFun::truncatedMean(veloMeas.begin(),veloMeas.end(), m_Ratio )/m_Normalisation;
        ++m_veloTracks;
        m_sumEffective += nTks;
      }else{
        // no velo clusters found
        if ( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug() << "   -> No VELO clusters found -> no dE/dx charge measured" << endmsg;
        return StatusCode::FAILURE;
      }
      // if we get here, all is OK
      return StatusCode::SUCCESS;
    }
  }
  // fall back option of getting the clusters from the container by LHCbID
  LHCb::VeloClusters *veloClusters =
    get<LHCb::VeloClusters*>(LHCb::VeloClusterLocation::Default);
  // try to load clusters from lhcbids
  std::vector<const LHCb::VeloCluster*> clusters;
  for( auto id : track->lhcbIDs() ){
    if( id.isVelo() ) clusters.push_back(veloClusters->object(id.veloID()));
  }
  if ( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << " -> Found " << track->lhcbIDs().size()
            << " LHCbIDs, used " << clusters.size() << endmsg;
  if(!clusters.empty()) {
    nTks = SiChargeFun::truncatedMean(clusters.begin(),clusters.end(),
                                      m_Ratio)/m_Normalisation;
    ++m_veloTracks;
    m_sumEffective += nTks;
  }else{
    nTks = 0.;
    if ( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "   -> No VELO clusters found -> no dE/dx charge measured" << endmsg;
    return StatusCode::FAILURE;
  }
  // if we get here, all is OK
  return StatusCode::SUCCESS;
}

StatusCode TrackVelodEdxCharge::i_cachedEdx(){
  m_Normalisation = m_dEdx->param<double>("Normalisation");
  m_Ratio = m_dEdx->param<double>("Ratio");
  if ( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "i_cachedEdx : Normalisation " << m_Normalisation
            << " Ratio " << m_Ratio << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode TrackVelodEdxCharge::finalize() {
  if( m_veloTracks > 0 ){
    info() << "Total tracks " << m_totalTracks
           << ", with VELO " << m_veloTracks
           << ", average effective number " << m_sumEffective/(double)m_veloTracks
           << endmsg;
  }
  return GaudiTool::finalize();  // must be called after all other actions
}
