// $Id: ITrackUpgrade.h,v 1.11 2010-09-08 09:15:49 graven Exp $
#ifndef HLTBASE_ITRACKUPGRADE_H
#define HLTBASE_ITRACKUPGRADE_H 1

// Include files
// from STL
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "HltBase/ITrackView.h"

// forward declarations
namespace LHCb {
class Track;
class State;
class LHCbID;
}

static const InterfaceID IID_ITrackUpgrade( "ITrackUpgrade", 3, 0 );

/** @class ITrackUpgrade ITrackUpgrade.h HltBase/ITrackUpgrade.h
 *
 *  functionality:
 *      upgrade a track adding info from other subdetectors
 *      uniform the tracking tools
 *
 *  @author Jose A. Hernando
 *  @date   2007-02-08
 */
class ITrackUpgrade : virtual public IAlgTool {
  public:
    // Return the interface ID
    static const InterfaceID &interfaceID() { return IID_ITrackUpgrade; }

    // TODO: replace by different instances for different behaviour...
    //      i.e. remove this, and provide what you want when you do
    //           tool<ITrackUpgrade>( reconame, ... )
    /// set this reconstruction as the default one
    // virtual StatusCode setReco(const std::string& name) = 0;

    /// upgrade a list of input tracks
    virtual StatusCode upgrade( const std::vector<const LHCb::Track *> &itracks,
                                std::vector<LHCb::Track *> &otracks ) = 0;

    /// upgrade an input track
    virtual StatusCode upgrade( const LHCb::Track &,
                                std::vector<LHCb::Track *> &tracks ) = 0;

    /// returns the view of the seed track (the volume of search window)
    virtual std::vector<Tf::IStationSelector *> view( const LHCb::Track &seed ) = 0;

    /// return the ids of the hits in the search window
    virtual std::vector<LHCb::LHCbID> lhcbIDsInView( const LHCb::Track &seed ) = 0;
};
#endif
