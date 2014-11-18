// $Id: GiGaLVolumeCnv.cpp,v 1.18 2009-10-15 10:01:36 silviam Exp $ 
// Include files 

// from Gaudi
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IDataSelector.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/DataObject.h"

// from DetDesc
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/CLIDLVolume.h"
#include "DetDesc/LVolume.h"
#include "DetDesc/Surface.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

// from Geant4
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"

// from GiGa
#include "GiGa/IGiGaSensDet.h"
#include "GiGa/IGiGaMagField.h"
#include "GiGa/GiGaUtil.h"
#include "GiGaCnv/IGiGaGeomCnvSvc.h"
#include "GiGaCnv/GiGaCnvUtils.h"
#include "GiGaCnv/GiGaVolumeUtils.h"

// local
#include "GiGaInstall.h"
#include "GiGaLVolumeCnv.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GiGaLVolumeCnv
//
// Author: Vanya Belyaev
// 2007-07-09: Sajan Easo, Gloria Corti - Modified for alignment
//-----------------------------------------------------------------------------

// Declaration of the converter Factory
DECLARE_CONVERTER_FACTORY( GiGaLVolumeCnv )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaLVolumeCnv::GiGaLVolumeCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
  , m_leaf( "", classID() ) 
{
  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Geo ) ; 
  setConverterName              ( "GiGaLVCnv"              ) ; 
}

//=============================================================================
// Destructor
//=============================================================================
GiGaLVolumeCnv::~GiGaLVolumeCnv(){}

//=============================================================================
// Class ID
//=============================================================================
const CLID& GiGaLVolumeCnv::classID() { 
  return CLID_LVolume; 
}

//=============================================================================
// StorageType
//=============================================================================
unsigned char GiGaLVolumeCnv::storageType () { 
  return GiGaGeom_StorageType ; 
} 

//=============================================================================
// Create the representation
//=============================================================================
StatusCode GiGaLVolumeCnv::createRep( DataObject* Object, 
                                      IOpaqueAddress*& Address ) {

  Address = 0 ; 
  if( 0 == Object                 ) 
    { return Error("CreateRep::DataObject* points to NULL"); } 

  const ILVolume* lv = dynamic_cast<const ILVolume*>( Object ) ; 
  if( 0 == lv            ) 
    { return Error("CreateRep::Bad cast to ILVolume*"); }
  if( 0 == geoSvc()      ) 
    { return Error("CreateRep::Conversion Service is unavailable"); }   
  /// look at the store  ( already converted? )
  if( 0 != GiGaVolumeUtils::findLVolume( lv->name() ) ) 
    { return StatusCode::SUCCESS ; }                   ///< RETURN !
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = addressCreator() ;
  if( 0 == addrCreator   ) 
    { return Error("CreateRep::Address Creator is unavailable"); } 
  StatusCode status = 
    addrCreator->createAddress( repSvcType  () , 
                                classID     () , 
                                m_leaf.par  () ,
                                m_leaf.ipar () , 
                                Address        );   
  if( status.isFailure() ) 
    { return Error("CreateRep::Error in Address Creation",status); }
  if( 0 == Address       ) 
    { return Error("CreateRep::Error Address is created"        ); }

  return updateRep( Object , Address ) ; 

}

