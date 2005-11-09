// $Id: TrackLongExtraSelector.cpp,v 1.1 2005-11-09 14:43:46 erodrigu Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"

// from CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Local
#include "TrackLongExtraSelector.h"
 
// Declaration of the Tool Factory
static const ToolFactory<TrackLongExtraSelector> s_factory;
const IToolFactory& TrackLongExtraSelectorFactory = s_factory;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackLongExtraSelector::TrackLongExtraSelector( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent ):
  GaudiTool(type, name, parent)
{
  declareProperty( "ShortDist",
                   m_shortDist = 100.0*mm );
  declareProperty( "ShortFieldExtrapolatorName",
                   m_shortFieldExtrapolatorName = "TrFastParabolicExtrapolator ");
  declareProperty( "LongFieldExtrapolatorName",
                   m_longFieldExtrapolatorName  = "TrHerabExtrapolator" );
  declareProperty( "ShortFieldExtrapolatorType",
                   m_shortFieldExtrapolatorType = "TrFastParabolicExtrapolator") ;
  declareProperty( "LongFieldExtrapolatorType",
                   m_longFieldExtrapolatorType  = "TrHerabExtrapolator" );

 declareInterface<ITrackExtraSelector>( this );
  
}

//=============================================================================
// Destructor
//=============================================================================
TrackLongExtraSelector::~TrackLongExtraSelector() {}

StatusCode TrackLongExtraSelector::initialize()
{
  // initialize
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize", sc );

  // request a short distance magnetic field extrapolator
  m_shortFieldExtrapolator = tool<ITrackExtrapolator>( m_shortFieldExtrapolatorType,
                                                       m_shortFieldExtrapolatorName,
                                                       this );

  // request extrapolator for going short distances in magnetic field
  m_longFieldExtrapolator = tool<ITrackExtrapolator>( m_longFieldExtrapolatorType,
                                                      m_longFieldExtrapolatorName,
                                                      this );
 
  return StatusCode::SUCCESS;
}

//=============================================================================
//
//=============================================================================
ITrackExtrapolator* TrackLongExtraSelector::select( const double zStart,
                                                    const double zEnd ) const
{
  ITrackExtrapolator* tExtra;
  if ( zEnd < m_startT ){ 
    tExtra = m_shortFieldExtrapolator;
  }
  else {
    tExtra = (fabs(zEnd-zStart) < m_shortDist ?  m_shortFieldExtrapolator : m_longFieldExtrapolator );
  }

  return tExtra;
}

//=============================================================================
