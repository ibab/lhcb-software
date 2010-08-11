// $Id$
// ============================================================================
#ifndef LOKI_ITRACKHYBRIDFACTORY_H 
#define LOKI_ITRACKHYBRIDFACTORY_H 1
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
#include "LoKi/TrackTypes.h"
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
  /** @class ITrackFunctorFactory LoKi/ITrackFunctorFactory.h
   *  The abstract interface to "hybrid factory"  
   *  @author Vanya BELYAEV ibelayev@physics.syr.edu
   *  @date   2007-06-10
   */
  class ITrackFunctorFactory : public virtual IAlgTool
  {
  public:
    // ========================================================================
    /// InterfaceID
    DeclareInterfaceID ( ITrackFunctorFactory , 1 , 0 ) ;
    // ========================================================================
  public:
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param cuts the placeholder for the result 
     *  @param context the context lines to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&   pycode       , 
      LoKi::Types::TrCut&  cuts         , 
      const std::string&   context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the context lines to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&   pycode       ,
      LoKi::Types::TrFun&  func         ,
      const std::string&   context = "" ) = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // functional part for LHCb::Track
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the context lines to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&   pycode       , 
      LoKi::Types::TrMap&  func         , 
      const std::string&   context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the context lines to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&   pycode       , 
      LoKi::Types::TrPipe& func         , 
      const std::string&   context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the context lines to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&     pycode       , 
      LoKi::Types::TrFunVal& func         , 
      const std::string&     context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the context lines to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&     pycode       , 
      LoKi::Types::TrCutVal& func         , 
      const std::string&     context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the context lines to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&      pycode       , 
      LoKi::Types::TrElement& func         , 
      const std::string&      context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the context lines to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&     pycode       , 
      LoKi::Types::TrSource& func         , 
      const std::string&     context = "" ) = 0 ;
    // ========================================================================
  protected:
    // ========================================================================
    /// virtual and protected destructor
    virtual ~ITrackFunctorFactory() ;       // virtual and protected destructor
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_ITRACKHYBRIDFACTORY_H
// ============================================================================
