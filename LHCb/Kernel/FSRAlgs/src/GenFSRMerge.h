#ifndef GENFSRMERGE_H
#define GENFSRMERGE_H 1

// Include files                                    
// from Gaudi                                                     
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/IGenericTool.h"            // Interface

// event model
#include "Event/GenFSR.h"

// local                                 
#include "FSRNavigator.h" 

//class IGenericTool;

/** @class GenMergeFSR GenMergeFSR.h                                    
 *                                                                            
 *                                                                            
 *  @author Davide Fazzini                                     
 *  @date   2015-06-25                                            
 */

class GenFSRMerge : public GaudiAlgorithm 
{
 public:
  // Standard constructor
  GenFSRMerge(const std::string& name, ISvcLocator* pSvcLocator);

  // Destructor
  virtual ~GenFSRMerge();
  
  // Algorithm initialization, execution and finalization
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 protected:
  // Merge the FSRs of one input file
  virtual StatusCode merge();

  // Reference to run records data service                    
  IDataProviderSvc* m_fileRecordSvc;
  
  // Location of FileRecords
  std::string m_FileRecordName;
  // Specific tag of summary data in FSR
  std::string m_FSRName;

 private:
  // Tool navigate FSR
  IFSRNavigator *m_navigatorTool; 
 
  int m_evtType;
  std::string m_genName;
  std::string m_method;
  
};

#endif // GENFSRMERGE_H
  
