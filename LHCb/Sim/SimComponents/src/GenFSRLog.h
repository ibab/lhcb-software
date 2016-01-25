#ifndef GENFSRLOG_H 
#define GENFSRLOG_H 1

// Include files 
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataManagerSvc.h"

// from Event 
#include "Event/GenFSR.h"

// from local
#include "FSRAlgs/IFSRNavigator.h"

/** @class GenFSRLog GenFSRLog.h
 *  
 *
 *  @author Davide Fazzini
 *  @date   2015-06-23
 */
class GenFSRLog : public GaudiAlgorithm {
public: 
  // Standard constructor
  GenFSRLog( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GenFSRLog( );          // Destructor

  virtual StatusCode initialize();  // Algorithm initialization
  virtual StatusCode execute   ();  // Algorithm execution
  virtual StatusCode finalize  ();  // Algorithm finalization

protected:

  virtual void printXmlFSR();       // Print the GenFSR in a file .xml 
  
  virtual void addGenCounters(LHCb::GenFSR* genFSR);
  virtual void addGenFractions(LHCb::GenFSR* genFSR);
  virtual void addGenCrossSections(LHCb::GenFSR* genFSR);
  virtual void addGeneratorCrossSections(LHCb::GenFSR* genFSR);
  virtual void addCutEfficiencies(LHCb::GenFSR* genFSR);
  
protected:

  IDataProviderSvc* m_fileRecordSvc;
  
  std::string m_fileRecordName;     // location of FileRecords
  std::string m_FSRName;            // specific tag of summary data in FSR
  std::string m_xmlFileName;        // xml file name   
  std::ofstream m_xmlFile;          // file xml

private:

  IFSRNavigator* m_navigatorTool;   // tool to navigate FSR 
  
};
#endif // GENFSRLOG_H
