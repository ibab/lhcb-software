
// local
#include "ChargedProtoANNPIDTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ChargedProtoANNPIDTool
//
// 2011-02-04 : Chris Jones
//-----------------------------------------------------------------------------

using namespace ANNGlobalPID;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ChargedProtoANNPIDTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoANNPIDTool::ChargedProtoANNPIDTool( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
: ChargedProtoANNPIDToolBase ( type, name, parent )
{
  // interface
  declareInterface<IChargedProtoANNPIDTool>(this);
  // JOs
  declareProperty( "SuppressANNPrintout", m_suppressANNPrintout = true );
  // Turn off Tuple printing during finalize
  setProperty( "NTuplePrint", false );
  //setProperty( "OutputLevel", 1 );
}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoANNPIDTool::~ChargedProtoANNPIDTool() {}

//=============================================================================
// Finalize
//=============================================================================
StatusCode ChargedProtoANNPIDTool::finalize()
{
  // cleanup
  m_annNets.clear();
  // return
  return ChargedProtoANNPIDToolBase::finalize();
}

//=============================================================================

IChargedProtoANNPIDTool::RetType
ChargedProtoANNPIDTool::annPID( const LHCb::ProtoParticle * proto,
                                const LHCb::ParticleID& pid,
                                const std::string& annPIDTune ) const
{
  IChargedProtoANNPIDTool::RetType annPID;

  // are we charged ....
  if ( proto && proto->track() ) 
  {

    // Get the track type
    const auto trackType = LHCb::Track::TypesToString( proto->track()->type() );
    
    // PID type
    const auto pidType = ( pid.abspid() == 11   ? "Electron" :
                           pid.abspid() == 13   ? "Muon"     :
                           pid.abspid() == 211  ? "Pion"     :
                           pid.abspid() == 321  ? "Kaon"     :
                           pid.abspid() == 2212 ? "Proton"   :
                           pid.abspid() == 0    ? "Ghost"    :
                           "UNDEFINED" );
    
    // Get the ANN for the given configuration
    const auto * ann = getANN( trackType, pidType, annPIDTune );
    
    // Get the value of the ANN PID
    if ( ann )
    {
      annPID.status = true;
      annPID.value  = ( ann->passCuts(proto) ? ann->netHelper()->getOutput(proto) : -2 );
    }
    
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << annPIDTune << " " << trackType << " " << pidType 
              << " Status=" << annPID.status << " Value=" << annPID.value
              << endmsg;
    }

  }
  
  // return the value
  return annPID;
}

//=============================================================================

const ChargedProtoANNPIDTool::NetConfig * 
ChargedProtoANNPIDTool::getANN( const std::string & trackType,
                                const std::string & pidType,
                                const std::string & netVersion ) const
{
  // ANN key
  const auto key = trackType+pidType+netVersion;

  // do we already have a network for this config ?
  const auto iANN = m_annNets.find(key);

  // Need to create it
  if ( iANN == m_annNets.end() )
  {
    m_annNets[key].reset( new NetConfig( trackType, pidType, netVersion,
                                         m_suppressANNPrintout, this ) );
    if ( !m_annNets[key]->isOK() )
    {
      Warning( "Problem creating ANNPID network for " + 
               trackType + " " + pidType + " " + netVersion ).ignore();
      m_annNets[key].reset( nullptr );
    }
    return m_annNets[key].get();
  }

  // return it
  return iANN->second.get();
}

//=============================================================================
