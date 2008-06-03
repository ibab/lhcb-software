// $Id: PhysCnv.cpp,v 1.6 2008-06-03 15:47:08 cattanem Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Cast.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysCnv.h"
// ============================================================================
/** @file
 *
 * Implementation file for class : PhysCnv
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
 *  @date 2006-02-16 
 */
// ============================================================================
/*  Trivial conversion function 
 *  @param vct vector of const particles 
 *  @return vector of non-const particles 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16 
 */
// ============================================================================
LHCb::Particle::Vector 
LoKi::Cnv::convert 
( const LHCb::Particle::ConstVector& vct )
{
  LHCb::Particle::Vector _v ( vct.size() ) ;
  std::transform 
    ( vct.begin() , vct.end() , _v.begin() , 
      LoKi::Cast::ConstAway<LHCb::Particle>() ) ;
  return _v ;
} 
// ============================================================================
/** Trivial conversion function 
 *  @param vct vector of const particles 
 *  @param return vector of non-const particles 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16 
 */
// ============================================================================
LHCb::Particle::Vector 
LoKi::Cnv::convert 
( const SmartRefVector<LHCb::Particle>& vct ) 
{
  LHCb::Particle::Vector _v ( vct.size() ) ;
  std::transform 
    ( vct.begin() , vct.end() , _v.begin() , 
      LoKi::Cast::ConstAway<LHCb::Particle>() ) ;
  return _v ;
} 
// ============================================================================
/** Trivial conversion function 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16 
 */
// ============================================================================
LHCb::Particle::Vector 
LoKi::Cnv::convert 
( const LoKi::Types::Range& vct ) 
{
  LHCb::Particle::Vector _v ( vct.size() ) ;
  std::transform 
    ( vct.begin() , vct.end() , _v.begin() , 
      LoKi::Cast::ConstAway<LHCb::Particle>() ) ;
  return _v ;
} 
// ============================================================================
/** Trivial conversion function 
 *  @param vct vector of const vertices 
 *  @param return vector of non-const vertices 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16 
 */
// ============================================================================
LHCb::Vertex::Vector 
LoKi::Cnv::convert 
( const LHCb::Vertex::ConstVector& vct ) 
{
  LHCb::Vertex::Vector _v ( vct.size() ) ;
  std::transform 
    ( vct.begin() , vct.end() , _v.begin() , 
      LoKi::Cast::ConstAway<LHCb::Vertex>() ) ;
  return _v ;
} 
// ============================================================================
/** Trivial conversion function 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16 
 */
// ============================================================================
LHCb::Vertex::Vector 
LoKi::Cnv::convert 
( const SmartRefVector<LHCb::Vertex>& vct )
{
  LHCb::Vertex::Vector _v ( vct.size() ) ;
  std::transform 
    ( vct.begin() , vct.end() , _v.begin() , 
      LoKi::Cast::ConstAway<LHCb::Vertex>() ) ;
  return _v ;
} 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
