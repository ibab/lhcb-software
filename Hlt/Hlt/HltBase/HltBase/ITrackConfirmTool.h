// $Id: ITrackConfirmTool.h,v 1.4 2009-04-28 07:03:37 graven Exp $
#ifndef ITRACKCONFIRMTOOL_H
#define ITRACKCONFIRMTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "HltBase/ParabolaHypothesis.h"

namespace LHCb {
class State;
class Track;
}

static const InterfaceID IID_ITrackConfirmTool( "ITrackConfirmTool", 1, 0 );

/** @class ITrackConfirmTool ITrackConfirmTool.h
 *
 *
 *  @author Johannes Albrecht
 *  @date   2007-01-26
 */
class ITrackConfirmTool : virtual public IAlgTool {
  public:
    // Return the interface ID
    static const InterfaceID &interfaceID() { return IID_ITrackConfirmTool; }

    virtual StatusCode tracks( const LHCb::State &seedState,
                               std::vector<LHCb::Track *> &outputTracks ) = 0;

    virtual ParabolaHypothesis prepareT( const LHCb::State &seedState,
                                         std::vector<LHCb::LHCbID> &ids ) = 0;
};
#endif // ITRACKCONFIRMTOOL_H
