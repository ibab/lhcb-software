// $Id: Particle2MCLink.cpp,v 1.6 2006-03-15 12:58:31 pkoppenb Exp $
// Include files 



// local
#include "DaVinciAssociators/Particle2MCLink.h"
#include "Event/MCParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle2MCLink
//
// 2004-04-29 : Philippe Charpentier
//-----------------------------------------------------------------------------
using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
//=============================================================================

StatusCode
Object2MCLink::setAlgorithm( const int method, 
                             const std::vector<std::string>& containerList)
{
  m_containerList = containerList;
  if( NULL != m_linkerAlg ) {
    if( Particle2MCMethod::algType[method] != m_linkerAlgType) {
      // We change the algorithm on the fly... risky!
      m_linkerAlg = NULL;
    } else {
      return setAlgInputData( m_linkerAlg, m_containerList);
    }
  } 
  m_linkerAlgType = Particle2MCMethod::algType[method];
  m_extension     = Particle2MCMethod::extension[method];
  return StatusCode::SUCCESS;
}

StatusCode
Object2MCLink::setAlgorithm( const std::string& algType,
                             const std::string& extension, 
                             const std::vector<std::string>& containerList)
{
  m_containerList = containerList;
  if( NULL != m_linkerAlg ) {
    if( algType != m_linkerAlgType) {
      // We change the algorithm on the fly... risky!
      m_linkerAlg = NULL;
    } else {
      return setAlgInputData( m_linkerAlg, m_containerList);
    }
  } 
  m_linkerAlgType = algType;
  m_extension     = extension;
  return StatusCode::SUCCESS;
}

StatusCode
Object2MCLink::locateAlgorithm( const std::string& algType, 
                                const std::string& algName, 
                                IAlgorithm*& alg, 
                                const std::vector<std::string>& inputData)
{
  if( NULL != m_myGaudiAlg)
    m_myGaudiAlg->debug() << "==> Calling locateAlgorithm with type "
                          << algType << ", name " << algName <<endreq;
  
  if( NULL != alg ) return StatusCode::SUCCESS;
  if( algType.empty() ) return NULL == m_myGaudiAlg ? StatusCode::FAILURE :
    m_myGaudiAlg->Error("No alg type given");
  IAlgManager* algMgr = NULL;
  StatusCode sc = m_myAlg->serviceLocator()->
    getService( "" , IAlgManager::interfaceID() , (IInterface*&) algMgr );
  if( sc.isFailure() ) { return NULL == m_myGaudiAlg ? sc :
      m_myGaudiAlg->Error("Could not locate IAlgManager ", sc );}
  if( 0 == algMgr    ) { return NULL == m_myGaudiAlg ? sc :
      m_myGaudiAlg->Error("Could not locate IAlgManager ", sc) ;}
  // check the existence of the algorithm
  typedef std::list<IAlgorithm*> Algs;
  Algs& algs = algMgr->getAlgorithms() ;
  for( Algs::iterator ia = algs.begin() ; algs.end() != ia ; ++ia )
  {
    if( 0 == *ia              ) { continue ; }
    if( (*ia)->name() != algName ) { continue ; }
    // algorithm is found ! 
    alg = *ia ;
    alg -> addRef() ;
    // Set its jobOptions
    return setAlgInputData( alg, inputData) ; 
  }
  // algorithm is noT found: try to create it! 
  sc = algMgr->createAlgorithm( algType , algName , alg );
  if( sc.isFailure()   ) { return NULL == m_myGaudiAlg ? sc :
      m_myGaudiAlg->Error("Could not create algorithm", sc ) ; }
  if( 0 == alg ) { return NULL == m_myGaudiAlg ? sc :
      m_myGaudiAlg->Error("Could not create algorithm"     ) ; }
  // add the reference to the new algorithm 
  alg -> addRef() ;
  
  // Now initialise the algorithm
  sc = alg->sysInitialize() ;
  if( sc.isFailure() ) { 
    return NULL == m_myGaudiAlg ? sc :
      m_myGaudiAlg->Error( "Error from algorithm " 
                      + algName + " initialization!", sc);
  }
  // Set jobOptions to the algorithm, supersede options file
  sc = setAlgInputData( alg, inputData);
  if( sc.isFailure() ) {
    return NULL == m_myGaudiAlg ? sc :
      m_myGaudiAlg->Error( "Error setting InputData property to " + algName, 
                           sc);
  }
  //
  return StatusCode::SUCCESS ;
}

