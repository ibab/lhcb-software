// $Id: GiGaInstall.h,v 1.5 2009-06-10 13:56:40 gcorti Exp $
#ifndef GIGACNV_GIGAINSTALL_H
#define GIGACNV_GIGAINSTALL_H 1

// Include files
#include <string>
// from Gaudi
#include "GaudiKernel/Transform3DTypes.h"
#include "GaudiKernel/StatusCode.h"


// forward declaration
class G4LogicalVolume      ; ///< Geant4 
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
   *  @param msg     printout msg 
   *  @return status code
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   20/01/2002
   */
  StatusCode installVolume
  ( G4LogicalVolume*      volume ,
    const std::string&    name   ,
    const Gaudi::Transform3D& matrix ,
    G4LogicalVolume*      mother , 
    MsgStream&            msg    ) ;
  
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
   *  @param msg     printout message 
   *  @return status code
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   20/01/2002
   */
  StatusCode installVolume
  ( const GiGaAssembly*   volume ,
    const std::string&    name   ,
    const Gaudi::Transform3D& matrix ,
    G4LogicalVolume*      mother ,
    MsgStream&            msg    ) ;
  
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
   *  @param msg     printout message
   *  @return status code
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   20/01/2002
   */
  StatusCode installVolume
  ( const GiGaVolume&     volume ,
    const std::string&    name   ,
    const Gaudi::Transform3D& matrix ,
    G4LogicalVolume*      mother ,
    MsgStream&            msg    ) ;

} ///< end of namespace GiGaInstall


#endif // GIGACNV_GIGAINSTALL_H
// ============================================================================
