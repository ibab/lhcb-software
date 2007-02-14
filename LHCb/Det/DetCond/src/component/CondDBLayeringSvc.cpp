// $Id: CondDBLayeringSvc.cpp,v 1.2 2007-02-14 16:13:31 marcocle Exp $
// Include files 

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ClassID.h"

// local
#include "CondDBLayeringSvc.h"

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
  std::vector<ICondDBReader*>::iterator layer;
  bool found_object = false;
  for ( layer = m_layers.begin(); layer != m_layers.end() && ! found_object; ++layer ) {
    found_object = (*layer)->getObject(path,when,data,descr,since,until,channel).isSuccess();
  }
  return (found_object) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//=========================================================================
//  get the list of child nodes of a folderset
//=========================================================================
StatusCode CondDBLayeringSvc::getChildNodes (const std::string &path, std::vector<std::string> &node_names)
{
  std::vector<ICondDBReader*>::iterator layer;
  bool found_folderset = false;
  for ( layer = m_layers.begin(); layer != m_layers.end() && ! found_folderset; ++layer ) {
    found_folderset = (*layer)->getChildNodes(path,node_names).isSuccess();
  }
  return (found_folderset) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//=============================================================================
