/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef       GIGA_GiGaEventActionDraw_H
#define       GIGA_GiGaEventActionDraw_H 1 
/// ===========================================================================


#include "GiGa/GiGaEventActionBase.h"

template <class EA> class GiGaEventActionFactory;

/** @class GiGaEventActionDraw GiGaEventActionDraw.h
 *    
 *  Example of "primitive" implementation of Event Action class
 *  It is just Draw!
 *   
 *  @author  Vanya Belyaev
 *  @date    17/03/2001
 */

class GiGaEventActionDraw: virtual public GiGaEventActionBase
{
  ///
  friend class GiGaEventActionFactory<GiGaEventActionDraw>;
  ///
 protected:
  ///
  GiGaEventActionDraw( const std::string& name , ISvcLocator* Loc );
  ///  
  virtual ~GiGaEventActionDraw();
  ////
 public: 
  ///
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;
  ///
  virtual void EndOfEventAction   ( const G4Event* event );
  /// 
 private:
  ///
  GiGaEventActionDraw(); ///< no default constructor
  GiGaEventActionDraw( const GiGaEventActionDraw& ); ///< no copy  
  GiGaEventActionDraw& operator=( const GiGaEventActionDraw& ) ; ///< no = 
  ///
 private:
  ///
};

/// ===========================================================================
#endif  //    GIGA_GiGaEventActionDraw_H
/// ===========================================================================
