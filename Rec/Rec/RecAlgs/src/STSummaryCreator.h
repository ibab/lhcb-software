// $Id: STSummaryCreator.h,v 1.1.1.1 2006-03-15 16:25:05 cattanem Exp $
#ifndef _STSUMMARYCREATOR_H_
#define _STSUMMARYCREATOR_H_


/** @class STSummaryCreator STSummaryCreator.h ITAlgorithms/STSummaryCreator
 *
 *  Class for filling ST summary info
 *
 *  @author M.Needham
 *  @date   10/11/2005
 */

#include <string>
#include "GaudiAlg/GaudiAlgorithm.h"

namespace LHCb{
  class STSummary;
};

class STSummaryCreator : public GaudiAlgorithm {

public:
  // Constructor: A constructor of this form must be provided.
  STSummaryCreator(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~STSummaryCreator(); 

  virtual StatusCode initialize();
  virtual StatusCode execute();

private:

  std::string m_detType;
  
  std::string m_inputClusterLocation;
  std::string m_inputDigitLocation;
  std::string m_outputLocation;

};

#endif
