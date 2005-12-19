// $Id: MCSTDigitCreator.h,v 1.1.1.1 2005-12-19 15:43:15 mneedham Exp $
#ifndef _MCSTDIGITCREATOR_H_
#define _MCSTDIGITCREATOR_H_


/** @class MCSTDigitCreator MCSTDigitCreator.h ITAlgorithms/MCSTDigitCreator
 *
 *  Class for filling IT summary info
 *
 *  @author M.Needham
 *  @date   10/03/2002
 */

#include <string>
#include "Event/MCSTDigit.h"
#include "GaudiAlg/GaudiAlgorithm.h"

namespace LHCb{
 class MCSTDeposit;
};

class MCSTDigitCreator : public GaudiAlgorithm {

public:
  // Constructor: A constructor of this form must be provided.
  MCSTDigitCreator(const std::string& name, 
                   ISvcLocator* pSvcLocator); 
  virtual ~MCSTDigitCreator(); 

  StatusCode initialize();

  StatusCode execute();
 
private:

  bool keepAdding(const LHCb::MCSTDeposit* firstDep, 
                  const LHCb::MCSTDeposit* secondDep) const;

  std::string m_detType;

  std::string m_inputLocation;
  std::string m_outputLocation;

};

#endif


