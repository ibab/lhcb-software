// $Id: GiGaDetectorElementCnv.cpp,v 1.7 2002-01-22 18:24:43 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2001/08/12 17:24:52  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.5  2001/07/25 17:19:31  ibelyaev
// all conversions now are moved from GiGa to GiGaCnv
//
// Revision 1.4  2001/07/24 11:13:55  ibelyaev
// package restructurization(III) and update for newer GiGa
//
// Revision 1.3  2001/07/15 20:45:09  ibelyaev
// the package restructurisation
// 
//  ===========================================================================
#define GIGACNV_GIGADETECTORELEMENTCNV_CPP
// ============================================================================
/// STL
#include <string>
#include <vector>
/// Gaudi 
#include "GaudiKernel/CnvFactory.h" 
#include "GaudiKernel/DataObject.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IDataSelector.h" 
#include "GaudiKernel/IAddressCreator.h" 
#include "GaudiKernel/IRegistry.h" 
/// DetDesc 
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/CLIDDetectorElement.h" 
/// Geant4
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
/// GiGa & GiGaCnv 
#include "GiGaCnv/IGiGaGeomCnvSvc.h"
#include "GiGaCnv/GiGaCnvUtils.h"
/// local 
#include "GiGaVolumeUtils.h"
#include "GiGaInstall.h"
#include "GiGaDetectorElementCnv.h"

// ============================================================================
/** @file GiGaDetectorElementcnv.cpp
 * 
 *  Implementation of class GiGaDetectorElementCnv
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
/// factory 
// ============================================================================
static const CnvFactory<GiGaDetectorElementCnv> 
s_GiGaDetectorElementCnvFactory ;
const       ICnvFactory&GiGaDetectorElementCnvFactory = 
s_GiGaDetectorElementCnvFactory ;

// ============================================================================
/** standard constructor 
 *  @param svc pointer to Service Locator 
 */
// ============================================================================
GiGaDetectorElementCnv::GiGaDetectorElementCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
  , m_leaf ( "" , classID() )
{
  setNameOfGiGaConversionService( "GiGaGeomCnvSvc" ); 
  setConverterName              ( "GiGaDECnv"      );
}; 

// ============================================================================
/// destructor 
// ============================================================================
GiGaDetectorElementCnv::~GiGaDetectorElementCnv(){}; 

// ============================================================================
/// Class ID
// ============================================================================
const CLID&         GiGaDetectorElementCnv::classID     () 
{ return CLID_DetectorElement ; }

// ============================================================================
/// StorageType 
// ============================================================================
const unsigned char GiGaDetectorElementCnv::storageType () 
{ return GiGaGeom_StorageType ; } 

// ============================================================================
/** create the representation
 *  @param Object  pointer to data object 
 *  @param Address address 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaDetectorElementCnv::createRep( DataObject*     Object  , 
                                              IOpaqueAddress*& Address ) 
{
  ///
  Address = 0 ; 
  if( 0 == Object                 ) 
    { return Error("CreateRep::DataObject* points to NULL"); } 
  ///
  IDetectorElement* de = 0 ; 
  try        { de = dynamic_cast<IDetectorElement*>( Object ) ; } 
  catch(...) { de =                                 0 ; } 
  if( 0 == de       ) 
    { return Error("createRep::Bad cast to IDetectorElement*"); }
  if( 0 == geoSvc() )
    { return Error("createRep::Conversion Service is unavailable"); }   
  /// 
  IGeometryInfo* gi = de->geometry(); 
  if( 0 == gi || 0 == gi->lvolume() )
    { 
      MsgStream log( msgSvc() , name() ); 
      log << MSG::WARNING 
          << " IGeometryInfo*/ILVolume* is not available for DE="      
          << de->name()    
          << " Consider it just as a holder of daughter DEs"  << endreq; 
      /// geometry information is not available, 
      /// consider DE just as a collection of daughter DetectorElements  
      for( IDetectorElement::IDEContainer::iterator ic = de->childBegin() ; 
           de->childEnd() != ic ; ++ic )
        {
          DataObject* object = dynamic_cast<DataObject*> ( *ic ) ;
          if( 0 == *ic || 0 == object ) 
            { return Error("createRep:: daughter points to NULL !" ) ;}
          StatusCode sc = GiGaCnvUtils::createRep( cnvSvc() , object );
          if( sc.isFailure() ) 
            { return Error("createRep:: could not create daughter element") ; }
        }
      ///
      return StatusCode::SUCCESS;
      ///
    }  
  /// here it is an ordinary detector element 
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = addressCreator() ;
  if( 0 == addrCreator   ) 
    { return Error("createRep::Address Creator is unavailable"); }
  StatusCode status = 
    addrCreator->createAddress( repSvcType   () , 
                                classID      () , 
                                m_leaf.par   () , 
                                m_leaf.ipar  () , 
                                Address         );
  if( status.isFailure() ) 
    { return Error("createRep::Error in Address Creation",status); }
  if( 0 == Address       ) 
    { return Error("createRep::Error Address is created"        ); }
  ///
  return updateRep( Object , Address ) ; 
  /// 
};

