// $Id: GiGaInstall.cpp,v 1.5 2009-06-10 13:56:40 gcorti Exp $

// Include files
// STD & STL 
#include <cstdlib>
#include <string>

// MathCore -> CLHEP
#include "ClhepTools/MathCore2Clhep.h"

// GaudiKernel
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

// Geant4 
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

// GiGaCnv
#include "GiGaCnv/GiGaAssembly.h"
#include "GiGaCnv/GiGaVolume.h"

// local
#include "GiGaInstall.h"

// ============================================================================
/** @file GiGaInstall.cpp 
 * 
 *  Implementation file for namespace  GiGaInstall
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 20/01/2002 
 */
// ============================================================================

// ============================================================================
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
// ============================================================================
StatusCode GiGaInstall::installVolume
( G4LogicalVolume*      volume , 
  const std::string&    name   , 
  const Gaudi::Transform3D& matrix , 
  G4LogicalVolume*      mother ,
  MsgStream&            msg    )
{
  if( 0 == volume ) 
    {
      msg << MSG::ERROR 
          << " GiGaInstall::installVolume1:: volume  is NULL! " << endmsg ;
      return StatusCode::FAILURE ; 
    }
  if( 0 == mother ) 
    { 
      msg << MSG::ERROR 
          << " GiGaInstall::installVolume1:: mother  is NULL! " << endmsg ;
      return StatusCode::FAILURE ; 
    }
  /// get the copy number 
  int ncopy = 1000 + mother->GetNoDaughters() ;
  {
    const std::string::size_type pos = name.find(':');
    if( std::string::npos != pos ) 
      {
        std::string tmp( name , pos + 1 , std::string::npos );
        ncopy = atoi( tmp.c_str() );
      }
  };
  /// create the placement 
  HepGeom::Transform3D clhepMatrix = LHCb::math2clhep::transform3D( matrix );
  new G4PVPlacement( clhepMatrix.inverse(), volume, name, mother, false, 
                     ncopy );
  ///
  msg << MSG::DEBUG
      << " GiGaInstall:: "
      << " new G4PVPlacement is created with the name '" << name  << "'" 
      << " copy number is "                              << ncopy
      << endmsg;
  ///
  return StatusCode::SUCCESS ;
  ///
}

// ============================================================================
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
// ============================================================================
StatusCode GiGaInstall::installVolume
( const GiGaAssembly*   volume , 
  const std::string&    name   , 
  const Gaudi::Transform3D& matrix , 
  G4LogicalVolume*      mother , 
  MsgStream&            msg    )
{
  /// check arguments 
  if( 0 == volume ) 
    {
      msg << MSG::ERROR 
          << " GiGaInstall::installVolume2:: volume  is NULL! " << endmsg ;
      return StatusCode::FAILURE ; 
    }
  if( 0 == mother ) 
    { 
      msg << MSG::ERROR 
          << " GiGaInstall::installVolume2:: mother  is NULL! " << endmsg ;
      return StatusCode::FAILURE ; 
    }
  ///
  for( GiGaAssembly::Volumes::const_iterator ivol = volume->volumes().begin() ;
       volume->volumes().end() != ivol ; ++ivol )
    {
      G4LogicalVolume* lv = ivol->first.first ;
      if( 0 == lv ) 
        {
          msg << MSG::ERROR 
              << " GiGaInstall::installVolume2:: corrupted assembly '"
              << volume->name() << "'" << endmsg ;
          return StatusCode::FAILURE;                             ///< RETURN 
        }
      StatusCode sc = 
        installVolume( lv                                , 
                       name + "#" + ivol->first.second   , 
                       ivol->second * matrix             , 
                       mother                            , 
                       msg                               );
      ///
      if( sc.isFailure() ) 
        { 
          msg << MSG::ERROR
              << " GiGaInstall::installVolume2:: recursive error! " << endmsg ;
          return sc ;                                           ///< RETURN 
        } 
    } 
  ///
  return StatusCode::SUCCESS ;
}

// ============================================================================
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
 *  @param msg     printout msg 
 *  @return status code
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   20/01/2002
 */
// ============================================================================
StatusCode GiGaInstall::installVolume
( const GiGaVolume&     volume ,
  const std::string&    name   ,
  const Gaudi::Transform3D& matrix ,
  G4LogicalVolume*      mother ,
  MsgStream&            msg    )
{
  /// check arguments 
  if( !volume.valid() ) 
    {
      msg << MSG::ERROR 
          << " GiGaInstall::installVolume3:: volume  is invalid " << endmsg ;
      return StatusCode::FAILURE ; 
    }
  if( 0 == mother ) 
    { 
      msg << MSG::ERROR 
          << " GiGaInstall::installVolume3:: mother  is NULL! " << endmsg ;
      return StatusCode::FAILURE ; 
    }
  /// 
  if(      0 != volume.volume   () ) 
    { return installVolume( volume.volume   () , 
                            name               , 
                            matrix             , 
                            mother             , 
                            msg                ); }
  else if( 0 != volume.assembly () ) 
    { return installVolume( volume.assembly () , 
                            name               , 
                            matrix             , 
                            mother             ,
                            msg                ); }
  ///
  msg << MSG::ERROR
      << " GiGaInstall::installVolume3:: fatal error! " << endmsg ;
  ///
  return StatusCode::FAILURE ;
}

// ============================================================================
// The End 
// ============================================================================
