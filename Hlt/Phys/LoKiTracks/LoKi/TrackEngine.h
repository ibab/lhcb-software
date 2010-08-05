// $Id$
// ===========================================================================
#ifndef LOKI_TRACKENGINE_H 
#define LOKI_TRACKENGINE_H 1
// ===========================================================================
// $URL$
// ===========================================================================
// Include files
// ===========================================================================
#include <string>
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/TrackTypes.h"
// ===========================================================================
namespace LoKi 
{  
  // =========================================================================
  namespace Hybrid 
  {
    // =======================================================================
    /** @class TrackEngine LoKi/TrackEngine.h
     *  
     *  Helper class for implementation of Hybrid Tools 
     *
     *  This file is a part of LoKi project - 
     *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
     *
     *  The package has been designed with the kind help from
     *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
     *  contributions and advices from G.Raven, J.van Tilburg, 
     *  A.Golutvin, P.Koppenburg have been used in the design.
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2004-06-30e
     *
     *  $Revision$
     *  Last Modification $Date$ 
     *                 by $Author$ 
     */
    class TrackEngine
    {
    public:
      // ======================================================================
      /// constructor
      TrackEngine () ;
      /// destructor 
      virtual ~TrackEngine( );
      // ======================================================================
    public:
      // ======================================================================
      /// add the cut 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::TrCuts&  cut  ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::TrFunc&  func ) const ;
      // ======================================================================
    public:
      // ======================================================================
      // functional part
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&             name , 
        const LoKi::Types::TrMaps&     fun  ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&             name , 
        const LoKi::Types::TrPipes&    fun  ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&             name , 
        const LoKi::Types::TrFunVals&  fun  ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&             name , 
        const LoKi::Types::TrElements& fun  ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&             name , 
        const LoKi::Types::TrSources&  fun  ) const ;
      // ======================================================================
    } ;  
    // ========================================================================
  } //                                            end of namespace LoKi::Hybrid
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CUTSHOLDERHELPER_H
// ============================================================================
