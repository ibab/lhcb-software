/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef GIGA_IGIGAGEOCNVSVC_H 
#define GIGA_IGIGAGEOCNVSVC_H 1
/// ===========================================================================

/// GiGa 
#include "GiGa/IGiGaCnvSvc.h"
#include "GiGa/IIDIGiGaGeoCnvSvc.h"
/// forward declaration from Geant4 
class G4VPhysicalVolume;


/** @class IGiGaGeoCnvSvc IGiGaGeoCnvSvc.h GiGa/IGiGaGeoCnvSvc.h
 *  
 *  definition of "minimal" geometry conversion interface 
 *
 *  @author Ivan Belyaev
 *  @date   21/07/2001
 */

class IGiGaGeoCnvSvc: virtual public IGiGaCnvSvc
{
  
public:
  
  /// Retrieve unique interface ID
  static const InterfaceID& interfaceID() { return IID_IGiGaGeoCnvSvc; }
  
  /** 
   * Retrieve the pointer to top-level "world" volume,
   * needed for Geant4 - root for the whole Geant4 geometry tree 
   * @return pointer to constructed(converted) geometry tree 
   */  
  virtual G4VPhysicalVolume*  G4WorldPV() = 0 ;
  
  /// virtual destructor 
  virtual ~IGiGaGeoCnvSvc(){};

};

/// ===========================================================================
#endif // GIGA_IGIGAGEOCNVSVC_H
/// ===========================================================================
