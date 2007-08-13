#ifndef STHIT_H 
#define STHIT_H 1

// Include files
#include "TfKernel/LineHit.h"
#include "LoKi/Range.h"

namespace Tf {

  /** @class STHit
   *  Store a IT & TT  coordinate
   *
   *  @authors S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   */

  class STHit : public LineHit {
  public:
    typedef DeSTSector DetectorElementType ;
    
    typedef st_hit_tag hit_type_tag; ///< the hit type tag

    // Constructor
    STHit::STHit( const DeSTSector& aSector, const LHCb::STLiteCluster& clus ) ;


    //== Simple accessors to internal data members
    int                  size() const { return m_cluster.pseudoSize() ; }
    double               frac() const { return m_cluster.interStripFraction() ; }
    const LHCb::STLiteCluster& cluster() const { return m_cluster; }
    const DeSTSector&    sector() const { return *m_sector; }

  private:
    const DeSTSector*          m_sector;
    LHCb::STLiteCluster        m_cluster;
  };

  /// Type for container for OTHit
  typedef std::vector<const STHit* > STHits;
  /// Type for range of STHits within a container 
  typedef LoKi::Range_<STHits> STHitRange ;
  /// Type for a container for STHitRange
  typedef std::vector<STHitRange> STHitRanges;

  ////////////////////////////////////////////////////////////////////////////////////
  // Inline function definitions
  ////////////////////////////////////////////////////////////////////////////////////

  inline STHit::STHit( const DeSTSector& aSector, const LHCb::STLiteCluster& clus )
    :  LineHit( aSector, clus),
       m_sector(&aSector),m_cluster(clus)
  { 
  }

  // our dynamic casts
  inline const STHit* HitBase::sthit() const { return type()==RegionID::IT || type()==RegionID::TT ? static_cast<const STHit*>(this) : 0 ; }

}

#endif // STHit_H
