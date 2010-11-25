// $Id: $
#ifndef FASTVELOTRACK_H 
#define FASTVELOTRACK_H 1

// Include files
#include "FastVeloHit.h"
#include "FastVeloSensor.h"
#include "Event/StateVector.h"

/** @class FastVeloTrack FastVeloTrack.h
 *  Working tracks, used inside the FastVeloTracking
 *
 *  @author Olivier Callot
 *  @date   2010-09-09
 */
class FastVeloTrack {
public: 
  /// Standard constructor
  FastVeloTrack( ); 

  virtual ~FastVeloTrack( ); ///< Destructor

  inline double rPred( double z )   const { return m_r0 + z * m_tr; }
  inline bool backward()            const { return m_backward; }
  inline unsigned int zone()        const { return m_zone; }
  inline unsigned int nbUsedRHits() const { return m_nbUsedRHits; }
  inline unsigned int nbRHits()     const { return m_rHits.size(); }
  inline int nbMissedSensors()      const { return m_missedSensors; }
  inline FastVeloHits& rHits()            { return m_rHits; }
  inline FastVeloHits& phiHits()          { return m_phiHits; }
  inline double rErr2( double z )   const { double dz = z-m_sz / m_s0; return m_trErr2 + dz * dz * m_trErr2; }

  double rChi2() {
    double chi2 = 0.;
    for ( FastVeloHits::const_iterator itH = m_rHits.begin(); m_rHits.end() != itH; ++itH ) {
      double d = m_r0 + (*itH)->z() * m_tr - (*itH)->global();
      chi2 += (*itH)->weight() * d * d;
    }
    return chi2/(m_rHits.size()-2 );
  }

  void setBackward( bool flag )           { m_backward = flag; }  
  void setZone( unsigned int zone )       {  m_zone = zone; }
  void addRHit( FastVeloHit* hit );
  void removeRHit( FastVeloHit* hit );
  void setMissedSensors( int nMiss )      { m_missedSensors = nMiss; }

  void tagUsedRHits() {
    for ( FastVeloHits::iterator itH = m_rHits.begin(); m_rHits.end() != itH; ++itH ) {
      (*itH)->setUsed();
    }
  }

  void untagUsedRHits() {
    for ( FastVeloHits::iterator itH = m_rHits.begin(); m_rHits.end() != itH; ++itH ) {
      (*itH)->clearUsed();
    }
  }

  void tagUsedPhiHits() {
    for ( FastVeloHits::iterator itH = m_phiHits.begin(); m_phiHits.end() != itH; ++itH ) {
      (*itH)->setUsed();
    }
  }

  bool allHitsIncluded( FastVeloHits& hits );

  double rInterpolated( double z );

  void setSpaceParametersFromR( double cosPhi, double sinPhi ) {
    m_x0 = m_r0 * cosPhi;
    m_y0 = m_r0 * sinPhi;
    m_tx = m_tr * cosPhi;
    m_ty = m_tr * sinPhi;
  }
  
  void setPhiClusters( FastVeloTrack& track,
                       double cosPhi, double sinPhi,
                       FastVeloHits::const_iterator it1, 
                       FastVeloHits::const_iterator it2 );

  void setPhiClusters( FastVeloTrack& track,
                       double x0, double tx, double y0, double ty,
                       FastVeloHit* h1, FastVeloHit* h2, FastVeloHit* h3 );
  
  void setPhiClusters( FastVeloHit* r1, int zone, 
                       FastVeloHit* h1, FastVeloHit* h2, FastVeloHit* h3 );
  
  void setPhiClusters( FastVeloHit* r1, FastVeloHit* r2, int zone, 
                       FastVeloHit* h1, FastVeloHit* h2 );
  
  bool addPhiCluster( FastVeloHit* hit, double maxChi2 );
  
  void fitTrack();

  bool removeWorstMultiple( double maxChi2, unsigned int minExpected, bool withSin = true );

