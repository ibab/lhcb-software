// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Time.h"

// local
#include "CondDBLayeringSvc.h"
#include "CondDBCommon.h"
#include "IOVListHelpers.h"

// Factory implementation
DECLARE_SERVICE_FACTORY(CondDBLayeringSvc)

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBLayeringSvc
//
// 2006-07-14 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

// This is needed otherwise the implementation of std::map does
// not find operator<(Gaudi::Time,Gaudi::Time).
namespace Gaudi { using ::operator<; }

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBLayeringSvc::CondDBLayeringSvc( const std::string& name, ISvcLocator* svcloc ):
  base_class(name,svcloc) {

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
// initialize
//=============================================================================
StatusCode CondDBLayeringSvc::initialize(){
  StatusCode sc = base_class::initialize();
  if (sc.isFailure()) return sc;

  MsgStream log(msgSvc(), name() );
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Initialize" << endmsg;

  // locate all the AccessSvcs layers
  std::vector<std::string>::iterator lname;
  for ( lname = m_layersNames.begin(); lname != m_layersNames.end(); ++lname ) {

    ICondDBReader *svcPtr;
    sc = service(*lname,svcPtr,true);
    if (  !sc.isSuccess() ) {
      log << MSG::ERROR << "Could not locate " << *lname << endmsg;
      return sc;
    }

    m_layers.push_back(svcPtr);
    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG << "Retrieved '" << *lname << "'" << endmsg;

  }

  return sc;
}

//=============================================================================
// finalize
//=============================================================================
StatusCode CondDBLayeringSvc::finalize(){
  MsgStream log(msgSvc(), name() );
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Finalize" << endmsg;

  std::vector<ICondDBReader*>::iterator layer;
  for ( layer = m_layers.begin(); layer != m_layers.end(); ++layer ) {
    if ( *layer ) (*layer)->release();
  }
  m_layers.clear();

  return base_class::finalize();
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

template <typename Channel>
ICondDBReader::IOVList CondDBLayeringSvc::i_getIOVs(const std::string & path, const IOV &iov, const Channel &channel)
{
  IOVList iovs;

  IOVList missing; // IOVs not found
  missing.push_back(iov);

  std::vector<ICondDBReader*>::iterator layer;
  // for each layer
  for ( layer = m_layers.begin();
        layer != m_layers.end() && !missing.empty();
        ++layer ) {

    IOVList layer_iovs;

    // look for the missing IOVs in this layer
    for ( IOVList::iterator m = missing.begin();
          m != missing.end();
          ++m ) {
      IOVList missing_iovs = (*layer)->getIOVs(path, *m, channel);
      // if we found something merge with the others in the layer
      if (!missing_iovs.empty()) {
        // ensure that the found IOVs do not overlap with the already available ones
        missing_iovs.front().since = std::max(missing_iovs.front().since, m->since);
        missing_iovs.back().until = std::min(missing_iovs.back().until, m->until);
        layer_iovs.insert(layer_iovs.end(), missing_iovs.begin(), missing_iovs.end());
      }
      else continue;
    }

    // if we got IOVs in this layer, we add them to the results list
    if (!layer_iovs.empty()) {
      iovs.insert(iovs.end(), layer_iovs.begin(), layer_iovs.end());
      std::sort(iovs.begin(), iovs.end());
      // regenerate the list of holes
      missing = IOVListHelpers::find_holes(iovs, iov);
    }
  }

  return iovs;
}

ICondDBReader::IOVList CondDBLayeringSvc::getIOVs(const std::string & path, const IOV &iov, cool::ChannelId channel)
{
  return i_getIOVs(path, iov, channel);
}

ICondDBReader::IOVList CondDBLayeringSvc::getIOVs(const std::string & path, const IOV &iov, const std::string & channel)
{
  return i_getIOVs(path, iov, channel);
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
// Force disconnection from database.
//=========================================================================
void CondDBLayeringSvc::disconnect() {
  std::vector<ICondDBReader*>::const_iterator layer;
  for ( layer = m_layers.begin(); layer != m_layers.end(); ++layer ) {
    (*layer)->disconnect();
  }
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
