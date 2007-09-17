// $Id: TrackCloneChecker.h,v 1.2 2007-09-17 09:24:37 jonrob Exp $
#ifndef TRACKCLONECHECKER_H
#define TRACKCLONECHECKER_H 1

#include <map>

#include "TrackCheckerBase.h"

// from Event/LinkerEvent
#include "Linker/LinkedFrom.h"

// Boost
#include "boost/lexical_cast.hpp"
#include "boost/format.hpp"

/** @class TrackCloneChecker TrackCloneChecker.h
 *
 *  Produce some simple plots for the Clone linker information
 *
 *  @author Chris Jones
 *  @date   2007-09-13
 */

class TrackCloneChecker : public TrackCheckerBase
{

public:

  /// Standard constructor
  TrackCloneChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackCloneChecker( ); ///< Destructor

  virtual StatusCode initialize();  ///< Algorithm initialize

  virtual StatusCode execute();  ///< Algorithm execution

  virtual StatusCode finalize();  ///< Algorithm finalize

private:

  /** @class TrackTally TrackCloneChecker.h
   *
   *  Counts track information for clones
   *
   *  @author Chris Jones
   *  @date   2007-09-13
   */
  class TrackTally
  {
  public:
    TrackTally()
      : totalClones(0), totalNonClones(0), totalGhosts(0),
        rejectedClones(0), rejectedNonClones(0), rejectedGhosts(0) { }
  public:
    unsigned long totalClones;
    unsigned long totalNonClones;
    unsigned long totalGhosts;
    unsigned long rejectedClones;
    unsigned long rejectedNonClones;
    unsigned long rejectedGhosts;
  public:
    /// Map for one tally object per track history type
    typedef std::map<LHCb::Track::History,TrackTally> Map;
  };

private:

  /// Does the given track have an MC Clone
  bool hasMCClone( const LHCb::Track * track );

  /// Get efficiency
  inline std::pair<double,double> getEff1( const double top,
                                           const double bot ) const
  {
    return std::pair<double,double>( ( bot>0 ? 100.0 * top/bot                          : 0 ),
                                     ( bot>0 ? 100.0 * sqrt((top/bot)*(1.-top/bot)/bot) : 0 ) );
  }

  /// Get efficiency
  inline std::pair<double,double> getEff2( const double top,
                                           const double bot ) const
  {
    return std::pair<double,double>( ( bot>0 ? top/bot       : 0 ),
                                     ( bot>0 ? sqrt(top)/bot : 0 ) );
  }

private:

  /// Clone linker information location in TES
  std::string m_cloneInfoTES;

  /// Summary map
  TrackTally::Map m_trackMap;

  /// KL distance cut
  double m_klCut;

  /// Number of rec tracks per MCParticle
  typedef std::map<const LHCb::MCParticle *, unsigned int> MCPmap;
  MCPmap m_MCPmap;

  /// Event count
  unsigned long m_nEvts;

};

#endif // TRACKCLONECHECKER_H
