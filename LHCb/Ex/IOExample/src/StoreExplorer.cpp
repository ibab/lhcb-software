// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/IOExample/src/StoreExplorer.cpp,v 1.1 2003-12-18 15:33:43 cattanem Exp $
//	====================================================================
//  StoreExplorer.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "TestAlgorithm.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ObjectContainerBase.h"
#include "GaudiKernel/SmartIF.h"

/** User example objects: StoreExplorer

    Create a non existing tree of leafs in the event data store
    Test the automatic leaf creation.
    Author:  M.Frank
    Version: 1.0
*/
class StoreExplorer : public TestAlgorithm {
  long    m_print, m_total, m_frqPrint;
  double  m_freq;
  long    m_exploreRelations, m_printMissing;
public:
  /// Constructor: A constructor of this form must be provided.
  StoreExplorer(const std::string& name, ISvcLocator* pSvcLocator)
  :	TestAlgorithm(name, pSvcLocator)  { 
    m_total = m_frqPrint = 0;
    m_freq = 0.0;
    m_print = 1;
    m_exploreRelations = 0;
    declareProperty("PrintMissing", m_printMissing);
    declareProperty("PrintEvt", m_print);
    declareProperty("PrintFreq", m_freq);
    declareProperty("ExploreRelations", m_exploreRelations);
  }
  /// Standard Destructor
  virtual ~StoreExplorer()     {  }
  
  void printObj(IRegistry* pReg, std::vector<bool>& flg)    {
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

  void explore(IRegistry* pObj, std::vector<bool>& flg)    {
    printObj(pObj, flg);
    if ( 0 != pObj )    {
      SmartIF<IDataManagerSvc> mgr(eventSvc());
      if ( mgr )    {
        typedef std::vector<IRegistry*> Leaves;
        Leaves leaves;
        StatusCode sc = mgr->objectLeaves(pObj, leaves);
        if ( sc.isSuccess() )  {
          for ( Leaves::const_iterator i=leaves.begin(); i != leaves.end(); i++ )   {
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

  /// Test procedure
  virtual StatusCode executeTest()    {
    MsgStream log(msgSvc(), name());
    SmartDataPtr<DataObject>   evt(eventSvc(),"/Event");
    if ( ((m_print > m_total++) || (m_freq*m_total > m_frqPrint)) && evt )    {
      if ( m_freq*m_total > m_frqPrint ) m_frqPrint++;
      std::vector<bool> flg;
      flg.push_back(true);
      explore(evt->registry(), flg);
      return StatusCode::SUCCESS;
    }
    else if ( evt )   {
      return StatusCode::SUCCESS;
    }
    error(__FILE__, "Cannot retrieve \"/Event\"!", __LINE__);
    return StatusCode::FAILURE;
  }
};

static const AlgFactory<StoreExplorer>  Factory;
const IAlgFactory& StoreExplorerFactory = Factory;
