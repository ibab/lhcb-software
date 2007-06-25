// $Id: MeasurementProvider.cpp,v 1.32 2007-06-25 14:35:40 mneedham Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// Event model
#include "Event/StateVector.h"

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
                                          const IInterface* parent ) : GaudiTool ( type, name , parent ),
    m_veloRProvider(  "MeasurementProviderT<MeasurementProviderTypes::VeloR>/VeloRMeasurementProvider" ),
    m_veloPhiProvider("MeasurementProviderT<MeasurementProviderTypes::VeloPhi>/VeloPhiMeasurementProvider" ),
    m_ttProvider(     "MeasurementProviderT<MeasurementProviderTypes::TT>/TTMeasurementProvider" ),
    m_itProvider(     "MeasurementProviderT<MeasurementProviderTypes::IT>/ITMeasurementProvider" ),
    m_otProvider(     "MeasurementProviderT<MeasurementProviderTypes::OT>/OTMeasurementProvider" ),
    m_muonProvider(   "MuonMeasurementProvider" )
{
  declareInterface<IMeasurementProvider>(this);
  declareProperty( "IgnoreVelo", m_ignoreVelo = false );
  declareProperty( "IgnoreTT",   m_ignoreTT   = false );
  declareProperty( "IgnoreIT",   m_ignoreIT   = false );
  declareProperty( "IgnoreOT",   m_ignoreOT   = false );
  declareProperty( "IgnoreMuon", m_ignoreMuon = false );
  declareProperty( "InitializeReference", m_initializeReference  = false ) ;
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
  /// Some of the long tracks do not have a momentum assigned to the
  /// velo states. Therefore, we need to keep track of the momentum.
  bool hasTState = track.hasStateAt(LHCb::State::AtT) ;
  LHCb::State tState;
  if(hasTState) tState = track.stateAt(LHCb::State::AtT);
  
  const std::vector<LHCbID>& ids = track.lhcbIDs();
  for ( std::vector<LHCbID>::const_iterator it = ids.begin();
        it != ids.end(); ++it ) {
    const LHCbID& id = *it;
	 // First look if the Measurement corresponding to this LHCbID
	 // is already in the Track, i.e. whether it has already been loaded!
    if ( track.isMeasurementOnTrack( id ) ) {
      warning() << "Measurement had already been loaded for the LHCbID"
		<< " channelID, detectorType = "
		<< id.channelID() << " , " << id.detectorType()
		<< "  -> Measurement loading skipped for this LHCbID!"
		<< endreq;
		continue;
    }
    Measurement* meas = measurement( id );
    if( meas ) {
      if( m_initializeReference ) {
	// Of course we would prefer to call directly the constructor
	// with reference, but unfortunatel we first need a generic
	// way to get the z-position of an lhcb id.  Note:
	// extrapolation is done by measurementproviders.
	LHCb::State state = track.closestState( meas->z() ) ;
	if( hasTState ) state.setQOverP(tState.qOverP());
	m_providermap[meas->type()]->update( *meas, LHCb::StateVector(state.stateVector(),state.z()) ) ;
     }
      track.addToMeasurements( *meas );
      delete meas;
    }
  }
  // Update the status flag of the Track
  track.setPatRecStatus( Track::PatRecMeas );

  if ( track.nLHCbIDs() != track.nMeasurements() )
    warning() << "-> Track (key=" << track.key()
              << "): loaded successfully only " << track.nMeasurements()
              << " Measurements out of " << track.nLHCbIDs()
              << " LHCbIDs!" << endreq;

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

Measurement* MeasurementProvider::measurement ( const LHCbID& id ) const
{
  const IMeasurementProvider* provider = m_providermap[measurementtype( id )] ;
  return provider ? provider->measurement(id) : 0 ; 
}

Measurement* MeasurementProvider::measurement ( const LHCbID& id, const LHCb::StateVector& state ) const
{
  const IMeasurementProvider* provider = m_providermap[measurementtype( id )] ;
  return provider ? provider->measurement(id,state) : 0 ; 
}

StatusCode  MeasurementProvider::update( LHCb::Measurement& m, const LHCb::StateVector& refvector ) const 
{
  const IMeasurementProvider* provider = m_providermap[m.type()] ;
  return provider ? provider->update(m,refvector) : StatusCode::FAILURE ; 
}

double MeasurementProvider::nominalZ( const LHCb::LHCbID& id ) const 
{
  const IMeasurementProvider* provider = m_providermap[measurementtype( id )] ;
  return provider ? provider->nominalZ(id) : 0 ; 
}

//=============================================================================
