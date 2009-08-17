// $Id: ICheckSelResults.h,v 1.3 2009-08-17 08:04:22 jpalac Exp $
// ============================================================================
#ifndef KERNEL_ICHECKSELRESULTS_H 
#define KERNEL_ICHECKSELRESULTS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD/STL
// ============================================================================
#include <string>
#include <vector>
// ============================================================================
// GaudKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
/** @class ICheckSelResults Kernel/ICheckSelResults.h
 *
 *  Thw abstract interface for checking of selection result 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date   2005-07-22
 */
class GAUDI_API ICheckSelResults : virtual public IAlgTool 
{
  // ==========================================================================
public:
  // ==========================================================================
  /// interface machinery
  DeclareInterfaceID(ICheckSelResults, 2, 0);
  // ==========================================================================
public:
  // ==========================================================================
  /// the actual type of "Selections"
  typedef std::string            Selection  ;
  /// the actual type of "Selections"
  typedef std::vector<Selection> Selections ;
  // ==========================================================================
public:
  // ==========================================================================
  /** check if the event has been (pre)selected 
   *  @return true if the event has been (pre)selected 
   */
  virtual bool isSelected () const = 0 ;
  // ==========================================================================  
  /** check if the event has been (pre)selected by a certain selection
   *  @param  selection selection name 
   *  @return true if the event has been (pre)selected 
   */
  virtual bool isSelected ( const Selection  & selection          ) const = 0 ;
  // ==========================================================================  
  /** check if the event has been (pre)selected by certain selection(s)
   *  @param  selections vector of selection names 
   *  @param  ANDMode    flag to distinguish AND/OR modes 
   *  @return true if the event has been (pre)selected by *ALL* 
   *    selections (AND-mode) or by at least one preselection (OR-mode)
   */
  virtual bool isSelected ( const Selections & selections         , 
                            const bool         ANDMode    = false ) const = 0 ;
  // ==========================================================================
protected:
  // ==========================================================================
  /// virtual and protected destructor 
  virtual ~ICheckSelResults() ;             // virtual and protected destructor 
  // ==========================================================================
};
// ============================================================================
// The END
// ============================================================================
#endif // KERNEL_ICHECKSELRESULTS_H
// ============================================================================
