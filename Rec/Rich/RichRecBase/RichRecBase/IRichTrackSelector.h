
//-----------------------------------------------------------------------------
/** @file IRichTrackSelector.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::ITrackSelector
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_IRICHTRACKSELECTOR_H
#define RICHRECBASE_IRICHTRACKSELECTOR_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// RichRecBase
#include "RichRecBase/RichTrackID.h"

// Event model
namespace LHCb
{
  class RichRecTrack;
  class Track;
}

static const InterfaceID IID_IRichTrackSelector ( "Rich::Rec::ITrackSelector", 1, 0 );

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------------
    /** @class ITrackSelector IRichTrackSelector.h RichRecBase/IRichTrackSelector.h
     *
     *  Interface to a utility tool for the RICH reconstruction providing generic
     *  track selection methods.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2006-08-12
     */
    //---------------------------------------------------------------------------------

    class ITrackSelector : virtual public IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID & interfaceID() { return IID_IRichTrackSelector; }

      /** Test if the given Track is selected
       *
       *  @param track Pointer to a Track
       *
       *  @return Boolean indicating if the track is selected
       *  @retval true  Track is selected
       *  @retval false Track is rejected
       */
      virtual bool trackSelected( const LHCb::Track * track ) const = 0;

      /** Test it the given RichRecTrack is selected
       *
       *  @param track Pointer to a RichRecTrack
       *
       *  @return Boolean indicating if the track is selected
       *  @retval true  Track is selected
       *  @retval false Track is rejected
       */
      virtual bool trackSelected( const LHCb::RichRecTrack * track ) const = 0;

      /// Returns the list of selected track types
      virtual const std::vector<std::string> & selectedTracks() const = 0;

    public:

      /// Returns the overall minimum momentum cut value
      virtual double minPCut() const = 0;

      /// Returns the overall maximum momentum cut value
      virtual double maxPCut() const = 0;

      /// Returns the overall minimum momentum cut value
      virtual double minPtCut() const = 0;

      /// Returns the overall maximum momentum cut value
      virtual double maxPtCut() const = 0;

      /// Returns the overall minimum chi^2 cut
      virtual double minChi2Cut() const = 0;

      /// Returns the overall maximum chi^2 cut
      virtual double maxChi2Cut() const = 0;

      /// Returns the charge selection
      virtual int chargeSel() const = 0;

      /// Returns the minimum likelihood cut
      virtual double minLikelihoodCut() const = 0;

      /// Returns the maximum likelihood cut
      virtual double maxLikelihoodCut() const = 0;

      /// Returns the minimum clone distance rejection cut
      virtual double minCloneDistCut() const = 0;

      /// Returns the maximum clone distance rejection cut
      virtual double maxCloneDistCut() const = 0;

      /// Returns the minimum ghost probaility cut
      virtual double minGhostProbCut() const = 0;

      /// Returns the maximum ghost probaility cut
      virtual double maxGhostProbCut() const = 0;

    public:

      /// Returns the overall minimum momentum cut value for the given track type
      virtual double minPCut( const Rich::Rec::Track::Type type ) const = 0;

      /// Returns the overall maximum momentum cut value for the given track type
      virtual double maxPCut( const Rich::Rec::Track::Type type ) const = 0;

      /// Returns the overall minimum momentum cut value for the given track type
      virtual double minPtCut( const Rich::Rec::Track::Type type ) const = 0;

      /// Returns the overall maximum momentum cut value for the given track type
      virtual double maxPtCut( const Rich::Rec::Track::Type type ) const = 0;

      /// Returns the overall minimum chi^2 cut for the given track type
      virtual double minChi2Cut( const Rich::Rec::Track::Type type ) const = 0;

      /// Returns the overall maximum chi^2 cut for the given track type
      virtual double maxChi2Cut( const Rich::Rec::Track::Type type ) const = 0;

      /// Returns the charge selection for the given track type
      virtual int chargeSel( const Rich::Rec::Track::Type type ) const = 0;

      /// Returns the minimum likelihood cut for the given track type
      virtual double minLikelihoodCut( const Rich::Rec::Track::Type type ) const = 0;

      /// Returns the maximum likelihood cut for the given track type
      virtual double maxLikelihoodCut( const Rich::Rec::Track::Type type ) const = 0;

      /// Returns the minimum clone distance rejection cut for the given track type
      virtual double minCloneDistCut( const Rich::Rec::Track::Type type ) const = 0;

      /// Returns the maximum clone distance rejection cut for the given track type
      virtual double maxCloneDistCut( const Rich::Rec::Track::Type type ) const = 0;

      /// Returns the minimum ghost probability cut for the given track type
      virtual double minGhostProbCut( const Rich::Rec::Track::Type type ) const = 0;

      /// Returns the maximum ghost probability cut for the given track type
      virtual double maxGhostProbCut( const Rich::Rec::Track::Type type ) const = 0;

    };

  }
}

#endif // RICHRECBASE_IRICHTRACKSELECTOR_H
