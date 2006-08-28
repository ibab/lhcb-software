
//-----------------------------------------------------------------------------
/** @file RichTrackSelectorBase.h
 *
 *  Header file for RICH reconstruction tool : RichTrackSelectorBase
 *
 *  CVS Log :-
 *  $Id: RichTrackSelectorBase.h,v 1.1 2006-08-28 11:11:55 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichTrackSelectorBase_H
#define RICHRECTOOLS_RichTrackSelectorBase_H 1

// Gaudi
#include "GaudiKernel/IJobOptionsSvc.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichBaseTrackSelector.h"

// RichKernel
#include "RichKernel/RichHashMap.h"

// Event model
#include "Event/RichRecTrack.h"
#include "Event/Track.h"

// boost
#include "boost/assign/list_of.hpp"
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

/** @namespace Rich
 *
 *  General namespace for RICH specific definitions
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class RichTrackSelectorBase RichTrackSelectorBase.h
   *
   *  Base class for Track Selectors
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2006-08-12
   */
  //-----------------------------------------------------------------------------

  class RichTrackSelectorBase : public RichRecToolBase,
                                virtual public IRichTrackSelector
  {

  public: // Gaudi methods

    /// Standard constructor
    RichTrackSelectorBase( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

    /// Destructor
    virtual ~RichTrackSelectorBase( );

    /// Initialize method
    StatusCode initialize();

    /// Finalize method
    StatusCode finalize();

  public: // interface methods

    /// Test if the given Track is selected
    virtual bool trackSelected( const LHCb::Track * track ) const;

    /// Test it the given RichRecTrack is selected
    virtual bool trackSelected( const LHCb::RichRecTrack * track ) const;

    /// Returns the list of selected track types
    virtual const std::vector<std::string> & selectedTracks() const;

    /// Returns the overall minimum momentum cut value
    virtual double minPCut() const;

    /// Returns the overall maximum momentum cut value
    virtual double maxPCut() const;

    /// Returns the overall minimum pt cut value
    virtual double minPtCut() const;

    /// Returns the overall maximum pt cut value
    virtual double maxPtCut() const;

    /// Returns the overall minimum chi^2 cut
    virtual double minChi2Cut() const;

    /// Returns the overall maximum chi^2 cut
    virtual double maxChi2Cut() const;

    /// Returns the charge selection
    virtual int chargeSel() const;

    /// Returns the overall minimum momentum cut value for the given track type
    virtual double minPCut( const Rich::Track::Type type ) const;

    /// Returns the overall maximum momentum cut value for the given track type
    virtual double maxPCut( const Rich::Track::Type type ) const;

    /// Returns the overall minimum momentum cut value for the given track type
    virtual double minPtCut( const Rich::Track::Type type ) const;

    /// Returns the overall maximum momentum cut value for the given track type
    virtual double maxPtCut( const Rich::Track::Type type ) const;

    /// Returns the overall minimum chi^2 cut for the given track type
    virtual double minChi2Cut( const Rich::Track::Type type ) const;

    /// Returns the overall maximum chi^2 cut for the given track type
    virtual double maxChi2Cut( const Rich::Track::Type type ) const;

    /// Returns the charge selection for the given track type
    virtual int chargeSel( const Rich::Track::Type type ) const;

  private: // defintions

    /// List of track names
    typedef std::vector<std::string> TrackNames;

  private: // methods

    /// Set up all Track selectors
    StatusCode setUpTracks();

    /// set things up for given track type
    StatusCode setUpTrack( const Rich::Track::Type type );

    /// returns minimum of two values
    template<class T>
    inline T min( const T t1, const T t2) const
    {
      return ( t1 < t2 ? t1 : t2 );
    }

    /// returns maximum of two values
    template<class T>
    inline T max( const T t1, const T t2) const
    {
      return ( t1 < t2 ? t2 : t1 );
    }

  private: // data

    /// Track types to accept
    TrackNames m_trNames;

    /// Mapping between track type and selection tool name to use
    typedef Rich::HashMap<Rich::Track::Type,std::string> ToolNames;
    ToolNames m_tkToolNames;

    /// Mapping between track type and selection tool pointer to use
    typedef Rich::HashMap<Rich::Track::Type,const Rich::IRichBaseTrackSelector*> TrackTools;
    TrackTools m_tkTools;

    // job options

    double m_minChi2Cut; ///< Min chi^2 cut
    double m_maxChi2Cut; ///< Max chi^2 cut

    double m_minPCut;    ///< Min p cut
    double m_maxPCut;    ///< Max p cut

    double m_minPtCut;   ///< Min pt cut
    double m_maxPtCut;   ///< Max pt cut

    int m_chargeSel;     ///< Charge selection (-1=negative,+1=positive,0=all)

    /// list of possible options
    std::vector<std::string> m_jobOpts;

  };

} // RICH namespace

#endif // RICHRECTOOLS_RichTrackSelectorBase_H
