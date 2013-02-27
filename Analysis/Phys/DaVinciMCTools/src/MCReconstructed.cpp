//=============================================================================
// $Id: MCReconstructed.cpp,v 1.2 2009-10-19 12:02:17 pkoppenb Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// the data
#include "Event/MCParticle.h"
#include "Event/ProtoParticle.h"

// from
//#include "Relations/RelationWeighted.h"

// from CaloUtils
#include "CaloUtils/ClusterFunctors.h"

// local
#include "MCReconstructed.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCReconstructed
//
// 2004-03-08 : Christian Jacoby
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCReconstructed )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  MCReconstructed::MCReconstructed( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
    : GaudiTool ( type, name , parent )
    , m_pCPPAsct(0)
    , m_pNPPAsct(0)
{
  declareInterface<IMCReconstructed>(this);
}

//=============================================================================
// Destructor
//=============================================================================
MCReconstructed::~MCReconstructed() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode MCReconstructed::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) return sc;

  debug() << "==> MCReconstructed initialize" << endmsg;

  m_pCPPAsct = new ProtoParticle2MCLinker( this, Particle2MCMethod::ChargedPP,
                                           LHCb::ProtoParticleLocation::Charged);
  m_pNPPAsct = new ProtoParticle2MCLinker( this, Particle2MCMethod::NeutralPP, LHCb::ProtoParticleLocation::Neutrals);
  return sc;
}


StatusCode MCReconstructed::finalize()
{
  if( 0 != m_pCPPAsct ) {
    delete m_pCPPAsct;
    m_pCPPAsct = 0;
  }

  if( 0 != m_pNPPAsct ) {
    delete m_pNPPAsct;
    m_pNPPAsct = 0;
  }

  return GaudiTool::finalize();
}



//=============================================================================
//  Method to check if a particle is reconstructed
//  input:  MCParticle
//  output: 0 = not rec'ted (default), 1 = long, 2 = TTT, 3 = VTT, 4=NEUTRAL
//=============================================================================
IMCReconstructed::RecCategory MCReconstructed::reconstructed( const LHCb::MCParticle *iMCstable )
{

  IMCReconstructed::RecCategory rected = IMCReconstructed::NotReconstructed;
  if ( 0==iMCstable ) Exception("NULL MCParticle");

  if (msgLevel(MSG::DEBUG)){
    debug() << "## Checking MCParticle: " << iMCstable->momentum() << " " << iMCstable->particleID().pid() << endmsg;
  }
  if (iMCstable->particleID().threeCharge() != 0) ///< for charged
  {
    LHCb::ProtoParticle* pp = m_pCPPAsct->firstP( iMCstable );
    while( pp )
    {
      if (msgLevel(MSG::DEBUG)) debug() << "    has an associated charged ProtoParticle " << pp
                                        << " " << pp->track() << " " << pp->calo() << endmsg;
      const LHCb::Track *ptrack = pp->track();
      if ( msgLevel(MSG::DEBUG)) {
        if ( ptrack ) debug() << "Track type is " << ptrack->type() << " clone? "
                              << ptrack->checkFlag(LHCb::Track::Clone) << endmsg ;
        else debug() << "No track"  << endmsg ;
      }
      if( ptrack && ! ptrack->checkFlag(LHCb::Track::Clone) ){
        switch( ptrack->type() )
        {
        case LHCb::Track::Long        : rected = IMCReconstructed::ChargedLong; break;
        case LHCb::Track::Downstream  : rected = IMCReconstructed::ChargedDownstream; break;
        case LHCb::Track::Upstream    : rected = IMCReconstructed::ChargedUpstream; break;

          // Other enum values not covered by the original logic
        case LHCb::Track::TypeUnknown :
        case LHCb::Track::Velo        :
        case LHCb::Track::VeloR       :
        case LHCb::Track::Ttrack      :
        case LHCb::Track::Muon        :
        default                       : break;
        }

        if (msgLevel(MSG::DEBUG)) debug() << "      rected = " << rected << endmsg;

        break;
      }

      pp = m_pCPPAsct->nextP();
    }
  }
  else ///< for neutrals
  {
    LHCb::ProtoParticle* pp = m_pNPPAsct->firstP( iMCstable );
    if( pp )
    {
      if (msgLevel(MSG::DEBUG)) debug() << "    has an associated neutral ProtoParticle." << endmsg;

      rected = IMCReconstructed::Neutral;

      // pp = m_pNPPAsct->nextP();
    }

    if (msgLevel(MSG::DEBUG)) debug() << "      rected = " << rected << endmsg;
  }


  if (msgLevel(MSG::DEBUG)) debug() << "  returning " << IMCReconstructed::text(rected) << endmsg;

  return rected;

}

//=============================================================================
