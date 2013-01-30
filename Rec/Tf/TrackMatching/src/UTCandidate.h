// $Id: UTCandidate.h,v 1.2 2007-12-03 07:07:21 cattanem Exp $
#ifndef TRACKMATCHING_UTCANDIDATE_H
#define TRACKMATCHING_UTCANDIDATE_H 1

// Include files
#include "GaudiKernel/KeyedObject.h"
#include "GaudiKernel/SerializeSTL.h"

// from GSL
#include "gsl/gsl_math.h"

// from STEvent
#include "Event/STCluster.h"

// from GaudiKernel
#include "Event/TrackTypes.h"

#include "GaudiKernel/boost_allocator.h"

// Namespace for locations in TDS
namespace UTCandidateLocation {
  static const std::string& Default = "Rec/Track/UTCandidates";
}

/** @class UTCandidate UTCandidate.h "TrackMatching/UTCandidate.h"
 *
 *  UTCandidate is a class which holds a set of UT clusters which can be used
 *  as a candidate set of UT clusters for a matched track. Also the 
 *  corresponding distances to the predicted track are stored here. In addition
 *  there is a flag which decides whether this set should be regarded as dead
 *  and also the layer number of the last added cluster is stored.
 *
 *  @author Jeroen van Tilburg Jeroen.van.Tilburg@cern.ch
 *  @date   2003-09-19
 */

class UTCandidate: public KeyedObject<int>
{

public: 

  /// constructor with arguments
  UTCandidate( LHCb::STCluster* cluster, double distance, 
               unsigned int lastLayer, const Gaudi::TrackVector& refVector );

  /// constructor with arguments from an old UTCandidate
  UTCandidate(UTCandidate* candidate, LHCb::STCluster* cluster, 
              double distance, unsigned int lastLayer,
              const Gaudi::TrackVector& refVector);

  /// Default Constructor 
  UTCandidate() 
    : m_utClusters(0),
      m_distances(0),
      m_dead(0.0),
      m_lastLayer(0) {};

  /// Destructor 
  ~UTCandidate() {}

  /// get the vector of UT Clusters
  const std::vector<LHCb::STCluster*>& ttClusters() const;

  /// get the number of UT Clusters
  unsigned int numUTClusters() const;

  /// get all distances
  const std::vector<double>& distances() const;

  /// get the vector of UT Clusters
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

  #ifndef _WIN32
    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(UTCandidate) == size ?
               boost::singleton_pool<UTCandidate, sizeof(UTCandidate)>::malloc() :
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
      boost::singleton_pool<UTCandidate, sizeof(UTCandidate)>::is_from(p) ?
      boost::singleton_pool<UTCandidate, sizeof(UTCandidate)>::free(p) :
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

  std::vector<LHCb::STCluster*> m_utClusters; ///< vector of UT clusters
  std::vector<double> m_distances;            ///< vector of corresponding dist
  std::vector<Gaudi::TrackVector> m_refVectors;///< vector of reference vectors
  double m_averageDistance;                   ///< average distance
  bool m_dead;                                ///< candidate is dead
  unsigned int m_lastLayer;                   ///< number of the last layer
  
};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline UTCandidate::UTCandidate( LHCb::STCluster* cluster, double distance, 
                                 unsigned int lastLayer,
                                 const Gaudi::TrackVector& refVector)
  : m_averageDistance( distance )
  , m_dead( false )
  , m_lastLayer( lastLayer )
{
  m_utClusters.push_back( cluster );
  m_distances.push_back( distance );
  m_refVectors.push_back( refVector );
}

inline UTCandidate::UTCandidate( UTCandidate* candidate,
                                 LHCb::STCluster* cluster, double distance, 
                                 unsigned int lastLayer,
                                 const Gaudi::TrackVector& refVector)
  : m_dead( false )
  , m_lastLayer( lastLayer )
{
  m_utClusters = candidate->ttClusters();
  m_utClusters.push_back( cluster );
  m_distances = candidate->distances();
  m_distances.push_back( distance );
  m_refVectors = candidate->refVectors();
  m_refVectors.push_back( refVector );
  unsigned int numClus = candidate->numUTClusters();
  m_averageDistance = (candidate->averageDistance() * numClus + distance ) / 
    ( numClus + 1 );
}  

inline const std::vector<LHCb::STCluster*>& UTCandidate::ttClusters() const 
{
  return m_utClusters;
}

inline unsigned int UTCandidate::numUTClusters() const
{
  return m_utClusters.size();
}

inline const std::vector<double>& UTCandidate::distances() const
{
  return m_distances;
}

inline const std::vector<Gaudi::TrackVector>& UTCandidate::refVectors() const
{
  return m_refVectors;
}

inline double UTCandidate::spread() const
{
  double spread = 0.0;
  std::vector<double>::const_iterator iDist;
  for ( iDist = m_distances.begin(); iDist != m_distances.end(); ++iDist ) {
    spread += gsl_pow_2( (*iDist) - m_averageDistance );
  }  
  spread /= double (this->numUTClusters()-1.0);

  return sqrt(spread);
}

inline double UTCandidate::lastDistance() const
{
  return m_distances.back();
}

inline double UTCandidate::averageDistance() const
{
  return m_averageDistance;
}

inline void UTCandidate::setDead( bool flag )
{
  m_dead = flag;
}

inline bool UTCandidate::isDead() const 
{
  return m_dead;
}

inline unsigned int UTCandidate::lastLayer() const
{
  return m_lastLayer;
}

inline std::ostream& UTCandidate::fillStream(std::ostream& s) const
{
  using GaudiUtils::operator<<;
  s << "{ "
    << " ttClusters:\t" << m_utClusters << std::endl
    << " distances:\t" << m_distances << std::endl
    << " averageDistance:\t" << (float)m_averageDistance << std::endl
    << " dead:\t" << m_dead << std::endl
    << " lastLayer:\t" << m_lastLayer << " } " << std::endl;
  return s;
}

//Defintion of UTCandidate vector
typedef std::vector<UTCandidate*> UTCandidates;

#endif // TRACKMATCHING_UTCANDIDATE_H
