// $Id: TrackMatch.h,v 1.5 2007-06-01 11:53:59 mneedham Exp $
#ifndef TRACKMATCHING_TRACKMATCH_H
#define TRACKMATCHING_TRACKMATCH_H 1

// Include files
// -------------
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"

// from TrackEvent
#include "Event/Track.h"

// from STEvent
#include "Event/STCluster.h"

#include "GaudiKernel/boost_allocator.h"

// Namespace for locations in TDS
namespace TrackMatchLocation {
  static const std::string& Default = "Rec/Track/Matches";
}

/** @class TrackMatch TrackMatch.h "TrackMatching/TrackMatch.h"
 *
 *  TrackMatch is a class which holds the matches between the velo tracks and
 *  the seed tracks. Additionally the chi2 of the match is stored as well.
 *
 *  @author Eduardo Rodrigues (adaptations to new track event model)
 *  @date   2005-09-08
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl
 *  @date   11-07-2002
 */

class TrackMatch: public ContainedObject
{

public: 

  /// constructor with arguments
  TrackMatch( LHCb::Track* veloTrack, LHCb::Track* seedTrack, double chi2 );
   
  /// Default Constructor 
  TrackMatch() 
    : m_veloTrack(0),
      m_seedTrack(0),
      m_chi2(0.0){}

  /// Destructor 
  ~TrackMatch() {}

 
  /// get velo track
  const LHCb::Track* veloTrack() const;

  /// get seed track
  const LHCb::Track* seedTrack() const;
  
  /// get chi2
  double chi2() const;

#ifndef _WIN32
    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(TrackMatch) == size ?
               boost::singleton_pool<TrackMatch, sizeof(TrackMatch)>::malloc() :
               ::operator new(size) );
    }

    /// placement operator new
    /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
    /// it is not needed in libstdc++ >= 3.4
    static void* operator new ( size_t size, void* pObj )
    {
      return ::operator new (size,pObj);
    }

    /// operator delete
    static void operator delete ( void* p )
    {
      boost::singleton_pool<TrackMatch, sizeof(TrackMatch)>::is_from(p) ?
      boost::singleton_pool<TrackMatch, sizeof(TrackMatch)>::free(p) :
      ::operator delete(p);
    }

    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete ( void* p, void* pObj )
    {
      ::operator delete (p, pObj);
    }
#endif


protected: 

private: 

  LHCb::Track* m_veloTrack;    /// pointer to the velo track
  LHCb::Track* m_seedTrack;    /// pointer to the seed track
  double m_chi2;               /// chi2 of the matching between velo and seed
  
};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline TrackMatch::TrackMatch( LHCb::Track* veloTrack, 
                               LHCb::Track* seedTrack, 
                               double chi2 )
  : m_veloTrack(veloTrack),
    m_seedTrack(seedTrack),
    m_chi2(chi2)
{
}

inline const LHCb::Track* TrackMatch::veloTrack() const 
{
  return m_veloTrack;
}

inline const LHCb::Track* TrackMatch::seedTrack() const 
{
  return m_seedTrack;
}

inline double TrackMatch::chi2() const 
{
  return m_chi2;
}

//Defintion of keyed container for TrackMatch
//typedef KeyedContainer<TrackMatch, Containers::HashMap> TrackMatches;
typedef ObjectVector<TrackMatch> TrackMatches;

#endif // TRACKMATCHING_TRACKMATCH_H
