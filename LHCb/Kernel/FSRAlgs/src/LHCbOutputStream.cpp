// $Id: LHCbOutputStream.cpp,v 1.23 2008/01/15 13:46:52 marcocle Exp $
#define LHCB_OUTPUTSTREAM_CPP

// Framework include files
#include "GaudiKernel/Tokenizer.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IPersistencySvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/strcasecmp.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/DataStoreItem.h"
#include "LHCbOutputStream.h"
#include "LHCbOutputStreamAgent.h"

#include <set>

// Define the algorithm factory for the standard output data writer
DECLARE_ALGORITHM_FACTORY(LHCbOutputStream)

// Standard Constructor
LHCbOutputStream::LHCbOutputStream(const std::string& name, ISvcLocator* pSvcLocator)
 : Algorithm(name, pSvcLocator)
{
  m_doPreLoad      = true;
  m_doPreLoadOpt   = false;
  m_verifyItems    = true;
  m_output         = "";
  m_outputName     = "";
  m_outputType     = "UPDATE";
  m_storeName      = "EventDataSvc";
  m_persName       = "EventPersistencySvc";
  m_agent          = new LHCbOutputStreamAgent(this);
  m_acceptAlgs     = new std::vector<Algorithm*>();
  m_requireAlgs    = new std::vector<Algorithm*>();
  m_vetoAlgs       = new std::vector<Algorithm*>();
  ///in the baseclass, always fire the incidents by default
  ///in e.g. RecordStream this will be set to false, and configurable
  m_fireIncidents  = true;
  declareProperty("ItemList",         m_itemNames);
  declareProperty("OptItemList",      m_optItemNames);
  declareProperty("Preload",          m_doPreLoad);
  declareProperty("PreloadOptItems",  m_doPreLoadOpt);
  declareProperty("Output",           m_output);
  declareProperty("OutputFile",       m_outputName);
  declareProperty("EvtDataSvc",       m_storeName);
  declareProperty("EvtConversionSvc", m_persName);
  declareProperty("AcceptAlgs",       m_acceptNames);
  declareProperty("RequireAlgs",      m_requireNames);
  declareProperty("VetoAlgs",         m_vetoNames);
  declareProperty("VerifyItems",      m_verifyItems);
  ///in the baseclass, always fire the incidents by default
  ///in RecordStream this will be set to false, and configurable

  // Associate action handlers with the AcceptAlgs, RequireAlgs & VetoAlgs properties
  m_acceptNames.declareUpdateHandler ( &LHCbOutputStream::acceptAlgsHandler , this );
  m_requireNames.declareUpdateHandler( &LHCbOutputStream::requireAlgsHandler, this );
  m_vetoNames.declareUpdateHandler   ( &LHCbOutputStream::vetoAlgsHandler   , this );
}

// Standard Destructor
LHCbOutputStream::~LHCbOutputStream()   {
  delete m_agent;
  delete m_acceptAlgs;
  delete m_requireAlgs;
  delete m_vetoAlgs;
}

