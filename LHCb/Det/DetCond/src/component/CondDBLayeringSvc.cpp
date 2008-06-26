// $Id: CondDBLayeringSvc.cpp,v 1.5 2008-06-26 14:22:45 marcocle Exp $
// Include files 

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Time.h"

// local
#include "CondDBLayeringSvc.h"
#include "CondDBCommon.h"

// Factory implementation
DECLARE_SERVICE_FACTORY(CondDBLayeringSvc)

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBLayeringSvc
//
// 2006-07-14 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBLayeringSvc::CondDBLayeringSvc( const std::string& name, ISvcLocator* svcloc ):
  Service(name,svcloc) {
  
  declareProperty("Layers",  m_layersNames );

  declareProperty("EnableXMLDirectMapping", m_xmlDirectMapping = true,
                  "Allow direct mapping from CondDB structure to"
                  " transient store.");

}
//=============================================================================
// Destructor
//=============================================================================
CondDBLayeringSvc::~CondDBLayeringSvc() {} 

//=============================================================================
// queryInterface
//=============================================================================
StatusCode CondDBLayeringSvc::queryInterface(const InterfaceID& riid,
                                               void** ppvUnknown){
  if ( IID_ICondDBReader.versionMatch(riid) ) {
    *ppvUnknown = (ICondDBReader*)this;
    addRef();
    return SUCCESS;
  } else if ( IID_ICondDBInfo.versionMatch(riid) )   {
    *ppvUnknown = (ICondDBInfo*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface(riid,ppvUnknown);
}

//=============================================================================
// initialize
//=============================================================================
StatusCode CondDBLayeringSvc::initialize(){
  StatusCode sc = Service::initialize();
  if (sc.isFailure()) return sc;

  MsgStream log(msgSvc(), name() );

  log << MSG::DEBUG << "Initialize" << endmsg;

  // locate all the AccessSvcs layers
  std::vector<std::string>::iterator lname;
  for ( lname = m_layersNames.begin(); lname != m_layersNames.end(); ++lname ) {
    
    ICondDBReader *svcPtr;
    sc = service(*lname,svcPtr,true);
    if (  !sc.isSuccess() ) {
      log << MSG::ERROR << "Could not locate " << *lname << endreq;
      return sc;
    }

    m_layers.push_back(svcPtr);
    log << MSG::DEBUG << "Retrieved '" << *lname << "'" << endreq;

  }
  
  return sc;
}

//=============================================================================
// finalize
//=============================================================================
StatusCode CondDBLayeringSvc::finalize(){
  MsgStream log(msgSvc(), name() );
  log << MSG::DEBUG << "Finalize" << endmsg;

  std::vector<ICondDBReader*>::iterator layer;
  for ( layer = m_layers.begin(); layer != m_layers.end(); ++layer ) {
    if ( *layer ) (*layer)->release();
  }
  m_layers.clear();

  return Service::finalize();
}

//=========================================================================
//  retrieve an object
//=========================================================================
StatusCode CondDBLayeringSvc::getObject (const std::string &path, const Gaudi::Time &when,
                                           DataPtr &data,
                                           std::string &descr, Gaudi::Time &since, Gaudi::Time &until, cool::ChannelId channel)
{
  StatusCode sc;
  if (m_xmlDirectMapping && isFolderSet(path)) {
    descr = "Catalog generated automatically by " + name();
    since = Gaudi::Time::epoch();
    until = Gaudi::Time::max();
    sc = CondDB::generateXMLCatalog(this,path,data);
  } else {
    std::vector<ICondDBReader*>::iterator layer;
    sc = StatusCode::FAILURE;
    for ( layer = m_layers.begin();
          layer != m_layers.end() && sc.isFailure();
          ++layer ) {
      sc = (*layer)->getObject(path,when,data,descr,since,until,channel);
    }
  }
  return sc;
}
StatusCode CondDBLayeringSvc::getObject (const std::string &path, const Gaudi::Time &when,
                                           DataPtr &data,
                                           std::string &descr, Gaudi::Time &since, Gaudi::Time &until, const std::string &channel)
{
  StatusCode sc;
  if (m_xmlDirectMapping && isFolderSet(path)) {
    descr = "Catalog generated automatically by " + name();
    since = Gaudi::Time::epoch();
    until = Gaudi::Time::max();
    sc = CondDB::generateXMLCatalog(this,path,data);
  } else {
    std::vector<ICondDBReader*>::iterator layer;
    sc = StatusCode::FAILURE;
    for ( layer = m_layers.begin();
          layer != m_layers.end() && sc.isFailure();
          ++layer ) {
      sc = (*layer)->getObject(path,when,data,descr,since,until,channel);
    }
  }
  return sc;
}

//=========================================================================
//  get the list of child nodes of a folderset
//=========================================================================
StatusCode CondDBLayeringSvc::getChildNodes (const std::string &path, std::vector<std::string> &node_names)
{
  return getChildNodes(path,node_names,node_names);
}

namespace {
  // helper function
  template <class Input, class Output>
  void merge(const Input &i, Output &o){
    typename Input::const_iterator it;
    for (it = i.begin(); it != i.end(); ++it){
      if (std::find(o.begin(),o.end(),*it)==o.end()) {
        o.push_back(*it);
      }
    }
  }
}
//=========================================================================
//  get the list of child nodes of a folderset
//=========================================================================
StatusCode CondDBLayeringSvc::getChildNodes (const std::string &path,
                                             std::vector<std::string> &folders,
                                             std::vector<std::string> &foldersets)
{
  // clear the destination vectors
  folders.clear();
  foldersets.clear();
  
  // Get the folders and foldersets from the dedicated alternative
  std::vector<std::string> tmpv1,tmpv2;
  StatusCode sc = StatusCode::FAILURE;
  std::vector<ICondDBReader*>::iterator layer;
  for ( layer = m_layers.begin(); layer != m_layers.end(); ++layer ) {
    if ((*layer)->getChildNodes(path,tmpv1,tmpv2).isSuccess()){
      // we consider it a success if it worked at least for one of the layers
      sc = StatusCode::SUCCESS;
      merge(tmpv1,folders);
      merge(tmpv2,foldersets);
    }
  }
  return sc;
}
//=========================================================================
// Tells if the path is available in the database.
//=========================================================================
bool CondDBLayeringSvc::exists(const std::string &path) {
  std::vector<ICondDBReader*>::const_iterator layer;
  for ( layer = m_layers.begin(); layer != m_layers.end(); ++layer ) {
    if ((*layer)->exists(path)) return true;
  }
  return false;
}

//=========================================================================
// Tells if the path (if it exists) is a folder.
//=========================================================================
bool CondDBLayeringSvc::isFolder(const std::string &path) {
  std::vector<ICondDBReader*>::const_iterator layer;
  for ( layer = m_layers.begin(); layer != m_layers.end(); ++layer ) {
    if ((*layer)->exists(path)) return (*layer)->isFolder(path);
  }
  return false;
}

//=========================================================================
// Tells if the path (if it exists) is a folderset.
//=========================================================================
bool CondDBLayeringSvc::isFolderSet(const std::string &path) {
  std::vector<ICondDBReader*>::const_iterator layer;
  for ( layer = m_layers.begin(); layer != m_layers.end(); ++layer ) {
    if ((*layer)->exists(path)) return (*layer)->isFolderSet(path);
  }
  return false;
}


//=========================================================================
// Collect the list of used tags and databases
//=========================================================================
void CondDBLayeringSvc::defaultTags ( std::vector<LHCb::CondDBNameTagPair>& tags ) const {
  // loop over layers
  std::vector<ICondDBReader*>::const_iterator layer;
  for ( layer = m_layers.begin(); layer != m_layers.end(); ++layer ) {
    (*layer)->defaultTags(tags);
  }
}

//=============================================================================
