// $Id: RootOutputStream.cpp,v 1.2 2010-09-27 15:43:53 frankb Exp $
#define GAUDISVC_PERSISTENCYSVC_OUTPUTSTREAM_CPP

// Framework include files
#include "GaudiKernel/Tokenizer.h"
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
#include "RootCnv/RootOutputStream.h"
#include "RootCnv/RootCnvSvc.h"

using namespace std;
using namespace Gaudi;

// Standard Constructor
RootOutputStream::RootOutputStream(const string& name, ISvcLocator* pSvcLocator)
: Algorithm(name, pSvcLocator)
{
  m_verifyItems    = true;
  m_output         = "";
  m_outputName     = "";
  m_outputType     = "UPDATE";
  m_storeName      = "EventDataSvc";
  m_persName       = "EventPersistencySvc";
  ///in the baseclass, always fire the incidents by default
  ///in e.g. RecordStream this will be set to false, and configurable
  m_fireIncidents  = true;
  declareProperty("ItemList",         m_itemNames);
  declareProperty("OptItemList",      m_optItemNames);
  declareProperty("Output",           m_output);
  declareProperty("OutputFile",       m_outputName);
  declareProperty("EvtDataSvc",       m_storeName);
  declareProperty("EvtConversionSvc", m_persName);
  declareProperty("AcceptAlgs",       m_acceptNames);
  declareProperty("RequireAlgs",      m_requireNames);
  declareProperty("VetoAlgs",         m_vetoNames);
  declareProperty("VerifyItems",      m_verifyItems);
  // Associate action handlers with the AcceptAlgs, RequireAlgs & VetoAlgs properties
  m_acceptNames.declareUpdateHandler (&RootOutputStream::acceptAlgsHandler , this);
  m_requireNames.declareUpdateHandler(&RootOutputStream::requireAlgsHandler, this);
  m_vetoNames.declareUpdateHandler   (&RootOutputStream::vetoAlgsHandler   , this);
}

// Standard Destructor
RootOutputStream::~RootOutputStream()   {
}

// initialize data writer
StatusCode RootOutputStream::initialize() {
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
StatusCode RootOutputStream::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Events output: " << m_events << endmsg;
  if(m_fireIncidents) m_incidentSvc->fireIncident(Incident(m_outputName,
    IncidentType::EndOutputFile));
  m_incidentSvc.reset();
  m_pDataProvider.reset();
  m_pDataManager.reset();
  m_pConversionSvc.reset();
  m_cnvSvc = 0;
  clearItems(m_optItemList);
  clearItems(m_itemList);
  return StatusCode::SUCCESS;
}

