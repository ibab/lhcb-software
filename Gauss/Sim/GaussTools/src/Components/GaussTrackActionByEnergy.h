// $Id: GaussTrackActionByEnergy.h,v 1.3 2007-01-12 15:36:42 ranjard Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/04/05 13:18:35  gcorti
// do not save photoelectrons by default
//
// Revision 1.1  2004/02/20 19:35:26  ibelyaev
//  major update
// 
// ============================================================================
#ifndef GAUSSTOOLS_GAUSSTRACKACTIONBYENERGY_H 
#define GAUSSTOOLS_GAUSSTRACKACTIONBYENERGY_H 1
// ============================================================================
// local
// ============================================================================
#include "GaussTools/GaussTrackActionZ.h"
// ============================================================================

/** @class GaussTrackActionByEnergy GaussTrackActionByEnergy.h 
 *  
 *  Track action, valid for certain region in Z based on 
 *  particle/children types 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-19
 */
class GaussTrackActionByEnergy : public GaussTrackActionZ 
{
  /// friend factory for instantiation 
  //  friend class GiGaFactory<GaussTrackActionByEnergy> ;
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
  GaussTrackActionByEnergy
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GaussTrackActionByEnergy();
  
private:
  
  // default constructor   is disabled 
  GaussTrackActionByEnergy() ; 
  // copy    constructor   is disabled 
  GaussTrackActionByEnergy           ( const GaussTrackActionByEnergy& ) ; 
  // assignement operator  is disabled 
  GaussTrackActionByEnergy& operator=( const GaussTrackActionByEnergy& ) ;
  
protected:
  
  bool   storeByEnergy () const { return m_storeByEnergy ; }
  double threshold     () const { return m_threshold     ; }
  
private:
  
  bool   m_storeByEnergy ;
  double m_threshold     ;
  bool   m_rejectRICHphe ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // GAUSSTOOLS_GAUSSTRACKACTIONBYENERGY_H
// ============================================================================
