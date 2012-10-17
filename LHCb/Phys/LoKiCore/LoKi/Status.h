// ============================================================================
#ifndef LOKI_STATUS_H 
#define LOKI_STATUS_H 1
// ============================================================================
// Include files
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/IAccept.h"
#include "Kernel/ICheckTool.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Interface.h"
// ============================================================================
/** @file Loki/Status.h
 *  collection of "checkers" deaing with generic tools
 *  @see ICheckTool 
 *  @see IAcceptTool
 *
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 *  @date   2010-08-02 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::Status 
   *  Collection of "checkers"-functors dealing with generic tools 
   *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
   *  @date   2010-08-02
   */
  namespace Status 
  {
    // ========================================================================
    /** @class Check     LoKi/Status.h
     *  Simple checker to "check" using ICheckTool 
     *  @see ICheckTool
     *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
     *  @date 2010-08-03
     */
    class Check : public LoKi::Functor<void,bool>
    {
    public:
      // ======================================================================
      /// constructor from tool nickname 
      Check ( const std::string& nikname ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Check() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Check* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( /* argument v */ ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Check () ;                         // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// tool nickname 
      std::string                     m_nickname ; // the tool nickname  
      /// the checker 
      mutable LoKi::Interface<ICheckTool> m_tool;  // the checker 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Accept LoKi/Status.h
     *  Simple checker to "check" using IAccept
     *  @see IAccept
     *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
     *  @date 2010-08-03
     */
    class Accept : public LoKi::Functor<void,bool>
    {
    public:
      // ======================================================================
      /// constructor from tool nickname 
      Accept ( const std::string& nikname ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Accept () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Accept* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( /* argument v */ ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Accept () ;                        // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// tool nickname 
      std::string                      m_nickname ;  // the tool nickname  
      /// the checker 
      mutable LoKi::Interface<IAccept> m_tool     ;  // the checker 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                            end of namespace LoKi::Status 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef ACCEPT
     *  Trivial checker/function to "check"
     *  @see IAccept 
     *  @see LoKi::Status::Accept
     *  @author Vanya BELYAEV  Ivan.BElyaev@nikhef.nl
     *  @date 2010-08-03
     */
    typedef LoKi::Status::Accept                                       ACCEPT ;
    // ========================================================================
    /** @typedef CHECK
     *  Trivial checker/function to "check"
     *  @see ICheckTool 
     *  @see LoKi::Status::Check
     *  @author Vanya BELYAEV  Ivan.BElyaev@nikhef.nl
     *  @date 2010-08-03
     */
    typedef LoKi::Status::Check                                         CHECK ;
    // ========================================================================
  } //                                             end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_STATUS_H
// ============================================================================
