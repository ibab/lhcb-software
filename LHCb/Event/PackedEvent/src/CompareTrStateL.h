// $Id: CompareTrStateL.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef COMPARETRSTATEL_H 
#define COMPARETRSTATEL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class CompareTrStateL CompareTrStateL.h
 *  Compare two containers of TrStateL for identity, module rounding effects
 *
 *  @author Olivier Callot
 *  @date   2005-03-16
 */
class CompareTrStateL : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CompareTrStateL( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CompareTrStateL( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_inputName;
  std::string m_testName;
};
#endif // COMPARETRSTATEL_H
