// $Id: AxPart2MCParticleAsct.cpp,v 1.1 2002-02-08 18:25:41 gcorti Exp $

// Include files
#include "GaudiTools/Associator.h"
#include "GaudiKernel/ToolFactory.h"
#include "LHCbEvent/AxPartCandidate.h"
#include "LHCbEvent/MCParticle.h"
#include "GaudiKernel/SmartDataPtr.h" 
#include "LHCbEvent/RefTable.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "AxPart2MCParticleAsct.h"

// Factory declaration for an AlgTool (an Associator inherits from it)
static ToolFactory<AxPart2MCParticleAsct> s_factory;
const IToolFactory& AxPart2MCParticleAsctFactory = s_factory;

//--------------------------------------------------------------------
//
//  ClassName   : AxPart2MCParticleAsct
//
//  Description : An example implementaion of an associator.
//                This example implements and associator between 
//                AxPartCandidate and MCParticle. 
//                This particular implementation retrieves the
//                link in the data where it is stored (SICb data)
//                as a 1-to-1 relation. This is a particular 
//                implementation but an associator can follow 
//                links in the data or retrieve it if available or
//                chosen.
//                It build a reverse map for convenience when
//                a reverse association is requested the first
//                time. A method to clean the cache needs to be 
//                implemented and is called by the Associator
//                base class when notified a new events is being
//                processed. 
//
//  Author       : G.Corti
//
//--------------------------------------------------------------------

//========================================================================
// Reset the AxPart2MCParticle associator to initial conditions
//========================================================================
StatusCode AxPart2MCParticleAsct::flushCache() {

  m_directTable = 0;
  m_inverseTable.clear();
  setDirectFlag( false );
  setInverseFlag( false ); 
  return StatusCode::SUCCESS;
}


//========================================================================
// Retriev 1-to-1 direct relation
// This specific example does not follow links in the classes (since they
// do not exist for this specific classes) but retrieves the shortcuts
// that are stored in SICb data.
//========================================================================
StatusCode AxPart2MCParticleAsct::i_retrieveDirect( ContainedObject* objFrom, 
                                  ContainedObject*& objTo, 
                                  const CLID idFrom  , const CLID idTo ) {

  MsgStream log(msgSvc(), name());

  // Check that the input/output objects are of the correct type
  if ( idFrom != AxPartCandidate::classID() ){
    objTo = 0;
    return StatusCode::FAILURE;
  }

  if ( idTo != MCParticle::classID() ) {
    objTo = 0;
    return StatusCode::FAILURE;
  }
    
  // Retrieve direct relation the first time it is requested
  // This specific implementation keeps its own copy of the direct
  // relations
  if ( !directExist() )   {
    StatusCode sc=buildDirect();
    if ( sc.isFailure() ) { 
      log << MSG::DEBUG << " Unable to find table " << endreq;
      objTo = 0;
      return StatusCode::FAILURE;
    }
  }

  // Find associated pointer after having verified the correct object type
  // has been provided (this is just an added protection, not really necessary)
  AxPartCandidate* pax;
  pax = dynamic_cast<AxPartCandidate*>(objFrom);
  if ( 0 == pax ) {
    objTo = 0;
    return StatusCode::FAILURE;
  }

  SmartRefVector<MCParticle>& mcparts = m_directTable->reference( pax );
  SmartRefVector<MCParticle>::iterator pmc;
  for ( pmc = mcparts.begin(); pmc != mcparts.end(); pmc++ )    {
    if ( 0 != (*pmc) )    {
       objTo = *pmc;
    }
  }
    
  return StatusCode::SUCCESS;
}
  
//========================================================================
// 1-to-many direct relation
// it should return a StatusCode::FAILURE but this is a fake implementation
// to show how this retrieval works in the AsctExampleAlgorithm
//========================================================================
StatusCode AxPart2MCParticleAsct::i_retrieveDirect( 
                                  ContainedObject*  objFrom ,          
                                  std::vector<ContainedObject*>& vObjTo,
                                  const CLID  idFrom , const CLID  idTo  ) {

  // Retrieve the 1-to-1 relation that exist and fill a vector with the
  // same associated MCParticle twice
  ContainedObject* objTo = 0;
  StatusCode sc = i_retrieveDirect( objFrom, objTo, idFrom, idTo );
  if ( sc.isFailure() ) return sc;
  vObjTo.push_back(objTo);
  vObjTo.push_back(objTo);
  return StatusCode::SUCCESS;

}


