// $Id: MergeEventAlg.cpp,v 1.1.1.1 2003-06-23 18:12:45 gcorti Exp $
#define MERGEEVENTALG_CPP 
// Include files 

// from STL
#include <iostream>

// from Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcManager.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/DataStoreItem.h"

// from LHCb
#include "Event/EventHeader.h"

// local
#include "MergeEventAlg.h"
//-----------------------------------------------------------------------------
// Implementation file for class : MergeEventAlg.
// This algorithm (based on SpillOverAlg) reads additional
// events into the Event Data store and puts them in other paths in the
// TES. It uses an "other Event Selector".
// for the moment it only allows merging of LHC background as a flat merging
//
// 2003-06-23 : Gloria CORTI
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const AlgFactory<MergeEventAlg>  Factory;
const IAlgFactory& MergeEventAlgFactory = Factory;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MergeEventAlg::MergeEventAlg(const std::string& name, 
                             ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator)
  , m_mergeType( "LHCBackground" )
  , m_mergeSelectorName( "LHCBkgSelector" )
  , m_mergeSelector(0)
  , m_mergeIt(0) 
{
  // Declare the algorithm's properties
  declareProperty( "MergeType",     m_mergeType );
  declareProperty( "EventSelector", m_mergeSelectorName );
	declareProperty( "ItemList",      m_itemNames );
}

