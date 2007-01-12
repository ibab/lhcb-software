// $Id: GiGaSensDetBudget.h,v 1.3 2007-01-12 15:36:55 ranjard Exp $
#ifndef GIGA_GIGASENSDETBUDGET_H 
#define GIGA_GIGASENSDETBUDGET_H 1

// Include files
// from GiGa 
#include "GiGa/GiGaSensDetBase.h"
#include "GiGa/GiGaMap.h"
#include "GiGa/GiGaHashMap.h"

// Forward declaration
//template <class TYPE> class GiGaFactory;

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
  //friend class GiGaFactory<GiGaSensDetBudget>;

public:
  
  /** finalize the sensitive detector 
   *  @see GiGaSensDetBase 
   *  @see GiGaBase 
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
  /** process the hit
   *  @see G4VSensitiveDetector
   *  @param step     pointer to current Geant4 step 
   *  @param history  pointert to touchable history 
   */
  virtual bool ProcessHits
  ( G4Step* step                , 
    G4TouchableHistory* history ) ;
  
  //protected:
  
  /** standard constructor 
   *  @see GiGaSensDetBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaSensDetBudget
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
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

// ============================================================================
// The END 
// ============================================================================
#endif // COMPONENT_GIGASENSDETBUDGET_H
// ============================================================================
