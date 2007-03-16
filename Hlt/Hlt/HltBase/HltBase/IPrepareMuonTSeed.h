// $Id: IPrepareMuonTSeed.h,v 1.2 2007-03-16 16:29:36 cattanem Exp $
#ifndef TMP_IPREPAREMUONTSEED_H 
#define TMP_IPREPAREMUONTSEED_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

//forward declarations
namespace LHCb {
  class Track;
  class State;
};

static const InterfaceID IID_IPrepareMuonTSeed ( "IPrepareMuonTSeed", 1, 0 
);

/** @class IPrepareMuonTSeed IPrepareMuonTSeed.h HltBase/IPrepareMuonSeed.h
 *  
 *
 *  @author Johannes Albrecht
 *  @date   2007-01-12
 */
class IPrepareMuonTSeed : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPrepareMuonTSeed; 
}

  virtual StatusCode prepareSeed( const LHCb::Track& muon,
                                  LHCb::State& seedState ) = 0; 
  
  
                                 
  

protected:

private:

};
#endif // TMP_IPREPAREMUONTSEED_H
