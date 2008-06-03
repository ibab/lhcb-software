// $Id: IPV2MC.h,v 1.2 2008-06-03 15:24:14 cattanem Exp $
// ============================================================================
#ifndef KERNEL_IPV2MC_H 
#define KERNEL_IPV2MC_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// Kernel ?
// ============================================================================
#include "Kernel/PV2MC.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help froml
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-19 
 */
// ============================================================================


/** @class IPV2MC IPV2MC.h Kernel/IPV2MC.h
 *  
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-03-19
 */
class IPV2MC 
  : virtual public IAlgTool
{
public:
  /// the actual type of PV -> MC relations 
  typedef LHCb::PV2MC          PV2MC        ;
  /// the actual type of MC -> PV relations 
  typedef LHCb::MC2PV          MC2PV        ;
  /// the actual type of PV -> Collision relations 
  typedef LHCb::PV2Collision   PV2Collision ;
  /// the actual type of Collision -> PV relations 
  typedef LHCb::Collision2PV   Collision2PV ;
public:
  
  /** return a relation table for 
   *  LHCb::RecVertex ---> LHCb::MCVertex relations
   *
   *  @code
   * 
   *  typedef IPV2MC::PV2MC Table ;
   *  typedef Table::Range  Range ;
   *
   *  // tool itself 
   *  const IPV2MC* tool = ... ;
   *  // get a table from the tool 
   *  const Table* table = tool->pv2MC() ;
   *  // primary vertex:
   *  const LHCb::RecVertex* prim = ... ;
   *  // get all related MC-vertices
   *  Range links = table->relations( prim ) ;
   *  // make  a loop over all links:
   *  for ( Range::iterator ilink = links.begin() ; 
   *                        links.end() != ilink ; ++ilink ) 
   *   {
   *      // (primary) MC-vertex:
   *      const LHCb::MCVertex* mc = ilink->to() ;
   *      // number of true MC track from the given primary MC:
   *      const int    nTrack = ilink->weight().first    ;
   *      // chi2 of th editance in between MC and RC vertex:
   *      const double chi2   = ilink->weight().second   ;
   *   }
   *
   *  @endcode 
   *
   *  @see LHCb::PV2MC
   *  @see LHCb::RecVertex
   *  @see LHCb::MCVertex
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-03-19
   */
  virtual const PV2MC* pv2MC() const = 0 ;
  
  /** return a relation table for 
   *  (primary)LHCb::MCVertex --> LHCb::RecVertex relations
   *
   *  @see LHCb::MC2PV
   *  @see LHCb::PrimVertex
   *  @see LHCb::MCVertex
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-03-19
   */
  virtual const MC2PV* mc2PV() const = 0 ;
  
  /** return a relation table for 
   *  LHCb::RecVertex ---> LHCb::GenCollision relations
   *
   *  @code
   * 
   *  typedef IPV2MC::PV2Collision Table ;
   *  typedef Table::Range         Range ;
   *
   *  // tool itself 
   *  const IPV2MC* tool = ... ;
   *  // get a table form the tool 
   *  const Table* table = tool->pv2Collision() ;
   *  // primary vertex:
   *  const LHCb::RecVertex* prim = ... ;
   *  // get all related pp-Collisions
   *  Range links = table->relations( prim ) ;
   *  // make  a loop over all links:
   *  for ( Range::iterator ilink = links.begin() ; 
   *                        links.end() != ilink ; ++ilink ) 
   *   {
   *      // pp-Collision
   *      const LHCb::GenCollision* collision = ilink->to() ;
   *      // number of true MC track from the given primary MC:
   *      const int    nTrack = ilink->weight().first    ;
   *      // chi2 of th editance in between MC and RC vertex:
   *      const double chi2   = ilink->weight().second   ;
   *   }
   *
   *  @endcode 
   *
   *  @see LHCb::PV2Collision
   *  @see LHCb::RecVertex
   *  @see LHCb::GenCollision
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-03-19
   */
  virtual const PV2Collision* pv2Collision() const = 0 ;
  
  /** return a relation table for 
   *  LHCb::GenCollision --> LHCb::RecVertex relations
   *
   *  @see LHCb::Collision2PV
   *  @see LHCb::RecVertex
   *  @see LHCb::GenCollision
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-03-19
   */
  virtual const Collision2PV* collision2PV() const = 0 ;

public:
  /// Return the unique interface identifier 
  static const InterfaceID& interfaceID() ; 
protected:
  // virtual and protected destructor 
  virtual ~IPV2MC( ); ///< virtual and protected destructor 
};

// ============================================================================
// The END
// ============================================================================
#endif // KERNEL_IPV2MC_H
// ============================================================================
