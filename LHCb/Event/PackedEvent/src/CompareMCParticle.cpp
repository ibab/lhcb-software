// $Id: CompareMCParticle.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/MCParticle.h"
// local
#include "CompareMCParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompareMCParticle
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CompareMCParticle>          s_factory ;
const        IAlgFactory& CompareMCParticleFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompareMCParticle::CompareMCParticle( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = MCParticleLocation::Default );
  declareProperty( "TestName"  , m_testName   = MCParticleLocation::Default+"Test" );
}
//=============================================================================
// Destructor
//=============================================================================
CompareMCParticle::~CompareMCParticle() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CompareMCParticle::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompareMCParticle::execute() {

  debug() << "==> Execute" << endmsg;

  MCParticles* old  = get<MCParticles>( m_inputName  );
  MCParticles* test = get<MCParticles>( m_testName  );

  if ( old->size() != test->size() ) {
    info() << "Old MCParticle size " << old->size()
           << " differs form Test " << test->size()
           << endreq;
    return StatusCode::FAILURE;
  }  
  MCParticles::const_iterator itOld  = old->begin();
  MCParticles::const_iterator itTest = test->begin();

  while ( old->end() != itOld ) {
    MCParticle* oPart = (*itOld++);
    MCParticle* tPart = (*itTest++);
    if ( oPart->key() != tPart->key() ) {
      info() << "Wrong key : old " <<  oPart->key() << " test " << tPart->key() << endreq;
    }
    bool isOK = true;
    if ( MSG::VERBOSE >= msgLevel() ) isOK = false; //== force printing
    if ( 5.e-3 < fabs( oPart->momentum().px() - tPart->momentum().px() ) ) isOK = false;
    if ( 5.e-3 < fabs( oPart->momentum().py() - tPart->momentum().py() ) ) isOK = false;
    if ( 5.e-3 < fabs( oPart->momentum().pz() - tPart->momentum().pz() ) ) isOK = false;
    double massTol =  1.e-5 * fabs( oPart->momentum().m() ) + /// float, relative...
      1.e-5 * fabs(  oPart->momentum().pz() ) +  // high energy photons rounding
      .001; // null mass/p protection.
    
    if ( massTol < fabs( oPart->momentum().m() - tPart->momentum().m() ) ) isOK = false;
    if ( oPart->hasOscillated() != tPart->hasOscillated() ) isOK = false;
    if ( oPart->helicity()      != tPart->helicity()      ) isOK = false;
    if ( oPart->originVertex()  != tPart->originVertex()  ) isOK = false;
    if ( oPart->collision()     != tPart->collision()  ) isOK = false;
    unsigned int kk;
    if ( oPart->endVertices().size() != tPart->endVertices().size() ) {
      isOK = false;
    } else { 
      for ( kk = 0; oPart->endVertices().size() > kk; kk++ ) {
        MCVertex* dum  = oPart->endVertices()[kk];  // convert smartref to pointers
        MCVertex* dum1 = tPart->endVertices()[kk];
        if ( dum != dum1 ) isOK = false;
      }
    }
    
    if ( !isOK ) {
      info() << "=== MCParticle key " << oPart->key() << endreq;
      info() << format( "  old momentum %12.3f %12.3f %12.3f %12.4f",
                        oPart->momentum().px(), oPart->momentum().py(),
                        oPart->momentum().pz(), oPart->momentum().m() )
             << endreq;
      info() << format( " test momentum %12.3f %12.3f %12.3f %12.4f",
                        tPart->momentum().px(), tPart->momentum().py(),
                        tPart->momentum().pz(), tPart->momentum().m() ) 
             << endreq;
      info() << format( "  old pid %6d osc %2d heli %d",
                        oPart->particleID().pid(),
                        oPart->hasOscillated(), oPart->helicity() ) 
             << " endVert " << oPart->originVertex() 
             << " coll " << oPart->collision() 
             << endreq;
      info() << format( " test pid %6d osc %2d heli %d",
                        tPart->particleID().pid(),
                        tPart->hasOscillated(), tPart->helicity() ) 
             << " endVert " << tPart->originVertex()
             << " coll " << tPart->collision() 
             << endreq << "  old endVertices ";
      for ( kk = 0; oPart->endVertices().size() > kk; kk++ ) {
        info() << " " << oPart->endVertices()[kk];
      }
      info() << endreq << " test endVertices ";
      for ( kk = 0; tPart->endVertices().size() > kk; kk++ ) {
        info() << " " << tPart->endVertices()[kk];
      }
      info() << endreq;   
    }
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CompareMCParticle::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