// initialize data writer
StatusCode LHCbOutputStream::initialize() {
  MsgStream log(msgSvc(), name());

  // Reset the number of events written
  m_events = 0;
  // Get access to the DataManagerSvc
  m_pDataManager = serviceLocator()->service(m_storeName);
  if( !m_pDataManager.isValid() )   {
    log << MSG::FATAL << "Unable to locate IDataManagerSvc interface" << endmsg;
    return StatusCode::FAILURE;
  }
  // Get access to the IncidentService
  m_incidentSvc = serviceLocator()->service("IncidentSvc");
  if( !m_incidentSvc.isValid() )  {
    log << MSG::WARNING << "Error retrieving IncidentSvc." << endmsg;
    return StatusCode::FAILURE;
  }
  // Get access to the assigned data service
  m_pDataProvider = serviceLocator()->service(m_storeName);
  if( !m_pDataProvider.isValid() )   {
    log << MSG::FATAL << "Unable to locate IDataProviderSvc interface of " << m_storeName << endmsg;
    return StatusCode::FAILURE;
  }
  if ( hasInput() )  {
    StatusCode status = connectConversionSvc();
    if( !status.isSuccess() )   {
      log << MSG::FATAL << "Unable to connect to conversion service." << endmsg;
      if(m_outputName!="" && m_fireIncidents) m_incidentSvc->fireIncident(Incident(m_outputName,
                                           IncidentType::FailOutputFile));
      return status;
    }
  }

  // Clear the list with optional items
  clearItems(m_optItemList);
  // Clear the item list
  clearItems(m_itemList);

  ItemNames::iterator i;
  // Take the new item list from the properties.
  for(i = m_itemNames.begin(); i != m_itemNames.end(); i++)   {
    addItem( m_itemList, *i );
  }

  // Take the new item list from the properties.
  for(i = m_optItemNames.begin(); i != m_optItemNames.end(); i++)   {
    addItem( m_optItemList, *i );
  }

  // Take the item list to the data service preload list.
  if ( m_doPreLoad )    {
    for(Items::iterator j = m_itemList.begin(); j != m_itemList.end(); j++)   {
      m_pDataProvider->addPreLoadItem( *(*j) ).ignore();
    }
    // Not working: bad reference counting! pdataSvc->release();
  }

  if ( m_doPreLoadOpt )    {
    for(Items::iterator j=m_optItemList.begin(); j!=m_optItemList.end(); j++) {
      m_pDataProvider->addPreLoadItem( *(*j) );
    }
  }
  log << MSG::INFO << "Data source: " << m_storeName  << " output: " << m_output << endmsg;

  // Decode the accept, required and veto Algorithms. The logic is the following:
  //  a. The event is accepted if all lists are empty.
  //  b. The event is provisionally accepted if any Algorithm in the accept list
  //     has been executed and has indicated that its filter is passed. This
  //     provisional acceptance can be overridden by the other lists.
  //  c. The event is rejected unless all Algorithms in the required list have
  //     been executed and have indicated that their filter passed.
  //  d. The event is rejected if any Algorithm in the veto list has been
  //     executed and has indicated that its filter has passed.
  decodeAcceptAlgs ().ignore();
  decodeRequireAlgs().ignore();
  decodeVetoAlgs   ().ignore();
  return StatusCode::SUCCESS;
}

// terminate data writer
StatusCode LHCbOutputStream::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Events output: " << m_events << endmsg;
  if(m_fireIncidents) m_incidentSvc->fireIncident(Incident(m_outputName,
                                       IncidentType::EndOutputFile));
  m_incidentSvc.reset();
  m_pDataProvider.reset();
  m_pDataManager.reset();
  m_pConversionSvc.reset();
  clearItems(m_optItemList);
  clearItems(m_itemList);
  return StatusCode::SUCCESS;
}

// Work entry point
StatusCode LHCbOutputStream::execute() {
  // Clear any previously existing item list
  clearSelection();
  // Test whether this event should be output
  if ( isEventAccepted() )  {
    StatusCode sc = writeObjects();
    clearSelection();
    m_events++;
    if(sc.isSuccess() && m_fireIncidents)
      m_incidentSvc->fireIncident(Incident(m_outputName,
                                           IncidentType::WroteToOutputFile));
    else if(m_fireIncidents)
      m_incidentSvc->fireIncident(Incident(m_outputName,
                                           IncidentType::FailOutputFile));
    return sc;
  }
  return StatusCode::SUCCESS;
}

// Select the different objects and write them to file
StatusCode LHCbOutputStream::writeObjects()  {
  // Connect the output file to the service
  StatusCode status = collectObjects();
  if ( status.isSuccess() )   {
    IDataSelector*  sel = selectedObjects();
    if ( sel->begin() != sel->end() )  {
      status = m_pConversionSvc->connectOutput(m_outputName, m_outputType);
      if ( status.isSuccess() )   {
        // Now pass the collection to the persistency service
        IDataSelector::iterator j;
        IOpaqueAddress* pAddress = 0;
        for ( j = sel->begin(); j != sel->end(); j++ )    {
          StatusCode iret = m_pConversionSvc->createRep( *j, pAddress );
          if ( !iret.isSuccess() )      {
            status = iret;
            continue;
          }
          IRegistry* pReg = (*j)->registry();
          pReg->setAddress(pAddress);
        }
        for ( j = sel->begin(); j != sel->end(); j++ )    {
          IRegistry* pReg = (*j)->registry();
          StatusCode iret = m_pConversionSvc->fillRepRefs( pReg->address(), *j );
          if ( !iret.isSuccess() )      {
            status = iret;
          }
        }
        // Commit the data if there was no error; otherwise possibly discard
        if ( status.isSuccess() )  {
          status = m_pConversionSvc->commitOutput(m_outputName, true);
        }
        else   {
          m_pConversionSvc->commitOutput(m_outputName, false).ignore();
        }
      }
    }
  }
  return status;
}

