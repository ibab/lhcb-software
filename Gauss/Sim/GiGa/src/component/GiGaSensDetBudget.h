// $Id: GiGaSensDetBudget.h,v 1.1 2002-05-04 20:20:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GIGA_GIGASENSDETBUDGET_H 
#define GIGA_GIGASENSDETBUDGET_H 1
// Include files
// GiGa 
#include "GiGa/GiGaSensDetBase.h"
#include "GiGa/GiGaMap.h"
#include "GiGa/GiGaHashMap.h"

/** @class GiGaSensDetBudget GiGaSensDetBudget.h 
 *  
 *  Sensitive Detector, which is able to calculate the material budget 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   04/05/2002
 */
class GiGaSensDetBudget : virtual public GiGaSensDetBase 
{
  /// friend factory for instantiation
  friend class GiGaSensDetFactory<GiGaSensDetBudget>;

public:
  
  /** finalize the sensitive detector 
   *  @see GiGaSensDetBase 
   *  @see GiGaBase 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
  /** process the hit
   *  @param step     pointer to current Geant4 step 
   *  @param history  pointert to touchable history 
   */
  virtual bool ProcessHits
  ( G4Step* step                , 
    G4TouchableHistory* history ) ;
  
protected:
  
  /** Standard constructor
   *  @see GiGaSensDetBase
   *  @see GiGaBase
   *  @param name   name of the sensitive detector
   *  @param svcloc pointer to servcie locator 
   */
  GiGaSensDetBudget
  ( const std::string& name   , 
    ISvcLocator*       svcloc );
  
  
  /// destructor (virtual and protected)
  virtual ~GiGaSensDetBudget() ;
  
private:
  
  ///  default  constructor  is  private
  GiGaSensDetBudget() ;
  ///  copy     constructor  is  private
  GiGaSensDetBudget
  ( const GiGaSensDetBudget& );
  ///  assignement operator  is private
  GiGaSensDetBudget& operator=
  ( const GiGaSensDetBudget& );
  
private:
  
  // type for budget counter 
  typedef GiGaHashMap<std::string,double> Budget;
  
  // material budget per Logical Volumes  
  Budget m_budgetL ;
  
  // material budget per Physical Volumes  
  Budget m_budgetP ;
  
  // material budget per Materials   
  Budget m_budgetM ;
  
  // flag  for accumulation per logical  volumes 
  bool   m_lvolume  ;
  // flag  for accumulation per physical volumes 
  bool   m_pvolume  ;
  // flag  for accumulation per materials 
  bool   m_material ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // COMPONENT_GIGASENSDETBUDGET_H
// ============================================================================
