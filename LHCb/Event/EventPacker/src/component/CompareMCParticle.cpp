// Include files 

#include "Event/MCParticle.h"
// local
#include "CompareMCParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompareMCParticle
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( CompareMCParticle )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  CompareMCParticle::CompareMCParticle( const std::string& name,
                                        ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::MCParticleLocation::Default );
  declareProperty( "TestName"  , m_testName   = LHCb::MCParticleLocation::Default+"Test" );
}

//=============================================================================
// Destructor
//=============================================================================
CompareMCParticle::~CompareMCParticle() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompareMCParticle::execute()
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  LHCb::MCParticles* old  = get<LHCb::MCParticles>( m_inputName  );
  LHCb::MCParticles* test = get<LHCb::MCParticles>( m_testName  );

  if ( old->size() != test->size() ) {
    err() << "Old MCParticle size " << old->size()
          << " differs form Test " << test->size()
          << endmsg;
    return StatusCode::FAILURE;
  }
  LHCb::MCParticles::const_iterator itOld  = old->begin();
  LHCb::MCParticles::const_iterator itTest = test->begin();

  while ( old->end() != itOld )
  {
    LHCb::MCParticle* oPart = (*itOld++);
    LHCb::MCParticle* tPart = (*itTest++);
    if ( oPart->key() != tPart->key() ) {
      warning() << "Wrong key : old " <<  oPart->key() << " test " << tPart->key() << endmsg;
    }
    bool isOK = true;
    if ( 5.e-3 < fabs( oPart->momentum().px() - tPart->momentum().px() ) ) isOK = false;
    if ( 5.e-3 < fabs( oPart->momentum().py() - tPart->momentum().py() ) ) isOK = false;
    if ( 5.e-3 < fabs( oPart->momentum().pz() - tPart->momentum().pz() ) ) isOK = false;
    double massTol =  1.e-5 * fabs( oPart->momentum().M() ) + /// float, relative...
      1.e-5 * fabs(  oPart->momentum().pz() ) +  // high energy photons rounding
      .001; // null mass/p protection.

    if ( massTol < fabs( oPart->momentum().M() - tPart->momentum().M() ) ) isOK = false;
    if ( oPart->originVertex()  != tPart->originVertex()  ) isOK = false;
    unsigned int kk;
    if ( oPart->endVertices().size() != tPart->endVertices().size() ) {
      isOK = false;
    } else {
      for ( kk = 0; oPart->endVertices().size() > kk; kk++ ) {
        const LHCb::MCVertex* dum  = oPart->endVertices()[kk];  // convert smartref to pointers
        const LHCb::MCVertex* dum1 = tPart->endVertices()[kk];
        if ( dum != dum1 ) isOK = false;
      }
    }

    if ( !isOK || MSG::VERBOSE >= msgLevel() ) {
      if( !isOK ) Warning( "Packed MCParticle info truncated. Set VERBOSE OutputLevel for details" ).ignore();
      debug() << "=== MCParticle key " << oPart->key() << endmsg;
      debug() << format( "  old momentum %12.3f %12.3f %12.3f %12.4f",
                         oPart->momentum().px(), oPart->momentum().py(),
                         oPart->momentum().pz(), oPart->momentum().M() )
              << endmsg;
      debug() << format( " test momentum %12.3f %12.3f %12.3f %12.4f",
                         tPart->momentum().px(), tPart->momentum().py(),
                         tPart->momentum().pz(), tPart->momentum().M() )
              << endmsg;
      debug() << format( "  old pid %6d ", oPart->particleID().pid())
              << " endVert " << oPart->originVertex()
              << endmsg;
      debug() << format( " test pid %6d ", tPart->particleID().pid() )
              << " endVert " << tPart->originVertex()
              << endmsg << "  old endVertices ";
      for ( kk = 0; oPart->endVertices().size() > kk; kk++ ) {
        debug() << " " << oPart->endVertices()[kk];
      }
      debug() << endmsg << " test endVertices ";
      for ( kk = 0; tPart->endVertices().size() > kk; kk++ ) {
        debug() << " " << tPart->endVertices()[kk];
      }
      debug() << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}
