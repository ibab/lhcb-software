// $Id: GiGaTrackActionEmpty.h,v 1.6 2002-05-07 12:21:36 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef       GIGA_GiGaTrackActionEmpty_H
#define       GIGA_GiGaTrackActionEmpty_H 1 
// ============================================================================
// GiGa
#include "GiGa/GiGaTrackActionBase.h"
// forward declaration 
template <class TYPE> class GiGaFactory;

/** @class GiGaTrackActionEmpty GiGaTrackActionEmpty.h 
 *
 *  Example of "primitive" implementation of Tracking action class
 *  It is just empty!
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    23/01/2001
 */

class GiGaTrackActionEmpty: virtual public GiGaTrackActionBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GiGaTrackActionEmpty>;

protected:

  /** standard constructor 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaTrackActionEmpty
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GiGaTrackActionEmpty();
  
public: 
  
  ///  G4 
  virtual void PreUserTrackingAction  ( const G4Track* );
  ///  G4 
  virtual void PostUserTrackingAction ( const G4Track* );
  
private:

  GiGaTrackActionEmpty(); ///< no default constructor
  GiGaTrackActionEmpty( const GiGaTrackActionEmpty& ); ///< no copy  
  GiGaTrackActionEmpty& operator=( const GiGaTrackActionEmpty& ) ; ///< no = 

};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///< GIGA_GiGaTrackActionEmpty_H
// ============================================================================
