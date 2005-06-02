// $Id: PackTrStateP.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef PACKTRSTATEP_H 
#define PACKTRSTATEP_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class PackTrStateP PackTrStateP.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2005-03-16
 */
class PackTrStateP : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PackTrStateP( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackTrStateP( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // PACKTRSTATEP_H