//=============================================================================
// Update the representation
//=============================================================================
StatusCode GiGaLVolumeCnv::updateRep( DataObject* Object, 
                                      IOpaqueAddress* /* Address */ ) {

  MsgStream msg( msgSvc() , name() ); 
  msg << MSG::VERBOSE << "UpdateRep::start" << endmsg;  

  if( 0 == Object                 ) 
    { return Error("UpdateRep::DataObject* points to NULL"); } 

  ILVolume* lv = dynamic_cast<ILVolume*>( Object ) ;  
  if( 0 == lv                     ) 
    { return Error("CreateRep::Bad cast to ILVolume*"); }
  if( 0 == cnvSvc()               ) 
    { return Error("UpdateRep::Conversion Service is unavailable"); } 
  /// look at the G4 static store
  if( 0 != GiGaVolumeUtils::findLVolume( lv->name() ) ) 
    { return StatusCode::SUCCESS ; }                         ///< RETURN!!!

  G4VSolid*   solid    = geoSvc()->solid    ( lv->solid       () );
  if( 0 == solid    ) 
    { return Error("CreateRep::Could not create Solid!") ; } 
  G4Material* material = geoSvc()->material ( lv->materialName() );
  if( 0 == material ) 
    { return Error("CreateRep::Could not locate Material=" + 
                   lv->materialName() ) ; } 
  G4LogicalVolume* G4LV = 
    geoSvc()->createG4LV ( solid , material , lv->name() );

  msg << MSG::DEBUG 
      << " new Volume/G4LogicalVolume is created with the name '"
      << lv->name() + "'" << endmsg ;  

  /// convert daugthers (if any) 
  StatusCode scAlign =  StatusCode::SUCCESS;
  for( ILVolume::PVolumes::const_iterator iPV = lv->pvBegin() ;  
       lv->pvEnd() != iPV ; ++iPV ) {
    
    const IPVolume* pv = *iPV; 
    if( 0 == pv ) { 
      return Error("updateRep:: IPVolume* points to NULL for " + lv->name() );
    }
    const GiGaVolume vol = geoSvc()->volume( pv->lvolumeName() );
    if( !vol.valid() ) { 
      return Error("updateRep:: Could not convert DPVolume for " + lv->name() );
    }

    Gaudi::Transform3D thePVTransformWithAlign = pv->matrix();
    
    // If to apply alignment get transformation from detector element
    if( geoSvc()-> useAlignment() ) {
      scAlign = transformWithAlignment( pv, thePVTransformWithAlign );
    }
 
    const Gaudi::Transform3D& thePVTransform = thePVTransformWithAlign;
    StatusCode sc = GiGaInstall::installVolume( vol, 
                                                lv->name() + "#" + pv->name(), 
                                                thePVTransform, 
                                                G4LV, 
                                                msg );
    if( sc.isFailure() ) { 
      return Error("updateRep:: coudl not install DPV for " + lv->name() );
    }
  }

  if( scAlign == StatusCode::FAILURE ) {
    msg << MSG::WARNING 
        << " GiGaLVolConv Misalignment: Multiple indistinguishable misalignable detector elements found inside  "
        <<  lv->name() << " .  Please Check Geometry DB  " << endmsg;    
  }

  // sensitivity
  if( !lv->sdName().empty() ) {
    if( 0 == G4LV->GetSensitiveDetector() ) {
      IGiGaSensDet* det = 0 ;
      StatusCode sc = geoSvc()->sensitive( lv->sdName(), det );
      if( sc.isFailure() ) { 
        return Error("updateRep:: Could no create SensDet ", sc );
      }
      if( 0 == det ) { 
        return Error("updateRep:: Could no create SensDet "); 
      }
      // set sensitive detector 
      G4LV->SetSensitiveDetector( det );
    } else {
      Warning( "SensDet is already defined to be '" +
               GiGaUtil::ObjTypeName( G4LV->GetSensitiveDetector() ) +"'");
    }
  }

  // magnetic field 
  if( !lv->mfName().empty() ) { 
    IGiGaFieldMgr* mgr = 0 ;
    StatusCode sc = geoSvc()->fieldMgr( lv -> mfName(), mgr );
    if( sc.isFailure() ) { 
      return Error( "updateRep:: Could no create FieldMgr ", sc ); 
    }
    if( 0 == mgr ) { 
      return Error( "updateRep:: Could no create FieldMgr " );
    }
    if( mgr->global() ) { 
      return Error( "updateRep:: FieldMgr is 'global'" );
    }
    if( 0 == mgr->fieldMgr() ) { 
      return Error( "updateRep:: FieldMgr is invalid " );
    }
    // set sensitive detector 
    G4LV->SetFieldManager( mgr->fieldMgr(), false );
  }

  // look again at the Geant4 static store
  if( 0 != GiGaVolumeUtils::findLVolume( lv->name() ) ) {
    return StatusCode::SUCCESS;                        ///< RETURN !!!           
  }                          

  // If it gets here it means there was a problem !
  return Error("UpdateRep:: failure in conversion of " + lv->name() );

}

