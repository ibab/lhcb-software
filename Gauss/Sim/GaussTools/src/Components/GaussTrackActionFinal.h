// $Id: GaussTrackActionFinal.h,v 1.3 2007-01-12 15:36:43 ranjard Exp $
#ifndef GAUSSTOOLS_GAUSSTRACKACTIONFINAL_H 
#define GAUSSTOOLS_GAUSSTRACKACTIONFINAL_H 1

#include "GaussTools/GaussTrackActionBase.h"

/** @class GaussTrackActionFinal GaussTrackActionFinal.h 
 *  
 *  Track action, which MUST be 'final' in the sequence 
 *  It makes the final decision 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-19
 */
class GaussTrackActionFinal : public GaussTrackActionBase 
{
  /// friend factory for instantiation 
  //  friend class GiGaFactory<GaussTrackActionFinal> ;
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
  GaussTrackActionFinal
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GaussTrackActionFinal();
private:
  
  // default constructor   is disabled 
  GaussTrackActionFinal() ; 
  // copy    constructor   is disabled 
  GaussTrackActionFinal           ( const GaussTrackActionFinal& ) ; 
  // assignement operator  is disabled 
  GaussTrackActionFinal& operator=( const GaussTrackActionFinal& ) ;
  
protected:
  
  bool storeMarked    () const { return m_storeMarked   ; }
  bool storeWithHits  () const { return m_storeWithHits ; }
  
private:
  
  bool m_storeMarked   ;
  bool m_storeWithHits ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // GAUSSTOOLS_GAUSSTRACKACTIONFINAL_H
// ============================================================================
