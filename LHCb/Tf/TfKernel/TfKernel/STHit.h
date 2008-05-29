
//-----------------------------------------------------------------------------
/** @file STHit.h
 *
 *  Header file for track finding class Tf::STHit
 *
 *  CVS Log :-
 *  $Id: STHit.h,v 1.5 2008-05-29 11:53:56 cattanem Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-05-30
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_STHIT_H 
#define TFKERNEL_STHIT_H 1

// TfKernel
#include "TfKernel/LineHit.h"

// LoKi
#include "LoKi/Range.h"

namespace Tf
{

  /** @class STHit
   *
   *  Representation of an IT or TT hit
   *
   *  @authors S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   */

  class STHit : public LineHit
  {

  public:

    typedef DeSTSector DetectorElementType; ///< Detector element type for ST
    typedef st_hit_tag        hit_type_tag; ///< The hit type tag

  public:

    /** Constructor from a DeSTSector and a STLiteCluster
     *  @param[in] aSector Reference to the associated DeSTSector
     *  @param[in] clus    The raw ST hit (STLiteCluster)
     */
    STHit::STHit( const DeSTSector& aSector, const LHCb::STLiteCluster& clus ) ;

  public: // Simple accessors to internal data members
   
    /** Access the cluster size */
    inline int                  size() const { return m_cluster.pseudoSize() ; }

    /** XXX???XXX Not sure what this is ?? */
    inline double               frac() const { return m_cluster.interStripFraction() ; }

    /** Access the raw STLiteCluster cluster for this hit */
    inline const LHCb::STLiteCluster& cluster() const { return m_cluster; }

    /** Access the associated DeSTSector for this hit */
    inline const DeSTSector&    sector() const { return *m_sector; }

  private:

    const DeSTSector*     m_sector;   ///< Pointer to the associated DeSTSector
    LHCb::STLiteCluster   m_cluster;  ///< The raw STLiteCluster for this hit

  };

  /// Type for container for OTHit
  typedef std::vector<const STHit* > STHits;
  /// Type for range of STHits within a container
  typedef LoKi::Range_<STHits> STHitRange ;
  /// Type for a container for STHitRange
  typedef std::vector<STHitRange> STHitRanges;

  //----------------------------------------------------------------------------
  // Inline function definitions
  //----------------------------------------------------------------------------
  inline STHit::STHit( const DeSTSector& aSector, const LHCb::STLiteCluster& clus )
    : LineHit   ( aSector, clus ),
      m_sector  ( &aSector      ),
      m_cluster ( clus          ) { }

  // our dynamic casts
  // Definition is in the HitBase class
  inline const STHit* HitBase::sthit() const
  {
    return ( type()==RegionID::IT || type()==RegionID::TT ? static_cast<const STHit*>(this) : NULL ) ;
  }

} // Tf namespace

#endif // TFKERNEL_STHIT_H