// Work entry point
StatusCode RootOutputStream::execute() {
  // Test whether this event should be output
  if ( isEventAccepted() )  {
    StatusCode sc = writeObjects();
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
StatusCode RootOutputStream::writeObjects()  {
  // Connect the output file to the service
  StatusCode status = m_pConversionSvc->connectOutput(m_outputName, m_outputType);
  if ( status.isSuccess() )   {
    set<DataObject*> unique;
    status = writeObjects(m_itemList,unique,true);
    if ( status.isSuccess() ) {
      status = writeObjects(m_optItemList,unique,false);
      if ( status.isSuccess() ) {
        unique.clear();
        status = writeObjectRefs(m_itemList,unique,true);
        if ( status.isSuccess() ) {
          status = writeObjectRefs(m_optItemList,unique,false);
        }
      }
    }
    // Commit the data if there was no error; otherwise possibly discard
    if ( status.isSuccess() )
      status = m_pConversionSvc->commitOutput(m_outputName, true);
    else
      m_pConversionSvc->commitOutput(m_outputName, false);
  }
  return status;
}


// Select the different objects and write them to file
StatusCode 
RootOutputStream::writeObjects(const Items& items,set<DataObject*>& unique, bool required)  {
  // Now pass the collection to the persistency service
  StatusCode status = StatusCode::SUCCESS;
  vector<IRegistry*> regs;

  for(Items::const_iterator i=items.begin(); i != items.end(); ++i)    {
    IOpaqueAddress* pAddress = 0;
    DataObject* obj = 0;
    StatusCode iret = m_pDataProvider->retrieveObject((*i)->path(), obj);
    if ( !iret.isSuccess() )      {
      if ( required ) status = iret;
      iret = m_cnvSvc->createNullRep((*i)->path());
      if ( !iret.isSuccess() )      {
        status = iret;
      }
      continue;
    }
    if ( unique.find(obj) != unique.end() ) {
      continue;
    }
    unique.insert(obj);

    IRegistry* pReg = obj->registry();
    iret = m_pConversionSvc->createRep( obj, pAddress );
    if ( !iret.isSuccess() )      {
      status = iret;
      continue;
    }
    pReg->setAddress(pAddress);
    regs.push_back(pReg);
  }
  return status;
}

// Select the different objects and write them to file
StatusCode 
RootOutputStream::writeObjectRefs(const Items& items,set<DataObject*>& unique,bool required)  {
  StatusCode status = StatusCode::SUCCESS;
  for(Items::const_iterator i=items.begin(); i != items.end(); ++i)    {
    DataObject* obj = 0;
    StatusCode iret = m_pDataProvider->retrieveObject((*i)->path(), obj);
    if ( !iret.isSuccess() )      {
      if ( required ) status = iret;
      iret = m_cnvSvc->createNullRef((*i)->path());
      if ( !iret.isSuccess() )      {
        status = iret;
      }
      continue;
    }
    if ( unique.find(obj) != unique.end() ) {
      continue;
    }
    unique.insert(obj);
    IRegistry* pReg = obj->registry();
    iret = m_pConversionSvc->fillRepRefs(pReg->address(),pReg->object());
    if ( !iret.isSuccess() )      {
      status = iret;
    }
  }
  return status;
}

// Remove all items from the output streamer list;
void RootOutputStream::clearItems(Items& itms)     {
  for ( Items::iterator i = itms.begin(); i != itms.end(); i++ )    {
    delete (*i);
  }
  itms.erase(itms.begin(), itms.end());
}

// Find single item identified by its path (exact match)
DataStoreItem*
RootOutputStream::findItem(const string& path)  {
  for(Items::const_iterator i=m_itemList.begin(); i != m_itemList.end(); ++i)  {
    if ( (*i)->path() == path )  return (*i);
  }
  for(Items::const_iterator j=m_optItemList.begin(); j != m_optItemList.end(); ++j)  {
    if ( (*j)->path() == path )  return (*j);
  }
  return 0;
}

// Add item to output streamer list
void RootOutputStream::addItem(Items& itms, const string& descriptor)   {
  MsgStream log(msgSvc(), name());
  int level = 0;
  size_t sep = descriptor.rfind("#");
  string obj_path (descriptor,0,sep);
  string slevel   (descriptor,sep+1,descriptor.length());
  if ( slevel == "*" )  {
    level = 9999999;
  }
  else   {
    level = atoi(slevel.c_str());
  }
  if ( m_verifyItems )  {
    size_t idx = obj_path.find("/",1);
    while(idx != string::npos)  {
      string sub_item = obj_path.substr(0,idx);
      if ( 0 == findItem(sub_item) )   {
        addItem(itms, sub_item+"#1");
      }
      idx = obj_path.find("/",idx+1);
    }
  }
  DataStoreItem* item = new DataStoreItem(obj_path, level);
  log << MSG::DEBUG << "Adding RootOutputStream item " << item->path()
    << " with " << item->depth()
    << " level(s)." << endmsg;
  itms.push_back( item );
}

// Connect to proper conversion service
StatusCode RootOutputStream::connectConversionSvc()   {
  StatusCode status = StatusCode(StatusCode::FAILURE, true);
  MsgStream log(msgSvc(), name());
  // Get output file from input
  string dbType, svc, shr;
  Tokenizer tok(true);
  tok.analyse(m_output, " ", "", "", "=", "'", "'");
  for(Tokenizer::Items::iterator i = tok.items().begin(); i != tok.items().end(); ++i)   {
    const string& tag = (*i).tag();
    const string& val = (*i).value();
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
  m_cnvSvc = 0;
  if ( !shr.empty() ) m_outputType += "|SHARED";
  // Get access to the default Persistency service
  // The default service is the same for input as for output.
  // If this is not desired, then a specialized RootOutputStream must overwrite
  // this value.
  if ( dbType.length() > 0 && svc.length()==0 )   {
    SmartIF<IPersistencySvc> ipers(serviceLocator()->service(m_persName));
    if( !ipers.isValid() )   {
      log << MSG::FATAL << "Unable to locate IPersistencySvc interface of " << m_persName << endmsg;
      return StatusCode::FAILURE;
    }
    IConversionSvc *cnvSvc = 0;
    status = ipers->getService(dbType, cnvSvc);
    if( !status.isSuccess() )   {
      log << MSG::FATAL << "Unable to locate IConversionSvc interface of database type " << dbType << endmsg;
      return status;
    }
    // Increase reference count and keep service.
    m_pConversionSvc = cnvSvc;
  }
  else if ( svc.length() > 0 )    {
    // On success reference count is automatically increased.
    m_pConversionSvc = serviceLocator()->service(svc);
    if( !m_pConversionSvc.isValid() )   {
      log << MSG::FATAL << "Unable to locate IConversionSvc interface of " << svc << endmsg;
      return StatusCode::FAILURE;
    }
  }
  else    {
    log << MSG::FATAL
      << "Unable to locate IConversionSvc interface (Unknown technology) " << endmsg
      << "You either have to specify a technology name or a service name!" << endmsg
      << "Please correct the job option \"" << name() << ".Output\" !"     << endmsg;
    return StatusCode::FAILURE;
  }
  m_cnvSvc = dynamic_cast<RootCnvSvc*>(m_pConversionSvc.get());
  return StatusCode::SUCCESS;
}

StatusCode RootOutputStream::decodeAcceptAlgs( ) {
  return decodeAlgorithms( m_acceptNames, &m_acceptAlgs );
}

void RootOutputStream::acceptAlgsHandler( Property& /* theProp */ )  {
  StatusCode sc = decodeAlgorithms( m_acceptNames, &m_acceptAlgs );
  if (sc.isFailure()) {
    throw GaudiException("Failure in RootOutputStream::decodeAlgorithms",
      "RootOutputStream::acceptAlgsHandler",sc);
  }
}

StatusCode RootOutputStream::decodeRequireAlgs( )  {
  return decodeAlgorithms( m_requireNames, &m_requireAlgs );
}

void RootOutputStream::requireAlgsHandler( Property& /* theProp */ )  {
  StatusCode sc = decodeAlgorithms( m_requireNames, &m_requireAlgs );
  if (sc.isFailure()) {
    throw GaudiException("Failure in RootOutputStream::decodeAlgorithms",
      "RootOutputStream::requireAlgsHandler",sc);
  }
}

StatusCode RootOutputStream::decodeVetoAlgs( )  {
  return decodeAlgorithms( m_vetoNames, &m_vetoAlgs );
}

void RootOutputStream::vetoAlgsHandler( Property& /* theProp */ )  {
  StatusCode sc = decodeAlgorithms( m_vetoNames, &m_vetoAlgs );
  if (sc.isFailure()) {
    throw GaudiException("Failure in RootOutputStream::decodeAlgorithms",
      "RootOutputStream::vetoAlgsHandler",sc);
  }
}

StatusCode 
RootOutputStream::decodeAlgorithms(StringArrayProperty& theNames,
                                   vector<Algorithm*>* theAlgs )  
{
  MsgStream log( msgSvc( ), name( ) );
  StatusCode result = StatusCode::FAILURE;
  SmartIF<IAlgManager> theAlgMgr(serviceLocator());

  // Reset the list of Algorithms
  theAlgs->clear( );
  if ( theAlgMgr.isValid() ) {
    // Build the list of Algorithms from the names list
    const vector<string> nameList = theNames.value( );
    vector<string>::const_iterator it;
    vector<string>::const_iterator itend = nameList.end( );
    for (it = nameList.begin(); it != itend; ++it) {
      // Check whether the supplied name corresponds to an existing
      // Algorithm object.
      const string &theName = (*it);
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
        vector<Algorithm*>::iterator ita;
        vector<Algorithm*>::iterator itaend = theAlgs->end( );
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

bool RootOutputStream::isEventAccepted( ) const  {
  typedef vector<Algorithm*>::const_iterator AlgIter;
  bool result = true;

  // Loop over all Algorithms in the accept list to see
  // whether any have been executed and have their filter
  // passed flag set. Any match causes the event to be
  // provisionally accepted.
  if ( ! m_acceptAlgs.empty() ) {
    result = false;
    for(AlgIter i=m_acceptAlgs.begin(),end=m_acceptAlgs.end(); i != end; ++i) {
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
  if ( result && ! m_requireAlgs.empty() ) {
    for(AlgIter i=m_requireAlgs.begin(),end=m_requireAlgs.end(); i != end; ++i) {
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
  if ( result && ! m_vetoAlgs.empty() ) {
    for(AlgIter i=m_vetoAlgs.begin(),end=m_vetoAlgs.end(); i != end; ++i) {
      if ( (*i)->isExecuted() && (*i)->filterPassed() ) {
        result = false;
        break;
      }
    }
  }
  return result;
}

bool RootOutputStream::hasInput() const {
  return !(m_itemNames.empty() && m_optItemNames.empty());
}
