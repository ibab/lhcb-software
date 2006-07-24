// $Id: TsaInitialization.h,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $
#ifndef _TSAINITIALIZATION_H_
#define _TSAINITIALIZATION_H_

/** @class TsaInitialization  TsaInitialization.h TsaAlgorithms/TsaInitialization.h
 *
 *  Class for Initialization
 *
 *  @author M.Needham
 *  @date   07/03/2002
 */

#include "TsaBaseAlg.h"
#include <string>

class IITDataSvc;
class IOTDataSvc;

class TsaInitialization: public TsaBaseAlg {

public:
  
  // Constructors and destructor
  TsaInitialization(const std::string& name, 
                   ISvcLocator* pSvcLocator); 
  virtual ~TsaInitialization();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();  

private:

  std::string m_otDataSvcName;
  std::string m_itDataSvcName;

  IOTDataSvc* m_otDataSvc; 
  IITDataSvc* m_itDataSvc; 
 
  bool m_initIT;
  bool m_initOT;

  double m_maxITClusters;

};

#endif // _TSAINITIALIZATION_H_











