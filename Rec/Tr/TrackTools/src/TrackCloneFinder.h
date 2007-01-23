// $Id: TrackCloneFinder.h,v 1.6 2007-01-23 16:36:39 erodrigu Exp $
#ifndef TRACKCLONEFINDER_H 
#define TRACKCLONEFINDER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from LHCbKernel
#include "Kernel/LHCbID.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackCloneFinder.h"

// from TrackEvent
#include "Event/Track.h"

// from TrackFitEvent
#include "Event/Measurement.h"

/** @class TrackCloneFinder TrackCloneFinder.h
 *  
 *
 *  @author Eduardo Rodrigues
 *  @date   2005-12-08
 */
class TrackCloneFinder : public GaudiTool,
                         virtual public ITrackCloneFinder {
public: 
  /// Standard constructor
  TrackCloneFinder( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~TrackCloneFinder( ); ///< Destructor

  StatusCode initialize();

  /** Compare two input Tracks and find whether one is a clone
   *  of the other based on some "overlap criteria".
   *  The corresponding flag may be set accordingly (NOT DONE BY DEFAULT)
   *  depending on the value of the "setFlag" argument.
   *  Note: the method ignores whether the Tracks themselves have been
   *        previously flagged as clones! It merely does a comparison.
   *  @return bool: True if one Track was set as a clone of the other.
   *                False otherwise.
   *  @param  track1 input 1st track
   *  @param  track2 input 2nd track
   *  @param  setFlag input parameter indicates whether the clone track
   *          is to be set as such (default = false)
   */
  virtual bool areClones( LHCb::Track& track1,
                          LHCb::Track& track2,
                          bool setFlag = false ) const;

protected:

private:
  /** Compare two input Tracks and find whether one is a clone
   *  of the other based on some "overlap criteria".
   *  Note: the method ignores whether the Tracks themselves have been
   *        previously flagged as clones! It merely does a comparison.
   *  @return bool: True if one Track was set as a clone of the other.
   *                False otherwise.
   *  @param  track1 input 1st track
   *  @param  track2 input 2nd track
   */
  bool clones( const LHCb::Track& track1, const LHCb::Track& track2 ) const;
  
  /** Calculate the number of common hits of a given LHCb type
   *  between two input Tracks.
   *  Note: hits can here mean either Measurements or LHCbIDs,
   *        depending on the settings for the "CompareAtLHCbIDsLevel"
   *        tool property:
   *        1) CompareAtLHCbIDsLevel = false (default):
   *           the comparison is based on the Measurements and it is
   *           therefore assumed that the Tracks have been fitted
   *        2) CompareAtLHCbIDsLevel = true:
   *           the comparison is based on the LHCbIDs and is therefore
   *           done on the Tracks as output by the pattern recognition
   *  @return unsigned int: the number of common hits
   *  @param  track1: input 1st track
   *  @param  track2: input 2nd track
   *  @param  lhcbidType: input LHCbID type
   *  @param  nHits1: the number of track1 hits of lhcbidType type
   *  @param  nHits2: the number of track2 hits of lhcbidType type
   */
  unsigned int nCommonHits( const LHCb::Track& track1,
                            const LHCb::Track& track2,
                            unsigned int lhcbidType,
                            unsigned int& nHits1,
                            unsigned int& nHits2 ) const;

  /** Calculate the number of common hits between two input Tracks.
   *  Note: hits can here mean either Measurements or LHCbIDs,
   *        depending on the settings for the "CompareAtLHCbIDsLevel"
   *        tool property:
   *        1) CompareAtLHCbIDsLevel = false (default):
   *           the comparison is based on the Measurements and it is
   *           therefore assumed that the Tracks have been fitted
   *        2) CompareAtLHCbIDsLevel = true:
   *           the comparison is based on the LHCbIDs and is therefore
   *           done on the Tracks as output by the pattern recognition
   *  @return unsigned int: the number of common hits
   */
  unsigned int nCommonHits( const LHCb::Track& track1,
                            const LHCb::Track& track2,
                            unsigned int& nHits1,
                            unsigned int& nHits2 ) const;

  /** Calculate the number of common hits between two input Tracks.
   *  Note: hits can here mean either Measurements or LHCbIDs,
   *        depending on the settings for the "CompareAtLHCbIDsLevel"
   *        tool property:
   *        1) CompareAtLHCbIDsLevel = false (default):
   *           the comparison is based on the Measurements and it is
   *           therefore assumed that the Tracks have been fitted
   *        2) CompareAtLHCbIDsLevel = true:
   *           the comparison is based on the LHCbIDs and is therefore
   *           done on the Tracks as output by the pattern recognition
   *  @return unsigned int: the number of common hits
   */
  unsigned int nCommonHits( const LHCb::Track& track1,
                            const LHCb::Track& track2 ) const;

  // Calculate the number of common LHCbIDs
  unsigned int nCommonLHCbIDs( const std::vector<LHCb::LHCbID>& ids1,
                               const std::vector<LHCb::LHCbID>& ids2 ) const;

  // Return a const vector of a Track's LHCbIDs of a given LHCbID type
  const std::vector<LHCb::LHCbID>
    lhcbIDsOfType( const LHCb::Track& track,
                   unsigned int lhcbidType = 0 ) const;

  bool areSettingsConsistent( const LHCb::Track& track1,
                              const LHCb::Track& track2 ) const;

private:
  bool m_debugLevel;
  
  // tool properties
  // ---------------
  /* fraction of hits in common that defines the overlap between the
   * two tracks and their being clones or not */
  double m_matchingFraction;
  /* compare the hits based on the list of LHCbIDs only instead of
   * comparing on the list of Measurements
   * (the difference comes from the possible outliers removed by
   * the track fit!) */
  bool m_compareAtLHCbIDsLevel;

};
#endif // TRACKCLONEFINDER_H
