// $Id: GiGaEventActionDraw.h,v 1.2 2007-01-12 15:36:45 ranjard Exp $ 
#ifndef       GIGA_GiGaEventActionDraw_H
#define       GIGA_GiGaEventActionDraw_H 1 
// include files  
// GiGa
#include "GiGa/GiGaEventActionBase.h"
// forward declaration
//template <class TYPE> class GiGaFactory;

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
  //friend class GiGaFactory<GiGaEventActionDraw>;
  
  //protected:
public:

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
