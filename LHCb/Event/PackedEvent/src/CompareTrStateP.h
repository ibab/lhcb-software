// $Id: CompareTrStateP.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef COMPARETRSTATEP_H 
#define COMPARETRSTATEP_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class CompareTrStateP CompareTrStateP.h
 *  Compare two versions of the TrStateP container.
 *
 *  @author Olivier Callot
 *  @date   2005-03-17
 */
class CompareTrStateP : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CompareTrStateP( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CompareTrStateP( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_inputName;
  std::string m_testName;
};
#endif // COMPARETRSTATEP_H