//=============================================================================
// Provide matrix transformation of physical volume taking into account
// misalignment in detector elements. Works only if parent is unique!
//=============================================================================
StatusCode GiGaLVolumeCnv::transformWithAlignment( const IPVolume* pv, 
                                                   Gaudi::Transform3D& resultMatrix ) {
  
  StatusCode sca =  StatusCode::SUCCESS;
  MsgStream msg( msgSvc() , name() );

  resultMatrix = pv->matrix();

  std::vector<const IDetectorElement*> foundDe;

  int aNumDetElem = 0;
     
  SmartDataPtr<IDetectorElement> cave(detSvc(),"/dd/Structure/LHCb"  );
  if( geoSvc()->alignAll() ) {    
    aNumDetElem = detElementByLVNameWithAlignment( cave, pv->lvolumeName(), foundDe ); 
  } else {
    /// loop on path of Detector elements to align
    const std::vector<std::string> detsToAlign = geoSvc()->alignDets();
    for( std::vector<std::string>::const_iterator iDet = detsToAlign.begin();
         detsToAlign.end() != iDet; ++iDet ) {
      SmartDataPtr<IDetectorElement> checkDet( detSvc(), *iDet );
      aNumDetElem = detElementByLVNameWithAlignment( checkDet, 
                                                     pv->lvolumeName(), 
                                                     foundDe );
    }  
  }

  //  if aNumDetElem = 0  no det element found. use the default orientation

  if( aNumDetElem >= 1 ) {   // one or more det elem found.  
  
    int aDetElemIndex = 0;
    int numDetElemAfterNpathSearch = 0;
    if( aNumDetElem > 1 ) {
      aDetElemIndex = findBestDetElemFromPVName( pv->name(), foundDe, 
                                                 numDetElemAfterNpathSearch );
    }
    
    if( numDetElemAfterNpathSearch <= 1 ) {
      const Gaudi::Transform3D& aDeltaMatrix = 
        foundDe[ aDetElemIndex ]->geometry()->ownToNominalMatrix();

      resultMatrix = aDeltaMatrix * (pv->matrix());

//       // begin test
//       if( foundDe[ aDetElemIndex ]->name() ==  
//           "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel0" ) {
       
//         msg << MSG::INFO << " Now for " << pv->name() << "  " << pv->lvolumeName()
//             << "  " << foundDe[ aDetElemIndex ]->name() << endmsg;
//         msg << MSG::INFO << "Deltamatrix for alignment for " << pv->name() << "   "
//             << aDeltaMatrix << endmsg;
//         msg << MSG::INFO << "Resultmatrix for alignment " << resultMatrix << endmsg;
//         DataObject* obj = NULL;
//         detSvc()->findObject("Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel0",obj);
//         IDetectorElement * de = dynamic_cast<IDetectorElement *>(obj);
//         if(de != 0 ) {    
//           msg << MSG::INFO << de->name() << endmsg;
//           msg << MSG::INFO << de->geometry()->matrix() << endmsg;
//           msg << MSG::INFO << de->geometry()->alignmentCondition()->name() << endmsg;
//           msg << MSG::INFO << de->geometry()->alignmentCondition()->matrix() << endmsg;
//           msg << MSG::INFO << de->geometry()->alignmentCondition()->toXml("dummy") 
//               << endmsg;
//           msg << MSG::INFO << de->geometry()->toGlobal(Gaudi::XYZPoint()) << endmsg;
//         }
        
//       } // end of test          
          
    } else { 
      // multiple det elem found even after npath search with physvol name. 
      // For now use the ideal geometry as the result.
      // a warning is printed at the end of converting the mother log vol.
      sca = StatusCode::FAILURE;
    }
  }

  return sca;

}


//===============================================================================
// Get the list of detector elemnts which are 
//   valid, have geomentry, have logical volume, corresponds (by name) to a
//   given logical volume, have AlignmentConditions
//   Example code 
//       // get the top-level detector element 
//       const IDetectorElement* lhcb =
//        getDet<IDetectorElement>( "/dd/Structure/LHCb" ) ;
//      or similar top det elements of the subdetector.
//      // prepare the output container
//      typedef  std::vector<const IDetectorElement*> OUTPUT ;
//      OUTPUT elements ;
//  
//      // get the name of logical volume
//      const std::string& lvName = ... ;
//      // use the function:
//      DetDesc:"detElementByLVNameWithAlignment ( lhcb , lvName , elements ) ;
//===============================================================================
int GiGaLVolumeCnv::detElementByLVNameWithAlignment( const IDetectorElement* det,
                                                     const std::string& lvName,
                                   std::vector<const IDetectorElement*>& dets ) {

  int result = 0 ;

  if ( 0 == det ) { return result; }   // RETURN 
  const IGeometryInfo* geo = det->geometry() ;
  
//   // test of the method
//   std::string::size_type posa= lvName.find("/dd/Geometry/BeforeMagnetRegion/Rich1");
//   if( posa !=  std::string::npos) {  
//   // end of test
//     if(geo->alignmentCondition() > 0 ) {
//       std::cout << "Det elem search  lvname  geolvname " << lvName << "  " 
//                 << geo->lvolumeName()<< "  " << det->name() << "  " 
//                 << geo->hasLVolume() << "  " << geo->hasSupport() << "  " 
//                 << geo->alignmentCondition() << std::endl;
//     }
//   }   
//   // end of test of the method

  if( 0 != geo                       && // valid 
      geo->hasLVolume()              && // has Log volume  
      geo->hasSupport()              && // has geometry support path
      0 != geo->alignmentCondition() && // has alignment 
      geo->lvolumeName() == lvName )  { // has correct name 
    
    dets.push_back( det ) ;                  // GOOD! 
    ++result ;
  }
  typedef IDetectorElement::IDEContainer CHILDREN ;
  const CHILDREN& children = det->childIDetectorElements() ;
  for ( CHILDREN::const_iterator ichild = children.begin() ; 
        children.end() != ichild ; ++ichild ) {
    result += detElementByLVNameWithAlignment ( *ichild , lvName , dets ) ; }
    return result ;                            // RETURN 
}


