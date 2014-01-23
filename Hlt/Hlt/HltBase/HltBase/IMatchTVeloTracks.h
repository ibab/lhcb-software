// $Id: IMatchTVeloTracks.h,v 1.4 2008-01-22 09:35:00 hernando Exp $
#ifndef HLTBASE_IMATCHTVELOTRACKS_H
#define HLTBASE_IMATCHTVELOTRACKS_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations
namespace LHCb
{
class Track;
}

static const InterfaceID IID_IMatchTVeloTracks( "IMatchTVeloTracks", 1, 1 );

/** @class IMatchTVeloTracks IMatchTVeloTracks.h HltBase/IMatchTVeloTracks.h
 *
 *
 *  @author Johannes Albrecht
 *  @date   2007-01-12
 */
class IMatchTVeloTracks : virtual public IAlgTool
{
  public:
    // Return the interface ID
    static const InterfaceID& interfaceID()
    {
        return IID_IMatchTVeloTracks;
    }

    virtual StatusCode match3dVelo( const LHCb::Track& veloTrack,
                                    const LHCb::Track& Ttrack,
                                    LHCb::Track& matchedTrack, double& x_dist,
                                    double& y_dist ) = 0;

    virtual double momentum( const LHCb::Track& velo,
                             const LHCb::Track& ttrack ) = 0;
};
#endif // HLTBASE_IMATCHTVELOTRACKS_H
