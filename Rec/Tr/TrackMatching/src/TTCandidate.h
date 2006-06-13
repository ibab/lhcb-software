// $Id: TTCandidate.h,v 1.4 2006-06-13 15:33:25 jvantilb Exp $
#ifndef TRACKMATCHING_TTCANDIDATE_H
#define TRACKMATCHING_TTCANDIDATE_H 1

// Include files
#include "GaudiKernel/KeyedObject.h"

// from GSL
#include "gsl/gsl_math.h"

// from STEvent
#include "Event/STCluster.h"

// from GaudiKernel
#include "Kernel/TrackTypes.h"

// Namespace for locations in TDS
namespace TTCandidateLocation {
  static const std::string& Default = "Rec/Track/TTCandidates";
}

/** @class TTCandidate TTCandidate.h "TrackMatching/TTCandidate.h"
 *
 *  TTCandidate is a class which holds a set of TT clusters which can be used
 *  as a candidate set of TT clusters for a matched track. Also the 
 *  corresponding distances to the predicted track are stored here. In addition
 *  there is a flag which decides whether this set should be regarded as dead
 *  and also the layer number of the last added cluster is stored.
 *
 *  @author Jeroen van Tilburg Jeroen.van.Tilburg@cern.ch
 *  @date   2003-09-19
 */

class TTCandidate: public KeyedObject<int>
{

public: 

  /// constructor with arguments
  TTCandidate( LHCb::STCluster* cluster, double distance, 
               unsigned int lastLayer, const Gaudi::TrackVector& refVector );

  /// constructor with arguments from an old TTCandidate
  TTCandidate(TTCandidate* candidate, LHCb::STCluster* cluster, 
              double distance, unsigned int lastLayer,
              const Gaudi::TrackVector& refVector);

  /// Default Constructor 
  TTCandidate() 
    : m_ttClusters(0),
      m_distances(0),
      m_dead(0.0),
      m_lastLayer(0) {};

  /// Destructor 
  virtual ~TTCandidate() {}

  /// get the vector of TT Clusters
  const std::vector<LHCb::STCluster*>& ttClusters() const;

  /// get the number of TT Clusters
  unsigned int numTTClusters() const;

  /// get all distances
  const std::vector<double>& distances() const;

  /// get the vector of TT Clusters
  const std::vector<Gaudi::TrackVector>& refVectors() const;

  /// get the spread of the distances
  double spread() const;

  /// get last distance
  double lastDistance() const;

  /// get average distance
  double averageDistance() const;

  /// set dead flag
  void setDead( bool flag );
  
  /// get the dead flag;
  bool isDead() const;

  /// get the last layer
  unsigned int lastLayer() const;

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

protected: 

private:

  std::vector<LHCb::STCluster*> m_ttClusters; ///< vector of TT clusters
  std::vector<double> m_distances;            ///< vector of corresponding dist
  std::vector<Gaudi::TrackVector> m_refVectors;///< vector of reference vectors
  double m_averageDistance;                   ///< average distance
  bool m_dead;                                ///< candidate is dead
  unsigned int m_lastLayer;                   ///< number of the last layer
  
};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline TTCandidate::TTCandidate( LHCb::STCluster* cluster, double distance, 
                                 unsigned int lastLayer,
                                 const Gaudi::TrackVector& refVector)
  : m_averageDistance( distance )
  , m_dead( false )
  , m_lastLayer( lastLayer )
{
  m_ttClusters.push_back( cluster );
  m_distances.push_back( distance );
  m_refVectors.push_back( refVector );
}

inline TTCandidate::TTCandidate( TTCandidate* candidate,
                                 LHCb::STCluster* cluster, double distance, 
                                 unsigned int lastLayer,
                                 const Gaudi::TrackVector& refVector)
  : m_dead( false )
  , m_lastLayer( lastLayer )
{
  m_ttClusters = candidate->ttClusters();
  m_ttClusters.push_back( cluster );
  m_distances = candidate->distances();
  m_distances.push_back( distance );
  m_refVectors = candidate->refVectors();
  m_refVectors.push_back( refVector );
  unsigned int numClus = candidate->numTTClusters();
  m_averageDistance = (candidate->averageDistance() * numClus + distance ) / 
    ( numClus + 1 );
}  

inline const std::vector<LHCb::STCluster*>& TTCandidate::ttClusters() const 
{
  return m_ttClusters;
}

inline unsigned int TTCandidate::numTTClusters() const
{
  return m_ttClusters.size();
}

inline const std::vector<double>& TTCandidate::distances() const
{
  return m_distances;
}

inline const std::vector<Gaudi::TrackVector>& TTCandidate::refVectors() const
{
  return m_refVectors;
}

inline double TTCandidate::spread() const
{
  double spread = 0.0;
  std::vector<double>::const_iterator iDist;
  for ( iDist = m_distances.begin(); iDist != m_distances.end(); ++iDist ) {
    spread += gsl_pow_2( (*iDist) - m_averageDistance );
  }  
  spread /= double (this->numTTClusters()-1.0);

  return sqrt(spread);
}

inline double TTCandidate::lastDistance() const
{
  return *(m_distances.rbegin());
}

inline double TTCandidate::averageDistance() const
{
  return m_averageDistance;
}

inline void TTCandidate::setDead( bool flag )
{
  m_dead = flag;
}

inline bool TTCandidate::isDead() const 
{
  return m_dead;
}

inline unsigned int TTCandidate::lastLayer() const
{
  return m_lastLayer;
}

inline std::ostream& TTCandidate::fillStream(std::ostream& s) const
{
  s << "{ "
    << " ttClusters:\t" << m_ttClusters << std::endl
    << " distances:\t" << m_distances << std::endl
    << " averageDistance:\t" << (float)m_averageDistance << std::endl
    << " dead:\t" << m_dead << std::endl
    << " lastLayer:\t" << m_lastLayer << " } " << std::endl;
  return s;
}

//Defintion of TTCandidate vector
typedef std::vector<TTCandidate*> TTCandidates;

#endif // TRACKMATCHING_TTCANDIDATE_H
