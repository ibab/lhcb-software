// $Id: TrackProjector.cpp,v 1.1.1.1 2005-03-31 14:50:18 erodrigu Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrackEvent
#include "Event/State.h"
#include "Event/Measurement.h"

// local
#include "TrackProjectors/TrackProjector.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackProjector
//
// 2005-03-10 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrackProjector>          s_factory ;
const        IToolFactory& TrackProjectorFactory = s_factory ;

//=============================================================================
// Retrieve a node with the results of the (last) projection
//=============================================================================
Node& TrackProjector::node() const
{
  return *m_node;
}

//=============================================================================
// Retrieve the chi squared of the (last) projection
//=============================================================================
double TrackProjector::chi2() const
{
  return m_node -> chi2();
}

//=============================================================================
// Retrieve the residual of the (last) projection
//=============================================================================
double TrackProjector::residual() const
{
  return m_node -> residual();
}

//=============================================================================
// Retrieve the error on the residual of the (last) projection
//=============================================================================
double TrackProjector::errResidual() const
{
  return m_node -> errResidual();
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackProjector::TrackProjector( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_node()
{
  declareInterface<ITrackProjector>( this );
}

//=============================================================================
// Destructor
//=============================================================================
TrackProjector::~TrackProjector() {}; 

//=============================================================================
