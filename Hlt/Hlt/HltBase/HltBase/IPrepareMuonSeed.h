// $Id: IPrepareMuonSeed.h,v 1.2 2007-02-14 17:23:52 cattanem Exp $
#ifndef TMP_IPREPAREMUONSEED_H 
#define TMP_IPREPAREMUONSEED_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

//forward declarations
namespace LHCb {
  class L0MuonCandidate;
  class State;
};

static const InterfaceID IID_IPrepareMuonSeed ( "IPrepareMuonSeed", 1, 0 );

/** @class IPrepareMuonSeed IPrepareMuonSeed.h tmp/IPrepareMuonSeed.h
 *  
 *
 *  @author Johannes Albrecht
 *  @date   2007-01-12
 */
class IPrepareMuonSeed : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPrepareMuonSeed; }

  virtual StatusCode prepareSeed( const LHCb::L0MuonCandidate& muonL0Cand,
                                  LHCb::State& seedState ) = 0; 
  
  
                                 
  

protected:

private:

};
#endif // TMP_IPREPAREMUONSEED_H
