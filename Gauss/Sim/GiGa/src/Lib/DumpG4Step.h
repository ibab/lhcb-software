// $Id: DumpG4Step.h,v 1.1 2004-02-20 18:13:35 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GIGA_DUMPG4STEP_H  
#define GIGA_DUMPG4STEP_H 1
// ============================================================================
// Include files
// ============================================================================

namespace GiGaUtil
{
  /** @fn DumpG4Step 
   * 
   *  simple function to dump G4Step
   *
   *  @param stream output stream 
   *  @param track pointer to G4Step
   *  @return outptut stream 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-02-20
   */
  std::ostream&  DumpG4Step  ( std::ostream& stream , 
                               const G4Step* step   ) ;
  
};


// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_DUMPG4STEP_H
// ============================================================================
