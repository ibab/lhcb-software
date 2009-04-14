// $Id: STCommonModeSim.h,v 1.1 2009-04-14 13:17:53 mneedham Exp $
#ifndef _STCommonModeSim_H_
#define _STCommonModeSim_H_

#include <vector>
#include <string>
#include <map>

#include "Kernel/STAlgBase.h"


#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"
#include "Event/STDigit.h"

class ISTSignalToNoiseTool;
class DeSTSector;

class STCommonModeSim : public ST::AlgBase {

public:

  // Constructor: A constructor of this form must be provided.
  STCommonModeSim(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~STCommonModeSim(); 

  StatusCode initialize();
  StatusCode execute();
 
  
private:

  LHCb::STDigits::const_iterator collectByPort(LHCb::STDigits::const_iterator& start,
					 LHCb::STDigits::const_iterator& end);


  void correctForCM(LHCb::STDigits::const_iterator& start,
		    LHCb::STDigits::const_iterator& end,
		    LHCb::STDigits* outputCont);
  
  void processPort(LHCb::STDigits::const_iterator& start,
		   LHCb::STDigits::const_iterator& end, 
                   LHCb::STDigits* outputCont);

  double rescale(const double value) const;
  

  std::string m_sigNoiseToolName;
  ISTSignalToNoiseTool* m_sigNoiseTool;

  SmartIF<IRndmGen> m_gaussDist;

  std::string m_inputLocation;
  std::string m_outputLocation;

  double m_outlierCut;
  double m_commonModeNoiseLevel;
  double m_cmNoise;
  double m_pedestal;  

  DeSTSector* m_cachedSector;

};

#endif








