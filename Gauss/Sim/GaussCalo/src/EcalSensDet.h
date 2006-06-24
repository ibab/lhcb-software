// $Id: EcalSensDet.h,v 1.7 2006-06-24 16:23:45 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2006/01/17 15:52:57  odescham
// v8r0 - Adapt to new Event Model & LHCb v20 migration
//
// Revision 1.5  2004/01/14 13:38:10  ranjard
// v6r0 - fix to be used with Gaudi v14r0
//
// Revision 1.4  2003/12/05 08:59:58  robbep
// Put fonctions and constants for the non-uniformity. The local
// non-uniformity (sin-like amplitude) is set to 0 for the moment.
//
// Revision 1.3  2003/07/08 10:22:50  robbep
// Adaptation to the new GaussCalo interface
//
// Revision 1.2  2003/07/07 16:27:46  ibelyaev
//  substitupe G4Material with G4MaterialCutsCouple
//
// Revision 1.1  2003/07/07 08:21:10  ibelyaev
//  split the general CaloSensDet class
//
// ============================================================================
#ifndef       GAUSSCALO_EcalSensDet_H
#define       GAUSSCALO_EcalSensDet_H 1 
// ============================================================================
// GaudiKernel
/// Ntupel Svc 
#include "GaudiKernel/INTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
/// GiGa 
#include "GiGa/GiGaSensDetBase.h"
#include "GiGa/GiGaHashMap.h"
/// local
#include "CaloSim.h"
#include "CaloHit.h"
#include "EHCalSensDet.h"
/// forward declarations 
template <class SD> class GiGaSensDetFactory;

class DeCalorimeter ;

/** @class EcalSensDet EcalSensDet.h
 *
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Patrick Robbe robbe@lal.in2p3.fr 
 *
 *  @date    23/01/2001
 */

class EcalSensDet: public EHCalSensDet 
{
  /// friend factory 
  friend class GiGaFactory<EcalSensDet>;
  
protected:
  
  /** fill the hit with the concrete information about the energy and the time.
   *  The function is to be called from ProcessHits method.
   *
   *  @param hit        hit  to be filled with the correct information, 
   *                    the cellID information is accessible from the hit
   *  @param prePoint   PreStepPoint  for the given step 
   *  @param globalTime the global time of prestep point 
   *  @param deposit    raw energy deposition for the step 
   *  @param track      the actual G4Track pbject (decoded from G4Step)
   *  @param pdef       the actual particle type  (decoded from G4Step/G4Track)
   *  @param material   the actual material       (decoded from G4Step) 
   *  @param step       the step itself, the most important 
   *                    information from the step 
   *                    is already decoded into prePoint,globalTime,track,
   *                    particle definition,material 
   *                    etc for efficiency reasons.
   *
   */  
  virtual StatusCode    fillHitInfo 
  ( CaloSubHit*                 hit         ,
    const HepPoint3D&           prePoint    ,
    const double                globalTime  , 
    const double                deposit     ,
    const G4Track*              track       , 
    const G4ParticleDefinition* pdef        ,
    const G4MaterialCutsCouple* material    ,
    const G4Step*               step        ) const ;
  
protected:
  
  /** standard constructor 
   *  @see CaloSensDet 
   *  @see GiGaSensDetBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  EcalSensDet
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~EcalSensDet() {};
  
private:
  
  // no default constructor 
  EcalSensDet() ; 
  // no copy constructor
  EcalSensDet           ( const EcalSensDet& ); 
  // no assignement  
  EcalSensDet& operator=( const EcalSensDet& ) ;
  
protected:
  
  /** Correction due to the local non uniformity due to the light 
   *  collection efficiency in cell cell
   */
  inline double localNonUniformity 
  ( const HepPoint3D& prePoint    , 
    const LHCb::CaloCellID& cell    ) const ;
  
private :
  // Amplitudes of the local non uniformity correction
  // depending on the region of the Ecal
  double                                      m_a_local_inner_ecal  ;
  double                                      m_a_local_middle_ecal ;
  double                                      m_a_local_outer_ecal  ;
  // Amplitudes of the global non uniformity correction
  // depending on the region of the Ecal
  double                                      m_a_global_inner_ecal  ;
  double                                      m_a_global_middle_ecal ;
  double                                      m_a_global_outer_ecal  ;
  // Correction for light reflection at the edges
  double                                      m_a_reflection_height ;
  double                                      m_a_reflection_width  ;
};
// ============================================================================




// ============================================================================
// Local Non Uniformity
// ============================================================================
/** Correction due to the local non uniformity due to the light 
 *  collection efficiency in cell cell
 */

inline double EcalSensDet::localNonUniformity
( const HepPoint3D& prePoint , 
  const LHCb::CaloCellID& cell ) const 
{
  
  // Only for ECal for the moment
  double correction = 1. ;
  
  // Find the position of the step
  double x        = prePoint.x() ;
  double y        = prePoint.y() ;
  // Center of the cell
  double x0       = calo()->cellX( cell ) ;
  double y0       = calo()->cellY( cell ) ;

  // Distance between fibers 
  // and correction amplitude
  double d        = 10.1 * mm ;
  double A_local  = m_a_local_inner_ecal ; // in inner Ecal
  double A_global = m_a_global_inner_ecal ;

  // Cell size
  double cellSize = calo()->cellSize( cell ) ;

  // Assign amplitude of non uniformity as a function of the
  // Ecal region
  
  if ( cell.area() == 0 ) { // outer Ecal
    A_local  = m_a_local_outer_ecal ;
    A_global = m_a_global_outer_ecal ;
    d        = 15.25 * mm ;
  }
  else if ( cell.area() == 1 ) { // middle Ecal
    A_local  = m_a_local_middle_ecal ;
    A_global = m_a_global_middle_ecal ;
  }  
    
  // Local uniformity is product of x and y sine-like functions
  // The Amplitude of the sin-like function is a function of x and 
  // y
  if ( A_local > LHCbMath::lowTolerance ) 
    correction += A_local / 2. * ( 1. - cos( 2.*pi * (x-x0)/d ) ) *
      ( 1. - cos( 2.*pi * (y-y0)/d ) ) ;

  double rX(0.) , rY(0.) , hCell(0.) ;
  
  // Global non uniformity
  if ( A_global > LHCbMath::lowTolerance ) {
    rX = x - x0 ;
    rY = y - y0 ;
    hCell = cellSize / 2. ;
    correction += 
      A_global * ( hCell - rX ) * ( rX + hCell ) / ( hCell * hCell )
      * ( hCell - rY ) * ( rY + hCell ) / ( hCell * hCell ) ;
  }
  
  // Light Reflexion on the edges
  if ( m_a_reflection_height > LHCbMath::lowTolerance ) {
    rX = rX / m_a_reflection_width ;
    rY = rY / m_a_reflection_width ;
    hCell = hCell / m_a_reflection_width ;
    correction += m_a_reflection_height * 
      ( exp( - fabs ( rX + hCell ) ) + exp( - fabs ( rX - hCell ) )  
        + exp( - fabs ( rY + hCell ) ) + exp( - fabs ( rY - hCell ) ) ) ;
  }
  
  return correction ;
};

// ============================================================================
// The END 
// ============================================================================
#endif  ///< GAUSSCALO_EcalSensDet_H
// ============================================================================
