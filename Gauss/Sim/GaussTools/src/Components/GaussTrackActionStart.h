// $Id: GaussTrackActionStart.h,v 1.1 2004-02-20 19:35:28 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GAUSSTOOLS_GAUSSTRACKACTIONSTART_H 
#define GAUSSTOOLS_GAUSSTRACKACTIONSTART_H 1
// ============================================================================
// GaussTools
// ============================================================================
#include "GaussTools/GaussTrackActionBase.h"
// ============================================================================

/** @class GaussTrackActionStart GaussTrackActionStart.h 
 *  
 *  Track action, which MUST be 'final' in the sequence 
 *  It makes the final decision 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-19
 */
class GaussTrackActionStart : public GaussTrackActionBase 
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GaussTrackActionStart> ;
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
  
protected:
  
  /** standard constructor 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GaussTrackActionStart
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GaussTrackActionStart();
private:
  
  // default constructor   is disabled 
  GaussTrackActionStart() ; 
  // copy    constructor   is disabled 
  GaussTrackActionStart           ( const GaussTrackActionStart& ) ; 
  // assignement operator  is disabled 
  GaussTrackActionStart& operator=( const GaussTrackActionStart& ) ;
  
private:
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // GAUSSTOOLS_GAUSSTRACKACTIONSTART_H
// ============================================================================
