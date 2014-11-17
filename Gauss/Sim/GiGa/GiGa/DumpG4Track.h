// $Id: DumpG4Track.h,v 1.2 2004-02-22 16:51:08 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2004/02/22 13:38:19  ibelyaev
//  relocate DumpG4*.h files
//
// Revision 1.1  2004/02/20 18:13:35  ibelyaev
//  major update in GiGaBase and GiGaTrajectory
// 
// ============================================================================
#ifndef GIGA_DUMPG4TRACK_H 
#define GIGA_DUMPG4TRACK_H 1
// ============================================================================
// Include files
// ============================================================================


namespace GiGaUtil
{ 
  /** @fn DumpG4Track
   * 
   *  simple function to dump G4Track
   *
   *  @param stream output stream 
   *  @param track pointer to G4Track
   *  @return outptut stream 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-02-20
   */
  std::ostream& DumpG4Track 
  ( std::ostream&  stream             , 
    const G4Track* track  = 0         ) ;  
  
  /** @fn DumpG4Track
   * 
   *  simple function to dump G4Track
   *
   *  @param track pointer to G4Track
   *  @param stream output stream 
   *  @return outptut stream 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-02-20
   */
  std::ostream& DumpG4Track 
  ( const G4Track* track  = 0         , 
    std::ostream&  stream = std::cout ) ;
  
  
}


// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_DUMPG4TRACK_H
// ============================================================================
