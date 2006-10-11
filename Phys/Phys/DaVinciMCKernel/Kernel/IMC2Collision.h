// $Id: IMC2Collision.h,v 1.1.1.1 2006-10-11 13:35:09 jpalac Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/03/18 18:26:43  ibelyaev
//  add (primary)MCVErtex->GenCollision links
// 
// ============================================================================
#ifndef KERNEL_IMC2COLLISION_H 
#define KERNEL_IMC2COLLISION_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// Kernel?
// ============================================================================
#include "Kernel/MC2Collision.h"
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-18 
 */
// ============================================================================


/** @class IMC2Collision IMC2Collision.h Kernel/IMC2Collision.h
 *  
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-03-18
 */
class IMC2Collision 
  : virtual public IAlgTool
{  
public: 
  // the actuial type of LHCb::MCVertex -> LHCb::GenCollision relations
  typedef LHCb::MCVertex2Collision   MCVertex2Collision ;
public :
  /** get the relation table for LHCb::MCVertex -> LHCb::GenCollision 
   * 
   *  @code 
   *
   *  // get tht tool 
   *  const IMC2Collision* t = tool<IMC2Collision>( .... ) ;
   * 
   *  // get the reation table form the tool 
   *  const LHCb::MCVertex2Collision* table = t->vertex2collision() ;
   *
   *  @endcode 
   *
   *  @see MC2Collision
   *  @see LHCb::MCVertex2Collision
   *  @see LHCb::MCVertex 
   *  @see LHCb::GenCollision
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  virtual const MCVertex2Collision* vertex2collision() const = 0 ;
public:  
  /// Return the unique interface identifier 
  static const InterfaceID& interfaceID() ;
protected:
  // virtual and protected destructor 
  virtual ~IMC2Collision( ); ///< virtual and protected destructor
};

// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_IMC2COLLISION_H
// ============================================================================
