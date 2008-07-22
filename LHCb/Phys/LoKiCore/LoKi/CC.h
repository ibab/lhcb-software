// $Id: CC.h,v 1.1 2008-07-22 13:00:57 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_CC_H 
#define LOKI_CC_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
#include <map>
// ============================================================================
#include "LoKi/CCCmp.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace CC CC.h LoKi/CC.h
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-07-22
   */
  namespace CC 
  {
    // ========================================================================
    /** simple function to make charge all conjugates inside of 
     *  the original string
     *  all substrings, which are known for IParticlePropertySvc 
     *  are subsutututed by their charge conjugates 
     *  @param orig the original sring 
     *  @return charge-conjugated string 
     */ 
    std::string cc ( const std::string&          orig      ) ;
    // ========================================================================
    /// the actual type of CC-map 
    typedef std::map<std::string,std::string,LoKi::CCCmp>  Map ; // CC-MAP
    // ========================================================================
    /** simple function to make charge conjugated inside the original string. 
     *  All substrings are subsutututed by their charge conjugates 
     *  @param orig the original sring 
     *  @param map_ the full map of substitutions 
     *  @param pos the starting position 
     *  @return charge-conjugated string 
     */
    inline std::string cc_
    ( const std::string&           decay   , 
      const LoKi::CC::Map&         map_    ,
      const std::string::size_type pos = 0 ) 
    {
      // check the validity of position 
      if  ( pos >= decay.size() ) { return decay ; }                  // RETURN
      // find the match:
      std::string::size_type        _p = std::string::npos ;
      LoKi::CC::Map::const_iterator _i = map_.end () ;
      // look for the "nearest" and "longest" match 
      for (  LoKi::CC::Map::const_iterator ic = map_.begin() ; 
             map_.end() != ic ; ++ic ) 
      {
        // find the particle 
        std::string::size_type p = decay.find ( ic->first , pos ) ;
        // match? 
        if ( std::string::npos == p ) { continue ; }                 // CONTINUE 
        // find the nearest match
        if ( p < _p )
        {
          _p = p  ;
          _i = ic ;
        }
      }
      // no match at all.
      if ( std::string::npos == _p || 
           map_.end ()       == _i ) { return decay ; }          // RETURN 
      
      // replace 
      std::string aux = decay ;
      aux.replace ( _p , _i -> first.size() , _i -> second ) ;      
      
      // advance the position 
      _p +=  _i -> second.size() ;
      // ... and start the recursion 
      return _p < aux.size() ? cc_ ( aux , map_ , _p ) : aux ;       // RETURN
    }
    // ========================================================================
  } // end of namespace LoKi::CC 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CC_H
// ============================================================================
