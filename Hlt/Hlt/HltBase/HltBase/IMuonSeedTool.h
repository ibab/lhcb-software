// $Id: IMuonSeedTool.h,v 1.5 2010-01-06 08:46:51 albrecht Exp $
#ifndef IMUONSEEDTOOL_H
#define IMUONSEEDTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations
namespace LHCb {
class L0MuonCandidate;
class Track;
}

static const InterfaceID IID_IMuonSeedTool( "IMuonSeedTool", 1, 0 );

/** @class IMuonSeedTool IMuonSeedTool.h
 *
 *
 *  @author Johannes Albrecht
 *  @date   2007-07-03
 */
class IMuonSeedTool : virtual public IAlgTool {
  public:
    // Return the interface ID
    static const InterfaceID &interfaceID() { return IID_IMuonSeedTool; }

    /*
     *    this method prepares a L0MuonCandidate to a track at the M2
     *    which can then be used to seed L0 confirmation
     *
     *    input:  a L0MuonCandidate
     *    output: a Track which hold a state at M2
     *            errors of the state define search window size for L0Confirmation
     */
    virtual StatusCode makeTrack( const LHCb::L0MuonCandidate &muonL0Cand,
                                  LHCb::Track &seedTrack ) = 0;

    /*
     *    this method prepares a track from MuonCoords to a track at the M2
     *    which can then be used to seed L0 confirmation
     *
     *    input:  a Track from the muon coords
     *    output: a Track which hold a state at M2
     *            errors of the state define search window size for L0Confirmation
     */
    virtual StatusCode makeTrack( const LHCb::Track &inputTrack,
                                  LHCb::Track &seedTrack ) = 0;
};
#endif // IMUONSEEDTOOL_H
