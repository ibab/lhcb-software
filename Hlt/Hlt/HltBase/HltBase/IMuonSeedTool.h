// $Id: IMuonSeedTool.h,v 1.1 2007-07-03 12:39:05 albrecht Exp $
#ifndef IMUONSEEDTOOL_H 
#define IMUONSEEDTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

//forward declarations
namespace LHCb {
  class L0MuonCandidate;
  class Track;
};

static const InterfaceID IID_IMuonSeedTool ( "IMuonSeedTool", 1, 0 );

/** @class IMuonSeedTool IMuonSeedTool.h
 *  
 *
 *  @author Johannes Albrecht
 *  @date   2007-07-03
 */
class IMuonSeedTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonSeedTool; }

  
  virtual StatusCode makeTrack( const LHCb::L0MuonCandidate& muonL0Cand,
                                LHCb::Track& seedTrack ) = 0; 
protected:
  
private:

};
#endif // IMUONSEEDTOOL_H
