// $Id$
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
/** @file LoKi/AlgoDecorator.h
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Dicts 
  {
    // ========================================================================
    /** @class Alg  LoKi/AlgoDecorator.h
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
      // ======================================================================
    protected:
      // ======================================================================
      /// the base 
      typedef GaudiPython::PyAlg<ALGORITHM>                              Base ;
      // ======================================================================
    public:
      // ======================================================================
      /** constructor from Python object and the name 
       *  @param self python object
       *  @param name name of algorithm instance 
       */
      Alg 
      ( PyObject*          self , 
        const std::string& name ) 
        : GaudiPython::PyAlg<ALGORITHM> ( self , name )
      {}
      // ======================================================================
    protected:
      // ======================================================================
      /// virtual destructor 
      virtual ~Alg(){}
      // ======================================================================
    public:
      // ======================================================================
      /// make "analyse" method overwritable form python 
      virtual StatusCode analyse  () 
      { return GaudiPython::call_python_method ( Base::_obj() , "analyse" ) ; }
      /// save methods 
      virtual StatusCode execute_ () { return ALGORITHM::execute () ; }
      /// save methods 
      virtual StatusCode analyse_ () { return ALGORITHM::analyse () ; }    
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is desabled
      Alg() ;                           //  the default constructor is desabled
      /// the copy constructor is desabled 
      Alg ( const Alg& );               //     the copy constructor is desabled 
      /// the assignement operator is desabled 
      Alg& operator=( const Alg& ) ;    // the assignement operator is desabled 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                             end of namespace LoKi::Dicts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_ALGODECORATOR_H
// ============================================================================
