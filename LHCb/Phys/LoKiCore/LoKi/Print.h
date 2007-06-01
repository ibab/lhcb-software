// $Id: Print.h,v 1.8 2007-06-01 11:35:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.8 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
// ============================================================================
#include "GaudiKernel/ToStream.h"
#include "GaudiKernel/MsgStream.h"
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
   *  Collection of useful utilities for printout 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-16
   */
  namespace Print
  {
    template <class TYPE> 
    inline  std::string toString ( const TYPE& value ) 
    { return Gaudi::Utils::toString ( value ) ; }
    template <class TYPE> 
    inline  std::string print    ( const TYPE& value ) 
    { return toString ( value ) ; }
    /** Trivial generic printout to some stream 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-16
     */
    template <class TYPE>
    inline
    std::ostream& toStream 
    ( std::ostream&   stream , 
      const TYPE&     object ) 
    { 
      return Gaudi::Utils::toStream ( stream , object ) ; 
    }
    template <class TYPE>
    inline MsgStream& toStream 
    ( MsgStream&   stream , 
      const TYPE&  object ) 
    { 
      if ( stream.isActive() )
      { Gaudi::Utils::toStream ( stream.stream() , object ) ; }
      return stream ;      
    }
  } // end of namespace LoKi::Print
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PRINT_H
// ============================================================================
