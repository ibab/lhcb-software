
// local
#include "CopySignalMCParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopySignalMCParticles
//
// 2015-03-24 : Chris Jones
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopySignalMCParticles::CopySignalMCParticles( const std::string& name,
                                              ISvcLocator* pSvcLocator)
: MicroDSTAlgorithm ( name , pSvcLocator ),
  m_cloner          ( NULL ),
  m_pCPPAsct        ( NULL ),
  m_pNPPAsct        ( NULL )
{
  declareProperty( "ICloneMCParticle", 
                   m_mcpClonerName = "MCParticleCloner" );
  declareProperty( "ICloneProtoParticle", 
                   m_ppClonerName = "ProtoParticleCloner" );
  declareProperty( "MCParticlesLocation", 
                   m_mcPsLoc = LHCb::MCParticleLocation::Default );
  declareProperty( "SaveAssociatedRecoInfo", m_saveRecoInfo = true );
  
  //setProperty( "OutputLevel", 1 );
}

//=============================================================================
// Destructor
//=============================================================================
CopySignalMCParticles::~CopySignalMCParticles() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopySignalMCParticles::initialize()
{
  const StatusCode sc = MicroDSTAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  // Load the cloner
  m_cloner = tool<ICloneMCParticle>( m_mcpClonerName, this );

  // If required, load the MC association objects and proto cloner
  if ( m_saveRecoInfo )
  {
    m_pCPPAsct = new ProtoParticle2MCLinker( this, 
                                             Particle2MCMethod::ChargedPP, 
                                             LHCb::ProtoParticleLocation::Charged );
    m_pNPPAsct = new ProtoParticle2MCLinker( this,
                                             Particle2MCMethod::NeutralPP, 
                                             LHCb::ProtoParticleLocation::Neutrals );
    m_ppCloner = tool<ICloneProtoParticle>( m_ppClonerName, this );
  }

  return sc;
}

//=========================================================================
/// Protoparticles list
//========================================================================
CopySignalMCParticles::ProtosVector
CopySignalMCParticles::getProtos(const LHCb::MCParticle* mcp) const
{
  ProtoParticle2MCLinker * asct = ( mcp->particleID().threeCharge()==0 ? 
                                    m_pNPPAsct : m_pCPPAsct );
  if ( !asct ) Exception("Null PP asociator");
  CopySignalMCParticles::ProtosVector ppv ;
  double w = 0;
  const LHCb::ProtoParticle * pp = asct->firstP(mcp,w);
  while( pp )
  {
    ppv.push_back(pp);
    pp = asct->nextP(w);
  }
  return ppv;
}

//=============================================================================
// Finalisation
//=============================================================================
StatusCode CopySignalMCParticles::finalize()
{
  // clean up
  delete m_pCPPAsct;
  delete m_pNPPAsct;
  // call base class and return
  return MicroDSTAlgorithm::finalize(); 
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CopySignalMCParticles::execute() 
{

  // load the primary MCParticles
  const LHCb::MCParticles * mcPs = getIfExists<LHCb::MCParticles>(m_mcPsLoc);
  if ( mcPs )
  {

    // Save a list of MCParticles that are signal and thus cloned
    std::vector<const LHCb::MCParticle *> clonedMCPs;

    // Loop over them and look for signal
    for ( const LHCb::MCParticle * mcP : *mcPs )
    {
      if ( mcP->fromSignal() )
      {
        // First clone the MCParticle
        if ( msgLevel(MSG::VERBOSE) ) verbose() << "Found Signal MCParticle" << endmsg;
        const LHCb::MCParticle * cloneMCP = (*m_cloner)(mcP);
        if ( msgLevel(MSG::VERBOSE) ) verbose() << "  -> Cloned " << *cloneMCP << endmsg;
        // save this in the list
        clonedMCPs.push_back(mcP);
      }
    }

    // If asked to, cloned the associated Reco level info.
    if ( m_saveRecoInfo )
    {
      // Loop over all selected MCPs and clone reco level info for them
      for ( const LHCb::MCParticle * mcP : clonedMCPs )
      {
        const ProtosVector protos = getProtos(mcP);
        for ( const LHCb::ProtoParticle * proto : protos )
        {
          // Clone this Proto
          if ( proto ) { (*m_ppCloner)(proto); }
        }
      }
    }

  }

  return StatusCode::SUCCESS;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopySignalMCParticles )

//=============================================================================
