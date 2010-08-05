// $Id$
// ============================================================================
#ifndef LOKI_ITRIGGERFACTORY_H 
#define LOKI_ITRIGGERFACTORY_H 1
// ============================================================================
// $URL$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/TriggerTypes.h"
// ============================================================================
/** @file
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *
 *  $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
namespace LoKi
{
  // ==========================================================================
  /** @class ITrgFunctorFactory LoKi/ITrgFunctorFactory.h
   *  The abstract interface to "hybrid factory"  
   *  @author Vanya BELYAEV ibelayev@physics.syr.edu
   *  @date   2007-06-10
   */
  class ITrgFunctorFactory : public virtual extend_interfaces1<IAlgTool>
  {
  public:
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param cuts the placeholder for the result 
     *  @param context the context lines to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&    pycode       , 
      LoKi::Types::TC_Cut&  cuts         , 
      const std::string&    context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the context lines to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&    pycode       ,
      LoKi::Types::TC_Fun&  func         ,
      const std::string&    context = "" ) = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // functional part for Hlt::Candidate
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the context lines to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&    pycode       , 
      LoKi::Types::TC_Map&  func         , 
      const std::string&    context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the context lines to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&    pycode       , 
      LoKi::Types::TC_Pipe& func         , 
      const std::string&    context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the context lines to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&      pycode       , 
      LoKi::Types::TC_FunVal& func         , 
      const std::string&      context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the context lines to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&      pycode       , 
      LoKi::Types::TC_CutVal& func         , 
      const std::string&      context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the context lines to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&       pycode       , 
      LoKi::Types::TC_Element& func         , 
      const std::string&       context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the context lines to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&      pycode       , 
      LoKi::Types::TC_Source& func         , 
      const std::string&      context = "" ) = 0 ;
    // ========================================================================
  protected:
    // ========================================================================
    /// virtual and protected destructor
    virtual ~ITrgFunctorFactory() ;         // virtual and protected destructor
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_ITRACKHYBRIDFACTORY_H
// ============================================================================
