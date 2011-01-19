// $Id: FSRNavigator.cpp,v 1.1 2009-06-12 08:17:50 panmanj Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

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
DECLARE_TOOL_FACTORY( FSRNavigator );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FSRNavigator::FSRNavigator( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_ToolName("")
{
  declareInterface<IFSRNavigator>(this);
  // get the File Records service
  m_fileRecordSvc = svc<IDataProviderSvc>("FileRecordDataSvc", true);
}
//=============================================================================
// Destructor
//=============================================================================
FSRNavigator::~FSRNavigator() {} 

//=============================================================================
std::vector< std::string > FSRNavigator::navigate(std::string rootname, std::string tag) {
  // navigate recursively through the FileRecord store and report addresses which contain the tag
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Explore: " << rootname << " for " << tag << endmsg;
  std::vector< std::string > addresses;
  SmartDataPtr<DataObject>   root(m_fileRecordSvc, rootname);
  if ( root ) {
    explore(root->registry(), tag, addresses);
  }
  return addresses;
}

//=============================================================================
void FSRNavigator::explore(IRegistry* pObj, std::string tag, std::vector< std::string >& addresses) {
  // add the addresses which contain the tag to the list and search through the leaves
  if ( 0 != pObj )    {
    std::string name = pObj->name();
    std::string::size_type f = name.find(tag);
    std::string id = pObj->identifier();

    // add this address to the list
    if ( f != std::string::npos ) addresses.push_back(id);

    // search through the leaves
    SmartIF<IDataManagerSvc> mgr(m_fileRecordSvc);
    if ( mgr )    {
      typedef std::vector<IRegistry*> Leaves;
      Leaves leaves;
      StatusCode sc = mgr->objectLeaves(pObj, leaves);
      if ( sc.isSuccess() )  {
        for ( Leaves::const_iterator iLeaf=leaves.begin(); iLeaf != leaves.end(); iLeaf++ )   {
	  // it is important to redefine leafRoot->registry() way back from the identifier 
	  std::string leafId = (*iLeaf)->identifier();
	  SmartDataPtr<DataObject> leafRoot(m_fileRecordSvc, leafId);
	  explore(leafRoot->registry(), tag, addresses);
        }
      }
    }
  }
}
