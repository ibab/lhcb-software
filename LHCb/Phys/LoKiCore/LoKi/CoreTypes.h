// $Id: CoreTypes.h,v 1.8 2008-10-19 16:11:40 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_CORETYPES_H 
#define LOKI_CORETYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/BasicFunctors.h"
// ============================================================================
/** @file
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
 *  @date 2006-02-10
 */
// ============================================================================
namespace LoKi 
{  
  // ==========================================================================
  /** @namespace LoKi::CoreTypes CoreTypes.h LoKi/CoreTypes.h
   *  The core namespace for some "core"  types 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  namespace CoreTypes 
  {
    // ====================================================================
    /// type of 'cuts' for  'void' (interface)
    typedef LoKi::BasicFunctors<void>::Predicate               FCuts  ;
    /// type of 'functions' 'void' (interface)
    typedef LoKi::BasicFunctors<void>::Function                FFunc  ;
    /// type of 'cuts' for  'void' (assignable)
    typedef LoKi::BasicFunctors<void>::PredicateFromPredicate  FCut   ; 
    /// type of 'functions' for LHCb::Particle  (assignable)
    typedef LoKi::BasicFunctors<void>::FunctionFromFunction    FFun   ;
    // ========================================================================
    /// type of 'cuts' for  'double' (interface)
    typedef LoKi::BasicFunctors<double>::Predicate             XCuts  ;
    /// type of 'functions' 'double' (interface)
    typedef LoKi::BasicFunctors<double>::Function              XFunc  ;
    /// type of 'cuts' for  'double' (assignable)
    typedef LoKi::BasicFunctors<double>::PredicateFromPredicate XCut  ; 
    /// type of 'functions' for LHCb::Particle  (assignable)
    typedef LoKi::BasicFunctors<double>::FunctionFromFunction  XFun   ;
    // ========================================================================
    /// maping or filtering : std::vector<double> -> std::vector<double> 
    typedef LoKi::BasicFunctors<double>::Map                   XMaps     ;
    typedef LoKi::Assignable<XMaps>::Type                      XMap      ;
    ///the same
    typedef LoKi::BasicFunctors<double>::Pipe                  XPipes    ; 
    typedef LoKi::Assignable<XPipes>::Type                     XPipe     ;
    /// function or element selection: std::vector<double> -> double 
    typedef LoKi::BasicFunctors<double>::FunVal                XFunVals  ;
    typedef LoKi::Assignable<XFunVals>::Type                   XFunVal   ;
    /// the same 
    typedef LoKi::BasicFunctors<double>::Element               XElements ; 
    typedef LoKi::Assignable<XElements>::Type                  XElement  ;
    /// source : void -> std::vector<double> 
    typedef LoKi::BasicFunctors<double>::Source                XSources  ; 
    typedef LoKi::Assignable<XSources>::Type                   XSource   ;
    /// source : void -> std::vector<double> 
    typedef LoKi::BasicFunctors<double>::Source                FSources  ; 
    typedef LoKi::Assignable<FSources>::Type                   FSource   ;
    // ========================================================================
  }  // end of namespace LoKi::CoreTypes
  // ==========================================================================
  /** @namespace LoKi::Types 
   *  The namespace to collect all basic LoKi types for 
   *  funtors and predicates 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  namespace Types 
  {
    // ====================================================================
    /// type of 'cuts' for  'void' (interface)
    typedef LoKi::CoreTypes::FCuts               FCuts  ;
    /// type of 'functions' 'void' (interface)
    typedef LoKi::CoreTypes::FFunc               FFunc  ;
    /// type of 'cuts' for  'void' (assignable)
    typedef LoKi::CoreTypes::FCut                FCut   ; 
    /// type of 'functions' 'void' (assignable)
    typedef LoKi::CoreTypes::FFun                FFun   ;
    // ========================================================================
    /// type of 'cuts' for  'double' (interface)
    typedef LoKi::CoreTypes::XCuts               XCuts  ;
    /// type of 'functions' 'double' (interface)
    typedef LoKi::CoreTypes::XFunc               XFunc  ;
    /// type of 'cuts' for  'double' (assignable)
    typedef LoKi::CoreTypes::XCut                XCut   ; 
    /// type of 'functions' double   (assignable)
    typedef LoKi::CoreTypes::XFun                XFun   ;
    // ========================================================================    
    /// maping or filtering : std::vector<double> -> std::vector<double> 
    typedef LoKi::CoreTypes::XMaps               XMaps     ;
    // ditto, but assignable 
    typedef LoKi::CoreTypes::XMap                XMap      ;
    ///the same
    typedef LoKi::CoreTypes::XPipes              XPipes    ; 
    // ditto, but assignable 
    typedef LoKi::CoreTypes::XPipe               XPipe     ;
    /// function or element selection: std::vector<double> -> double 
    typedef LoKi::CoreTypes::XFunVals            XFunVals  ;
    // ditto, but assignable 
    typedef LoKi::CoreTypes::XFunVal             XFunVal   ;
    /// the same 
    typedef LoKi::CoreTypes::XElements           XElements ; 
    // ditto, but assignable 
    typedef LoKi::CoreTypes::XElement            XElement  ; 
    /// source : void -> std::vector<double> 
    typedef LoKi::CoreTypes::XSources            XSources  ; 
    // ditto, but assignable 
    typedef LoKi::CoreTypes::XSource             XSource   ; 
    /// source : void -> std::vector<double> 
    typedef LoKi::CoreTypes::FSources            FSources  ; 
    // ditto, but assignable 
    typedef LoKi::CoreTypes::FSource             FSource   ; 
    // ========================================================================
  }  // end of namespace LoKi::Types
  // ==========================================================================
}  // end of namespace LoKi
namespace LoKi 
{
  // ==========================================================================
  // the specialized printout 
  // ==========================================================================
  template <>
  inline std::ostream& 
  Constant<void,bool>::fillStream ( std::ostream& s ) const 
  { return s << ( this->m_value ? "FALL" : "FNONE" ) ; }
  // ==========================================================================  
  template <>
  inline std::ostream& 
  Constant<double,bool>::fillStream ( std::ostream& s ) const 
  { return s << ( this->m_value ? "XALL" : "XNONE" ) ; }
  // ==========================================================================  
} // end of namespace LoKi
// ========================================================================
// The END 
// ========================================================================
#endif // LOKI_CORETYPES_H
// ========================================================================
