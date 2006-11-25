// $Id: Print.h,v 1.6 2006-11-25 19:12:56 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2006/05/23 10:58:07  ibelyaev
//  minor fix in LoKi/Print.h
//
// ============================================================================
#ifndef LOKI_PRINT_H 
#define LOKI_PRINT_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
// ============================================================================
#include "GaudiKernel/Map.h"
#include "GaudiKernel/HashMap.h"
#include "GaudiKernel/VectorMap.h"
// ============================================================================
template <class TYPE> class SmartRefVector ;
template <class TYPE> class SmartRef       ;
// ============================================================================

// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @todo many functions to be replaced from new GaudiKernel ToStream.h
 *   
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16 
 */
// ============================================================================

namespace LoKi 
{
  /** @namespace LoKi::Print Print.h LoKi/Print.h
   *  
   *  Collection of useful utilities for printout 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-16
   */
  namespace Print
  {
    /// convert double  value to string 
    std::string        print ( const double value ) ;
    /// convert integer value to string 
    std::string        print ( const long   value ) ;
    /// convert boolean value to string 
    std::string        print ( const bool   value ) ;
    /// convert integer value to string 
    inline std::string print ( const int    value ) 
    { return print( long(value) ) ; }
    /// convert integer value to string 
    inline std::string print ( const size_t value ) 
    { return print( long(value) ) ; }
    
    /** @fn toStream 
     *  Trivial generic printout to some stream 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-16
     */
    template <class STREAM,class TYPE>
    inline STREAM& toStream 
    ( STREAM&         stream , 
      const TYPE&     object ) 
    { stream << object ;  return stream ; }
    
    /** @fn _toStream 
     *  Trivial generic printout to some stream 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-16
     */
    template <class STREAM,class TYPE>
    inline STREAM& _toStream 
    ( STREAM&         stream , 
      const TYPE&     object ) 
    { return toStream( stream , object ) ;  }
    
    /** @fn toStream 
     *  Trivial specialization for string (a'la Python)
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-16
     */
    template <class STREAM>
    inline STREAM& toStream 
    ( STREAM&            stream , 
      const std::string& object ) 
    {
      if      ( std::string::npos != object.find('\'') ) 
      { stream << "\"" << object << "\"" ; }
      else 
      { stream << "'"  << object << "'"  ; }
      return stream ; 
    }
    
    /** @fn toStream
     *  Trivial generic printout to some stream 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-16
     */        
    template <class STREAM,class ITERATOR>
    STREAM& toStream 
    ( STREAM&            stream      , 
      ITERATOR           first       , 
      ITERATOR           last        , 
      const std::string& comma = "," ) 
    {
      bool skip = true  ;
      for ( ; first != last ; ++first  ) 
      { 
        if ( skip ) { skip = false    ; }
        else        { stream << comma ; }
        toStream ( stream , *first ) ;
      }
      return stream ;
    }

    /** @fn toStream
     *  Trivial generic printout to some stream 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-16
     */        
    template <class STREAM,class ITERATOR>
    STREAM& _toStream 
    ( STREAM&            stream      , 
      ITERATOR           first       , 
      ITERATOR           last        , 
      const std::string& comma = "," ) 
    {
      bool skip = true  ;
      for ( ; first != last ; ++first  ) 
      { 
        if ( skip ) { skip = false    ; }
        else        { stream << comma ; }
        _toStream ( stream , *first ) ;
      }
      return stream ;
    }
    
    /** @fn toStream 
     *  Trivial specialization for pairs (a'la Python's tuples)
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-16
     */
    template <class STREAM,class TYPE1,class TYPE2>
    STREAM& toStream 
    ( STREAM&                       stream , 
      const std::pair<TYPE1,TYPE2>& object ) 
    {
      stream << " (" ;
      toStream ( stream , object.first  ) ;
      stream << ","  ;
      toStream ( stream , object.second ) ;
      stream << ")" ;
      return stream ;
    } ;

    /** @fn toStream 
     *  Trivial specialization for pairs (a'la Python's dictionary entries)
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-16
     */
    template <class STREAM,class TYPE1,class TYPE2>
    STREAM& _toStream 
    ( STREAM&                       stream , 
      const std::pair<TYPE1,TYPE2>& object ) 
    {
      stream << " " ;
      toStream ( stream , object.first  ) ;
      stream << ": " ;
      toStream ( stream , object.second ) ;
      //
      return stream ;
    } ;
    
