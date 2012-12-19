#ifndef MUONTILEDIGITINFO_H 
#define MUONTILEDIGITINFO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class MuonTileDigitInfo MuonTileDigitInfo.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2005-12-30
 */
class MuonTileDigitInfo : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MuonTileDigitInfo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonTileDigitInfo( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:

};
#endif // MUONTILEDIGITINFO_H
