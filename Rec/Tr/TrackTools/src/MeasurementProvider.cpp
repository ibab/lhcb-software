// $Id: MeasurementProvider.cpp,v 1.40 2009-07-08 14:14:52 wouter Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// Event model
#include "TrackKernel/TrackTraj.h"

// local
#include "MeasurementProvider.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : MeasurementProvider
//
// 2005-04-14 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( MeasurementProvider );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MeasurementProvider::MeasurementProvider( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent ) 
  : GaudiTool ( type, name , parent ),
    m_veloRProvider(  "MeasurementProviderT<MeasurementProviderTypes::VeloR>/VeloRMeasurementProvider" ),
    m_veloPhiProvider("MeasurementProviderT<MeasurementProviderTypes::VeloPhi>/VeloPhiMeasurementProvider" ),
    m_ttProvider(     "MeasurementProviderT<MeasurementProviderTypes::TT>/TTMeasurementProvider" ),
    m_itProvider(     "MeasurementProviderT<MeasurementProviderTypes::IT>/ITMeasurementProvider" ),
    m_otProvider(     "OTMeasurementProvider" ),
    m_muonProvider(   "MuonMeasurementProvider" )
{
  declareInterface<IMeasurementProvider>(this);
  declareProperty( "IgnoreVelo", m_ignoreVelo = false );
  declareProperty( "IgnoreTT",   m_ignoreTT   = false );
  declareProperty( "IgnoreIT",   m_ignoreIT   = false );
  declareProperty( "IgnoreOT",   m_ignoreOT   = false );
  declareProperty( "IgnoreMuon", m_ignoreMuon = false );
  declareProperty( "InitializeReference", m_initializeReference  = true ) ;

  declareProperty( "VeloRProvider", m_veloRProvider ) ;
  declareProperty( "VeloPhiProvider", m_veloPhiProvider ) ;
  declareProperty( "TTProvider", m_ttProvider ) ;
  declareProperty( "ITProvider", m_itProvider ) ;
  declareProperty( "OTProvider", m_otProvider ) ;
  declareProperty( "MuonProvider", m_muonProvider ) ;
}

//=============================================================================
// Destructor
//=============================================================================
MeasurementProvider::~MeasurementProvider() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode MeasurementProvider::initialize() 
{
  debug() << "MeasurementProvider::initialize()" << endmsg;
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class

  m_providermap.clear() ;
  m_providermap.resize(LHCb::Measurement::Muon+1,0) ;

  if(!m_ignoreVelo) {
    sc = m_veloRProvider.retrieve() ;
    if (sc.isFailure()) return sc;  
    m_providermap[LHCb::Measurement::VeloR] = &(*m_veloRProvider) ;
   
    sc = m_veloPhiProvider.retrieve() ;
    if (sc.isFailure()) return sc; 
    m_providermap[LHCb::Measurement::VeloPhi] = &(*m_veloPhiProvider) ;
  }

  if(!m_ignoreTT) {
    sc = m_ttProvider.retrieve() ;
    if (sc.isFailure()) return sc; 
    m_providermap[LHCb::Measurement::TT] = &(*m_ttProvider) ;
  }
  
  if(!m_ignoreIT) {
    sc = m_itProvider.retrieve() ;
    if (sc.isFailure()) return sc;    
    m_providermap[LHCb::Measurement::IT] = &(*m_itProvider) ;
  }

  if(!m_ignoreOT) {
    sc = m_otProvider.retrieve() ;
    if (sc.isFailure()) return sc; 
    m_providermap[LHCb::Measurement::OT] = &(*m_otProvider) ;
  }

  if(!m_ignoreMuon) {
    sc = m_muonProvider.retrieve() ;
    if (sc.isFailure()) return sc; 
    m_providermap[LHCb::Measurement::Muon] = &(*m_muonProvider) ;
  }
  return sc ;
}

