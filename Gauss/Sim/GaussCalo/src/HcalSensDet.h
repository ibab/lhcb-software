// $Id: HcalSensDet.h,v 1.2 2003-07-07 16:27:46 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/07/07 08:21:12  ibelyaev
//  split the general CaloSensDet class
//
// ============================================================================
#ifndef       GAUSSCALO_HcalSensDet_H
#define       GAUSSCALO_HcalSensDet_H 1 
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

/** @class HcalSensDet HcalSensDet.h
 *
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Patrick Robbe robbe@lal.in2p3.fr 
 *
 *  @date    23/01/2001
 */

class HcalSensDet: public EHCalSensDet 
{
  /// friend factory 
  friend class GiGaFactory<HcalSensDet>;
  
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
  HcalSensDet
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~HcalSensDet() {};
  
private:
  
  // no default constructor 
  HcalSensDet() ; 
  // no copy constructor
  HcalSensDet           ( const HcalSensDet& ); 
  // no assignement  
  HcalSensDet& operator=( const HcalSensDet& ) ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///< GAUSSCALO_HcalSensDet_H
// ============================================================================
