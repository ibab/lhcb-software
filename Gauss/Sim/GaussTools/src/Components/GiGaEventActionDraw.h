// $Id: GiGaEventActionDraw.h,v 1.1 2002-12-12 15:19:30 witoldp Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/09/26 18:10:51  ibelyaev
//  repackageing: add all concrete implementations from GiGa
//
// Revision 1.6  2002/05/07 12:21:34  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
#ifndef       GIGA_GiGaEventActionDraw_H
#define       GIGA_GiGaEventActionDraw_H 1 
// ============================================================================
// include files  
// GiGa
#include "GiGa/GiGaEventActionBase.h"
// forward declaration
template <class TYPE> class GiGaFactory;

/** @class GiGaEventActionDraw GiGaEventActionDraw.h
 *    
 *  Example of "primitive" implementation of Event Action class
 *  It is just Draw!
 *   
 *  @author  Vanya Belyaev
 *  @date    17/03/2001
 */

class GiGaEventActionDraw: public GiGaEventActionBase
{
  /// frined factory for instantiatio 
  friend class GiGaFactory<GiGaEventActionDraw>;
  
protected:
  
  /** standard constructor 
   *  @see GiGaEventActionBase
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaEventActionDraw
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// desctrustor (virtual and protected)  
  virtual ~GiGaEventActionDraw();
  
public: 
  
  /// G4
  virtual void EndOfEventAction   ( const G4Event* event );
  
private:
  
  GiGaEventActionDraw(); ///< no default constructor
  GiGaEventActionDraw( const GiGaEventActionDraw& ); ///< no copy  
  GiGaEventActionDraw& operator=( const GiGaEventActionDraw& ) ; ///< no = 
  
private:
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  //    GIGA_GiGaEventActionDraw_H
// ============================================================================
