// $Id: LoKiCore.h,v 1.24 2010-04-06 20:06:39 ibelyaev Exp $
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
#include "LoKi/Functions.h"
#include "LoKi/Math.h"
#include "LoKi/Operators.h"
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
#include "LoKi/Monitoring.h"
#include "LoKi/Info.h"
#include "LoKi/ExtraInfo.h"
#include "LoKi/Filters.h"
#include "LoKi/Sources.h"
#include "LoKi/HistoBook.h"
#include "LoKi/apply.h"
#include "LoKi/Streamers.h"
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
  namespace Algs       {} 
  namespace Cast       {}
  namespace Adapters   {}
  namespace Constants  {}
  namespace Kinematics {}
  namespace Geometry   {}
  namespace Particles  {}  
  namespace Types      {}
  namespace Cuts       {}
  namespace Objects    {}
  namespace Monitoring {}
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