    /** @fn toStream 
     *  Trivial specialization for vectors (a'la Python's lists)
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-16
     */
    template <class STREAM,class TYPE>
    STREAM& toStream 
    ( STREAM&                  stream , 
      const std::vector<TYPE>& object ) 
    {
      stream << " [";
      toStream ( stream , object.begin() , object.end() ) ;
      stream << "]" ;
      return stream ;
    } ;
    
    /** @fn toStream 
     *  Trivial specialization for maps (a'la Python's dictionaries)
     *  @see LoKi::Print::_toStream
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-16
     */
    template <class STREAM,class TYPE1,class TYPE2>
    STREAM& toStream 
    ( STREAM&                      stream , 
      const std::map<TYPE1,TYPE2>& object ) 
    {
      stream << " {" ;
      _toStream ( stream , object.begin() , object.end() ) ;
      stream << "}"  ;
      return stream  ;
    } ;
    
    /** @fn toStream 
     *  Trivial specialization for GaudiUtils::VectorMap 
     *  (a'la Python's dictionaries)
     *  @see LoKi::Print::_toStream
     *  @see GaudiUtils::VectorMap
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-16
     */
    template <class STREAM,class TYPE1,class TYPE2>
    STREAM& toStream 
    ( STREAM&                                   stream , 
      const GaudiUtils::VectorMap<TYPE1,TYPE2>& object ) 
    {
      stream << " {" ;
      _toStream ( stream , object.begin() , object.end() ) ;
      stream << "}"  ;
      return stream  ;
    } ;
    
    /** @fn toStream 
     *  Trivial specialization for GaudiUtils::Map 
     *  (a'la Python's dictionaries)
     *  @see LoKi::Print::_toStream
     *  @see GaudiUtils::Map
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-16
     */
    template <class STREAM,class TYPE1,class TYPE2>
    STREAM& toStream 
    ( STREAM&                                   stream , 
      const GaudiUtils::Map<TYPE1,TYPE2>& object ) 
    {
      stream << " {" ;
      _toStream ( stream , object.begin() , object.end() ) ;
      stream << "}"  ;
      return stream  ;
    } ;
    
    /** @fn toStream 
     *  Trivial specialization for GaudiUtils::HashMap 
     *  (a'la Python's dictionaries)
     *  @see LoKi::Print::_toStream
     *  @see GaudiUtils::HashMap
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-16
     */
    template <class STREAM,class TYPE1,class TYPE2>
    STREAM& toStream 
    ( STREAM&                                   stream , 
      const GaudiUtils::HashMap<TYPE1,TYPE2>& object ) 
    {
      stream << " {" ;
      _toStream ( stream , object.begin() , object.end() ) ;
      stream << "}"  ;
      return stream  ;
    } ;
    
    /** @fn toStream 
     *  Trivial specialization for SmartRefVector
     *  (a'la Python's lists)
     *  @see SmartRefVector
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-16
     */
    template <class STREAM,class TYPE>
    STREAM& toStream 
    ( STREAM&                     stream , 
      const SmartRefVector<TYPE>& object ) 
    {
      stream << " (" ;
      toStream ( stream , object.begin() , object.end() ) ;
      stream << ")"  ;
      return stream  ;
    } ;
    
    /** @fn toStream 
     *  Trivial specialization for SmartRef
     *  @see SmartRef
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-16
     */
    template <class STREAM,class TYPE>
    STREAM& toStream 
    ( STREAM&               stream , 
      const SmartRef<TYPE>& object ) 
    {
      if (!object) { return stream << " NULL" ;}
      stream << (*object) ;
      return stream ;
    } ;
    
    /** @fn toString
     *  Generic conversion to the string
     *  @see LoKi::Print::toStream
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-16
     */        
    template <class TYPE>
    inline std::string toString ( const TYPE& object ) 
    {
      std::ostringstream stream ;
      toStream ( stream , object ) ;
      return stream.str() ;      
    }    
    
  }; // end of namespace LoKi::Print
  
}; // end of namespace LoKi


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PRINT_H
// ============================================================================
