// $Id: TestUpdateMgr.h,v 1.4 2006-01-19 18:32:11 marcocle Exp $
#ifndef TESTUPDATEMGR_H 
#define TESTUPDATEMGR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

class IUpdateManagerSvc;
class DataObject;

/** @class TestUpdateMgr TestUpdateMgr.h
 *  
 *  Algorith used to test the functionalities of the update manager.
 *
 *  @author Marco CLEMENCIC
 *  @date   2005-04-27
 */
class TestUpdateMgr : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TestUpdateMgr( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestUpdateMgr( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

#define TUM_method(X) virtual StatusCode i_updateMethod##X(){info()<<"method "<<X<<" called"<<endmsg;return StatusCode::SUCCESS;}
  TUM_method(1);
  TUM_method(2);
  TUM_method(3);
  TUM_method(4);
  TUM_method(5);

private:

  class InternalClass {
  public:
    StatusCode myTinyMethod(){
      std::cout << "***InternalClass::myTinyMethod()***" << std::endl;
      return StatusCode::SUCCESS;
    };
    virtual ~InternalClass(){}
  };

  InternalClass m_intermediate;

  DataObject *m_dummyUMSentry;

  size_t m_evtCount;

};
#endif // TESTUPDATEMGR_H
