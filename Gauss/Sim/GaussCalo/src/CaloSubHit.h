// $Id: CaloSubHit.h,v 1.7 2006-01-17 15:52:57 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2004/01/14 13:38:10  ranjard
// v6r0 - fix to be used with Gaudi v14r0
//
// Revision 1.5  2003/12/09 08:28:39  ibelyaev
//  fix for missing != operator for CaloCellID class
//
// Revision 1.4  2003/07/22 19:05:33  ibelyaev
//  improve doxygen documentation
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
#ifndef CALOSIM_CALOSUBHIT_H 
#define CALOSIM_CALOSUBHIT_H 1
// Kernel
#include "Kernel/CaloCellID.h"
// GiGa 
#include "GiGa/GiGaHashMap.h"
#include "GiGa/GiGaUtil.h"
// Include files
#include "GaussTools/GaussHitBase.h"
// Local 
#include "CaloSimHash.h"

/** @class CaloSubHit CaloSubHit.h
 * 
 *  Elementary "sub-hit" for calorimeter devices.
 *  The class represents an list of energy depositions 
 *  from given particle in the given calorimeter cell.
 *
 *  These objects are not expected to be stored directly 
 *  in Geant4 hits collections. 
 *  These objects are used to be collected in objects of type 
 *  <tt>CaloHit</tt> 
 *
 *  @see CaloHit
 *  @see CaloSubHit
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-03
 */
class CaloSubHit : public GaussHitBase 
{
public:
  
  /// type for the hits "Energy" deposition 
  typedef double Energy                     ;  
  /// type for "time" index  
  typedef char   Time                       ;
  // actual type of 
  typedef GiGaHashMap<Time,Energy> TheMap   ;
  // iterator type 
  typedef TheMap::const_iterator   iterator ;

public:
  
  /** Standard constructor
   *  @param cellID  cellID  of the detector cell  
   *  @param trackID trackID of the particle
   */
  CaloSubHit ( const LHCb::CaloCellID& cellID  = LHCb::CaloCellID() , 
               const int         trackID = 0            ) ;
  
  /** copy constructor 
   *  @param right object to be copied 
   */
  CaloSubHit ( const CaloSubHit& right                  ) ;
  
  /// clone method (virtual constructor) 
  virtual CaloSubHit* clone() const ;
  
  /// destructor 
  virtual ~CaloSubHit() ;
  
  void* operator new    ( size_t    ) ;
  void  operator delete ( void *hit ) ;
  
  /// access to cell ID for given hit  
  const LHCb::CaloCellID& cellID() const          { return m_cellID        ; }  
  // set new cell ID for given hit 
  void setCellID ( const LHCb::CaloCellID& cell ) {        m_cellID = cell ; }
  
  /** add energy deposition for given hit (safe method)
   *  Error flags:
   *   - inconsistent track ID :  300 
   *   - inconsistent cell  ID :  301
   * 
   *  @code 
   *
   *  CaloSubHit*        hit   = ... ;
   *  CaloSubHit::Time   time  = ... ;
   *  CaloSubHit::Energy e     = ... ;
   *  const int          track = ... ;
   *  const LHCb::CaloCellID&  cell  = ... ;
   * 
   *  StatusCode sc  = hit->add( track , cell , time , energy ) ;
   *  if( sc.isFailure() ) { .... } 
   *
   *  @endcode 
   * 
   *  @param track  trackID for energy deposition
   *  @param cell   cellID  for the detector cell
   *  @param time   the time of the energy deposition 
   *  @param energy deposition itself 
   *  @return status code 
   */
  StatusCode add ( const int         track , 
                   const LHCb::CaloCellID& cell    , 
                   const Time        time    , 
                   const Energy      energy  )
  {
    if (    trackID () != track   ) { return StatusCode( 300 ) ; }
    if ( !( cellID  () == cell  ) ) { return StatusCode( 301 ) ; }    
    return add( time , energy );
  };
  
  /** add energy deposition for given hit (fast method)
   * 
   *  @code 
   *
   *  CaloSubHit*        hit  = ... ;
   *  CaloSubHit::Time   time = ... ;
   *  CaloSubHit::Energy e    = ... ;
   * 
   *  hit->add( time , energy ) ;
   *
   *  @endcode 
   * 
   *  @param track  trackID for energy deposition
   *  @param cell   cellID  for the detector cell
   *  @param time   the time of the energy deposition 
   *  @param energy deposition itself 
   *  @return status code 
   */
  StatusCode add ( const Time   time   , 
                   const Energy energy )
  {
    m_map( time ) += energy ;
    return StatusCode::SUCCESS ;
  };
  
  /** access for map iterator "begin" (const)
   *  
   *  @code 
   *  
   *  CaloSubHit* hit = ... ;
   *
   *  for( CaloSubHit::iterator entry = hit->begin() ; 
   *       hit->end() != entry ; ++entry ) 
   *    {
   *       CaloSubHit::Energy e = entry->first   ;
   *       CaloSubHit::Time   t = entry->second  ; 
   *    }
   *  
   *   @endcode 
   *   @return 'begin' iterator to the sequence of entries 
   */
  iterator begin   () const { return m_map.begin () ; }
  /** access for map iterator "end"   (const)
   *  
   *  @code 
   *  
   *  CaloSubHit* hit = ... ;
   *
   *  for( CaloSubHit::iterator entry = hit->begin() ; 
   *       hit->end() != entry ; ++entry ) 
   *    {
   *       CaloSubHit::Energy e = entry->first   ;
   *       CaloSubHit::Time   t = entry->second  ; 
   *    }
   *  
   *   @endcode 
   *   @return 'end' iterator to teh sequence of entries 
   */
  iterator end     () const { return m_map.end   () ; }
  
  /// number of entries/map size 
  size_t   size    () const { return m_map.size  () ; }
  size_t   entries () const { return       size  () ; }
  
  /** overall subhit evergy  (integrated over the time )
   * 
   *  @code 
   *  
   *  CaloSubHit* hit = ... ;
   *  CaloSubHit::Energy e = hit->energy() ;
   * 
   *  @endcode 
   *  @return total subhit energy (total energy deposition from given particle 
   *          in the given calorimeter cell)
   */
  Energy   energy  () const 
  {
    Energy e = 0 ;
    for( iterator entry = begin() ; end() != entry ; ++entry ) 
      { e += entry->second ; } 
    return e ;
  };
  
private:
  
  LHCb::CaloCellID  m_cellID ;
  TheMap      m_map    ;
  
};
// ============================================================================

// ============================================================================
/** @fn  caloSubHit
 *  Fast cast of G4VHit interface to concrete Gauss implementation
 *  @param  g4   pointer to G4VHit interface 
 *  @return cast (dynamic or static) to CaloSubHit
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline CaloSubHit* caloSubHit( G4VHit* g4 )
{
  GiGaUtil::FastCast<G4VHit,CaloSubHit> cast ;
  return cast( g4 );  
}
// ============================================================================

// ============================================================================
/** @fn  caloSubHit
 *  Fast cast of GaussHitBase interface to concrete Gauss implementation
 *  @param  g4   pointer to GaussHitBase interface 
 *  @return cast (dynamic or static) to CaloSubHit
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline CaloSubHit* caloSubHit( GaussHitBase* g4 )
{
  GiGaUtil::FastCast<GaussHitBase,CaloSubHit> cast ;
  return cast( g4 );  
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOSIM_CALOSUBHIT_H
// ============================================================================
