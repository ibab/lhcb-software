// $Id: TrackProjectorSelector.cpp,v 1.1 2006-12-15 19:11:38 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrackEvent
#include "Event/Measurement.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackProjector.h"

// local
#include "TrackProjectorSelector.h"

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrackProjectorSelector );
//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
TrackProjectorSelector::TrackProjectorSelector( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackProjectorSelector>( this );
  //FIXME: as soon as the warnings in GaudiAlg on multiple tools are gone, we 
  //       can remove the different names for the 
  declareProperty( "VeloR",   m_projNames[Measurement::VeloR]   = "TrajProjector<Velo>/VeloR" );
  declareProperty( "VeloPhi", m_projNames[Measurement::VeloPhi] = "TrajProjector<Velo>/VeloPhi" );
  declareProperty( "TT",      m_projNames[Measurement::TT]      = "TrajProjector<ST>/TT" );
  declareProperty( "IT",      m_projNames[Measurement::IT]      = "TrajProjector<ST>/IT" );
  declareProperty( "OT",      m_projNames[Measurement::OT]      = "TrajOTProjector" );
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrackProjectorSelector::~TrackProjectorSelector() {};

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------
StatusCode TrackProjectorSelector::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize!", sc );

  m_projectors.clear();
  for (ProjectorNames::const_iterator i=m_projNames.begin();i!=m_projNames.end();++i) { 
     m_projectors.insert(i->first, tool<ITrackProjector>( i->second ));
     debug() << " projector for " << i->first << " : " << i->second << endmsg;
  }
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// select the projector; 
/// TODO: return an object which represents the binding of the measurement 
///       and projector (taking care of any downcasting here, when creating
///       such an object)
//-----------------------------------------------------------------------------
ITrackProjector* TrackProjectorSelector::projector( const LHCb::Measurement& m ) const
{
  Projectors::const_iterator i = m_projectors.find(m.type());
  if ( i == m_projectors.end() || i->second ==0 ) {
    warning() << "No projector in for measurement of type " << m.type() << "!";
    return 0;
  }
  return i->second;
}