//========================================================================
// 1-to-1 inverse relation
//========================================================================
StatusCode AxPart2MCParticleAsct::i_retrieveInverse( 
                                        ContainedObject* objFrom,
                                        ContainedObject*&  objTo,
                                        const CLID idFrom, const CLID idTo) {
  MsgStream log(msgSvc(), name());

  // Verify input/output objects are of the correct type
  if ( idFrom != MCParticle::classID() ) {
    objTo = 0;
    return StatusCode::FAILURE;
  }
  
  if ( idTo != AxPartCandidate::classID() ) {
    objTo = 0;
    return StatusCode::FAILURE;
  }

  // If this is the first request for an inverse relation it will build a 
  // table to keep in memory, otherwise it will use the existing one
  if ( !inverseExist() ) {
    StatusCode sc=buildInverse();
    if ( sc.isFailure() ) {
      std::cout << " Unable to find table " << std::endl;
      objTo = 0;
      return StatusCode::FAILURE;
    }
  }

  // After checking that the object provided is an MCParticle 
  // (added protection) retrieve the associated AxPartCandidate
  MCParticle* pmc;
  pmc = dynamic_cast<MCParticle*>(objFrom);
  if ( 0 == pmc ) {
    objTo = 0;
    return StatusCode::FAILURE;
  }
  objTo = m_inverseTable[pmc];

  return StatusCode::SUCCESS; 
}
  
//========================================================================
// 1-to-many inverse relation
// this is the correct implemenation for this relation where in the data
// at the moment only a 1-to-1 relation in both direction is possible
//========================================================================
StatusCode AxPart2MCParticleAsct::i_retrieveInverse( 
                                  ContainedObject* /* objFrom */ ,
                                  std::vector<ContainedObject*>& vObjTo,
                                  const CLID /* idFrom */, 
                                  const CLID /* idTo */ ) {

  vObjTo.clear();
  return StatusCode::FAILURE;
}

//========================================================================
// Retrieve shortcut table from data
// In this case is a SmartRefTable filled by a converter
//========================================================================
StatusCode AxPart2MCParticleAsct::buildDirect() {

  if ( directExist() ) return StatusCode::SUCCESS;

  MsgStream log(msgSvc(), name());

  // Retrive a SmartRefTable filled by a converter (FOR THIS SPECIFIC EXAMPLE)
  // the whichTable() method is implemented in the Associator base class and
  // the string location of the data MUST be set in the JobOptions file
  DataObject* pObject;
  StatusCode sc = eventSvc()->retrieveObject(whichTable(),pObject);
  if ( sc.isFailure() ) {
    log << MSG::DEBUG << " Failed to find association in data " << endreq;
    return sc;
  }

  m_directTable = dynamic_cast<DirTable*> (pObject);
  if ( 0 == m_directTable ) { return StatusCode::FAILURE; }  

  // Set a flag that the local copy of the direct association (a choice for
  // this example) has been made for this event
  setDirectFlag( true );
  return StatusCode::SUCCESS;

}

//========================================================================
// Build inverse table
// An associator could decide not to keep an inverse table, but in general
// this is a time-consuming operation and the Associator base class provides
// an empty buildInverse method and a set of methods to verify if that has
// been done.
//========================================================================
StatusCode AxPart2MCParticleAsct::buildInverse() {

  if ( inverseExist() ) return StatusCode::SUCCESS; 

  // For this specific example check if the direct relation has already been
  // retrieved, otherwise do it
  if ( !directExist() ) {
    StatusCode sc = buildDirect();
    if ( sc.isFailure() ) {
      std::cout << " Unable to find table " << std::endl;
      return StatusCode::FAILURE;
    }
  }

  // Loop over all direct relations to build inverse
  for ( DirTable::iterator it= m_directTable->begin() ;
                           m_directTable->end() != it; ++it ) {
    SmartRefVector<MCParticle>& mcparts = (*it).second;
    SmartRefVector<MCParticle>::iterator pMC;
    pMC= mcparts.begin();
    const AxPartCandidate* ip = 
          reinterpret_cast<const AxPartCandidate*>((*it).first);
    AxPartCandidate* ipp = const_cast<AxPartCandidate*>(ip);
    m_inverseTable[*pMC] = ipp;
  }

  // Set flag for inverse table existing
  setInverseFlag( true ); 
  return StatusCode::SUCCESS;

}

//========================================================================
// Standard Constructor.                     
//========================================================================
AxPart2MCParticleAsct::AxPart2MCParticleAsct( const std::string& type,   
                                    const std::string& name,         
				    const IInterface* parent ) 
  : Associator ( type, name, parent ), m_inverseTable() { 

  m_directTable = 0;
  setDirectFlag( false );
  
}

//========================================================================
// Destructor
//========================================================================
AxPart2MCParticleAsct::~AxPart2MCParticleAsct() {

  m_inverseTable.clear();
}




