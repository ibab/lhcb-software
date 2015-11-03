// ====================================================================
//  AddressKillerAlg
// --------------------------------------------------------------------
//
//	Author    : Markus Frank
//
// ====================================================================
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartIF.h"

/// Namespace LHCb
namespace LHCb  {

  /** AddressKillerAlg
   *
   * Small algorith to make the entire datastore anonymous.
   * All persistent info of all addresses is entirely removed.
   *
   * @author:  M.Frank
   * @version: 1.0
   */
  class AddressKillerAlg : public Algorithm {
    /// Reference to data provider service
    IDataProviderSvc* m_dataSvc;
    /// Name of the data provider service
    std::string       m_dataSvcName;
    /// Name of the root leaf (obtained at initialize)
    std::string       m_rootName;
  public:
    typedef std::vector<IRegistry*> Leaves;

    /// Standard algorithm constructor
    AddressKillerAlg(const std::string& name, ISvcLocator* pSvcLocator)
      : Algorithm(name, pSvcLocator), m_dataSvc(0)  {
      declareProperty("DataSvc", m_dataSvcName="EventDataSvc");
    }
    /// Standard Destructor
    virtual ~AddressKillerAlg()     {
    }

    /// Initialize
    virtual StatusCode initialize()   {
      MsgStream log(msgSvc(), name());
      m_rootName = "";
      StatusCode sc = service(m_dataSvcName, m_dataSvc, true);
      if ( sc.isSuccess() )  {
	SmartIF<IDataManagerSvc> mgr(m_dataSvc);
	if ( mgr )  {
	  m_rootName = mgr->rootName();
	  return sc;
	}
	log << MSG::ERROR << "Failed to retrieve IDataManagerSvc interface." << endmsg;
	return StatusCode::FAILURE;
      }
      log << MSG::ERROR << "Failed to access service \"" << m_dataSvcName << "\"." << endmsg;
      return StatusCode::FAILURE;
    }

    /// Finalize
    virtual StatusCode finalize() {
      if ( m_dataSvc ) m_dataSvc->release();
      m_dataSvc = 0;
      return StatusCode::SUCCESS;
    }

    void explore(IRegistry* pObj)    {
      SmartIF<IDataManagerSvc> mgr(m_dataSvc);
      if ( mgr && pObj )    {
	Leaves leaves;
	const IOpaqueAddress* addr = pObj->address();
	if ( addr )   {
	  // NOT for MDF top level address!!!!
	  if ( !(addr->svcType() == RAWDATA_StorageType && pObj->identifier() == m_rootName) )  {
	    MsgStream log(msgSvc(), name());
	    log << MSG::INFO << "Remove store address \"" << pObj->identifier() << "\"." << endmsg;
	    pObj->setAddress(0);
	  }
	}
	StatusCode sc = mgr->objectLeaves(pObj, leaves);
	if ( sc.isSuccess() )  {
	  for (Leaves::const_iterator i=leaves.begin(); i != leaves.end(); i++ )
	    explore(*i);
	}
      }
    }

    /// Execute procedure
    virtual StatusCode execute()    {
      SmartDataPtr<DataObject> root(m_dataSvc,m_rootName);
      if ( root )  {
	explore(root->registry());
	return StatusCode::SUCCESS;
      }
      return StatusCode::FAILURE;
    }
  };

  DECLARE_COMPONENT(AddressKillerAlg)
}

