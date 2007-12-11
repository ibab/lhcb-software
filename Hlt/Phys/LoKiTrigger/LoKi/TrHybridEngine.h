// $Id: TrHybridEngine.h,v 1.2 2007-12-11 18:56:21 ibelyaev Exp $
// ===========================================================================
#ifndef LOKI_TRHYBRIDENGINE_H 
#define LOKI_TRHYBRIDENGINE_H 1
// ===========================================================================
// Include files
// ===========================================================================
#include <string>
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/ITrHybridTool.h"
// ===========================================================================
namespace LoKi 
{  
  namespace Hybrid 
  {
    /** @class TrEngine TrHybridEngine.h LoKi/TrHybridEngine.h
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
     *  @date   2004-06-30
     */
    class TrEngine
    {
    public:
      /// constructor
      TrEngine () ;
      /// destructor 
      virtual ~TrEngine( );
    public:
      // ======================================================================
      /// add the cut 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::TrCuts&  cut  ) const ;
      // ======================================================================
      /// add the cut 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::RVCuts&  cut  ) const ;
      // ======================================================================
      /// add the cut 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::TTrCuts& cut  ) const ;
      // ======================================================================
      /// add the cut 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::TrVCuts& cut  ) const ;
      // ======================================================================
      /// add the cut 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::RVVCuts& cut  ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::TrFunc&  func ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&          name , 
      const LoKi::Types::RVFunc&  func ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::TTrFunc& func ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::TrVFunc& fun  ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::RVVFunc& fun  ) const ;
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
    public:
      /// get the reference tracks   ( for Trigger/Hlt)
      std::vector<LHCb::Track*>*     tracks   () const ;
      /// get the reference vertces  ( for Trigger/Hlt)
      std::vector<LHCb::RecVertex*>* vertices () const ;
    } ;  
  } //end of namespace LoKi::Hybrid
} // end of namespace LoKi
// ===========================================================================
// The END 
// ===========================================================================
#endif // LOKI_CUTSHOLDERHELPER_H
// ===========================================================================
