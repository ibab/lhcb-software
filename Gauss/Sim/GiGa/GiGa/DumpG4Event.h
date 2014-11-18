// $Id: DumpG4Event.h,v 1.1 2004-02-22 16:51:08 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GIGA_DUMPG4EVENT_H 
#define GIGA_DUMPG4EVENT_H 1
// Include files
class G4Event ;

namespace GiGaUtil 
{
  
  /** @fn DumpG4Event
   * 
   *  simple function to dump class G4Event
   *
   *  @param  stream output stream 
   *  @param  event  pointer to class G4Event
   *  @return output stream 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-02-20
   */
  std::ostream& DumpG4Event 
  ( std::ostream&  stream             , 
    const G4Event* event  = 0         ) ;  
  
  /** @fn DumpG4Event
   * 
   *  simple function to dump class G4Event
   *
   *  @param  event  pointer to class G4Event
   *  @param  stream output stream 
   *  @return output stream 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-02-20
   */
  std::ostream& DumpG4Event
  ( const G4Event* event  = 0         , 
    std::ostream&  stream = std::cout ) ;
  

}

// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_DUMPG4EVENT_H
// ============================================================================
