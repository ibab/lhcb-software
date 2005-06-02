// $Id: PackTrStateL.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef PACKTRSTATEL_H 
#define PACKTRSTATEL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class PackTrStateL PackTrStateL.h
 *  Pack the TrStateL container
 *
 *  @author Olivier Callot
 *  @date   2005-03-16
 */
class PackTrStateL : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PackTrStateL( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackTrStateL( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // PACKTRSTATEL_H
