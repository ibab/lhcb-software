// $Id: UnpackTrStateP.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef UNPACKTRSTATEP_H 
#define UNPACKTRSTATEP_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class UnpackTrStateP UnpackTrStateP.h
 *  Unpack the TrStateP
 *
 *  @author Olivier Callot
 *  @date   2005-03-16
 */
class UnpackTrStateP : public GaudiAlgorithm {
public: 
  /// Standard constructor
  UnpackTrStateP( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackTrStateP( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // UNPACKTRSTATEP_H
