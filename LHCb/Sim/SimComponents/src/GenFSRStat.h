#ifndef MCSTATTOOLS_GENFSRSTAT_H 
#define MCSTATTOOLS_GENFSRSTAT_H 1

// Include files 
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataManagerSvc.h"

// from Event                
#include "Event/GenFSR.h"

// from Kernel
#include "FSRAlgs/IFSRNavigator.h"


/** @class GenFSRStat GenFSRStat.h MCStatTools/GenFSRStat.h
 *  
 *
 *  @author Davide Fazzini
 *  @date   2015-07-29
 */
class GenFSRStat : public GaudiAlgorithm {
public: 
  /// Standard constructor
  GenFSRStat( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GenFSRStat( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
protected:

  virtual void printHtmlFSR();  // Print the GenFSR in a file .xml
  
  virtual void writeGeneratorCounters(LHCb::GenFSR* genFRS, int count);
  virtual void writeCutEfficiencies(LHCb::GenFSR* genFRS, int count);
  virtual void writeGenHadronCounters(LHCb::GenFSR* genFRS, int countGen);
  virtual void writeAccHadronCounters(LHCb::GenFSR* genFRS, int countAcc);  
  virtual void writeGeneratorCrossSections(LHCb::GenFSR* genFRS, int count);  
  virtual void writeFinalComments(LHCb::GenFSR* genFSR);
  
  const std::string getCurrentTime();
  const std::string getEvtTypeDesc(int evtType);


protected:

  IDataProviderSvc* m_fileRecordSvc;

  std::string m_fileRecordName;     // location of FileRecords                   
  std::string m_FSRName;            // specific tag of summary data in FSR           
  std::string m_htmlFileName;       // html file name      
  std::string m_htmlFileLocation;   // html file location
  std::ofstream m_htmlFile;         // file html          

private:

  IFSRNavigator* m_navigatorTool;   // tool to navigate FSR

  // Informations not stored in the FSR
  std::string m_dddb;   
  std::string m_simCond;
  std::string m_gaussVersion;
  std::string m_appConfigFile;
  std::string m_appConfigVersion;
  std::string m_prodID;

};
#endif // MCSTATTOOLS_GENFSRSTAT_H