// Place holder to create configurable data store agent
bool LHCbOutputStream::collect(IRegistry* dir, int level)    {
  if ( level < m_currentItem->depth() )   {
    if ( dir->object() != 0 )   {
      /*
      std::cout << "Analysing ("
                << dir->name()
                << ") Object:"
                << ((dir->object()==0) ? "UNLOADED" : "LOADED")
                << std::endl;
      */
      m_objects.push_back(dir->object());
      return true;
    }
  }
  return false;
}

/// Collect all objects to be written to the output stream
StatusCode LHCbOutputStream::collectObjects()   {
  MsgStream log(msgSvc(), name());
  StatusCode status = StatusCode::SUCCESS;
  Items::iterator i;
  // Traverse the tree and collect the requested objects
  for ( i = m_itemList.begin(); i != m_itemList.end(); i++ )    {
    DataObject* obj = 0;
    m_currentItem = (*i);
    StatusCode iret = m_pDataProvider->retrieveObject(m_currentItem->path(), obj);
    if ( iret.isSuccess() )  {
      iret = m_pDataManager->traverseSubTree(obj, m_agent);
      if ( !iret.isSuccess() )  {
        status = iret;
      }
    }
    else  {
      log << MSG::ERROR << "Cannot write mandatory object(s) (Not found) "
          << m_currentItem->path() << endmsg;
      status = iret;
    }
  }
  // Traverse the tree and collect the requested objects (tolerate missing items here)
  for ( i = m_optItemList.begin(); i != m_optItemList.end(); i++ )    {
    DataObject* obj = 0;
    m_currentItem = (*i);
    StatusCode iret = m_pDataProvider->retrieveObject(m_currentItem->path(), obj);
    if ( iret.isSuccess() )  {
      iret = m_pDataManager->traverseSubTree(obj, m_agent);
    }
    if ( !iret.isSuccess() )    {
      if(log.level() <= MSG::DEBUG )
        log << MSG::DEBUG << "Ignore request to write non-mandatory object(s) "
            << m_currentItem->path() << endmsg;
    }
  }

  if (status.isSuccess()){
    // Remove duplicates from the list of objects, preserving the order in the list
    std::set<DataObject*> unique;
    std::vector<DataObject*> tmp; // temporary vector with the reduced list
    tmp.reserve(m_objects.size());
    for (std::vector<DataObject*>::iterator o = m_objects.begin(); o != m_objects.end(); ++o) {
      if (!unique.count(*o)) {
        // if the pointer is not in the set, add it to both the set and the temporary vector
        unique.insert(*o);
        tmp.push_back(*o);
      }
    }
    m_objects.swap(tmp); // swap the data of the two vectors
  }

  return status;
}

// Clear collected object list
void LHCbOutputStream::clearSelection()     {
  m_objects.erase(m_objects.begin(), m_objects.end());
}

// Remove all items from the output streamer list;
void LHCbOutputStream::clearItems(Items& itms)     {
  for ( Items::iterator i = itms.begin(); i != itms.end(); i++ )    {
    delete (*i);
  }
  itms.erase(itms.begin(), itms.end());
}

// Find single item identified by its path (exact match)
DataStoreItem*
LHCbOutputStream::findItem(const std::string& path)  {
  for(Items::const_iterator i=m_itemList.begin(); i != m_itemList.end(); ++i)  {
    if ( (*i)->path() == path )  return (*i);
  }
  for(Items::const_iterator j=m_optItemList.begin(); j != m_optItemList.end(); ++j)  {
    if ( (*j)->path() == path )  return (*j);
  }
  return 0;
}

// Add item to output streamer list
void LHCbOutputStream::addItem(Items& itms, const std::string& descriptor)   {
	MsgStream log(msgSvc(), name());
  int level = 0;
  size_t sep = descriptor.rfind("#");
  std::string obj_path (descriptor,0,sep);
  std::string slevel   (descriptor,sep+1,descriptor.length());
  if ( slevel == "*" )  {
    level = 9999999;
  }
  else   {
    level = atoi(slevel.c_str());
  }
  if ( m_verifyItems )  {
    size_t idx = obj_path.find("/",1);
    while(idx != std::string::npos)  {
      std::string sub_item = obj_path.substr(0,idx);
      if ( 0 == findItem(sub_item) )   {
        addItem(itms, sub_item+"#1");
      }
      idx = obj_path.find("/",idx+1);
    }
  }
  DataStoreItem* item = new DataStoreItem(obj_path, level);
  if(log.level() <= MSG::DEBUG )
    log << MSG::DEBUG << "Adding LHCbOutputStream item " << item->path()
        << " with " << item->depth()
        << " level(s)." << endmsg;
  itms.push_back( item );
}

