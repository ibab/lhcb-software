// $Id: Particle2MCChi2.cpp,v 1.9 2004-07-29 07:58:48 phicharp Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// event
#include "Event/MCParticle.h"
#include "Event/Particle.h"

// local
#include "Particle2MCChi2.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle2MCChi2
//
// 17/05/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<Particle2MCChi2>          s_factory ;
const        IAlgFactory& Particle2MCChi2Factory = s_factory ; 

#define ifMsg(sev) msg << sev; if( msg.level() <= (sev) ) msg

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2MCChi2::Particle2MCChi2( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : AsctAlgorithm ( name , pSvcLocator )
  , m_chi2( 100. )
  , m_p2MCLink(NULL)
{
  m_outputTable = Particle2MCAsctLocation ;

  declareProperty( "Chi2Cut", m_chi2 );
}

//=============================================================================
// Destructor
//=============================================================================
Particle2MCChi2::~Particle2MCChi2() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Particle2MCChi2::initialize() {
 
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;

  StatusCode sc = GaudiAlgorithm::initialize();
  if( !sc.isSuccess() ) return sc;

  m_p2MCLink = new Object2MCLink( this,
                                    Particle2MCMethod::WithChi2, 
                                    m_inputData);
  if( NULL == m_p2MCLink ) {
    return Error("Cannot create Object2MCLinkedTo helper");
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2MCChi2::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;

  // Create an association table and register it in the TES
  Particle2MCAsct::Table* table = 
    "" != outputTable() ? new Particle2MCAsct::Table() : NULL;

  //////////////////////////////////
  // Loop on Particles containers //
  //////////////////////////////////

  for( std::vector<std::string>::iterator inp = m_inputData.begin(); 
       m_inputData.end()!= inp; inp++) {
    // Create a linker table
    const std::string linkContainer = 
      *inp + Particle2MCMethod::extension[Particle2MCMethod::Chi2];
    Object2MCLink::Linker*
      linkerTable = m_p2MCLink->linkerTable( linkContainer );

    if( NULL == table && NULL == linkerTable) continue;
    
    // Get Particles
    SmartDataPtr<Particles> parts (eventSvc(), *inp);
    if( 0 == parts ) continue;
    int npp = parts->size();
    int nass = 0;
    int nrel = 0;
    ifMsg(MSG::DEBUG) << "    " << npp 
                      << " Particles retrieved from " << *inp << endreq;

    for( Particles::const_iterator pIt=parts->begin();
         parts->end() != pIt; pIt++) {
      ifMsg(MSG::VERBOSE) << "    Particle " << (*pIt)->key();
      MCParticle* mcPart = m_p2MCLink->first( *pIt);
      bool found = false;
      double minChi2 = 0;
      while( NULL != mcPart ) {
        double weight = m_p2MCLink->weight();
        if( weight <= m_chi2 ) {
          if( !found ) {
            ifMsg(MSG::VERBOSE) << " associated to MCParts";
          }
          ifMsg(MSG::VERBOSE) << " - " << mcPart->key();
          if( NULL != table ) table->relate( *pIt, mcPart);
          if( NULL != linkerTable) 
            linkerTable->link( *pIt, mcPart, weight);
          found = true;
          nrel++;
        }
        if( minChi2 == 0 || weight < minChi2) minChi2 = weight;
        mcPart = m_p2MCLink->next();
      }
      if( found) nass++;
      else {
        ifMsg(MSG::VERBOSE) << " not associated to any MCPart, minChi2 = " 
                            << minChi2;
      }
      ifMsg(MSG::VERBOSE) << endreq;
    }
    ifMsg(MSG::DEBUG) 
      << "Out of " << npp << " Particles in " << *inp << ", "
      << nass << " are associated, "
      << nrel << " relations found" << endreq;
  }
      
  // Now register the table in the TES
  if( NULL != table ) {
    put( table, outputTable());
    ifMsg(MSG::DEBUG) << "     Registered table " 
                      << outputTable() << endreq;
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Particle2MCChi2::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;
  if( NULL != m_p2MCLink ) delete m_p2MCLink;
  m_p2MCLink = NULL;
  return GaudiAlgorithm::finalize();
}

//=============================================================================
