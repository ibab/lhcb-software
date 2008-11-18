// $Id: CompareCaloHypo.h,v 1.1.1.1 2008-11-18 17:12:59 ocallot Exp $
#ifndef COMPARECALOHYPO_H 
#define COMPARECALOHYPO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class CompareCaloHypo CompareCaloHypo.h
 *  Compare two containers of CaloHypo
 *
 *  @author Olivier Callot
 *  @date   2008-11-14
 */
class CompareCaloHypo : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CompareCaloHypo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CompareCaloHypo( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_testName;
};
#endif // COMPARECALOHYPO_H