//=============================================================================
// Load all the Measurements from the list of LHCbIDs on the input Track
//=============================================================================
StatusCode MeasurementProvider::load( Track& track ) const
{ 
  const std::vector<LHCbID>& ids = track.lhcbIDs();

  std::vector<LHCbID> newids ;
  for ( std::vector<LHCbID>::const_iterator it = ids.begin();
        it != ids.end(); ++it ) {
    const LHCbID& id = *it;
	 // First look if the Measurement corresponding to this LHCbID
	 // is already in the Track, i.e. whether it has already been loaded!
    if ( track.isMeasurementOnTrack( id ) ) {
      Warning("Found measurements already loaded on track!",StatusCode::SUCCESS,0) ;
      if( msgLevel( MSG::DEBUG ) || msgLevel( MSG::VERBOSE ) )
	debug() << "Measurement had already been loaded for the LHCbID"
		<< " channelID, detectorType = "
		<< id.channelID() << " , " << id.detectorType()
		<< "  -> Measurement loading skipped for this LHCbID!"
		<< endreq;
    } else newids.push_back( id ) ;
  }
  
  // create a reference trajectory
  LHCb::TrackTraj reftraj(track) ;
  
  // create all measurements for selected IDs
  LHCb::Track::MeasurementContainer newmeasurements ;
  addToMeasurements(newids,newmeasurements,reftraj) ;
  
  // remove all zeros, just in case.
  LHCb::Track::MeasurementContainer::iterator newend 
    = std::remove_if(newmeasurements.begin(),newmeasurements.end(),
                     std::bind2nd(std::equal_to<LHCb::Measurement*>(),static_cast<LHCb::Measurement*>(0))) ;
  if(newend != newmeasurements.end()) {
    warning() << "Some measurement pointers are zero: " << int(newmeasurements.end() - newend) << endreq ;
    newmeasurements.erase(newend,newmeasurements.end()) ;
  }
  
  // add the measurements to the track
  track.addToMeasurements( newmeasurements ) ;
  
  // Update the status flag of the Track
  track.setPatRecStatus( Track::PatRecMeas );
  
  return StatusCode::SUCCESS;
}

inline LHCb::Measurement::Type measurementtype(const LHCb::LHCbID& id)
{
  LHCb::Measurement::Type rc = LHCb::Measurement::Unknown ;
  switch( id.detectorType() ) {
  case LHCb::LHCbID::Velo: 
    rc = id.isVeloR() ? LHCb::Measurement::VeloR : LHCb::Measurement::VeloPhi ;
    break ;
  case LHCb::LHCbID::TT:   rc = LHCb::Measurement::TT ; break ;
  case LHCb::LHCbID::IT:   rc = LHCb::Measurement::IT ; break ;
  case LHCb::LHCbID::OT:   rc = LHCb::Measurement::OT ; break ;
  case LHCb::LHCbID::Muon: rc = LHCb::Measurement::Muon ; break ;
  default: {}
  }
  return rc ;
}

//=============================================================================
// Construct a Measurement of the type of the input LHCbID
//=============================================================================

Measurement* MeasurementProvider::measurement ( const LHCbID& id, bool localY ) const
{
  const IMeasurementProvider* provider = m_providermap[measurementtype( id )] ;
  return provider ? provider->measurement(id, localY) : 0 ; 
}

Measurement* MeasurementProvider::measurement ( const LHCbID& id, const LHCb::ZTrajectory& state, bool localY ) const
{
  const IMeasurementProvider* provider = m_providermap[measurementtype( id )] ;
  return provider ? provider->measurement(id,state, localY) : 0 ; 
}

//-----------------------------------------------------------------------------
/// Create a list of measurements from a list of LHCbIDs
//-----------------------------------------------------------------------------
void MeasurementProvider::addToMeasurements( const std::vector<LHCb::LHCbID>& ids,
                                             std::vector<LHCb::Measurement*>& measurements,
                                             const LHCb::ZTrajectory& tracktraj) const 
{
  // map the ids to measurement-providers
  std::vector<std::vector<LHCb::LHCbID> > idsbytype(m_providermap.size());
  for ( std::vector<LHCbID>::const_iterator it = ids.begin(); it != ids.end(); ++it )
    idsbytype[measurementtype( *it )].push_back( *it );
  // now call all the providers
  for( size_t i = 0; i<m_providermap.size(); ++i)
    if( m_providermap[i] && !idsbytype[i].empty()) 
      m_providermap[i]->addToMeasurements( idsbytype[i], measurements, tracktraj ) ;
}

//-----------------------------------------------------------------------------
/// Get the z-position of a measurement with this ID
//-----------------------------------------------------------------------------
double MeasurementProvider::nominalZ( const LHCb::LHCbID& id ) const 
{
  const IMeasurementProvider* provider = m_providermap[measurementtype( id )] ;
  return provider ? provider->nominalZ(id) : 0 ; 
}

//=============================================================================
