// $Id: GaussTrackActionGeneral.h,v 1.2 2007-01-12 15:36:44 ranjard Exp $
#ifndef GAUSSTOOLS_GAUSSTRACKACTIONGENERAL_H 
#define GAUSSTOOLS_GAUSSTRACKACTIONGENERAL_H 1

// local
#include "GaussTools/GaussTrackActionZ.h"


/** @class GaussTrackActionGeenral GaussTrackActionGeneral.h 
 *  
 *  Track action, valid for certain region in Z based on 
 *  general properties of the track/particle
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-19
 */
class GaussTrackActionGeneral : virtual public GaussTrackActionZ 
{
  /// friend factory for instantiation 
  //friend class GiGaFactory<GaussTrackActionGeneral> ;
public:
  
  /** perform action 
   *  @see G4VUserTrackingAction
   *  @param pointer to new track opbject 
   */
  virtual void PreUserTrackingAction  ( const G4Track* ) ;
  
  /** perform action 
   *  @see G4VUserTrackingAction
   *  @param pointer to new track opbject 
   */
  virtual void PostUserTrackingAction ( const G4Track* ) ;
  
  //protected:
  
  /** standard constructor 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GaussTrackActionGeneral
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GaussTrackActionGeneral();
  
private:
  
  // default constructor   is disabled 
  GaussTrackActionGeneral() ; 
  // copy    constructor   is disabled 
  GaussTrackActionGeneral           ( const GaussTrackActionGeneral& ) ; 
  // assignement operator  is disabled 
  GaussTrackActionGeneral& operator=( const GaussTrackActionGeneral& ) ;
  
protected:
  
  bool storeAll       () const { return m_storeAll        ; }
  bool storePrimaries () const { return m_storePrimaries  ; }
  bool storeDecays    () const { return m_storeDecays     ; }
  bool storePreDecays () const { return m_storePreDecays  ; }
  
private:
  
  bool m_storeAll       ;
  bool m_storePrimaries ;
  bool m_storeDecays    ;  
  bool m_storePreDecays ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // GAUSSTOOLS_GAUSSTRACKACTIONGENERAL_H
// ============================================================================
