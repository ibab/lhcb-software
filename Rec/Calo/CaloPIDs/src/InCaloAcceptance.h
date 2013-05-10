// ============================================================================
#ifndef CALOPIDS_INCALOACCEPTANCE_H 
#define CALOPIDS_INCALOACCEPTANCE_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
#include "CaloTrackTool.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/IInAcceptance.h"   
// ============================================================================
// TrackInterfaces 
// ============================================================================
#include "TrackInterfaces/ITrackExtrapolator.h"
// ============================================================================

/** @class InCaloAcceptance InCaloAcceptance.h
 *  
 *  The general tool to determine if the reconstructed charged track 
 *  "is in Calorimeter acceptance"
 *
 *  @author Victor EGORYCHEV Victor.Egorychev@cern.ch
 *  @author Vanya  BELYAEV    ibelyaev@phsycis.syr.edu
 *  @date   2006-05-28
 */
class InCaloAcceptance 
  : public virtual IInAcceptance 
  , public Calo::CaloTrackTool
{
  /// friend factory for instantiation 
  friend class ToolFactory<InCaloAcceptance> ; 
public:
  /// initialization @see IAlgTool 
  virtual StatusCode initialize() ; 
  /** check the track is in acceptance of given calorimeter 
   *  @see IInAcceptance
   *  @param  track track to be checked
   *  @return true if the track is in acceptance
   */
  virtual bool inAcceptance ( const LHCb::Track* track ) const ;
  //  
protected:
  /// check if the point "is in acceptance"
  inline bool ok ( const Gaudi::XYZPoint& point ) const ;
  /// get the plane 
  const Gaudi::Plane3D& plane() const { return m_plane ; }
protected:
  /// Standard constructor
  InCaloAcceptance 
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  /// virtual destructor 
  virtual ~InCaloAcceptance( ){};
private:
  InCaloAcceptance() ;
  InCaloAcceptance            ( const InCaloAcceptance& ) ;
  InCaloAcceptance& operator= ( const InCaloAcceptance& ) ;  
private:
  /// use fiducial volume or real volume ?
  bool                  m_fiducial ;
  LHCb::State::Location m_loc   ;
  ///
  Gaudi::Plane3D        m_plane    ;
  /// Local storage 
  mutable LHCb::State   m_state ;
} ;
// ============================================================================
/// check if the point "is in acceptance"
// ============================================================================
inline bool InCaloAcceptance::ok ( const Gaudi::XYZPoint& point ) const 
{
  const CellParam* cell = calo()->Cell_( point ) ;
  if ( 0 == cell || !cell->valid() ) { return false ; }    // RETURN
  // check for fiducial volume ?
  if ( !m_fiducial )                 { return true  ; }    // RETURN
  // check for neighbors 
  const CaloNeighbors& neighbors = cell->neighbors() ;
  // regular cell: >= 8 valid neighbours 
  if ( 8 <= neighbors.size() )       { return true  ; }    // RETURN
  // incomplete neibors: border of 2 zones ?
  const unsigned int area = cell->cellID().area() ;
  for ( CaloNeighbors::const_iterator inei = neighbors.begin() + 1;
        neighbors.end() != inei ; ++inei )
  { if ( area != inei->area() )      { return true  ; } }  // RETURN 
  // finally 
  return false;
}
// ===========================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOPIDS_INCALOACCEPTANCE_H
// ============================================================================
