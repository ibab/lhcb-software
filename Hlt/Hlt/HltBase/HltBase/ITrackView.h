// $Id: ITrackView.h,v 1.4 2009-10-07 06:37:25 graven Exp $
#ifndef HLTBASE_ITRACKVIEW_H
#define HLTBASE_ITRACKVIEW_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "TfKernel/TStationHitManager.h"
#include "Kernel/LHCbID.h"
#include "Event/Track.h"

static const InterfaceID IID_ITrackView( "ITrackView", 1, 0 );

/** @class ITrackView ITrackView.h HltBase/ITrackView.h
 *
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2008-06-19
 */
class ITrackView : virtual public IAlgTool
{
  public:
    // Return the interface ID
    static const InterfaceID& interfaceID()
    {
        return IID_ITrackView;
    }

    // return view of a track (the volume search window open by this seed track)
    virtual std::vector<Tf::IStationSelector*> view( const LHCb::Track& seed ) = 0;

    // return the hits on the volume search windows of this seed track
    virtual std::vector<LHCb::LHCbID> lhcbIDsInView( const LHCb::Track& seed ) = 0;

  protected:
  private:
};
#endif // HLTBASE_ITRACKVIEW_H
