// $Id: TrackMatch.h,v 1.1.1.1 2005-09-09 08:51:16 erodrigu Exp $
#ifndef TRACKMATCHING_TRACKMATCH_H
#define TRACKMATCHING_TRACKMATCH_H 1

// Include files
// -------------
#include "GaudiKernel/KeyedObject.h"

// from TrackEvent
#include "Event/Track.h"

// from ITEvent
#include "Event/ITCluster.h"

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
  TrackMatch( Track* veloTrack, Track* seedTrack, double chi2 );

  /// constructor with arguments
  TrackMatch (Track* veloTrack, Track* seedTrack, double chi2,
              std::vector<ITCluster*> ttClusters );
  
  /// Default Constructor 
  TrackMatch() 
    : m_veloTrack(0),
      m_seedTrack(0),
      m_chi2(0.0),
      m_ttClusters(0) {};

  /// Destructor 
  virtual ~TrackMatch() {}

  /// set velo Track
  void setVeloTrack( Track* veloTrack );
  
  /// get velo track
  const Track* veloTrack() const;

  /// set seed Track
  void setSeedTrack( Track* seedTrack );
  
  /// get seed track
  const Track* seedTrack() const;

  /// set chi2
  void setChi2( double chi2 );
  
  /// get chi2
  double chi2() const;

  /// get the vector of TT Clusters
  const std::vector<ITCluster*>& ttClusters() const;

  /// get the number of TT Clusters
  int numTTClusters() const;

  /// set the vector of TT Clusters
  void setTTClusters( const std::vector<ITCluster*>& ttClusters );

  /// add a cluster to the vector of TT Clusters
  void addTTCluster( ITCluster* cluster );

  /// get the vector of TT Clusters chi2
  const std::vector<double>& ttChi2s() const;

  /// set the vector of TT Clusters chi2
  void setTTChi2s( const std::vector<double>& ttChi2s );

  /// add a chi2 to the vector of TT Cluster chi2.
  void addTTChi2( double chi2 );

//  /// Fill the ASCII output stream
//  virtual std::ostream& fillStream(std::ostream& s) const;

protected: 

private: 

  Track* m_veloTrack;    /// pointer to the velo track
  Track* m_seedTrack;    /// pointer to the seed track
  double m_chi2;              /// chi2 of the matching between velo and seed
  std::vector<ITCluster*> m_ttClusters; /// vector of TT clusters
  std::vector<double> m_ttChi2s; /// vector of chi2 of TT clusters
  
};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline TrackMatch::TrackMatch( Track* veloTrack, 
                               Track* seedTrack, 
                               double chi2 )
 : m_ttClusters(0)
{
  m_veloTrack = veloTrack;
  m_seedTrack = seedTrack;
  m_chi2 = chi2;
}

inline TrackMatch::TrackMatch( Track* veloTrack, Track* seedTrack, 
                               double chi2,
                               std::vector<ITCluster*> ttClusters )
{
  m_veloTrack = veloTrack;
  m_seedTrack = seedTrack;
  m_chi2 = chi2;
  m_ttClusters = ttClusters;
} 

inline const Track* TrackMatch::veloTrack() const 
{
  return m_veloTrack;
}

inline void TrackMatch::setVeloTrack (Track* value )
{
  m_veloTrack = value; 
}

inline const Track* TrackMatch::seedTrack() const 
{
  return m_seedTrack;
}

inline void TrackMatch::setSeedTrack( Track* value )
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

inline const std::vector<ITCluster*>& TrackMatch::ttClusters() const
{
  return m_ttClusters;
}

inline int TrackMatch::numTTClusters() const
{
  return m_ttClusters.size();
}

inline void TrackMatch::setTTClusters( const std::vector<ITCluster*>& value )
{
  m_ttClusters = value;
}

inline void TrackMatch::addTTCluster( ITCluster* cluster )
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

/*inline std::ostream& TrackMatch::fillStream(std::ostream& s) const
{
  s << "{ "
    << " veloTrack:\t" << m_veloTrack << std::endl
    << " seedTrack:\t" << m_seedTrack << std::endl
    << " chi2:\t" << (float)m_chi2 << std::endl
    << " ttClusters:\t" << m_ttClusters << " } " << std::endl;
  return s;
}*/

//Defintion of keyed container for TrackMatch
typedef KeyedContainer<TrackMatch, Containers::HashMap> TrackMatches;

#endif // TRACKMATCHING_TRACKMATCH_H
