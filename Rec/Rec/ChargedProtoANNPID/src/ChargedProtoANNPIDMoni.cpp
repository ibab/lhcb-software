// $Id: $
// Include files

// local
#include "ChargedProtoANNPIDMoni.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ChargedProtoANNPIDMoni
//
// 2012-01-12 : Chris Jones
//-----------------------------------------------------------------------------

using namespace ANNGlobalPID;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoANNPIDMoni )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoANNPIDMoni::ChargedProtoANNPIDMoni( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : ChargedProtoANNPIDAlgBase ( name , pSvcLocator )
{ 
  // histo base dir
  setProperty ( "HistoTopDir", "PROTO/" );
  // print some stats on the final plots
  setProperty ( "HistoPrint", true ); 
}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoANNPIDMoni::~ChargedProtoANNPIDMoni() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoANNPIDMoni::execute()
{
  // Load the charged ProtoParticles
  const auto * protos = getIfExists<LHCb::ProtoParticles>( m_protoPath );
  if ( !protos ) return Warning( "No ProtoParticles at '" + m_protoPath + "'", 
                                 StatusCode::SUCCESS );

  // Loop over ProtoParticles
  for ( const auto * P : *protos )
  {

    // Skip velo tracks (never have ANN PID information)
    if ( P->track()->type() == LHCb::Track::Velo  ||
         P->track()->type() == LHCb::Track::VeloR  ) { continue; }

    // Get the track type
    const auto type = LHCb::Track::TypesToString( P->track()->type() );

    // Fill plots
    plot1D( P->info( LHCb::ProtoParticle::ProbNNe, 0 ),
            type+"/ElectronANN", type+" Electron ANN PID", 
            0.0, 1.0, 100 );
    plot1D( P->info( LHCb::ProtoParticle::ProbNNmu, 0 ),
            type+"/MuonANN", type+" Muon ANN PID", 
            0.0, 1.0, 100 );
    plot1D( P->info( LHCb::ProtoParticle::ProbNNpi, 0 ),
            type+"/PionANN", type+" Pion ANN PID", 
            0.0, 1.0, 100 );
    plot1D( P->info( LHCb::ProtoParticle::ProbNNk, 0 ),
            type+"/KaonANN", type+" Kaon ANN PID", 
            0.0, 1.0, 100 );
    plot1D( P->info( LHCb::ProtoParticle::ProbNNp, 0 ),
            type+"/ProtonANN", type+" Proton ANN PID", 
            0.0, 1.0, 100 );
    plot1D( P->info( LHCb::ProtoParticle::ProbNNghost, 0 ),
            type+"/GhostANN", type+" Ghost ANN PID", 
            0.0, 1.0, 100 );

  }

  return StatusCode::SUCCESS;
}

//=============================================================================
