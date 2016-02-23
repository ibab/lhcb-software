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
  : TupleToolBase ( type, name, parent )
{
  declareInterface<IParticleTupleTool>(this);
  declareProperty( "ANNPIDTunes", m_pidTunes = 
      { "MC12TuneV2","MC12TuneV3","MC12TuneV4","MC15TuneV1" } 
                   );
}

//=============================================================================

StatusCode TupleToolANNPID::initialize()
{
  const StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  m_pidTool =
    tool<ANNGlobalPID::IChargedProtoANNPIDTool>( "ANNGlobalPID::ChargedProtoANNPIDTool",
                                                 "ChargedProtoANNPID" );

  info() << "PID Tunes = " << m_pidTunes << endmsg;

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
    // Fill the ANNPID variables for those that are defined
    ANNGlobalPID::IChargedProtoANNPIDTool::RetType res;
    // Electrons
    res = m_pidTool->annPID(P->proto(),LHCb::ParticleID(11),pidTune);
    if ( res.status ) { sc = sc && tuple->column( prefix+"_"+pidTune+"_ProbNNe", res.value ); }
    // Muons
    res = m_pidTool->annPID(P->proto(),LHCb::ParticleID(13),pidTune);
    if ( res.status ) { sc = sc && tuple->column( prefix+"_"+pidTune+"_ProbNNmu", res.value ); }
    // Pions
    res = m_pidTool->annPID(P->proto(),LHCb::ParticleID(211),pidTune);
    if ( res.status ) { sc = sc && tuple->column( prefix+"_"+pidTune+"_ProbNNpi", res.value ); }
    // Kaons
    res = m_pidTool->annPID(P->proto(),LHCb::ParticleID(321),pidTune);
    if ( res.status ) { sc = sc && tuple->column( prefix+"_"+pidTune+"_ProbNNk", res.value ); }
    // Protons
    res = m_pidTool->annPID(P->proto(),LHCb::ParticleID(2212),pidTune);
    if ( res.status ) { sc = sc && tuple->column( prefix+"_"+pidTune+"_ProbNNp", res.value ); }
    // Ghosts
    res = m_pidTool->annPID(P->proto(),LHCb::ParticleID(0),pidTune);
    if ( res.status ) { sc = sc && tuple->column( prefix+"_"+pidTune+"_ProbNNghost", res.value ); }
  }
  
  // return
  return sc;
}

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolANNPID )
