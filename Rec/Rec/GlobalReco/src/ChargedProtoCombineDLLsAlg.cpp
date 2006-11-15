
//-----------------------------------------------------------------------------
/** @file ChargedProtoCombineDLLsAlg.cpp
 *
 * Implementation file for algorithm ChargedProtoCombineDLLsAlg
 *
 * CVS Log :-
 * $Id: ChargedProtoCombineDLLsAlg.cpp,v 1.1 2006-11-15 13:46:29 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 15/11/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "ChargedProtoCombineDLLsAlg.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoCombineDLLsAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoCombineDLLsAlg::ChargedProtoCombineDLLsAlg( const std::string& name,
                                                        ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator )
{
  // Job Options
  declareProperty( "ProtoParticleLocation",
                   m_protoPath = LHCb::ProtoParticleLocation::Charged );
}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoCombineDLLsAlg::~ChargedProtoCombineDLLsAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ChargedProtoCombineDLLsAlg::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoCombineDLLsAlg::execute()
{
  // Load the charged ProtoParticles
  LHCb::ProtoParticles * protos = get<LHCb::ProtoParticles>( m_protoPath );

  // Loop over the protos
  for ( LHCb::ProtoParticles::iterator iP = protos->begin();
        iP != protos->end(); ++iP )
  {

    // Does this ProtoParticle have any PID information ?
    if ( !(*iP)->hasInfo(LHCb::ProtoParticle::NoPID) )
    {

      // Combined DLL data object for this proto
      CombinedLL combDLL(0);

      // Add any RICH info
      addRich(*iP,combDLL);

      // Add any MUON info
      addMuon(*iP,combDLL);

      // Add any CALO info
      addCalo(*iP,combDLL);

      // Store the final combined DLL values into the ProtoParticle
      (*iP)->addInfo( LHCb::ProtoParticle::CombDLLe,  combDLL.elDLL-combDLL.piDLL );
      (*iP)->addInfo( LHCb::ProtoParticle::CombDLLmu, combDLL.muDLL-combDLL.piDLL );
      (*iP)->addInfo( LHCb::ProtoParticle::CombDLLpi, 0 ); // by definition
      (*iP)->addInfo( LHCb::ProtoParticle::CombDLLk,  combDLL.kaDLL-combDLL.piDLL );
      (*iP)->addInfo( LHCb::ProtoParticle::CombDLLp,  combDLL.prDLL-combDLL.piDLL );

    } // Has PID info

  } // loop over protos

  return StatusCode::SUCCESS;
}

void
ChargedProtoCombineDLLsAlg::addRich( LHCb::ProtoParticle * proto, CombinedLL & combDLL ) const
{
  // Add RICH Dll information. Default, for when not available is 0
  combDLL.elDLL += proto->info ( LHCb::ProtoParticle::RichDLLe,  0 );
  combDLL.muDLL += proto->info ( LHCb::ProtoParticle::RichDLLmu, 0 );
  combDLL.piDLL += proto->info ( LHCb::ProtoParticle::RichDLLpi, 0 );
  combDLL.kaDLL += proto->info ( LHCb::ProtoParticle::RichDLLk,  0 );
  combDLL.prDLL += proto->info ( LHCb::ProtoParticle::RichDLLp,  0 );
}

void
ChargedProtoCombineDLLsAlg::addMuon( LHCb::ProtoParticle * proto, CombinedLL & combDLL ) const
{
  combDLL.elDLL += proto->info ( LHCb::ProtoParticle::ProtoParticle::MuonBkgLL,  0 );
  combDLL.muDLL += proto->info ( LHCb::ProtoParticle::ProtoParticle::MuonMuLL,   0 );
  combDLL.piDLL += proto->info ( LHCb::ProtoParticle::ProtoParticle::MuonBkgLL,  0 );
  combDLL.kaDLL += proto->info ( LHCb::ProtoParticle::ProtoParticle::MuonBkgLL,  0 );
  combDLL.prDLL += proto->info ( LHCb::ProtoParticle::ProtoParticle::MuonBkgLL,  0 );
}

void
ChargedProtoCombineDLLsAlg::addCalo( LHCb::ProtoParticle * proto, CombinedLL & combDLL ) const
{
  // DLL(el)
  combDLL.elDLL += proto->info ( LHCb::ProtoParticle::EcalPIDe, 0  );
  combDLL.elDLL += proto->info ( LHCb::ProtoParticle::HcalPIDe, 0  );
  combDLL.elDLL += proto->info ( LHCb::ProtoParticle::PrsPIDe , 0  );
  combDLL.elDLL += proto->info ( LHCb::ProtoParticle::BremPIDe, 0  );
  // DLL(mu)
  combDLL.muDLL += proto->info ( LHCb::ProtoParticle::EcalPIDmu, 0 );
  combDLL.muDLL += proto->info ( LHCb::ProtoParticle::HcalPIDmu, 0 );
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedProtoCombineDLLsAlg::finalize()
{
  // execute base class finalise and return
  return GaudiAlgorithm::finalize();
}

//=============================================================================
