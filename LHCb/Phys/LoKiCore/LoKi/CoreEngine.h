// $Id: CoreEngine.h,v 1.1 2008-10-19 16:11:40 ibelyaev Exp $
#ifndef LOKI_COREENGINE_H 
#define LOKI_COREENGINE_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ICoreAntiFactory.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class CoreEngine LoKi/CoreEngine.h
     *  
     *  Helper class for implementation of C++/Python "Hybrid" solution
     *
     *  This file is a part of LoKi project - 
     *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
     *
     *  The package has been designed with the kind help from
     *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
     *  contributions and advices from G.Raven, J.van Tilburg, 
     *  A.Golutvin, P.Koppenburg have been used in the 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-18
     */
    class CoreEngine
    {
    public: 
      // ======================================================================
      /// Standard constructor
      CoreEngine () ; 
      // ======================================================================
      // destructor 
      virtual ~CoreEngine () ; // Destructor
      // ======================================================================      
    public: // "void -> ... "
      // ======================================================================
      /// propagate the cut to the tool 
      StatusCode process
      ( const std::string&        name , 
        const LoKi::Types::FCuts& cut  ) const ;
      /// propagate the cut to the tool 
      StatusCode process 
      ( const std::string&        name , 
        const LoKi::Types::FFunc& cut  ) const ;
      // ======================================================================
    public: // "double -> ... "
      // ======================================================================
      /// propagate the cut to the tool 
      StatusCode process
      ( const std::string&        name , 
        const LoKi::Types::XCuts& cut  ) const ;
      /// propagate the cut to the tool 
      StatusCode process 
      ( const std::string&        name , 
        const LoKi::Types::XFunc& cut  ) const ;
      // ======================================================================
    public: // some functional stuff 
      // ======================================================================
      /// propagate the cut to the tool 
      StatusCode process
      ( const std::string&           name , 
        const LoKi::Types::XMaps&    cut  ) const ;
      /// propagate the cut to the tool 
      StatusCode process 
      ( const std::string&           name , 
        const LoKi::Types::XFunVals& cut  ) const ;
      // ======================================================================
      /// propagate the cut to the tool 
      StatusCode process 
      ( const std::string&           name , 
        const LoKi::Types::XSources& cut  ) const ;
      // ======================================================================
    };
    // ========================================================================
  } // end of namespace LoKi::Hybrid 
  // ==========================================================================
} //end of namespace LoKi 
// ============================================================================
// The END  
// ============================================================================
#endif // LOKI_COREENGINE_H
// ============================================================================
