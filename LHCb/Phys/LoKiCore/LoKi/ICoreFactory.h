// $Id: ICoreFactory.h,v 1.1 2008-10-19 16:11:40 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_IFUNCTORFACTORY_H 
#define LOKI_IFUNCTORFACTORY_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKerbel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/CoreTypes.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class ICoreFactory LoKi/IFunctorFactory.h
     *  
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-10-19
     */
    class ICoreFactory : public virtual IAlgTool 
    {
    public: 
      // ======================================================================
      /// Return the unique interface ID
      static const InterfaceID& interfaceID() ;
      // ======================================================================
    public: // (void->...)
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context context code fragment to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode       , 
        LoKi::Types::FCut&   cuts         , 
        const std::string&   context = "" )  = 0 ;
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context context code fragment to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode       , 
        LoKi::Types::FFun&   cuts         , 
        const std::string&   context = "" )  = 0 ;
      // ======================================================================
    public: // (double->...)
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context context code fragment to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode       , 
        LoKi::Types::XCut&   cuts         , 
        const std::string&   context = "" )  = 0 ;
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context context code fragment to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&   pycode       , 
        LoKi::Types::XFun&   cuts         , 
        const std::string&   context = "" )  = 0 ;
      // ======================================================================
    public:
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context context code fragment to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&  pycode , 
        LoKi::Types::XMap&  cuts  , // vector<double> -> vector<double>
        const std::string&   context = "" )  = 0 ;
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context context code fragment to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&    pycode       , 
        LoKi::Types::XFunVal& cuts         , // vetcor<double> -> double 
        const std::string&    context = "" )  = 0 ;
      // ======================================================================
      /** "Factory": get the the object form python code 
       *  @param pycode the python pseudo-code of the function
       *  @param cuts the placeholder for the result 
       *  @param context context code fragment to be executed 
       *  @return StatusCode 
       */
      virtual StatusCode get
      ( const std::string&    pycode       , 
        LoKi::Types::XSource& cuts         ,  // void -> vector<double>
        const std::string&    context = "" )  = 0 ;
      // ======================================================================
    protected:
      // ======================================================================
      /// virtual & protected destructor 
      virtual ~ICoreFactory () ;              // virtual & protected destructor 
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Hybrid 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IFUNCTORFACTORY_H
// ============================================================================
