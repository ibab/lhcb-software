// $Id: IGiGaGeoSrc.h,v 1.3 2002-01-22 18:20:52 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/08/12 13:25:22  ibelyaev
// improvements in Doxygen documentation
//
// Revision 1.1  2001/07/25 17:18:08  ibelyaev
// move all conversions from GiGa to GiGaCnv
//
// Revision 1.1  2001/07/23 13:11:45  ibelyaev
// the package restructurisation(II)
// 
// ============================================================================
#ifndef GIGA_IGIGAGEOSRC_H 
#define GIGA_IGIGAGEOSRC_H 1
// ============================================================================
/// GaudiKernel
#include "GaudiKernel/IInterface.h"
/// GiGa 
#include "GiGa/IIDIGiGaGeoSrc.h"
/// forward declaration from Geant4 
class G4VPhysicalVolume;


/** @interface IGiGaGeo IGiGaGeo.h "GiGa/IGiGaGeoSrc.h"
 *  
 *  definition of "minimal" geometry source interface for GiGa
 *
 *  @author Ivan Belyaev
 *  @date   21/07/2001
 */

class IGiGaGeoSrc: virtual public IInterface
{
public:
  
  /// Retrieve unique interface ID
  static const InterfaceID& interfaceID() { return IID_IGiGaGeoSrc; }
  
  /** retrieve the pointer to top-level "world" volume,
   *  needed for Geant4 - root for the whole Geant4 geometry tree 
   *  @return pointer to constructed(converted) geometry tree 
   */  
  virtual G4VPhysicalVolume*  world     () = 0 ;
  
  /** retrieve the pointer to top-level "world" volume,
   *  needed for Geant4 - root for the whole Geant4 geometry tree 
   *  @att obsolete method!
   *  @return pointer to constructed(converted) geometry tree 
   */  
  virtual G4VPhysicalVolume*  G4WorldPV () = 0 ;
  
  /// virtual destructor 
  virtual ~IGiGaGeoSrc(){};
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // GIGA_IGIGAGEOSRC_H
// ============================================================================
