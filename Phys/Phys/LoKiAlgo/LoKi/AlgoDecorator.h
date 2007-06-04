// $Id: AlgoDecorator.h,v 1.2 2007-06-04 20:22:21 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_ALGODECORATOR_H 
#define LOKI_ALGODECORATOR_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiPython
// ============================================================================
#include "GaudiPython/Algorithm.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Algo.h"
// ============================================================================
namespace LoKi
{
  namespace Dicts 
  {
    /** @class Alg AlgoDecorator.h LoKi/AlgoDecorator.h
     *  The major wrapper class for LoKi::Algo
     *  @see GaudiPython::PyAlg 
     *  @see LoKi::Algo
     *  @see LoKi::AlgoMC
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu 
     *  @date   2006-09-01
     */
    template <class ALGORITHM> 
    class Alg : public GaudiPython::PyAlg<ALGORITHM>
    {
    protected:
      typedef GaudiPython::PyAlg<ALGORITHM> Base ;
    public:
      /** constructor from Python object and the name 
       *  @param self python object
       *  @param name name of algorithm instance 
       */
      Alg 
      ( PyObject*          self , 
        const std::string& name ) 
        : GaudiPython::PyAlg<ALGORITHM> ( self , name )
      {};
    protected:
      /// virtual destructor 
      virtual ~Alg(){}
    public:
      /// make "analyse" method overwritable form python 
      virtual StatusCode analyse  () 
      { return GaudiPython::call_python_method ( Base::_obj() , "analyse" ) ; }
      /// save methods 
      virtual StatusCode execute_ () { return ALGORITHM::execute () ; }
      /// save methods 
      virtual StatusCode analyse_ () { return ALGORITHM::analyse () ; }    
    private:
      // the default constructor is desabled
      Alg() ;
      // the copy constructor is desabled 
      Alg ( const Alg& );
      // the assignement operator is desabled 
      Alg& operator=( const Alg& ) ;
    } ;
  } // end of namespace LoKi::Dicts 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ALGODECORATOR_H
// ============================================================================
