// Include files 

// from Gaudi
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/StatEntity.h"

// local
#include "FSRNavigator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FSRNavigator
//
// 2010-10-05 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FSRNavigator )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FSRNavigator::FSRNavigator( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_ToolName(""), m_fileRecordSvc(0)
{
  declareInterface<IFSRNavigator>(this);
}
//=============================================================================
// Destructor
//=============================================================================
FSRNavigator::~FSRNavigator() {} 

//=============================================================================
// Initialisation
//=============================================================================
StatusCode FSRNavigator::initialize() {

  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( !sc ) return sc ; 

  // get the File Records service
  m_fileRecordSvc = svc<IDataProviderSvc>("FileRecordDataSvc", true);

  return StatusCode::SUCCESS;
}

//=============================================================================
std::vector< std::string > FSRNavigator::navigate(std::string rootname, std::string tag) {
  // navigate recursively through the FileRecord store and report addresses which contain the tag
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Explore: " << rootname << " for " << tag << endmsg;
  std::vector< std::string > addresses;
  SmartDataPtr<DataObject>   root(m_fileRecordSvc, rootname);
  if ( root ) {
    if ( msgLevel(MSG::VERBOSE) ) verbose() << "Root Found: " << rootname << " for " << tag << endmsg;
    explore(root->registry(), tag, addresses);
  }
  return addresses;
}

//=============================================================================
void FSRNavigator::explore(IRegistry* pObj, std::string tag, std::vector< std::string >& addresses) {
  // add the addresses which contain the tag to the list and search through the leaves
  if ( msgLevel(MSG::DEBUG) ) debug() << "Exploring Registry Object: " << endmsg;
  if ( 0 != pObj )    
  {
    if ( msgLevel(MSG::VERBOSE) ) verbose() << "Object is not null" << endmsg;
    std::string name = pObj->name();
    if ( msgLevel(MSG::VERBOSE) ) verbose() << "Object is " << name << "and I'm looking for " << tag << endmsg;
    std::string::size_type f = name.find(tag);
    std::string id = pObj->identifier();
    
    // add this address to the list
    if ( f != std::string::npos ) addresses.push_back(id);

    // search through the leaves
    SmartIF<IDataManagerSvc> mgr(m_fileRecordSvc);
    if ( mgr )    
    {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << "Getting the leaves " << endmsg;
      typedef std::vector<IRegistry*> Leaves;
      Leaves leaves;
      StatusCode sc = mgr->objectLeaves(pObj, leaves);
      if ( sc.isSuccess() )  
      {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << "Found some leaves, moving to iterate over " << leaves.size() << endmsg;
        for ( Leaves::const_iterator iLeaf=leaves.begin(); iLeaf != leaves.end(); iLeaf++ )   
        {
          
          if (!(*iLeaf) )
          {
            if ( msgLevel(MSG::VERBOSE) ) verbose() << "Leaf is Null " << endmsg;
            continue;
          }
          
          
          // it is important to redefine leafRoot->registry() way back from the identifier 
          std::string leafId = (*iLeaf)->identifier();
          SmartDataPtr<DataObject> leafRoot(m_fileRecordSvc, leafId);
          if (!(leafRoot) )
          {
            if ( msgLevel(MSG::VERBOSE) ) verbose() << "LeafRoot is Null " << endmsg;
            continue;
          }
          
          
          explore(leafRoot->registry(), tag, addresses);
        }
      }
    }
  }
}
