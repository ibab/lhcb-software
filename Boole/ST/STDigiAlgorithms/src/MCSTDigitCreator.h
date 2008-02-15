// $Id: MCSTDigitCreator.h,v 1.1.1.1 2008-02-15 13:18:48 cattanem Exp $
#ifndef MCSTDIGITCREATOR_H
#define MCSTDIGITCREATOR_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

namespace LHCb {
  class MCSTDeposit;
};

/** @class MCSTDigitCreator MCSTDigitCreator.h
 *
 *  Algorithm for creating MCSTDigits from MCSTDeposits.
 *
 *  @author M.Needham
 *  @date   10/03/2002
 */

class MCSTDigitCreator : public GaudiAlgorithm {

public:
  // Constructor
  MCSTDigitCreator( const std::string& name, ISvcLocator* pSvcLocator); 

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

#endif // MCSTDIGITCREATOR_H 
