// $Id: CaloSubHit.h,v 1.2 2002-12-13 16:52:57 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
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
 *  The class represents an list of energy depositios 
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
  CaloSubHit( const CaloCellID& cellID  = CaloCellID() , 
              const int         trackID = 0            ) ;
  
  /** copy constructor 
   *  @param right object to be copied 
   */
  CaloSubHit( const CaloSubHit& right                  ) ;
  
  /// clone method (virtual constructor) 
  virtual CaloSubHit* clone() const ;
  
  /// destructor 
  virtual ~CaloSubHit() ;
  
  void* operator new    ( size_t    ) ;
  void  operator delete ( void *hit ) ;
  
  /// access to cell ID for given hit  
  const CaloCellID& cellID() const         { return m_cellID        ; }  
  // set new cell ID for given hit 
  void setCellID( const CaloCellID& cell ) {        m_cellID = cell ; }
  
  /** add energy deposition for given hit (safe method)
   *  Error flags:
   *   - inconsistent track ID :  300 
   *   - inconsistent cell  ID :  301
   * 
   *  @param track  trackID for energy deposition
   *  @param cell   cellID  for the detector cell
   *  @param time   the time of the energy deposition 
   *  @param energy deposition itself 
   *  @return status code 
   */
  StatusCode add ( const int         track , 
                   const CaloCellID& cell    , 
                   const Time        time    , 
                   const Energy      energy  )
  {
    if( trackID () != track ) { return StatusCode( 300 ) ; }
    if( cellID  () != cell  ) { return StatusCode( 301 ) ; }    
    return add( time , energy );
  };
  
  /** add energy deposition for given hit (fast method)
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
  
  /// access for map iterator "begin" (const)
  iterator begin  () const { return m_map.begin () ; }
  /// access for map iterator "end"   (const)
  iterator end    () const { return m_map.end   () ; }
  
  /// number of entries/map size 
  size_t   size   () const { return m_map.size  () ; }

private:
  
  CaloCellID  m_cellID ;
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
};
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
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOSIM_CALOSUBHIT_H
// ============================================================================
