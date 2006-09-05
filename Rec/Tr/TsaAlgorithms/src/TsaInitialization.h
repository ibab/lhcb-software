// $Id: TsaInitialization.h,v 1.2 2006-09-05 15:48:51 mneedham Exp $
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
class ITTDataSvc;

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

  std::string m_otDataSvcType;
  std::string m_itDataSvcType;
  std::string m_ttDataSvcType;
  std::string m_otDataSvcName;
  std::string m_itDataSvcName;
  std::string m_ttDataSvcName;

  IOTDataSvc* m_otDataSvc; 
  IITDataSvc* m_itDataSvc; 
  ITTDataSvc* m_ttDataSvc;
 
  bool m_initIT;
  bool m_initOT;
  bool m_initTT;


};

#endif // _TSAINITIALIZATION_H_