// ============================================================================
/** update the  representation
 *  @param Object  pointer to data object 
 *  @param Address address 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaDetectorElementCnv::updateRep( DataObject*     Object  , 
                                              IOpaqueAddress* /* Address */ ) 
{
  ///
  MsgStream log( msgSvc() , name() ); 
  log << MSG::INFO << "updateRep::start " 
      << Object->registry()->identifier() << endreq; 
  ///
  if( 0 == Object                 ) 
    { return Error("updateRep::DataObject* points to NULL"); } 
  ///
  IDetectorElement* de = 0 ; 
  try        { de = dynamic_cast<IDetectorElement*>( Object ) ; } 
  catch(...) { de =                                 0 ; } 
  if( 0 == de        ) 
    { return Error("updateRep::Bad cast to IDetectorElement*"); }
  if( 0 == geoSvc()  )
    { return Error("updateRep::Conversion Service is unavailable"); } 
  ///
  IGeometryInfo* gi = de->geometry() ; 
  if( 0 == gi )
    { return Error("updateRep:: IGeometryInfo* is not available for " + 
                   de->name() ); }
  const ILVolume*      lv = gi->lvolume () ;
  if( 0 == lv )
    { return Error("updateRep:: ILVolume*      is not available for " + 
                   de->name() ); }
  //
  // // look at G4 physical volume store and check 
  // //  if it was converted exlicitely or imlicitely
  //    {
  //      std::string path ( de->name() );
  //      do
  //        {
  //          G4VPhysicalVolume* pv = 0; 
  //          G4PhysicalVolumeStore& store = *G4PhysicalVolumeStore::GetInstance();
  //          for( unsigned int indx = 0 ; indx < store.size() ; ++indx )
  //            { if( path == store[indx]->GetName() ) { pv = store[indx] ; break; } }
  //          /// it was converted EXPLICITELY or IMPLICITELY !!!
  //          if( 0 != pv ) 
  //            {
  //              MsgStream log( msgSvc() , name() ) ; 
  //              log << MSG::INFO 
  //                  << "DE=" << de->name() 
  //                  << " was already EXPLICITELY/IMPLICITELY converted for PV=" 
  //                  << pv->GetName() << endreq; 
  //              return StatusCode::SUCCESS; 
  //            }                          /// RETURN !!!
  //          ///
  //          path.erase( path.find_last_of('/') ); 
  //        } 
  //      while( std::string::npos != path.find_last_of('/') ); 
  //    }
  /// it have not been converted yet!!! convert it!
  
  const GiGaVolume volume  = geoSvc()->volume( lv->name() );
  if( !volume.valid() ) 
    { return Error("updateRep:: could not convert LV="+lv->name() ); }
  G4VPhysicalVolume*   PV = geoSvc()->world() ; 
  if( 0 == PV )
    { return Error("updateRep:: G4WorldPV is not available!" ) ; }
  /// install detector element into world volume 
  StatusCode sc = 
    GiGaInstall::installVolume( volume                  , 
                                de->name             () , 
                                gi->matrix           () , 
                                PV->GetLogicalVolume () , 
                                log                     ) ;
  if( sc.isFailure() )
    { return Error("updateRep:: could not place PhysVolume!", sc) ; }
  /// look again in the store 
  if( 0 != GiGaVolumeUtils::findPVolume( de->name() ) )
    { return StatusCode::SUCCESS ; }               ///< RETURN !!!
  ///
  /// a little bit strange lines due to names of Assemblies  
  /// return 
  ///  Error("updateRep:: could not convert DetectorElement="+de->name()) ;
  return StatusCode::SUCCESS ;
  ///
};

// ============================================================================












