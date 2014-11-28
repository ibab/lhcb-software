// $Id: CaloHit.h,v 1.7 2006-01-17 15:52:57 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2004/01/14 13:38:10  ranjard
// v6r0 - fix to be used with Gaudi v14r0
//
// Revision 1.5  2003/07/22 19:05:29  ibelyaev
//  improve doxygen documentation
//
// Revision 1.4  2003/07/10 11:27:56  ibelyaev
//  fixes to please the gcc3.2 compiler
//
// Revision 1.3  2003/07/08 19:40:57  ibelyaev
//  Sensitive Plane Detector + improved printout
//
// Revision 1.2  2002/12/13 16:52:57  ibelyaev
//  put updated versions of the packages
//
// Revision 1.2  2002/12/07 21:19:14  ibelyaev
//  few optimization updates
//
// Revision 1.1  2002/12/07 14:41:44  ibelyaev
//  add new Calo stuff
// 
// ============================================================================
#ifndef CALOSIM_CALOHIT_H 
#define CALOSIM_CALOHIT_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/StatusCode.h" 
// Kernel
#include "Kernel/CaloCellID.h"
// GiGa 
#include "GiGa/GiGaUtil.h"
// G4 
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
// local 
#include "CaloSim.h"
#include "CaloSubHit.h"
#include "CaloSimHash.h"
// forward decalration
template <class T> class G4THitsCollection;

/** @class CaloHit CaloHit.h
 * 
 *  Elementary "hit" for calorimeter devices.
 *  The class represents an list of energy depositions 
 *  from all MC particles for given calorimeter cell 
 *
 *  The new created hit (of type <tt>CaloHit</tt>
 *  will be attributed to the parent particle of the track 
 *  with 2 exceptions
 *   - if the track is already forced to be stored 
 *   - if the track is produces outside the "forbidden" zone 
 * 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-03
 */

class CaloHit : public G4VHit
{
public:
  
  /// the actual data type for particle 
  typedef int                               TrackID  ;
  /// collection of sub-hits 
  typedef GiGaHashMap<TrackID,CaloSubHit*>  TheMap   ;
  /// actual type of the iterator 
  typedef TheMap::const_iterator            iterator ;
  
public:
  
  /** standard  constructor 
   *  @param cell ID of calorimeter cell 
   */
  CaloHit( const LHCb::CaloCellID& cell = LHCb::CaloCellID() ) ;

  /** copy constructor 
   *  @param right object to be copied 
   */
  CaloHit( const CaloHit& right );
  
  /// clone method (virtual constructor)  
  virtual CaloHit* clone() const ;
  
  /// destructor / delete all subhits 
  virtual ~CaloHit() ;
  
  void* operator  new    ( size_t    ) ;
  void  operator  delete ( void *hit ) ;
  
  /// get the cellID 
  const LHCb::CaloCellID& cellID () const          { return m_cellID        ; }
  // set new cellID 
  void  setCellID ( const LHCb::CaloCellID& cell ) {        m_cellID = cell ; }
  
  /** get the hit for the given particle 
   * 
   *  @code 
   * 
   *  CaloHit*                hit   = ... ;
   *  const CaloHit::TrackID  track = ... ;
   *  
   *  CaloSubHit*& subhit = hit->hit( track ) ;
   *  if ( 0 == subhit ) 
   *        { subhit = new CaloSubHit( hit->cellID() , track ) ; } 
   *  
   *  @endcode
   *
   *  @return the sub-hit for the given track id (or NULL) 
   */
  CaloSubHit*& hit( const TrackID track )    { return m_map( track ) ; }
  
  /** access to 'begin' iterator of the sub-hit collections (const)
   *  
   *  @code
   *
   *  CaloHit* hit = ... ;
   *  
   *  for ( CaloHit::iterator entry = hit->begin() ; 
   *        hit->end() != entry ; ++entry  ) 
   *       {
   *         const CaloHit::TrackID track  = entry->first  ; 
   *         const CaloSubHit*      subhit = entry->second ; 
   *       }
   *
   *  @endcode
   *  
   *  @return 'begin' iterator for sequence of subhits
   */
  iterator begin     () const { return m_map.begin () ; }
  /** access to 'end'   iterator of the sub-hit collections (const)
   *  
   *  @code
   *
   *  CaloHit* hit = ... ;
   *  
   *  for ( CaloHit::iterator entry = hit->begin() ; 
   *        hit->end() != entry ; ++entry  ) 
   *       {
   *         const CaloHit::TrackID track  = entry->first  ; 
   *         const CaloSubHit*      subhit = entry->second ; 
   *       }
   *
   *  @endcode
   *  
   *  @return 'end' iterator for sequence of subhits
   */
  iterator end       () const { return m_map.end   () ; }
  
  /// number of entries/map size 
  size_t   size      () const { return m_map.size  () ; }
  size_t   entries   () const { return       size  () ; }
  
  /// total number of (sub)entries 
  size_t   totalSize () const 
  {
    size_t size = 0 ;
    for( iterator hit = begin() ; end() != hit ; ++hit ) 
      { if( 0 != hit->second ) { size += hit->second->size() ; } }
    return size ;
  };
  /** the total energy (integrated over all time slots  and all particles)
   *  
   *  @code
   * 
   *  const CaloHit*           hit = ... ;
   *  const CaloSubHit::Energy e   = hit->energy() ;
   *
   *  @endcode 
   *  
   *  @return the total energy (integrated over all time 
   *                             slots  and all particles)
   */
  CaloSubHit::Energy energy () const 
  {
    CaloSubHit::Energy e = 0 ;
    for( iterator entry = begin() ; end() != entry ; ++entry ) 
      {
        const CaloSubHit* hit = entry -> second  ;
        if( 0 != hit ) { e += hit->energy() ; } 
      }
    return e ;
  }
  
private:
  
  LHCb::CaloCellID  m_cellID ;
  TheMap      m_map    ;
};

/// type for the hit collection 
typedef G4THitsCollection<CaloHit> CaloHitsCollection ;

// ============================================================================
/** @fn  caloHits
 *  Fast cast of G4VHitsCollection interface to concrete Gauss implementation
 *  @param  g4   pointer to G4VHitsCollection interface 
 *  @return cast (dynamic or static) to CaloHitsColelction*
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline CaloHitsCollection* caloHits( G4VHitsCollection* g4 )
{
  GiGaUtil::FastCast<G4VHitsCollection,CaloHitsCollection> cast ;
  return cast( g4 );  
}

// ============================================================================
/** @fn  caloHit
 *  Fast cast of G4VHit interface to concrete Gauss implementation
 *  @param  g4   pointer to G4VHit interface 
 *  @return cast (dynamic or static) to CaloHit
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline CaloHit* caloHit( G4VHit* g4 )
{
  GiGaUtil::FastCast<G4VHit,CaloHit> cast ;
  return cast( g4 );  
}


// ============================================================================
// The END 
// ============================================================================
#endif // CALOSIM_CALOHIT_H
// ============================================================================