  bool removeWorstRAndPhi( double maxChi2, unsigned int minExpected );

  void solve();

  double distance( FastVeloHit* hit ) { return hit->distance( m_x0 + hit->z() * m_tx, m_y0 + hit->z() * m_ty ); }
 
  double chi2( FastVeloHit* hit ) { return hit->chi2( m_x0 + hit->z() * m_tx, m_y0 + hit->z() * m_ty ); }

  double xAtLastR() { return m_x0 + m_tx * m_rHits.back()->z(); }
 
  double xAtHit( FastVeloHit* hit ) { return m_x0 + m_tx * hit->z(); }

  double yAtHit( FastVeloHit* hit ) { return m_y0 + m_ty * hit->z(); }

  double xAtZ( double z ) { return m_x0 + m_tx * z; }

  double yAtZ( double z ) { return m_y0 + m_ty * z; }

  double rAtZ( double z ) {
    double x = m_x0 + m_tx * z;
    double y = m_y0 + m_ty * z;
    return sqrt( x*x+y*y);
  }
  
  bool addBestRCluster ( FastVeloSensor* sensor, double maxChi2 );

  void updateRParameters() {
    FastVeloHits::iterator itH;
    for ( itH = m_rHits.begin(); m_rHits.end() != itH; ++itH ) {
      (*itH)->setStartingPoint( m_x0 + (*itH)->z() * m_tx, m_y0 + (*itH)->z() * m_ty );
    }
    fitTrack();
  }

  void updatePhiWeights() {
    FastVeloHits::iterator itH;
    for ( itH = m_phiHits.begin(); m_phiHits.end() != itH; ++itH ) {
      (*itH)->setPhiWeight( m_x0 + (*itH)->z() * m_tx, m_y0 + (*itH)->z() * m_ty );
    }
    fitTrack();
  }

  bool addClustersToTrack ( FastVeloHits& hitList, double maxChi2ToAdd, double maxChi2PerHit, bool reSort = false ) {
    bool added = false;
    FastVeloHits::const_iterator itH;
    for ( itH = hitList.begin() ; hitList.end() != itH ; ++itH ) {
      added |= addPhiCluster( *itH, maxChi2ToAdd );
    }
    if ( added ) {
      if ( reSort ) std::sort( m_phiHits.begin(), m_phiHits.end(), FastVeloHit::DecreasingByZ() );
      added = removeWorstMultiple( maxChi2PerHit, 3, false );
    } 
    return added;
  }

  void addPhiClusters( FastVeloHits& list ) {
    FastVeloHits::const_iterator itH;
    for ( itH = list.begin() ; list.end() != itH ; ++itH ) {
      m_phiHits.push_back( *itH );

    }
    fitTrack();
  }
  

  bool addBestPhiCluster ( FastVeloHits& hitList, double maxChi2ToAdd );

  bool addBestClusterOtherSensor ( FastVeloHits& hitList, double maxChi2ToAdd );

  void addToFit( FastVeloHit* hit ) {
    m_qFactor = -1.;
    double a = hit->a();
    double b = hit->b();
    double c = hit->c();
    double z = hit->z();
    double w = hit->weight();
    
    m_sa2   += w * a * a;
    m_sa2z  += w * a * a * z;
    m_sa2z2 += w * a * a * z * z;
    m_sab   += w * a * b;
    m_sabz  += w * a * b * z;
    m_sabz2 += w * a * b * z * z;
    m_sb2   += w * b * b;
    m_sb2z  += w * b * b * z;
    m_sb2z2 += w * b * b * z * z;
    m_sac   += w * a * c;
    m_sacz  += w * a * c * z;
    m_sbc   += w * b * c;
    m_sbcz  += w * b * c * z;
  }
  
