// $Id: $

#include "TupleToolANNPID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolANNPID
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolANNPID::TupleToolANNPID( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : TupleToolBase ( type, name, parent ),
    m_pidTool     ( NULL )
{
  declareInterface<IParticleTupleTool>(this);
  declareProperty( "ANNPIDTunes", m_pidTunes = {"MC12TuneV2","MC12TuneV3"} );
}

//=============================================================================

StatusCode TupleToolANNPID::initialize()
{
  const StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  m_pidTool =
    tool<ANNGlobalPID::IChargedProtoANNPIDTool>( "ANNGlobalPID::ChargedProtoANNPIDTool",
                                                 "ChargedProtoANNPID" );

  return sc;
}

//=============================================================================

StatusCode TupleToolANNPID::fill( const LHCb::Particle*
                                  , const LHCb::Particle* P
                                  , const std::string& head
                                  , Tuples::Tuple& tuple )
{
  StatusCode sc = StatusCode::SUCCESS;

  // Only run on charged stable particles
  if ( !P || !P->proto() || !P->proto()->track() ) return sc;

  // get the prefix
  const std::string prefix = fullName(head);

  // Loop over PID tunes
  for ( const auto& pidTune : m_pidTunes )
  {
    // Fill the ANNPID variables
    sc = sc && tuple->column( prefix+"_"+pidTune+"_ProbNNe", 
                              m_pidTool->annPID(P->proto(),LHCb::ParticleID(11),pidTune) );
    sc = sc && tuple->column( prefix+"_"+pidTune+"_ProbNNmu", 
                              m_pidTool->annPID(P->proto(),LHCb::ParticleID(13),pidTune) );
    sc = sc && tuple->column( prefix+"_"+pidTune+"_ProbNNpi", 
                              m_pidTool->annPID(P->proto(),LHCb::ParticleID(211),pidTune) );
    sc = sc && tuple->column( prefix+"_"+pidTune+"_ProbNNk", 
                              m_pidTool->annPID(P->proto(),LHCb::ParticleID(321),pidTune) );
    sc = sc && tuple->column( prefix+"_"+pidTune+"_ProbNNp", 
                              m_pidTool->annPID(P->proto(),LHCb::ParticleID(2212),pidTune) );
    sc = sc && tuple->column( prefix+"_"+pidTune+"_ProbNNghost", 
                              m_pidTool->annPID(P->proto(),LHCb::ParticleID(0),pidTune) ); 
  }
  
  // return
  return sc;
}

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolANNPID )
