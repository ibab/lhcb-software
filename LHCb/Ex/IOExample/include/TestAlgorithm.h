//	====================================================================
//  TestAlgorithm.cpp
//	--------------------------------------------------------------------
//
//	Package   : Tests/LHCb
//
//	Author    : Markus Frank
//
//	====================================================================

// Framework include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/DataObject.h"
#include <vector>
class EventHeader;

/** User example objects: TestAlgorithm

    Description:
    A small class reading a track N-tuple from a root file.

    Base Class:
    Algorithm

    Author:  M.Frank
    Version: 1.0
*/
class TestAlgorithm : public Algorithm {
protected: 
  typedef       std::vector<std::pair<std::string,long> > Errors;
  long          m_nerr;
  long          m_numEvent;
  EventHeader*  m_event;
  Errors        m_errorList;

public:
	/// Constructor: A constructor of this form must be provided.
	TestAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);
  /// Standard Destructor
  virtual ~TestAlgorithm();
  /// Initialize
  virtual StatusCode initialize();
  /// Overloadable callback for initializing individual testing algorithms
  virtual StatusCode initializeTest();
  /// Finalize
  virtual StatusCode finalize();
  /// Overloadable callback for finalizing individual testing algorithms
  virtual StatusCode finalizeTest();
  /// Event callback
  virtual StatusCode execute();
  /// Event callback
  virtual StatusCode executeTest();
  virtual long error(const std::string& fname, long lineNo);
  virtual long error(const std::string& fname, const std::string& desc, long lineNo);
  virtual long error(const std::string& fname, long lineNo, const std::string& desc);
  float rndmFloat(float mi, float ma);
  int   rndmInt(int mi, int ma);
  virtual DataObject* retrieveObject(IDataProviderSvc* svc, const std::string& path, const char* fname, long line);
  virtual StatusCode registerEvtObj(const std::string& path, DataObject* pObj, const char* fname, long line);
};
