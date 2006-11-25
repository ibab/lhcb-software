// $Id: LoKiCore.h,v 1.15 2006-11-25 19:12:55 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.15 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.14  2006/10/10 09:03:21  ibelyaev
//  many tiny fixed needed for good&valid dictionaries
//
// Revision 1.13  2006/05/02 14:29:09  ibelyaev
//  censored
//
// ============================================================================
// Include files 
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Welcome.h"
#include "LoKi/Exception.h"
#include "LoKi/IReporter.h"
#include "LoKi/ErrorReport.h"
#include "LoKi/Report.h"
#include "LoKi/AuxFunBase.h"
#include "LoKi/Power.h"
#include "LoKi/cpower.h"
#include "LoKi/Functions.h"
#include "LoKi/Math.h"
#include "LoKi/Operators.h"
#include "LoKi/Range.h"
#include "LoKi/NamedRange.h"
#include "LoKi/RangeList.h"
#include "LoKi/Record.h"
#include "LoKi/Tokens.h"
#include "LoKi/Selected.h"
#include "LoKi/select.h"
#include "LoKi/Combiner.h"
#include "LoKi/ILoKiSvc.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/Kinematics.h"
#include "LoKi/FuncAdapters.h"
#include "LoKi/Constants.h"
#include "LoKi/Objects.h"
#include "LoKi/Algs.h"
#include "LoKi/CoreTypes.h"
#include "LoKi/Print.h"
#include "LoKi/Geometry.h"
#include "LoKi/Cast.h"
#include "LoKi/Helpers.h"
#include "LoKi/Keeper.h"
#include "LoKi/FuncCast.h"
#include "LoKi/WrongMass.h"
#include "LoKi/Interface.h"
#include "LoKi/AddRef.h"
#include "LoKi/Release.h"
#include "LoKi/Convertible.h"
#include "LoKi/Base.h"
#include "LoKi/Const.h"
#include "LoKi/ConstIterator.h"
#include "LoKi/ConstView.h"
#include "LoKi/GetType.h"
// ============================================================================

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
 *  @date 2006-11-25 
 */
// ============================================================================

/** @namespace LoKi 
 *  The major namespace for all LoKi project
 *  all classes, functions and onbjects (Except for operators)
 *  are places inside this namespace
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
namespace LoKi
{
  /** @namespace LoKi::Algs 
   *  Namespace with few generic "STL-like" algorithms 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25 
   */
  namespace Algs {} 
  /** @namespace LoKi::Cast
   *  Casting functions 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-11-25
   */
  namespace  Cast {}
  /** @namespace LoKi::Adapters 
   *  Basic adapter functions 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-11-25
   */
  namespace Adapters {}
  /** @namespace LoKi::Constants
   *  Namespace  with the collection of useful constants
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-07-10
   */
  namespace Constants {}
  /** @namespace LoKi::Kinematics 
   *  collection of few trivial functions for kinematical evaluations
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-01-17
   */
  namespace Kinematics {}
  /** @namespace  LoKi::Geometry
   *  Namespace with the collection of useful "geoemtry" functions   
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-17
   */
  namespace Geometry{}
  /** @namespace LoKi::Particles
   *  Simple namespace with functions related to particles 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25
   */
  namespace  Particles {}  
  /** @namespace LoKi::Print
   *  Collection of useful utilities for printout 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-16
   */
  namespace Print {}
  /** @namespace LoKi::Types 
   *  colelction of basic LoKi-types 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-16
   */
  namespace Types {}
  /** @namespace LoKi::Cuts
   *  collelction of basic LoKi- functions and cuts  
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-16
   */
  namespace Cuts  {}
  /** @namespace LoKi::Objects 
   *  Collection of useful small objects for LoKi 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-08
   */
  namespace Objects {}
}


// ============================================================================
// The END 
// ============================================================================
