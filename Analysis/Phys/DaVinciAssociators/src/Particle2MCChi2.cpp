// $Id: Particle2MCChi2.cpp,v 1.3 2002-07-15 15:17:17 phicharp Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

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


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2MCChi2::Particle2MCChi2( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_outputTable( Particle2MCAsctLocation )
  , m_chi2( 100. )
{
  m_inputData.push_back( ParticleLocation::Production );
  declareProperty( "InputData", m_inputData );
  declareProperty( "OutputTable", m_outputTable );
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
  std::string myAlgName = name() + "AlgWithChi2";
  std::string myAsctName = name() + "AsctWithChi2";

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  // Retrieve a weighted associator on which a threshold will be applied
  StatusCode sc = toolSvc()->retrieveTool( "Particle2MCWithChi2Asct" , 
                                           myAsctName,
                                           m_pAsctChi2);
  if( sc.isFailure() || 0 == m_pAsctChi2) {
    log << MSG::FATAL << "    Unable to retrieve the WithChi2 Associator tool" 
        << endreq;
    return sc;
  }
  
  /* Now set up the WithChi2 associator such that it gives 
     the correct information (local to this algorithm) */
  
  IProperty* prop = 
    dynamic_cast<IProperty*>( m_pAsctChi2 );
  if( prop ) {
    std::string myLocation = outputTable() + "TempWithChi2";
    sc = prop->setProperty( "Location", myLocation);
    if( sc.isSuccess() ) {
      log << MSG::DEBUG << "Property Location set to " << myLocation
          << " in Asct " << myAsctName << endreq;
    }
    sc = prop->setProperty( "AlgorithmName", myAlgName);
    if( sc.isSuccess() ) {
      log << MSG::DEBUG << "Property AlgorithmName set to " << myAlgName
          << " in Asct " << myAsctName << endreq;
    }
  }
  // Now set the algorithm's properties...
  prop = dynamic_cast<IProperty*>( m_pAsctChi2->algorithm() );
  if( prop ) {
    std::string propString ;
    std::string sep = "\"";
    propString = "[";
    for( std::vector<std::string>::iterator inp = m_inputData.begin();
         m_inputData.end() != inp; inp++ ) {
      propString = propString + sep + *inp ;
      sep = "\",\"";
    }
    propString = propString + "\"]";
    sc = prop->setProperty( "InputData", propString);
    if( sc.isSuccess() ) {
      log << MSG::DEBUG << "Property InputData set to "
          << propString << " in algo " << myAlgName << endreq;
    }
    else {
      log << MSG::DEBUG << " **** Error setting Property InputData of "
          << myAlgName << " to " << propString << endreq;
    }
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2MCChi2::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  // Create an association table and register it in the TES
  Particle2MCAsct::Table* table = new Particle2MCAsct::Table();

  // NOTE: there is no need to access the MCParticle container, 
  // since we retrieve them through the relation table

  //////////////////////////////////
  // Loop on Particles containers //
  //////////////////////////////////

  for( vector<std::string>::iterator inp = m_inputData.begin(); 
       m_inputData.end()!= inp; inp++) {
    // Get Particles
    SmartDataPtr<Particles> parts (eventSvc(), *inp);
    if( 0 != parts ) {
      log << MSG::DEBUG << "    Particles retrieved from " << *inp 
          << endreq;
    }
    else {
      log << MSG::ERROR
          << "    *** Could not retrieve Particles from " << *inp
          << endreq;
      continue;
    }

    if( m_pAsctChi2->tableExists() ) {
      for( Particles::const_iterator pIt=parts->begin();
           parts->end() != pIt; pIt++) {
        MCsFromParticleChi2 mcRange = 
          m_pAsctChi2->rangeWithHighCutFrom( *pIt, m_chi2) ;
        if( !mcRange.empty() ) {
          for( MCsFromParticleChi2Iterator mcIt = mcRange.begin();
               mcRange.end() != mcIt; mcIt++ ) {
            table->relate( *pIt, mcIt->to());
          }
        }
      }
    } else {
      log << MSG::WARNING << "Chi2Asct table not found" << endreq;
    }
  }
      
  // Now register the table in the TES
  StatusCode sc = eventSvc()->registerObject( outputTable(), table);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "     *** Could not register table " << outputTable()
        << endreq;
    delete table;
    return sc;
  } else {
    log << MSG::DEBUG << "     Registered table " << outputTable() << endreq;
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Particle2MCChi2::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