// Connect to proper conversion service
StatusCode LHCbOutputStream::connectConversionSvc()   {
  StatusCode status = StatusCode(StatusCode::FAILURE, true);
  MsgStream log(msgSvc(), name());
  // Get output file from input
  std::string dbType, svc, shr;
  Tokenizer tok(true);
  tok.analyse(m_output, " ", "", "", "=", "'", "'");
  for(Tokenizer::Items::iterator i = tok.items().begin(); i != tok.items().end(); ++i)   {
    const std::string& tag = (*i).tag();
    const std::string& val = (*i).value();
    switch( ::toupper(tag[0]) )    {
    case 'D':
      m_outputName = val;
      break;
    case 'T':
      dbType = val;
      break;
    case 'S':
      switch( ::toupper(tag[1]) )   {
      case 'V':    svc = val;      break;
      case 'H':    shr = "YES";    break;
      }
      break;
    case 'O':   // OPT='<NEW<CREATE,WRITE,RECREATE>, UPDATE>'
      switch( ::toupper(val[0]) )   {
      case 'R':
        if ( ::strncasecmp(val.c_str(),"RECREATE",3)==0 )
          m_outputType = "RECREATE";
        else if ( ::strncasecmp(val.c_str(),"READ",3)==0 )
          m_outputType = "READ";
        break;
      case 'C':
      case 'N':
      case 'W':
        m_outputType = "NEW";
        break;
      case 'U':
        m_outputType = "UPDATE";
        break;
      default:
        m_outputType = "???";
        break;
      }
      break;
    default:
      break;
    }
  }
  if ( !shr.empty() ) m_outputType += "|SHARED";
  // Get access to the default Persistency service
  // The default service is the same for input as for output.
  // If this is not desired, then a specialized LHCbOutputStream must overwrite
  // this value.
  if ( dbType.length() > 0 || svc.length() > 0 )   {
    std::string typ = dbType.length()>0 ? dbType : svc;
    SmartIF<IPersistencySvc> ipers(serviceLocator()->service(m_persName));
    if( !ipers.isValid() )   {
      log << MSG::FATAL << "Unable to locate IPersistencySvc interface of " << m_persName << endmsg;
      return StatusCode::FAILURE;
    }
    IConversionSvc *cnvSvc = 0;
    status = ipers->getService(typ, cnvSvc);
    if( !status.isSuccess() )   {
      log << MSG::FATAL << "Unable to locate IConversionSvc interface of database type " << typ << endmsg;
      return status;
    }
    // Increase reference count and keep service.
    m_pConversionSvc = cnvSvc;
  }
  else    {
    log << MSG::FATAL
        << "Unable to locate IConversionSvc interface (Unknown technology) " << endmsg
        << "You either have to specify a technology name or a service name!" << endmsg
        << "Please correct the job option \"" << name() << ".Output\" !"     << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode LHCbOutputStream::decodeAcceptAlgs( ) {
  return decodeAlgorithms( m_acceptNames, m_acceptAlgs );
}

void LHCbOutputStream::acceptAlgsHandler( Property& /* theProp */ )  {
  StatusCode sc = decodeAlgorithms( m_acceptNames, m_acceptAlgs );
  if (sc.isFailure()) {
    throw GaudiException("Failure in LHCbOutputStream::decodeAlgorithms",
                         "LHCbOutputStream::acceptAlgsHandler",sc);
  }
}

StatusCode LHCbOutputStream::decodeRequireAlgs( )  {
  return decodeAlgorithms( m_requireNames, m_requireAlgs );
}

void LHCbOutputStream::requireAlgsHandler( Property& /* theProp */ )  {
  StatusCode sc = decodeAlgorithms( m_requireNames, m_requireAlgs );
  if (sc.isFailure()) {
    throw GaudiException("Failure in LHCbOutputStream::decodeAlgorithms",
                         "LHCbOutputStream::requireAlgsHandler",sc);
  }
}

StatusCode LHCbOutputStream::decodeVetoAlgs( )  {
  return decodeAlgorithms( m_vetoNames, m_vetoAlgs );
}

void LHCbOutputStream::vetoAlgsHandler( Property& /* theProp */ )  {
  StatusCode sc = decodeAlgorithms( m_vetoNames, m_vetoAlgs );
  if (sc.isFailure()) {
    throw GaudiException("Failure in LHCbOutputStream::decodeAlgorithms",
                         "LHCbOutputStream::vetoAlgsHandler",sc);
  }
}

StatusCode LHCbOutputStream::decodeAlgorithms( StringArrayProperty& theNames,
                                           std::vector<Algorithm*>* theAlgs )
{
  // Reset the list of Algorithms
  theAlgs->clear( );

  MsgStream log( msgSvc( ), name( ) );

  StatusCode result = StatusCode::FAILURE;

  SmartIF<IAlgManager> theAlgMgr(serviceLocator());
  if ( theAlgMgr.isValid() ) {
    // Build the list of Algorithms from the names list
    const std::vector<std::string> nameList = theNames.value( );
    std::vector<std::string>::const_iterator it;
    std::vector<std::string>::const_iterator itend = nameList.end( );
    for (it = nameList.begin(); it != itend; ++it) {
      // Check whether the supplied name corresponds to an existing
      // Algorithm object.
      const std::string &theName = (*it);
      SmartIF<IAlgorithm> &theIAlg = theAlgMgr->algorithm(theName);
      Algorithm*  theAlgorithm;
      if ( theIAlg.isValid() ) {
        result = StatusCode::SUCCESS;
        try{
          theAlgorithm = dynamic_cast<Algorithm*>(theIAlg.get());
        } catch(...){
          result = StatusCode::FAILURE;
        }
      }
      if ( result.isSuccess( ) ) {
        // Check that the specified algorithm doesn't already exist in the list
        std::vector<Algorithm*>::iterator ita;
        std::vector<Algorithm*>::iterator itaend = theAlgs->end( );
        for (ita = theAlgs->begin(); ita != itaend; ++ita) {
          Algorithm* existAlgorithm = (*ita);
          if ( theAlgorithm == existAlgorithm ) {
            result = StatusCode::FAILURE;
            break;
          }
        }
        if ( result.isSuccess( ) ) {
          theAlgorithm->addRef();
          theAlgs->push_back( theAlgorithm );
        }
      }
      else {
        log << MSG::INFO << theName << " doesn't exist - ignored" << endmsg;
      }
    }
    result = StatusCode::SUCCESS;
  }
  else {
    log << MSG::FATAL << "Can't locate ApplicationMgr!!!" << endmsg;
  }
  return result;
}

bool LHCbOutputStream::isEventAccepted( ) const  {
  typedef std::vector<Algorithm*>::iterator AlgIter;
  bool result = true;

  // Loop over all Algorithms in the accept list to see
  // whether any have been executed and have their filter
  // passed flag set. Any match causes the event to be
  // provisionally accepted.
  if ( ! m_acceptAlgs->empty() ) {
    result = false;
    for(AlgIter i=m_acceptAlgs->begin(),end=m_acceptAlgs->end(); i != end; ++i) {
      if ( (*i)->isExecuted() && (*i)->filterPassed() ) {
        result = true;
        break;
      }
    }
  }

  // Loop over all Algorithms in the required list to see
  // whether all have been executed and have their filter
  // passed flag set. Any mismatch causes the event to be
  // rejected.
  if ( result && ! m_requireAlgs->empty() ) {
    for(AlgIter i=m_requireAlgs->begin(),end=m_requireAlgs->end(); i != end; ++i) {
      if ( !(*i)->isExecuted() || !(*i)->filterPassed() ) {
        result = false;
        break;
      }
    }
  }

  // Loop over all Algorithms in the veto list to see
  // whether any have been executed and have their filter
  // passed flag set. Any match causes the event to be
  // rejected.
  if ( result && ! m_vetoAlgs->empty() ) {
    for(AlgIter i=m_vetoAlgs->begin(),end=m_vetoAlgs->end(); i != end; ++i) {
      if ( (*i)->isExecuted() && (*i)->filterPassed() ) {
        result = false;
        break;
      }
    }
  }
  return result;
}

bool LHCbOutputStream::hasInput() const {
  return !(m_itemNames.empty() && m_optItemNames.empty());
}