//=============================================================================
// Destructor
//=============================================================================
MergeEventAlg::~MergeEventAlg( ) {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MergeEventAlg::initialize() {

  MsgStream msg(msgSvc(), name());

  // Check that the merging type is of valid type
  if( m_mergeType != "LHCBackground" ) {
    msg << MSG::INFO << "Invalid merging type" << endreq;
    return StatusCode::FAILURE;
  }

  // Print the fact that this background will be loaded
  msg << MSG::INFO 
      << m_mergeType << " events will be added to the main event"
      << endreq;
  
  // Get the service manager interface of the service locator
  SmartIF<ISvcManager> svcMgr  ( IID_ISvcManager, serviceLocator());

  // Create and initialize the Merge Event Selector
  IService* pISvc;
  StatusCode sc = svcMgr->createService( "EventSelector",
                                         m_mergeSelectorName, pISvc );
  if( sc.isFailure() )  {
    msg << MSG::FATAL << "Error creating mergeSelector" << endreq;
    return sc;
  }

  // Get the necessary base class and interface
  m_mergeSelector = dynamic_cast<Service*>(pISvc);
  m_mergeISelector = dynamic_cast<IEvtSelector*>(pISvc);
  sc = m_mergeSelector->initialize();
  if( sc.isFailure() )  {
    msg << MSG::FATAL << "Error initializing " << m_mergeSelectorName << endreq;
    return sc;
  }
 
  // Clear the item list that will be loaded by this event selector
  clearItems(m_itemList);

  ItemNames::iterator i;
  // Take the new item list from the properties.
  for(i = m_itemNames.begin(); i != m_itemNames.end(); i++)   {
    addItem( m_itemList, *i );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Remove all items from the list
//=============================================================================
void MergeEventAlg::clearItems(Items& itms)     {
  for ( Items::iterator i = itms.begin(); i != itms.end(); i++ )    {  
    delete (*i);
  }
  itms.erase(itms.begin(), itms.end());
}

//=============================================================================
// Add item to the list
//=============================================================================
void MergeEventAlg::addItem(Items& itms, const std::string& descriptor)   {

	MsgStream msg(msgSvc(), name());
  int sep = descriptor.rfind("#");
  int level = 0;
  std::string obj_path (descriptor,0,sep);
  std::string slevel   (descriptor,sep+1,descriptor.length());
  if ( slevel == "*" )  {
    level = 9999999;
  }
  else   {
    level = atoi(slevel.c_str());
  }

  std::string newPath = resetPath( obj_path );

  DataStoreItem* item = new DataStoreItem(newPath, level);
  msg << MSG::DEBUG << "Will load and reset path for item " << item->path()
      << " with " << item->depth() 
      << " level(s)." << endreq;
  itms.push_back( item );
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MergeEventAlg::execute() {

  StatusCode sc = StatusCode::FAILURE;
  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "execute" << endmsg;

  // Get requested LHC background events
  std::string subPath = "LHCBackground";
  sc = readAndLoadEvent( subPath );
  if( !sc.isSuccess() ) {
    msg << MSG::ERROR << "Error in loading LHCBackground" << endreq;
    return sc;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MergeEventAlg::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "finalize" << endmsg;

  clearItems(m_itemList);
  m_mergeSelector->finalize();

  return StatusCode::SUCCESS;
} 

//=============================================================================
//  Read events via additional event selector and load in new path all
//  specified items
//=============================================================================
StatusCode MergeEventAlg::readAndLoadEvent( std::string& subPath ) {

  MsgStream msg( msgSvc(), name() ); 
  msg << MSG::DEBUG << "readAndLoadEvent" << endmsg;
 
  if ( m_mergeIt == 0 ) {  // first event from this algorithm event selector
     m_mergeIt = m_mergeISelector->begin(); // Open file and read event
  }
  else {    // Read next event
    *(m_mergeIt) = m_mergeISelector->next(*m_mergeIt);
  }  

  if(*(m_mergeIt) == *(m_mergeISelector->end())){
     msg << MSG::ERROR << "Last event reached on " << m_mergeSelectorName 
         << " input stream" << endreq;
     return StatusCode::FAILURE;
  }

  // Find the /Event directory node of the background event.
  std::string eventPath = "/Event/"+subPath; 
  IOpaqueAddress* iadd = *(*m_mergeIt);
  SmartIF<IDataManagerSvc> dataMgr( eventSvc() );
  StatusCode scr = StatusCode::FAILURE;
  scr = dataMgr->registerAddress( eventPath, iadd );
  if( !scr.isSuccess() ) {
    msg << MSG::ERROR << "Error setting event root to " << eventPath 
        << endreq;
    return scr;
  }

  DataObject *pEvent = 0;
  StatusCode sc = eventSvc()->retrieveObject( eventPath, pEvent );
  if( !sc.isSuccess() ) {
    msg << MSG::ERROR << "Unable to retrieve " << eventPath << endreq;
    return sc;
  }
  
  // Load Event Header
  SmartDataPtr<EventHeader> evt( eventSvc(), 
                                 eventPath+"/"+EventHeaderLocation::Default );
  if( 0 == evt ) {
    msg << MSG::ERROR << "Unable to retrieve " 
        <<eventPath<< "/Header" << endreq;
     return StatusCode::FAILURE;
  }
  else {
    msg << MSG::DEBUG << "Loading " << eventPath << " event Number "
        << evt->evtNum() << ",     Run " << evt->runNum() << endreq;
  }
  
  // Read and load list as in properties
  StatusCode scLinks  = StatusCode::FAILURE;
  StatusCode scLeaves = StatusCode::FAILURE;
  Items::iterator i;
  for ( i = m_itemList.begin(); i != m_itemList.end(); i++ )    {
    std::string mcPath = eventPath + "/" + (*i)->path();
    DataObject* pMC = NULL;
    sc = eventSvc()->retrieveObject( mcPath, pMC );
    if( sc.isSuccess() )  {
      // Reset links for object
      scLinks = resetLinks( subPath, pMC );
      if( !scLinks.isSuccess() ) {
        msg << MSG::ERROR << "Error in setting links for " 
            << mcPath << endreq;
        return scLinks;
      }
      // Read all leaves down to specified depth
      scLeaves = readLeaves( subPath, pMC, (*i)->depth() );
      if( !scLeaves.isSuccess() ) {
        msg << MSG::ERROR << "Error in loading leaves"
            << endreq;
        return scLeaves;
      }
    }
    else {
      msg << MSG::ERROR << "Required Item " << mcPath << " is missing " 
          << endreq;
      return sc;
    }
  }

  return StatusCode::SUCCESS;

}

//=============================================================================
// Read leaves attached to an object until specified depth and reset 
// links' path
//=============================================================================
StatusCode MergeEventAlg::readLeaves( std::string& subPath,
                                      const DataObject* pObj, long depth ) {
  
  MsgStream msg( msgSvc(), name() );
  
  depth = depth-1;
  if( depth <=0 ) return StatusCode::SUCCESS;
  
  // Find and load all the leafs out of the object
  std::vector<IRegistry*> leaves;
  SmartIF<IDataManagerSvc> dataMgr( eventSvc() );
  StatusCode sc = dataMgr->objectLeaves(pObj, leaves);
  if( !sc.isSuccess() ) return sc;
  DataObject *pMCObj = 0;
  std::string objPath;
  for( std::vector<IRegistry*>::iterator ileaf=leaves.begin();
       ileaf!=leaves.end(); ++ileaf) {
    sc = eventSvc()->retrieveObject(*ileaf, objPath, pMCObj );
    if( sc.isFailure() ) {
      return sc;
    } 
    sc = resetLinks( subPath, pMCObj );
    if( !sc.isSuccess() ) return sc;
    sc = readLeaves( subPath, pMCObj, depth );
    if( !sc.isSuccess() ) return sc;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Reset links to be consistent with new path
//=============================================================================
StatusCode MergeEventAlg::resetLinks( std::string& subPath,
                                      const DataObject* pMCObj ) {

  MsgStream msg( msgSvc(), name() );

  LinkManager::LinkVector oldlinks = pMCObj->linkMgr()->m_linkVector;
  if( oldlinks.empty() ) return StatusCode::SUCCESS;
  std::string mainPath = "/Event";
  LinkManager::LinkIterator ilink;
  std::vector< std::pair< std::string, long > > refs;
  for(ilink = oldlinks.begin(); ilink!= oldlinks.end(); ++ilink) {
    refs.push_back( std::make_pair( (*ilink)->path(), (*ilink)->ID() ) );
  }
  pMCObj->linkMgr()->clearLinks();
  for( std::vector< std::pair< std::string, long > >::iterator il=refs.begin();
       refs.end()!=il; ++il ) {
    std::string newPath = resetPath( (*il).first, subPath );
    long itemp = (*il).second;
    long lid = pMCObj->linkMgr()->addLink( newPath, NULL );
    if( lid != itemp ) {
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Reset path if subPath empty remove event if it exist from path
// if not add subPath after event, if event is not in path
//=============================================================================
std::string MergeEventAlg::resetPath( std::string& oldPath,
                                      const std::string& subPath ) {

  // Check if /Event already in path and reset
  std::string newPath = oldPath;
  std::string mainPath = "/Event";
  unsigned int pos = newPath.find(mainPath);
  if( pos != std::string::npos ) {
    newPath = newPath.erase( pos, mainPath.length()+1 );
    if( subPath != "" ) {
      newPath = mainPath+"/"+subPath+"/"+newPath;
    }
  }
  else {
    newPath = subPath+newPath;
  }
  
  return newPath;
}

//=============================================================================
