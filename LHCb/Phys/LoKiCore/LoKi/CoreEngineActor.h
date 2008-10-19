// $Id: CoreEngineActor.h,v 1.1 2008-10-19 16:11:40 ibelyaev Exp $
#ifndef LOKI_COREENGINEACTOR_H 
#define LOKI_COREENGINEACTOR_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/ICoreAntiFactory.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class CoreEngineActor LoKi/CoreEngine.h
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
    class CoreEngineActor 
    {
    public:
      // ======================================================================
      // get the static instance 
      static CoreEngineActor& instance() ;
      /// connect the hybrid tool for code translation 
      StatusCode connectTool (       LoKi::Hybrid::ICoreAntiFactory* tool ) ;
      /// disconnect the tool 
      StatusCode releaseTool ( const LoKi::Hybrid::ICoreAntiFactory* tool ) ;
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
    protected:
      // ======================================================================
      /// Standard constructor
      CoreEngineActor() ;
      /// virtual & protected destrucutor  
      virtual ~CoreEngineActor( ); // virtual & protected destrucutor  
      // ======================================================================
    private:
      // ======================================================================
      // just to save some lines 
      template <class TYPE>
      inline StatusCode _add 
      ( const std::string& name , const TYPE& cut ) const ; 
      // ======================================================================
    private:
      // ======================================================================
      // the copy contructor is disabled 
      CoreEngineActor           ( const CoreEngineActor& ) ;
      // the assignement operator is disabled 
      CoreEngineActor& operator=( const CoreEngineActor& ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// the tool itself 
      LoKi::Interface<LoKi::Hybrid::ICoreAntiFactory> m_tool ; // the tool itself 
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Hybrid 
  // ==========================================================================
} //end of namespace LoKi 
// ============================================================================
// The END  
// ============================================================================
#endif // LOKI_COREENGINEACTOR_H
// ============================================================================
