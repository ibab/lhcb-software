// $Id: STCommonModeSim.h,v 1.2 2010-04-07 09:27:38 mneedham Exp $
#ifndef _STCommonModeSim_H_
#define _STCommonModeSim_H_

#include <vector>
#include <string>
#include <map>

#include "Kernel/STAlgBase.h"


#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"
#include "Event/STDigit.h"

class ISTPedestalSimTool;
class ISTCMSimTool;


class STCommonModeSim : public ST::AlgBase {

public:

  // Constructor: A constructor of this form must be provided.
  STCommonModeSim(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~STCommonModeSim(); 

  StatusCode initialize();
  StatusCode execute();
 
  
private:

  //typedef std::pair<double,LHCb::STChannelID> digitPair;

  LHCb::STDigits::const_iterator collectByPort(LHCb::STDigits::const_iterator& start,
					 LHCb::STDigits::const_iterator& end);


  
  void processPort(LHCb::STDigits::const_iterator& start,
		   LHCb::STDigits::const_iterator& end, 
                   LHCb::STDigits* outputCont);


  StatusCode loadCutsFromConditions();

  SmartIF<IRndmGen> m_gaussDist; 

  std::string m_inputLocation;
  std::string m_outputLocation;
  double m_outlierCut;
  bool m_forceOptions;
  std::string m_conditionLocation;
  std::string m_pedestalToolName;
  ISTPedestalSimTool* m_pedestalTool;
  std::string m_cmToolName;
  ISTCMSimTool* m_cmTool;


};

#endif