StatusCode
Object2MCLink::setAlgInputData( IAlgorithm*& alg, 
                                  const std::vector<std::string>& inputData)
{
  StatusCode sc = StatusCode::SUCCESS;
  /* If requested, pass an InputData property to the algorithm */
  if( 0 != inputData.size() ) {
    IProperty* prop = dynamic_cast<IProperty*>( alg );
    
    if( prop ) {
      // Set OutputTable property to "" avoiding Relations table
      sc = prop->setProperty( "OutputTable", "");
      if( !sc.isSuccess() ) {
        return NULL == m_myGaudiAlg ? sc :
          m_myGaudiAlg->Error( "Setting property OutputTable in " 
                               + alg->name(), sc);
      }
      std::string propString ;
      std::string sep = "\"";
      propString = "[";
      for( std::vector<std::string>::const_iterator inp = inputData.begin();
           inputData.end() != inp; inp++ ) {
        std::string inpStr = *inp;
        if( std::string::npos == inpStr.find( "/Particles") &&
             std::string::npos == inpStr.find( "/ProtoP") )
           inpStr += "/Particles";
        propString = propString + sep + inpStr ;
        sep = "\",\"";
      }
      propString = propString + "\"]";
      sc = prop->setProperty( "InputData", propString);
      if( sc.isSuccess() ) {
        if( NULL != m_myGaudiAlg )
          m_myGaudiAlg->debug() << "Property InputData set to "
                                << propString << " in algo " 
                                << alg->name() << endreq;
      } else {
        return NULL == m_myGaudiAlg ? sc :
          m_myGaudiAlg->Error( "Unable to set Property InputData in algo " 
                          + alg->name(), sc);
      }
    } else {
      return NULL == m_myGaudiAlg ? sc :
        m_myGaudiAlg->Warning( "Unable to get IProperty pointer for algo " 
                          + alg->name(), sc);
    }
  }  
  return sc;
}

bool
Object2MCLink::notFound( const std::string& contName)
{
  To test( m_myAlg->eventSvc(), NULL, contName + m_extension);
  return test.notFound();
}

bool
Object2MCLink::notFound()
{
  for( std::vector<std::string>::const_iterator contIt = 
         m_containerList.begin();
       m_containerList.end() != contIt; contIt++) {
    if( notFound(*contIt) ) return true;
  }
  return false;
}
  
