// $Id: RawDataSelector.cpp,v 1.8 2006-06-29 15:58:35 frankb Exp $
//====================================================================
//	OnlineMDFEvtSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : MDF
//
//  Description: The LHCb::RawDataSelector component is able
//               to produce a list of event references given 
//               a set of "selection criteria".
//
//	Author     : M.Frank
//====================================================================

// Include files
#include "MDF/StorageTypes.h"
#include "MDF/RawDataAddress.h"
#include "MDF/RawDataSelector.h"
#include "GaudiKernel/Tokenizer.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataManagerSvc.h"

/// Set connection
StatusCode LHCb::RawDataSelector::LoopContext::connect(const std::string& specs)  {
  m_conSpec = specs;
  m_bindDsc = StreamDescriptor::bind(m_conSpec);
  if ( m_bindDsc.ioDesc > 0 )   {
    m_accessDsc = StreamDescriptor::accept(m_bindDsc);
    return m_accessDsc.ioDesc > 0 ? StatusCode::SUCCESS : StatusCode::FAILURE;
  }
  return StatusCode::FAILURE;
}

/// close connection
void LHCb::RawDataSelector::LoopContext::close()    {
  DSC::close(m_accessDsc);
  DSC::close(m_bindDsc);
}

LHCb::RawDataSelector::RawDataSelector(const std::string& nam, ISvcLocator* svcloc)
: Service( nam, svcloc), m_rootCLID(CLID_NULL)
{
}

// IInterface::queryInterface
StatusCode LHCb::RawDataSelector::queryInterface(const InterfaceID& riid, void** ppvIf)
{
  if (riid == IID_IEvtSelector)  {
    *ppvIf = (IEvtSelector*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface(riid, ppvIf);
}

/// IService implementation: Db event selector override
StatusCode LHCb::RawDataSelector::initialize()
{
  // Initialize base class
  StatusCode status = Service::initialize();
  MsgStream log(msgSvc(), name());
  if ( !status.isSuccess() )    {
    log << MSG::ERROR << "Error initializing base class Service!" << endreq;
    return status;
  }
  // Get DataSvc
  IDataManagerSvc* eds = 0;
  status = serviceLocator()->service("EventDataSvc", eds);
  if( !status.isSuccess() ) {
    log << MSG::ERROR << "Cannot get IID_IDataManagerSvc from EventDataSvc" << endmsg;
    return status;
  }
  m_rootCLID = eds->rootCLID();
  eds->release();
  log << MSG::DEBUG << "Selection CLID:" << m_rootCLID << endmsg;
  return status;
}

StatusCode LHCb::RawDataSelector::next(Context& ctxt) const
{
  LoopContext* pCtxt = dynamic_cast<LoopContext*>(&ctxt);
  if ( pCtxt != 0 )   {
    StatusCode sc = pCtxt->receiveData(msgSvc());
    if ( !sc.isSuccess() ) {
      MsgStream log(msgSvc(),name());
      log << MSG::ERROR << "Failed to receieve the next event." << endmsg;
    }
    return sc;
  }
  return StatusCode::FAILURE;
}

StatusCode LHCb::RawDataSelector::next(Context& ctxt, int jump) const
{
  if ( jump > 0 ) {
    for ( int i = 0; i < jump; ++i ) {
      StatusCode status = next(ctxt);
      if ( !status.isSuccess() ) {
        return status;
      }
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode LHCb::RawDataSelector::previous(Context& /* ctxt */) const
{
  MsgStream log(msgSvc(), name());
  log << MSG::FATAL << " EventSelector Iterator, operator -- not supported " << endmsg;
  return StatusCode::FAILURE;
}

StatusCode LHCb::RawDataSelector::previous(Context& ctxt, int jump) const
{
  if ( jump > 0 ) {
    for ( int i = 0; i < jump; ++i ) {
      StatusCode status = previous(ctxt);
      if ( !status.isSuccess() ) {
        return status;
      }
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode LHCb::RawDataSelector::releaseContext(Context*& ctxt) const
{
  LoopContext* pCtxt = dynamic_cast<LoopContext*>(ctxt);
  if ( pCtxt ) {
    pCtxt->close();
    delete pCtxt;
    pCtxt = 0;
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode 
LHCb::RawDataSelector::createAddress(const Context& ctxt, IOpaqueAddress*& pAddr) const
{
  const LoopContext* pctxt = dynamic_cast<const LoopContext*>(&ctxt);
  if ( pctxt ) {
    if ( !pctxt->banks().empty() )  {
      RawDataAddress* pA = new RawDataAddress(RAWDATA_StorageType,m_rootCLID,pctxt->specs(),"0",0,0);
      pA->setFileOffset(pctxt->offset());
      pA->setBanks(&pctxt->banks());
      pA->setData(pctxt->data());
      pA->setDataLength(pctxt->dataLength());
      pAddr = pA;
      return StatusCode::SUCCESS;
    }
  }
  pAddr = 0;
  return StatusCode::FAILURE;
}

StatusCode 
LHCb::RawDataSelector::resetCriteria(const std::string& criteria,Context& context) const
{
  MsgStream log(messageService(), name());
  LoopContext* ctxt = dynamic_cast<LoopContext*>(&context);
  if ( ctxt )  {
    std::string crit = criteria.substr(5);
    ctxt->close();
    StatusCode sc = ctxt->connect(crit);
    if ( !sc.isSuccess() )  {
      log << MSG::ERROR << "Failed to connect to:" << crit << endmsg;
    }
    return sc;
  }
  log << MSG::ERROR << "Invalid iteration context." << endmsg;
  return StatusCode::FAILURE;
}
