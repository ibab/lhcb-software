// $Id: UnpackTrStateL.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef UNPACKTRSTATEL_H 
#define UNPACKTRSTATEL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class UnpackTrStateL UnpackTrStateL.h
 *  Unpack the TrStateL
 *
 *  @author Olivier Callot
 *  @date   2005-03-16
 */
class UnpackTrStateL : public GaudiAlgorithm {
public: 
  /// Standard constructor
  UnpackTrStateL( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackTrStateL( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // UNPACKTRSTATEL_H
