// $Id: GiGaInstall.h,v 1.1 2002-01-22 18:24:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GIGACNV_GIGAINSTALL_H
#define GIGACNV_GIGAINSTALL_H 1
// Include files
#include <string>
// GaudiKernel
#include "GaudiKernel/StatusCode.h"
// forward declaration
class G4LogicalVolume      ; ///< Geant4 
class HepTransform3D       ; ///< CLHEP 
class GiGaAssembly         ; ///< GiGaCnv 
class GiGaVolume           ; ///< GiGaCnv
class MsgStream            ; ///< GaudiKernel
/** @class GiGaInstall GiGaInstall.h
 *
 *  Collection of useful method, used internally by GiGa converters
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   20/01/2002
 */

namespace GiGaInstall
{

  /** @function installVolume
   *
   *  Install GiGa volume inside mother logical volume
   *  with given name and position
   *
   *  if copy number is not set by volume name it will be set
   *  automatically using formula: "1000 + volume number"
   *
   *  @param volume  logical volume to be installed
   *  @param name    name of the volume
   *  @param matrix  transformation matrix
   *  @param mother  mother logical volume
   *  @param log     printout log 
   *  @return status code
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   20/01/2002
   */
  StatusCode installVolume
  ( G4LogicalVolume*      volume ,
    const std::string&    name   ,
    const HepTransform3D& matrix ,
    G4LogicalVolume*      mother , 
    MsgStream&            log    ) ;
  
  /** @function installVolume
   *
   *  Install GiGa volume inside mother logical volume
   *  with given name and position
   *
   *  if copy number is not set by volume name it will be set
   *  automatically using formula: "1000 + volume number"
   *
   *  @param volume  assembly volume to be installed
   *  @param name    name of the volume
   *  @param matrix  transformation matrix
   *  @param mother  mother logical volume
   *  @param log     printout log 
   *  @return status code
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   20/01/2002
   */
  StatusCode installVolume
  ( const GiGaAssembly*   volume ,
    const std::string&    name   ,
    const HepTransform3D& matrix ,
    G4LogicalVolume*      mother ,
    MsgStream&            log    ) ;
  
  /** @function installVolume
   *
   *  Install GiGa volume inside mother logical volume
   *  with given name and position
   *
   *  if copy number is not set by volume name it will be set
   *  automatically using formula: "1000 + volume number"
   *
   *  @param volume  GiGa     volume to be installed
   *  @param name    name of the volume
   *  @param matrix  transformation matrix
   *  @param mother  mother logical volume
   *  @param log     printout log 
   *  @return status code
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   20/01/2002
   */
  StatusCode installVolume
  ( const GiGaVolume&     volume ,
    const std::string&    name   ,
    const HepTransform3D& matrix ,
    G4LogicalVolume*      mother ,
    MsgStream&            log    ) ;

}; ///< end of namespace GiGaInstall

// ============================================================================
// The End
// ============================================================================
#endif // GIGACNV_GIGAINSTALL_H
// ============================================================================
