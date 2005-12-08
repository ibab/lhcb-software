// $Id: SolidTicks.h,v 1.11 2005-12-08 19:20:01 jpalac Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.10  2005/12/02 18:36:55  jpalac
//
// ! 2005-02-12 - Juan Palacios
//  - Add Kernel/LHCbDefinitions to requirements
//  - Change CLHEP geometry classes to LHCbDefinitions typedefs:
//             * These typedefs point to MathCore classes with the
//               exception of Gaudi::Plane3D, which pointe to HepPoint3D.
//               Awaiting implementation of MathCore Plane3D class.
//  - Make changes to all code to adapt to MathCore syntax
//  - TO-DO: Not compiling due to Plane3D operaitons with MathCore classes
//
// Revision 1.9  2002/05/15 14:25:25  ibelyaev
//  bug fix
//
// Revision 1.8  2002/05/11 18:25:46  ibelyaev
//  see $DETDESCROOT/doc/release.notes 11 May 2002
//
// ===========================================================================
#ifndef     DETDESC_SOLIDTICKS_H
#define     DETDESC_SOLIDTICKS_H 1 
/// STD & STL includes 
#include <cmath>
#include <functional>
#include <algorithm>
// CLHEP includes 
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"
#include "Kernel/Plane3DTypes.h"
// DetDesc includes 
#include "DetDesc/ISolid.h" 
#include "DetDesc/SolidMath.h" 

/** @file 
 *
 *  Collection of technical methods for manipulation of  
 *  ISolid::Tick ans ISolid::Ticks. Also some additional                
 *  mathematical utilities are supplied.
 *   
 *  @author      Vanya Belyaev   Ivan.Belyaev@itep.ru 
 *  @date        10.02.2000 
 */

/** @namespace SolidTicks SolidTicks.h "DetDesc/SolidTicks.h"
 * 
 *  SolidTicks - a collection of technical methods for manipulation of  
 *  ISolid::Tick ans ISolid::Ticks. Also some additional                
 *  mathematical utilities are supplied.
 *  
 *  @author      Vanya Belyaev   Ivan.Belyaev@itep.ru 
 *  @date        10.02.2000 
 */
namespace SolidTicks
{
  
  /** Remove all adjancent ticks 
   *  @author      Vanya Belyaev   Ivan.Belyaev@itep.ru 
   *  @date        10.02.2000 
   *  @see ISolid 
   *  @param ticks   container of "ticks" (sorted!) 
   *  @param point   point for line perametrisation 
   *  @param vect    vector along the line
   *  @param solid   reference to SOLID 
   *  @return number of ticks 
   */
  template <class SOLID, class aPoint, class aVector>
  inline unsigned int  
  RemoveAdjancent
  ( ISolid::Ticks     & ticks , 
    const aPoint      & point , 
    const aVector     & vect  , 
    const SOLID       & solid )
  {
    // local typedefs 
    typedef std::vector<size_t>     Indexes  ; 
    typedef ISolid::Tick            Tick     ; 
    typedef ISolid::Ticks::iterator iterator ;
    // no abjancent ?
    if     ( ticks.size() <  2 ) { ticks.clear() ;    return 0 ; }    // RETURN
    else if( ticks.size() == 2 ) 
      {
        Tick tick1 = ticks.front () ;           // first tick
        Tick tick2 = ticks.back  () ;           // last  tick 
        Tick tick  = 0.5 * ( tick1 + tick2 ) ;  // middle tick
        if( solid.isInside( point + vect * tick ) ) { return 2 ; } // RETURN 
        else                        { ticks.clear() ; return 0 ; } // RETURN 
      }  
    // perform removing of adjancent  ticks
    Indexes tmp              ; 
    Tick    tickNext = 0.0   ;
    Tick    tickPrev = 0.0   ; 
    bool    boolPrev = true  ; 
    bool    boolNext = true  ; 
    for ( iterator it = ticks.begin() ; it != ticks.end() ; ++it ) 
      {
        // the last point is to be treated in a specific way
        if     ( ticks.end   () != it + 1 ) 
          { tickNext = 0.5 * ( (*it) + *(it+1) ) ; 
          boolNext = solid.isInside( point + vect * tickNext );  }  
        // get the index 
        unsigned int index = it - ticks.begin();                       
        /**  to write the last  tick it is enought 
         *   to have the previous interval "inside" 
         */
        if      ( ticks.end  () == it + 1 ) 
          { if( !boolPrev             ) { tmp.push_back( index ) ; } }
        /** to write the first tick it is enought 
         *  to have the first    interval "inside" 
         */
        else if ( ticks.begin() == it     ) 
          { if( !boolNext             ) { tmp.push_back( index ) ; } }
        /**  to write the "regular" tick, it should 
         *   separate 2 different zones! 
         */
        else                                
          { if(  boolPrev == boolNext ) { tmp.push_back( index ) ; } }
        ///
        boolPrev = boolNext; 
        tickPrev = tickNext;
      }
    // remove ticks (from the end!)
    Indexes::reverse_iterator cri = tmp.rbegin();
    while( cri != tmp.rend() ) { ticks.erase( ticks.begin() + *cri++ );  }  
    // get the final answer 
    return ticks.size();
  };
  