  void subtractToFit( FastVeloHit* hit ) {
    m_qFactor = -1.;
    double a = hit->a();
    double b = hit->b();
    double c = hit->c();
    double z = hit->z();
    double w = hit->weight();
    
    m_sa2   -= w * a * a;
    m_sa2z  -= w * a * a * z;
    m_sa2z2 -= w * a * a * z * z;
    m_sab   -= w * a * b;
    m_sabz  -= w * a * b * z;
    m_sabz2 -= w * a * b * z * z;
    m_sb2   -= w * b * b;
    m_sb2z  -= w * b * b * z;
    m_sb2z2 -= w * b * b * z * z;
    m_sac   -= w * a * c;
    m_sacz  -= w * a * c * z;
    m_sbc   -= w * b * c;
    m_sbcz  -= w * b * c * z;
  }

  double qFactor() {
    if ( 0 < m_qFactor ) return m_qFactor;
    m_qFactor = 0.;
    FastVeloHits::const_iterator itH;
    for ( itH = m_rHits.begin(); m_rHits.end() != itH ; ++itH ) {
      m_qFactor += ( *itH )->chi2( m_x0 + (*itH)->z() * m_tx, m_y0 + (*itH)->z() * m_ty);
    }
    int nbUsed = 0;
    for ( itH = m_phiHits.begin(); m_phiHits.end() != itH ; ++itH ) {
      m_qFactor += ( *itH )->chi2( m_x0 + (*itH)->z() * m_tx, m_y0 + (*itH)->z() * m_ty);
      if ( 0 != (*itH)->nbUsed() ) ++nbUsed;
    }
    m_qFactor = m_qFactor / ( m_rHits.size() + m_phiHits.size() - 4 );
    m_qFactor += 2. * double( nbUsed ) / double( m_phiHits.size() );
    return m_qFactor;
  }

  int nbUnusedPhiHits() {
    int nb = 0;
    for ( FastVeloHits::iterator itH = m_phiHits.begin(); m_phiHits.end() != itH ; ++itH ) {
      if ( 0 == (*itH)->nbUsed() ) ++nb;
    }
    return nb;
  }
  
  bool isValid() const { return m_valid; }

  void setValid( bool flag ) { m_valid = flag; }

  LHCb::StateVector state( double z ) {
    LHCb::StateVector temp;
    temp.setX ( m_x0 + z * m_tx );
    temp.setY ( m_y0 + z * m_ty );
    temp.setZ ( z  );
    temp.setTx( m_tx );
    temp.setTy( m_ty );
    temp.setQOverP( 0. );
    return temp;
  }

  double zBeam() { return -( m_x0 * m_tx + m_y0 * m_ty ) / ( m_tx * m_tx + m_ty * m_ty ); }

  Gaudi::TrackSymMatrix covariance( double z );

  struct DecreasingByRLength  {
    bool operator() (const FastVeloTrack& lhs, const FastVeloTrack& rhs) const { return lhs.nbRHits() > rhs.nbRHits(); }
  };


protected:

private:
  double        m_s0;
  double        m_sr;
  double        m_sz;
  double        m_srz;
  double        m_sz2;
  unsigned int  m_zone;
  bool          m_backward;  
  double        m_r0;
  double        m_tr;
  double        m_r0Err2;
  double        m_trErr2;

  int           m_nbUsedRHits;
  FastVeloHits  m_rHits;
  int           m_missedSensors;
  bool          m_valid;

  FastVeloHits  m_phiHits;
  double m_x0;
  double m_tx;
  double m_y0;
  double m_ty;
  double m_qFactor;
  double m_sumGlobal;
  double m_sinDPhi;
  
  //== Fit cumulative values
  double m_sa2;
  double m_sa2z;
  double m_sa2z2;
  double m_sab;
  double m_sabz;
  double m_sabz2;
  double m_sb2;
  double m_sb2z;
  double m_sb2z2;
  double m_sac;
  double m_sacz;
  double m_sbc;
  double m_sbcz;
};

typedef std::vector<FastVeloTrack> FastVeloTracks;

#endif // FASTVELOTRACK_H
