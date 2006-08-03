// $Id: TrackMatch.h,v 1.4 2006-08-03 09:14:38 mneedham Exp $
#ifndef TRACKMATCHING_TRACKMATCH_H
#define TRACKMATCHING_TRACKMATCH_H 1

// Include files
// -------------
#include "GaudiKernel/KeyedObject.h"

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

class TrackMatch: public KeyedObject<int>
{

public: 

  /// constructor with arguments
  TrackMatch( LHCb::Track* veloTrack, LHCb::Track* seedTrack, double chi2 );
  
  /// constructor with arguments
  TrackMatch( LHCb::Track* veloTrack, LHCb::Track* seedTrack, double chi2,
              std::vector<LHCb::STCluster*> ttClusters );
  
  /// Default Constructor 
  TrackMatch() 
    : m_veloTrack(0),
      m_seedTrack(0),
      m_chi2(0.0),
      m_ttClusters(0) {};

  /// Destructor 
  ~TrackMatch() {}

  /// set velo Track
  void setVeloTrack( LHCb::Track* veloTrack );
  
  /// get velo track
  const LHCb::Track* veloTrack() const;

  /// set seed Track
  void setSeedTrack( LHCb::Track* seedTrack );
  
  /// get seed track
  const LHCb::Track* seedTrack() const;

  /// set chi2
  void setChi2( double chi2 );
  
  /// get chi2
  double chi2() const;

  /// get the vector of TT Clusters
  const std::vector<LHCb::STCluster*>& ttClusters() const;

  /// get the number of TT Clusters
  int numTTClusters() const;

  /// set the vector of TT Clusters
  void setTTClusters( const std::vector<LHCb::STCluster*>& ttClusters );

  /// add a cluster to the vector of TT Clusters
  void addTTCluster( LHCb::STCluster* cluster );

  /// get the vector of TT Clusters chi2
  const std::vector<double>& ttChi2s() const;

  /// set the vector of TT Clusters chi2
  void setTTChi2s( const std::vector<double>& ttChi2s );

  /// add a chi2 to the vector of TT Cluster chi2.
  void addTTChi2( double chi2 );


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
  std::vector<LHCb::STCluster*> m_ttClusters; /// vector of TT clusters
  std::vector<double> m_ttChi2s;              /// vector of chi2 of TT clusters
  
};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline TrackMatch::TrackMatch( LHCb::Track* veloTrack, 
                               LHCb::Track* seedTrack, 
                               double chi2 )
  : m_veloTrack(veloTrack),
    m_seedTrack(seedTrack),
    m_chi2(chi2),
    m_ttClusters(0)
{
}

inline TrackMatch::TrackMatch( LHCb::Track* veloTrack, LHCb::Track* seedTrack, 
                               double chi2,
                               std::vector<LHCb::STCluster*> ttClusters ):
m_veloTrack(veloTrack),
m_seedTrack(seedTrack),
m_chi2(chi2),
m_ttClusters(ttClusters)
{
} 

inline const LHCb::Track* TrackMatch::veloTrack() const 
{
  return m_veloTrack;
}

inline void TrackMatch::setVeloTrack ( LHCb::Track* value )
{
  m_veloTrack = value; 
}

inline const LHCb::Track* TrackMatch::seedTrack() const 
{
  return m_seedTrack;
}

inline void TrackMatch::setSeedTrack( LHCb::Track* value )
{
  m_seedTrack = value; 
}

inline double TrackMatch::chi2() const 
{
  return m_chi2;
}

inline void TrackMatch::setChi2( double value )
{
  m_chi2 = value; 
}

inline const std::vector<LHCb::STCluster*>& TrackMatch::ttClusters() const
{
  return m_ttClusters;
}

inline int TrackMatch::numTTClusters() const
{
  return m_ttClusters.size();
}

inline void
TrackMatch::setTTClusters( const std::vector<LHCb::STCluster*>& value )
{
  m_ttClusters = value;
}

inline void TrackMatch::addTTCluster( LHCb::STCluster* cluster )
{
  m_ttClusters.push_back(cluster);
}

inline const std::vector<double>& TrackMatch::ttChi2s() const
{
  return m_ttChi2s;
}

inline void TrackMatch::setTTChi2s( const std::vector<double>& value )
{
  m_ttChi2s = value;
}

inline void TrackMatch::addTTChi2( double chi2 )
{
  m_ttChi2s.push_back(chi2);
}

//Defintion of keyed container for TrackMatch
typedef KeyedContainer<TrackMatch, Containers::HashMap> TrackMatches;

#endif // TRACKMATCHING_TRACKMATCH_H
