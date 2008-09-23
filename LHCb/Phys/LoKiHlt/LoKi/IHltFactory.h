// $Id: IHltFactory.h,v 1.2 2008-09-23 13:13:35 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_IHltFACTORY_H 
#define LOKI_IHltFACTORY_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/OdinTypes.h"
#include "LoKi/L0Types.h"
#include "LoKi/HLTTypes.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class IHltFactory
     *  The functor factory for creation of Hlt-related  LoKi functors 
     *
     *  Helper interface for implementation of C++/Python "Hybrid" solution
     *
     *  This file is a part of LoKi project - 
     *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
     *
     *  The package has been designed with the kind help from
     *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
     *  contributions and advices from G.Raven, J.van Tilburg, 
     *  A.Golutvin, P.Koppenburg have been used in the design.
     *
     *  @author Vanya BELYAEV Ivabn.Belyaev@nikhef.nl
     *  @date 2008-09-18
     */
    class IHltFactory : public virtual IAlgTool 
    {
    public: 
      // ======================================================================
      /// Return the unique interface ID
      static const InterfaceID& interfaceID() ;
      // ======================================================================
    public: // L0
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context context code fragment to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode       , 
        LoKi::Types::L0_Cut& cuts         , 
        const std::string&   context = "" )  = 0 ;
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context context code fragment to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode       , 
        LoKi::Types::L0_Fun& cuts         , 
        const std::string&   context = "" )  = 0 ;
      // ======================================================================      
    public: // O1 
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context context code fragment to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&     pycode       , 
        LoKi::Types::ODIN_Cut& cuts         , 
        const std::string&     context = "" )  = 0 ;
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context context code fragment to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&     pycode       , 
        LoKi::Types::ODIN_Fun& cuts         , 
        const std::string&     context = "" )  = 0 ;
      // ======================================================================      
    public: // HLT
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context context code fragment to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&    pycode       , 
        LoKi::Types::HLT_Cut& cuts         , 
        const std::string&    context = "" )  = 0 ;
      // ========================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context context code fragment to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&    pycode       , 
        LoKi::Types::HLT_Fun& cuts         , 
        const std::string&    context = "" )  = 0 ;
      // ======================================================================      
    protected:
      // ======================================================================
      /// virtual & protected destructor 
      virtual ~IHltFactory () ;               // virtual & protected destructor 
      // ======================================================================
    } ; 
    // ========================================================================
  } // end of namespace LoKi::Hybrid  
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IHltFACTORY_H
// ============================================================================


