// $Id: IPrepareMuonTSeed.h,v 1.3 2007-05-22 10:12:55 asatta Exp $
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

/** @class IPrepareMuonSeed IPrepareMuonSeed.h tmp/IPrepareMuonSeed.h
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