  /** Sort Ticks, eliminate duplicates and remove all adjancent ticks 
   *  @author      Vanya Belyaev   Ivan.Belyaev@itep.ru 
   *  @date        10.02.2000 
   *  @see ISolid 
   *  @param ticks   container of "ticks" (unsorted!) 
   *  @param point   point for line perametrisation 
   *  @param vect    vector along the line
   *  @param solid   reference to SOLID 
   *  @return number of ticks 
   */
  template <class SOLID, class aPoint, class aVector>
  inline unsigned int  
  RemoveAdjancentTicks
  ( ISolid::Ticks     & ticks , 
    const aPoint  & point , 
    const aVector & vect  , 
    const SOLID       & solid )
  {
    //     useful local typedefs 
    typedef ISolid::Tick            Tick     ; 
    typedef ISolid::Ticks::iterator iterator ;
    // (1) sort container  
    std::sort( ticks.begin() , ticks.end() ) ; 
    // (2) eliminate duplicates and (3) shrink container 
    ticks.erase( std::unique( ticks.begin() , ticks.end() )  , ticks.end() ); 
    // remove adjancent 
    return RemoveAdjancent( ticks , point , vect , solid );
  };

  /** Eliminate duplicates and remove all adjancent ticks, 
   *  Assume that "ticks" are already sorted  and 
   *  all adjancent ticks are removed!
   *  @author      Vanya Belyaev   Ivan.Belyaev@itep.ru 
   *  @date        10.02.2000 
   *  @see ISolid 
   *  @param ticks   container of "ticks" (sorted!) 
   *  @param point   point for line perametrisation 
   *  @param vect    vector along the line
   *  @param tickMin minimal value of tick 
   *  @param tickMax maximal value of tick 
   *  @param solid   reference to SOLID 
   *  @return number of ticks 
   */
  template <class SOLID, class aPoint, class aVector>
  inline unsigned int  
  RemoveAdjancentTicks
  ( ISolid::Ticks      & ticks   ,
    const aPoint   & point   , 
    const aVector  & vect    , 
    const ISolid::Tick & tickMin , 
    const ISolid::Tick & tickMax , 
    const SOLID        & solid   )
  {
    // useful local typedefs 
    typedef ISolid::Tick            Tick     ; 
    typedef ISolid::Ticks::iterator iterator ;
    // valid arguments? 
    if( tickMin >= tickMax ) { ticks.clear();            return 0 ; } // RETURN
    // remove all garbage what is  less than 'tickMin' 
    iterator itMin = 
      std::remove_if( ticks.begin () ,
                      ticks.end   () ,  
                      std::bind2nd( std::less<Tick>    () , tickMin ) );
    // remove all garbage what is  larger than 'tickMax' 
    iterator itMax = 
      std::remove_if( ticks.begin () , 
                      itMin          , 
                      std::bind2nd( std::greater<Tick> () , tickMax ) );
    ticks.erase  ( itMax , ticks.end() ) ;
    
    // empty ticks! 
    if( ticks.empty() )
      {
        const Tick middle = 0.5 * ( tickMin + tickMax ) ;
        if( solid.isInside(  point  + middle * vect ) ) 
          {
            ticks.push_back( tickMin ) ; 
            ticks.push_back( tickMax ) ; 
          }
        return ticks.size() ;
      }
    
    // first tick 
    if( ticks.front () != tickMin ) 
      { 
        const Tick middle  = 0.5 * ( tickMin      + ticks.front() ) ;
        if( solid.isInside   ( point + middle * vect ) )
          { ticks.insert     ( ticks.begin() , tickMin ) ; }
      }
    
    // last tick 
    if( ticks.back  () != tickMax ) 
      { 
        const Tick middle  = 0.5 * ( ticks.back() +  tickMax ) ;
        if( solid.isInside   ( point + middle * vect ) )
          { ticks.push_back  (                 tickMax ) ; }
      }
    // adjancent are already removed 
    return ticks.size () ;
  };
  
}; ///< end of namespace SolidTicks

// ============================================================================
// The End 
// ============================================================================
#endif   ///<   DETDESC_SOLIDTICKS_H
// ============================================================================






