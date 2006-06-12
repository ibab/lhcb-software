// $Id: Bases.h,v 1.1 2006-06-12 15:27:28 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, verison $Relations:$#i
// ============================================================================
// $Log: not supported by cvs2svn $  
// ============================================================================
#ifndef RELATIONS_BASES_H 
#define RELATIONS_BASES_H 1
// ============================================================================
// Include files
// ============================================================================

namespace Relations 
{
  /** @struct BaseRange
   *  helper structure to make easy interactive manipulations
   *  with Reflex dictionaries 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-06
   */
  struct BaseRange {} ;
  
  /** @struct BaseTable
   *  helper structure to make easy interacyive manipulations
   *  with Reflex dictionaries 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-06
   */
  struct BaseTable {} ;
  
  /** @struct BaseWeightedTable
   *  helper structure to make easy interacyive manipulations
   *  with Reflex dictionaries 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-06
   */
  struct BaseWeightedTable  : public BaseTable {} ;
  
}

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_BASES_H
// ============================================================================
