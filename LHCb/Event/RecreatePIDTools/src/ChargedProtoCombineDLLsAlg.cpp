
//-----------------------------------------------------------------------------
/** @file ChargedProtoCombineDLLsAlg.cpp
 *
 * Implementation file for algorithm ChargedProtoCombineDLLsAlg
 *
 * CVS Log :-
 * $Id: ChargedProtoCombineDLLsAlg.cpp,v 1.23 2010-03-08 01:46:40 odescham Exp $
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
  , m_elCombDll(0xFFFF)
  , m_muCombDll(0xFFFF)
  , m_prCombDll(0xFFFF)
  , m_piCombDll(0xFFFF)
  , m_kaCombDll(0xFFFF)
{

  // context specific locations
  if ( context() == "HLT" || context() == "Hlt" )
  {
    m_protoPath = LHCb::ProtoParticleLocation::HltCharged;
  }
  else
  {
    m_protoPath = LHCb::ProtoParticleLocation::Charged;
  }

  // Job Options
  declareProperty( "ProtoParticleLocation", m_protoPath );

  declareProperty("ElectronDllDisable"  , m_elDisable);
  declareProperty("MuonDllDisable"      , m_muDisable);
  declareProperty("KaonDllDisable"      , m_kaDisable);
  declareProperty("ProtonDllDisable"    , m_piDisable);
  declareProperty("PionllDisable"       , m_prDisable);

  m_maskTechnique["RICH"] = 0x1;
  m_maskTechnique["MUON"] = 0x2;
  m_maskTechnique["ECAL"] = 0x4;
  m_maskTechnique["HCAL"] = 0xF;
  m_maskTechnique["PRS"]  = 0x10;
  m_maskTechnique["SPD"]  = 0x20;
  m_maskTechnique["BREM"] = 0x40;
  m_maskTechnique["CALO"] = 0x7C;
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
  StatusCode scc = GaudiAlgorithm::initialize();
  if ( scc.isFailure() ) return scc;

  for(std::vector<std::string>::iterator itech = m_elDisable.begin(); itech != m_elDisable.end(); ++itech){
    if(0 == m_maskTechnique[to_upper(*itech)]){
      error() << "Electron PID technique " << *itech << " unknown"<< endmsg;
      scc = StatusCode::FAILURE;
    }
    m_elCombDll &= ~m_maskTechnique[to_upper(*itech)] ;
  }
  for(std::vector<std::string>::iterator itech = m_muDisable.begin(); itech != m_muDisable.end(); ++itech){
    if(0 == m_maskTechnique[to_upper(*itech)]){
      error() << "Muon PID technique " << *itech << " unknown"<< endmsg;
      scc = StatusCode::FAILURE;
    }
    m_muCombDll &= ~m_maskTechnique[to_upper(*itech)] ;
  }
  for(std::vector<std::string>::iterator itech = m_prDisable.begin(); itech != m_prDisable.end(); ++itech){
    if(0 == m_maskTechnique[to_upper(*itech)]){
      error() << "Proton PID technique " << *itech << " unknown"<< endmsg;
      scc = StatusCode::FAILURE;
    }
    m_prCombDll &= ~m_maskTechnique[to_upper(*itech)] ;
  }
  for(std::vector<std::string>::iterator itech = m_piDisable.begin(); itech != m_piDisable.end(); ++itech){
    if(0 == m_maskTechnique[to_upper(*itech)]){
      error() << "Pion PID technique " << *itech << " unknown"<< endmsg;
      scc = StatusCode::FAILURE;
    }
    m_piCombDll &= ~m_maskTechnique[to_upper(*itech)] ;
  }
  for(std::vector<std::string>::iterator itech = m_kaDisable.begin(); itech != m_kaDisable.end(); ++itech){
    if(0 == m_maskTechnique[to_upper(*itech)]){
      error() << "Kaon PID technique " << *itech << " unknown"<< endmsg;
      scc = StatusCode::FAILURE;
    }
    m_kaCombDll &= ~m_maskTechnique[to_upper(*itech)] ;
  }

  info() << "Using retuned RICH el and mu DLL values in combined DLLs" << endmsg;
  if ( 0 == m_elCombDll ) Warning( "Not creating Combined DLL for electron hypothesis", StatusCode::SUCCESS );
  if ( 0 == m_muCombDll ) Warning( "Not creating Combined DLL for muon hypothesis", StatusCode::SUCCESS );
  if ( 0 == m_piCombDll ) Warning( "Not creating Combined DLL for pion hypothesis", StatusCode::SUCCESS );
  if ( 0 == m_kaCombDll ) Warning( "Not creating Combined DLL for kaon hypothesis", StatusCode::SUCCESS );
  if ( 0 == m_prCombDll ) Warning( "Not creating Combined DLL for proton hypothesis", StatusCode::SUCCESS );

  return scc;
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
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Creating Combined DLLs for ProtoParticle " << (*iP)->key()
                << endmsg;

    // Remove any current Combined DLL information
    (*iP)->removeCombinedInfo();

    bool hasPIDinfo(false);

    // Combined DLL data object for this proto
    CombinedLL combDLL(0);

    // Add any RICH info
    hasPIDinfo |= addRich(*iP,combDLL);

    // Add any MUON info
    hasPIDinfo |= addMuon(*iP,combDLL);

    // Add any CALO info
    hasPIDinfo |= addCalo(*iP,combDLL);

    if ( hasPIDinfo )
    {
      // Store the final combined DLL values into the ProtoParticle
      (*iP)->addInfo( LHCb::ProtoParticle::CombDLLe,  combDLL.elDLL-combDLL.piDLL );
      (*iP)->addInfo( LHCb::ProtoParticle::CombDLLmu, combDLL.muDLL-combDLL.piDLL );
      (*iP)->addInfo( LHCb::ProtoParticle::CombDLLpi, 0 ); // by definition
      (*iP)->addInfo( LHCb::ProtoParticle::CombDLLk,  combDLL.kaDLL-combDLL.piDLL );
      (*iP)->addInfo( LHCb::ProtoParticle::CombDLLp,  combDLL.prDLL-combDLL.piDLL );
    }
    else
    { 
      (*iP)->addInfo( LHCb::ProtoParticle::NoPID, 1 );
    }

  } // loop over protos
  counter("CombineDLL ==> " + m_protoPath )+= protos->size();

  return StatusCode::SUCCESS;
}

bool
ChargedProtoCombineDLLsAlg::addRich( LHCb::ProtoParticle * proto, CombinedLL & combDLL )
{
  // Add RICH Dll information.
  const bool ok = ( m_maskTechnique["RICH"] && proto->hasInfo(LHCb::ProtoParticle::RichPIDStatus) );
  if ( ok )
  {
    // Apply renormalisation of RICH el and mu DLL values
    // Eventually, should make these tunable job options ....
    if( 0 != m_elCombDll ) combDLL.elDLL += 7.0*tanh( proto->info(LHCb::ProtoParticle::RichDLLe,0)/40.0  );
    if( 0 != m_muCombDll ) combDLL.muDLL += 7.0*tanh( proto->info(LHCb::ProtoParticle::RichDLLmu,0)/5.0 );
    if( 0 != m_piCombDll ) combDLL.piDLL += proto->info ( LHCb::ProtoParticle::RichDLLpi , 0 );
    if( 0 != m_kaCombDll ) combDLL.kaDLL += proto->info ( LHCb::ProtoParticle::RichDLLk  , 0 );
    if( 0 != m_prCombDll ) combDLL.prDLL += proto->info ( LHCb::ProtoParticle::RichDLLp  , 0 );
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> Adding RICH info " << combDLL << endmsg;
  }
  return ok;
}

bool
ChargedProtoCombineDLLsAlg::addMuon( LHCb::ProtoParticle * proto, CombinedLL & combDLL )
{
  const bool ok = ( m_maskTechnique["MUON"] && proto->hasInfo(LHCb::ProtoParticle::MuonPIDStatus) );
  if ( ok )
  {
    if( 0 != m_elCombDll ) combDLL.elDLL += proto->info ( LHCb::ProtoParticle::MuonBkgLL,0 );
    if( 0 != m_muCombDll ) combDLL.muDLL += proto->info ( LHCb::ProtoParticle::MuonMuLL, 0 );
    if( 0 != m_piCombDll ) combDLL.piDLL += proto->info ( LHCb::ProtoParticle::MuonBkgLL,0 );
    if( 0 != m_kaCombDll ) combDLL.kaDLL += proto->info ( LHCb::ProtoParticle::MuonBkgLL,0 );
    if( 0 != m_prCombDll ) combDLL.prDLL += proto->info ( LHCb::ProtoParticle::MuonBkgLL,0 );
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> Adding MUON info " << combDLL << endmsg;
  }
  return ok;
}

bool
ChargedProtoCombineDLLsAlg::addCalo( LHCb::ProtoParticle * proto, CombinedLL & combDLL )
{
  bool ok = false;
  if ( m_maskTechnique["ECAL"] && ( proto->hasInfo(LHCb::ProtoParticle::EcalPIDe) ||
                                    proto->hasInfo(LHCb::ProtoParticle::EcalPIDmu) ) )
  {
    if( 0 != m_elCombDll ) combDLL.elDLL += proto->info ( LHCb::ProtoParticle::EcalPIDe, 0  );
    if( 0 != m_muCombDll ) combDLL.muDLL += proto->info ( LHCb::ProtoParticle::EcalPIDmu, 0 );
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> Adding ECAL info " << combDLL << endmsg;
    ok = true;
  }

  if ( m_maskTechnique["HCAL"] && ( proto->hasInfo(LHCb::ProtoParticle::HcalPIDe) ||
                                    proto->hasInfo(LHCb::ProtoParticle::HcalPIDmu) ) )
  {
    if( 0 != m_elCombDll ) combDLL.elDLL += proto->info ( LHCb::ProtoParticle::HcalPIDe, 0  );
    if( 0 != m_muCombDll ) combDLL.muDLL += proto->info ( LHCb::ProtoParticle::HcalPIDmu, 0 );
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> Adding HCAL info " << combDLL << endmsg;
    ok = true;
  }

  if ( m_maskTechnique["PRS"] && proto->hasInfo(LHCb::ProtoParticle::PrsPIDe) )
  {
    if( 0 != m_elCombDll ) combDLL.elDLL += proto->info ( LHCb::ProtoParticle::PrsPIDe , 0  );
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> Adding PRS  info " << combDLL << endmsg;
    ok = true;
  }

  if ( m_maskTechnique["BREM"] && proto->hasInfo(LHCb::ProtoParticle::BremPIDe) )
  {
    if( 0 != m_elCombDll ) combDLL.elDLL += proto->info ( LHCb::ProtoParticle::BremPIDe, 0  );
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> Adding BREM info " << combDLL << endmsg;
    ok = true;
  }

  return ok;
}

//=============================================================================
