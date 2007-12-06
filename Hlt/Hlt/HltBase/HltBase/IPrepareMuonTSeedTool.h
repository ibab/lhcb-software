// $Id: IPrepareMuonTSeedTool.h,v 1.1 2007-12-06 16:07:36 hernando Exp $
#ifndef TMP_IPREPAREMUONTSEEDTOOL_H 
#define TMP_IPREPAREMUONTSEEDTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

//forward declarations
namespace LHCb {
  class Track;
};

static const InterfaceID IID_IPrepareMuonTSeedTool ( "IPrepareMuonTSeedTool", 1, 0 
);

/** @class IPrepareMuonTSeedTool 
 *  
 *
 *  @author  Sandra Amato
 *  @date   2007-12-04
 */
class IPrepareMuonTSeedTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPrepareMuonTSeedTool; 
}

  virtual StatusCode prepareSeed( const LHCb::Track& muon,
                                  LHCb::Track& seedTrack ) = 0;
  
                                 
  

protected:

private:

};
#endif // TMP_IPREPAREMUONTSEEDTOOL_H
