// $Id: StoreExplorer.cpp,v 1.1 2004-06-17 15:29:33 cattanem Exp $
//	====================================================================
//  StoreExplorer.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "StoreExplorer.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/ObjectContainerBase.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/AlgFactory.h"

/** User example objects: StoreExplorer

    Algorithm to navigate the transient event store
    Author:  M.Frank
    Version: 2.0
*/
static const AlgFactory<StoreExplorer>  Factory;
const IAlgFactory& StoreExplorerFactory = Factory;

// Constructor: A constructor of this form must be provided.
StoreExplorer::StoreExplorer(const std::string& name, ISvcLocator* pSvcLocator)
  :	GaudiAlgorithm(name, pSvcLocator)  { 
  m_total = m_frqPrint = 0;
  m_freq = 0.0;
  m_print = 1;
  m_exploreRelations = false;
  declareProperty("PrintEvt", m_print);
  declareProperty("PrintFreq", m_freq);
  declareProperty("ExploreRelations", m_exploreRelations);
}
  
void StoreExplorer::printObj(IRegistry* pReg, std::vector<bool>& flg)    {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO;
  for (size_t j = 1; j < flg.size(); j++ )     {
    if ( !flg[j-1] && flg[j] ) log << "| ";
    else if ( flg[j] ) log << "  ";
    else          log << "| ";
  }
  log << "+--> " << pReg->name();
  if ( pReg->address() )  {
    log << " [Address: CLID=" << (void*)pReg->address()->clID();
    log << " Type=" << (void*)pReg->address()->svcType() << "]";
  }
  else  {
    log << " [No Address]";
  }
  DataObject* p = pReg->object();
  if ( p )   {
    std::string typ = System::typeinfoName(typeid(*p));
    log << "  " << typ.substr(0,32);
  }
  else    {
    log << "  (Unloaded) ";
  }
  ObjectContainerBase* base = dynamic_cast<ObjectContainerBase*>(p);
  if ( base )  {
    log << " [" << base->numberOfObjects() << "]";
  }
  log << endmsg;
}

void StoreExplorer::explore(IRegistry* pObj, std::vector<bool>& flg)    {
  printObj(pObj, flg);
  if ( 0 != pObj )    {
    SmartIF<IDataManagerSvc> mgr(eventSvc());
    if ( mgr )    {
      typedef std::vector<IRegistry*> Leaves;
      Leaves leaves;
      StatusCode sc = mgr->objectLeaves(pObj, leaves);
      if ( sc.isSuccess() )  {
        for( Leaves::const_iterator i=leaves.begin(); i != leaves.end(); i++ )
        {
          const std::string& id = (*i)->identifier();
          DataObject* p = 0;
          if ( eventSvc()->retrieveObject(id, p).isSuccess() )  {
            if ( id != "/Event/Rec/Relations" || m_exploreRelations )   {
              flg.push_back(i+1 == leaves.end());
              explore(*i, flg);
              flg.pop_back();
            }
          }
          else {
            flg.push_back(i+1 == leaves.end());
            printObj(*i, flg);
            flg.pop_back();
          }
        }
      }
    }
  }
}

StatusCode StoreExplorer::execute()    {
  DataObject* evt = get<DataObject>("/Event");
  if ( ((m_print > m_total++) || (m_freq*m_total > m_frqPrint)) && evt )    {
    if ( m_freq*m_total > m_frqPrint ) m_frqPrint++;
    std::vector<bool> flg;
    flg.push_back(true);
    explore(evt->registry(), flg);
  }
  return StatusCode::SUCCESS;
}