void
Object2MCLink::createLinks( const std::string& contName )
{
  // First find the contname is in the list
  if( m_containerList.end() == std::find( m_containerList.begin(), 
                                          m_containerList.end(), 
                                          contName) ) {
    // Container was not in the list... add it!
    m_containerList.push_back( contName );
    if( NULL != m_linkerAlg ) {
      if( NULL != m_myGaudiAlg )
        m_myGaudiAlg->debug() << "    Add " << contName << " to InputData of "
                              << m_linkerAlg->name() << endreq;
      StatusCode sc = setAlgInputData( m_linkerAlg, m_containerList);
      if( sc.isFailure() ) 
        if( NULL != m_myGaudiAlg)
          m_myGaudiAlg->err() << "Error adding " << contName
                              << " to InputData of " << m_linkerAlg->name();
      
    }
  }
  for( std::vector<std::string>::const_iterator contIt = 
         m_containerList.begin();
       m_containerList.end() != contIt; contIt++) {
    const std::string name = *contIt + m_extension;
    To test( m_myAlg->eventSvc(), NULL, name);
    if( test.notFound() && "" != m_linkerAlgType) {
      if( NULL != m_myGaudiAlg)
        m_myGaudiAlg->debug() << "Link table for " << name
                              << " not found, creating it" << endreq;
      if( NULL == m_linkerAlg ) {
        // Create the algorithm, as it has to be called
        StatusCode sc = locateAlgorithm( m_linkerAlgType, 
                                         m_myAlg->name() + "." + 
                                         m_linkerAlgType, 
                                         m_linkerAlg, 
                                         m_containerList);
        if( !sc.isSuccess() ) {
        if( NULL != m_myGaudiAlg)
          m_myGaudiAlg->err() << "Cannot locate algorithm of type " 
                              << m_linkerAlgType
                              << endreq;
        break;
        }
      }
      // Call the algorithm to get the table done
      if( NULL != m_myGaudiAlg)
        m_myGaudiAlg->debug() << "==> Executing Linker builder algorithm "
                              << m_linkerAlg->name() << endreq;
      m_linkerAlg->sysExecute() ;
      test = To( m_myAlg->eventSvc(), NULL, name);
    }
    if( *contIt == contName ) m_linkTo = test;
  }
}

Object2MCLink::To*
Object2MCLink::getLink( const std::string& contName )
{
  // If the lists are empty, create them
  std::string name = contName+ m_extension;
  To test( m_myAlg->eventSvc(), NULL, name);
  if( test.notFound() ) {
    createLinks( contName );
  } else {
    m_linkTo = test;
  }
  return &m_linkTo;  
}

int
Object2MCLink::associatedMCP( const KeyedObject<int>* obj)
{
  int n = 0;
  for( MCParticle* mcPart = firstMCP(obj); 
       NULL != mcPart; 
       mcPart = nextMCP(), n++);
  return n;
}

MCParticle* 
Object2MCLink::firstMCP( const KeyedObject<int>* part, double& weight)
{
  MCParticle* mcPart = firstMCP(part);
  weight = NULL != mcPart ? weightMCP() : 0.;
  return mcPart;
}


MCParticle* 
Object2MCLink::firstMCP( const KeyedObject<int>* part)
{
  if( NULL == m_myAlg ) return NULL;

  std::string contName = 
    containerName(dynamic_cast<const ContainedObject*>(part)) ;
  if( "/Event/" == contName.substr(0,7) ) {
    contName = contName.substr(7);
  }
  if("" == contName) {
    m_linkTo = To( m_myAlg->eventSvc(), NULL, "");
    return NULL;
  }
  To* link =  getLink( contName );
  
  return NULL != link ? link->first(part) : NULL;
}


// Helper methods to create a LinkerWithKey table if needed
Object2MCLink::Linker* 
Object2MCLink::linkerTable( const std::string& name)
{
  if( NULL == m_myAlg ) return NULL;
  To test(m_myAlg->evtSvc(),NULL, name);
  return linkerTable( name, test);
}


Object2MCLink::Linker* 
Object2MCLink::linkerTable( const std::string& name, To& test)
{
  if( NULL == m_myAlg ) return NULL;
  if( test.notFound() ) {
    m_linkerTable = Object2MCLink::Linker( m_myAlg->evtSvc(), 
                                           m_myAlg->msgSvc(), 
                                           name);
    if( NULL != m_myGaudiAlg) {
      m_myGaudiAlg->debug() << "Linker table " << name << " created" <<endreq;
    }
    test = To( m_myAlg->evtSvc(), NULL, name);
    return &m_linkerTable;
  } else {
    if( NULL != m_myGaudiAlg)
      m_myGaudiAlg->debug() << "Linker table " << name << " found" <<endreq;
  }
  return NULL;
}

bool
Object2MCLink::checkAssociation( const KeyedObject<int>* obj,
                                 const MCParticle* mcPart)
{
  const MCParticle* mcp = firstMCP(obj);
  while( NULL != mcp ) {
    if( mcp == mcPart ) return true;
    mcp = nextMCP();
  }
  return false;
}