//=============================================================================
// Search best detector element in TDS provided the name of a physical volume
//=============================================================================
int GiGaLVolumeCnv::findBestDetElemFromPVName( std::string pvName,
                    std::vector<const IDetectorElement*> foundDe, 
                                               int& numDetElemFound ) 
{
  // search through the npath strings of the detelem list found
  // using the phys vol names. SE 5-3-2007.

  MsgStream msg( msgSvc() , name() );

  typedef std::vector<const IDetectorElement*> vecDe;
  numDetElemFound = 0;
  int index = 0;
 
  msg << MSG::DEBUG << "For misalignment Multiple det elem for phys vol name =  "
      << pvName << endmsg;
  
  for( vecDe::iterator ait = foundDe.begin(); ait != foundDe.end(); ++ait){
    
    std::string deSupportPathName = "";
    
    StatusCode scs = detector_element_support_path( (*ait), deSupportPathName );
    if( scs == StatusCode::SUCCESS ) {

      std::string::size_type pos =   deSupportPathName.rfind( pvName);
      msg << MSG::DEBUG << "PhysVol search for misalignment  " << deSupportPathName 
          << "   " << pvName << endmsg;
      
      if( pos != std::string::npos ) {

        int expectedPos = deSupportPathName.length()-pvName.length();
        if( expectedPos == (int) pos  ) {
          numDetElemFound++;
          index = ait-foundDe.begin();
          if( numDetElemFound > 1 ) {
            msg << MSG::DEBUG << pos << "  " << deSupportPathName 
                << "  " << pvName << "  " << numDetElemFound 
                << "  " << (*foundDe.begin())->name() << endmsg;
          }
        }
        
      }
        
    }
    
  }  // end of for loop
  
  if( numDetElemFound > 1 ) { // too many det elem with this phys vol.
    // print a warning to check the detelem setup.
    // to avoid verbose output print, the printing is done after converting the mother log vol.
    msg << MSG::DEBUG << " PhysVol Alignment: Too many det elem: " << numDetElemFound 
        << " for  " << pvName << std::endl
        << " Please Check xmldb for  "<< (*foundDe.begin())->name() << endmsg;
  }
  
  return index; 
  
}

//=========================================================================
// Get the PV path of the given DE
// method temporarily copied from DetElmFinder on 12-03-2007. SE, M.Cl.
//=========================================================================
StatusCode GiGaLVolumeCnv::detector_element_support_path(const IDetectorElement *de, 
                                                         std::string &path, 
                                                         const std::string &parent_path) {
  
  StatusCode sc = StatusCode::SUCCESS;

  // initialize the output variable
  path = parent_path;

  // get the parent geometryInfo (supportIGeometryInfo() is public)
  IGeometryInfo* sgi = de->geometry()->supportIGeometryInfo();

  if (NULL == sgi) {
    return sc;
  }

  // If the parent path was not specified, I have to find it recursively
  if (parent_path.empty()){
    if (de->parentIDetectorElement()) {
      sc = detector_element_support_path(de->parentIDetectorElement(),path);
      if (!sc.isSuccess()) {
        return sc;
      }
    } else { // this should never happen
      return sc;
    }
  }
  
  // get support LVolume
  const ILVolume* lv = sgi->lvolume();

  // define a PVolumePath (this is simply std::vector<const IPVolume*> )

  ILVolume::PVolumePath volumePath;

  // Fill it with PVolume* corresponding to the supportPath().
  sc = lv->traverse( de->geometry()->supportPath().begin(),
                     de->geometry()->supportPath().end(),
                     volumePath );
  if (!sc.isSuccess()) {
    MsgStream msg(msgSvc(),name());
    msg << MSG::ERROR 
        << "GiGaLVolCnv det elem search: Cannot traverse the support path  " 
        << de->name() << endmsg;
    return sc;
  }

  // build the string
  for( ILVolume::PVolumePath::iterator pv = volumePath.begin(); 
       pv != volumePath.end(); ++pv ) {
    path += "/" + (*pv)->name();
    
  }

  return sc;
}
//===============================================================================








