// $Id: LoKi.h,v 1.4 2007-08-15 13:46:51 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LOKI_H 
#define LOKI_LOKI_H 1
// ============================================================================
// Include files
// ============================================================================
#include "LoKi/LoKiCore.h"
#include "LoKi/LoKiMC.h"
#include "LoKi/LoKiGen.h"
#include "LoKi/LoKiGenMC.h"
#include "LoKi/LoKiPhys.h"
#include "LoKi/LoKiPhysMC.h"
#include "LoKi/LoKiAlgo.h"
#include "LoKi/LoKiAlgoMC.h"
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
 *  @date 2006-04-09 
 */
// ============================================================================
/** @namepace LoKi 
 *  The major namespace in LoKi project
 *  All classes resided in it
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-04-09
 */
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::Cuts
   *  The namespace with all LoKi fuctions and cuts 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-04-09
   */
  namespace Cuts   {} 
  // ==========================================================================
  /** @namespace LoKi::Cast 
   *  few casting utilities  
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-17
   */
  namespace  Cast {}
  // ==========================================================================
  /** @namespace LoKi::Constants 
   *  Namespace  with the collection of useful constants
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-07-10
   */
  namespace Constants {}  
  // ==========================================================================
  /** @namespace LoKi::Algs 
   *  Namespace with few generic "STL-like" algorithms 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25 
   */
  namespace Adapters {}
  // ==========================================================================
  /** @namespace LoKi::Constants
   *  Namespace  with the collection of useful constants
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-07-10
   */
  namespace Kinematics {}
  // ==========================================================================
  /** @namespace  LoKi::Geometry
   *  Namespace with the collection of useful "geoemtry" functions   
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-17
   */
  namespace Geometry{}
  // ==========================================================================
  /** @namespace LoKi::Print
   *  Collection of useful utilities for printout 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-16
   */
  namespace Print {}
  // ==========================================================================
  /** @namespace LoKi::Objects 
   *  Collection of useful small objects for LoKi 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-08
   */
  namespace Objects {}
  // ==========================================================================
  /** @namespace LoKi::Particles
   *  Simple namespace with functions related to particles 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25
   */
  namespace  Particles {}  
  // ==========================================================================
  /** @namespace LoKi::Vertices 
   *  Simple namespace with functions related to vertices 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25
   */
  namespace  Vertices  {}  
  // ==========================================================================
  /** @namespace LoKi::MCParticles
   *  Simple namespace with functions related to MC-particles 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25
   */
  namespace  MCParticles {}  
  // ==========================================================================
  /** @namespace LoKi::MCVertices 
   *  Simple namespace with functions related to MC-vertices 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25
   */
  namespace  MCVertices  {}  
  // ==========================================================================
  /** @namespace LoKi::GenParticles
   *  Simple namespace with functions related 
   *  to Generator particles 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25
   */
  namespace  GenParticles {}  
  // ==========================================================================
  /** @namespace LoKi::GenVertices
   *  Simple namespace with functions related to 
   *  Generator particles 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25
   */
  namespace  GenVertices {}  
  // ==========================================================================
  /** @namespace LoKi::Types 
   *  colelction of basic LoKi-types 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-16
   */
  namespace Types {}
  // ==========================================================================
  /** @namespace LoKi::AlgoTypes
   *  basic types used for LoKi::Algo class 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-23
   */
  namespace AlgoTypes {}  
  // ==========================================================================
  /** @namespace LoKi::CoreTypes 
   *  The core namespace for some "core"  types 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  namespace CoreTypes {}
  // ==========================================================================
  /** @namespace LoKi::MCTypes 
   *  The core namespace for some Monte Carlo  types 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  namespace MCTypes {}
  // ==========================================================================
  /** @namespace LoKi::GenTypes 
   *  The core namespace for some Generator types 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  namespace GenTypes {}
  /** @namespace LoKi::GenMCTypes 
   *  The core namespace for some Generator-MC types 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  // ==========================================================================
  namespace GenMCTypes {}
  /** @namespace LoKi::PhysMCTypes 
   *  The core namespace for some Phys-MC types 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  namespace PhysMCTypes {}
  // ==========================================================================
  /** @namespace LoKi::PhysTypes 
   *  The core namespace for some Phys types 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  namespace PhysTypes {}
  // ==========================================================================
  /** @namespace LoKi::Extract
   *  The core namespace for "extraction" function
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  namespace Extract {}
  // ==========================================================================
  /** @namespace  LoKi::Algs
   *  Helper namespace with auxillary 
   *  functions/algorithms of the general interest 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  namespace  Algs {}
  // ==========================================================================
  /** @namespace  LoKi::MCAlgs
   *  Helper namespace with MC algorithms  
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  namespace  MCAlgs {}
  // ==========================================================================
  /** @namespace  LoKi::GenAlgs
   *  Helper namespace with Generator algorithms  
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  namespace  GenAlgs {}
  // ==========================================================================
  /** @namespace  LoKi::PhysAlgs
   *  Helper namespace with Phys algorithms  
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  namespace  PhysAlgs {}
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKI_H
// ============================================================================
