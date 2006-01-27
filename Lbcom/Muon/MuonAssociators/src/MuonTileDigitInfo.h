// $Id: MuonTileDigitInfo.h,v 1.1 2006-01-27 17:15:59 asarti Exp $
#ifndef MUONTILEDIGITINFO_H 
#define MUONTILEDIGITINFO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/MuonDigit.h"
#include "Event/MCMuonDigit.h"

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
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

};
#endif // MUONTILEDIGITINFO_H
