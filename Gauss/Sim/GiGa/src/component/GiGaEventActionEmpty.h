// $Id: GiGaEventActionEmpty.h,v 1.7 2002-05-07 12:21:34 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef       GIGA_GiGaEventActionEmpty_H
#define       GIGA_GiGaEventActionEmpty_H 1 
// ============================================================================
// include files  
// GiGa
#include "GiGa/GiGaEventActionBase.h"
// forward declaration
template <class TYPE> class GiGaFactory;

/** @class GiGaEventActionEmpty GiGaEventActionEmpty.h 
 *    
 *  Example of "primitive" implementation of Event Action class
 *  It is just empty!
 * 
 *  @author  Vanya Belyaev
 *  @date    17/03/2001
*/

class GiGaEventActionEmpty: public GiGaEventActionBase
{
  /// friend factory for instantiatioin 
  friend class GiGaFactory<GiGaEventActionEmpty>;
  
protected:

 /** standard constructor 
   *  @see GiGaEventActionBase
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaEventActionEmpty
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  ///  destructor (virtual and protected)
  virtual ~GiGaEventActionEmpty();

public:
  
  virtual void BeginOfEventAction ( const G4Event* );
  virtual void EndOfEventAction   ( const G4Event* );
  /// 
private:
  ///
  GiGaEventActionEmpty() ; ///< no default constructor
  GiGaEventActionEmpty( const GiGaEventActionEmpty& ) ; ///< no copy  
  GiGaEventActionEmpty& operator=( const GiGaEventActionEmpty& ) ; ///< no = 
  ///
private:
  ///
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///<  GIGA_GiGaEventActionEmpty_H
// ============================================================================
