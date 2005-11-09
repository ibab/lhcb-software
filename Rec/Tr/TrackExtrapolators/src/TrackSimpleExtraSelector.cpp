// $Id: TrackSimpleExtraSelector.cpp,v 1.1 2005-11-09 14:43:46 erodrigu Exp $

#include "TrackSimpleExtraSelector.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"

// Declaration of the Tool Factory
static const ToolFactory<TrackSimpleExtraSelector> s_factory;
const IToolFactory& TrackSimpleExtraSelectorFactory = s_factory;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackSimpleExtraSelector::TrackSimpleExtraSelector(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent):
  GaudiTool(type, name, parent)
{
  declareInterface<ITrackExtraSelector>( this );

  declareProperty( "ExtrapolatorName",
                   m_extraName = "TrFastParabolicExtrapolator" );
}

//=============================================================================
// Destructor
//=============================================================================
TrackSimpleExtraSelector::~TrackSimpleExtraSelector() {}


//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackSimpleExtraSelector::initialize()
{
  // initialize
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  m_extrapolator= tool<ITrackExtrapolator>( m_extraName, m_extraName, this );

  return StatusCode::SUCCESS;
}

//=============================================================================
// 
//=============================================================================
ITrackExtrapolator* TrackSimpleExtraSelector::select( const double,
                                                      const double) const
{
  return m_extrapolator;
}

//=============================================================================
