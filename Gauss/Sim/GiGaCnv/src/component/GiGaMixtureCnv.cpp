// $Id: GiGaMixtureCnv.cpp,v 1.8 2009-10-15 10:01:53 silviam Exp $
#define GIGACNV_GIGAMIXTURECNV_CPP 1 
// ============================================================================
#include "GaudiKernel/CnvFactory.h" 
#include "GaudiKernel/IAddressCreator.h" 
#include "GaudiKernel/IOpaqueAddress.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataSelector.h"
/// DetDesc 
#include "DetDesc/Mixture.h"
/// GiGa 
#include "GiGa/GiGaException.h" 
#include "GiGa/IGiGaSetUpSvc.h" 
// GiGacnv 
#include "GiGaCnv/GiGaCnvUtils.h"
/// Geant4 
#include "G4Element.hh"
#include "G4Material.hh"
/// local 
#include "AddTabulatedProperties.h"
#include "GiGaMixtureCnv.h" 

DECLARE_CONVERTER_FACTORY( GiGaMixtureCnv )

/// constructor 
GiGaMixtureCnv::GiGaMixtureCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
  , m_leaf ( "" , classID() )
{
  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Geo ) ; 
  setConverterName              ( "GiGaMixtureCnv"         ) ;
}

/// destructor 
GiGaMixtureCnv::~GiGaMixtureCnv(){}; 
/// Class ID
const CLID&  GiGaMixtureCnv::classID            () 
{ return Mixture::classID() ; }
/// StorageType 
unsigned char GiGaMixtureCnv::storageType () 
{ return GiGaGeom_StorageType; } 

/// Create representation 
StatusCode GiGaMixtureCnv::createRep
( DataObject*      object  , 
  IOpaqueAddress*& address ) 
{
  ///
  address = 0 ; 
  ///
  if( 0 == object   ) { return Error("createRep::DataObject* points to NULL");} 
  Mixture* mixture = dynamic_cast<Mixture*>( object ) ;  
  if( 0 == mixture  ) { return Error("createRep::Bad cast to Mixture*"      );}
  if( 0 == cnvSvc() ) { return Error("CreateRep::CnvSvc is NULL!"           );}
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = addressCreator() ;
  if( 0 == addrCreator   ) 
    { return Error("CreateRep::AddressCreator is unavailable"); }
  StatusCode status = 
    addrCreator->createAddress( repSvcType  () , 
                                classID     () , 
                                m_leaf.par  () , 
                                m_leaf.ipar () , address );   
  if( status.isFailure() )
    { return Error("CreateRep::Error in Addres creation"); }
  if( 0 == address       )
    { return Error("CreateRep::Created Address is invalid"); }
  ///
  return updateRep( object , address ) ; 
  /// 
}

StatusCode GiGaMixtureCnv::updateRep
( DataObject*         object  , 
  IOpaqueAddress*  /* Address */ ) 
{
  ///
  { MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << "UpdateRep::start" << endreq; } 
  ///
  if( 0 == object   ) { return Error("UpdateRep::DataObject* points to NULL");}
  Mixture* mixture =  dynamic_cast<Mixture*>( object ) ;  
  if( 0 == mixture  ) { return Error("UpdateRep::Bad cast to Mixture*"); }
  if( 0 == cnvSvc() ) { return Error("UpdateRep::Conversion Service is NULL!");}
  const IRegistry* registry = object->registry();
  if( 0 == registry ) { return Error("IRegistry* points to NULL!");}
  /// check if the mixture is already converted
  const std::string& mixtureName = registry->identifier() ;
  G4bool warning=false;  
  if( 0 !=  G4Material::GetMaterial( mixtureName , warning) ) 
    { return StatusCode::SUCCESS; } 
  /// convert all items:
  {
    IDataSelector dataSelector; 
    for( unsigned int index = 0 ; 
         index < (unsigned int) mixture->nOfItems() ; ++index ) 
      { 
        Element* element = mixture->element( index ); 
        if( 0 == element ) 
          { return Error("UpdateRep::Element* point to NULL for Mixture=" + 
                         mixtureName ); } 
        dataSelector.push_back( element ); 
      } 
    StatusCode status = GiGaCnvUtils::createReps ( cnvSvc() , &dataSelector );
    if( status.isFailure() )
      { return Error("UpdateRep::could not convert elements for " + 
                     mixtureName , status); }
  }      
  /// create new material
  G4Material* NewMaterial = 0 ; 
  if( 0 == mixture->nOfItems() )
    {
      NewMaterial = new G4Material( mixtureName                 , 
                                    mixture->Z               () , 
                                    mixture->A               () , 
                                    mixture->density         () , 
                                    (G4State) mixture->state () ,  
                                    mixture->temperature     () , 
                                    mixture->pressure        () ); 
    }  
  else
    {
      /// 
      NewMaterial = new G4Material( mixtureName                 , 
                                    mixture->density         () , 
                                    mixture->nOfItems        () ,
                                    (G4State) mixture->state () ,  
                                    mixture->temperature     () , 
                                    mixture->pressure        () ); 
      
      for( unsigned int index = 0 ; 
           index < (unsigned int) mixture->nOfItems() ; ++index ) 
        {
          G4Material* mat = 
            G4Material::GetMaterial( mixture          ->
                                     element( index ) ->
                                     registry      () ->
                                     identifier    () ); 
          if( 0 == mat ) 
            { return Error("UpdateRep::could not extract material=" + 
                           mixture          -> 
                           element( index ) ->
                           registry      () ->
                           identifier    () ); }
          NewMaterial->
            AddMaterial( mat , mixture->elementFraction( index ) ); // 
        }
    }
  /// add tabulated properties
  if( !mixture->tabulatedProperties().empty() )
    {
      if( 0 == NewMaterial->GetMaterialPropertiesTable() )
        { NewMaterial->
            SetMaterialPropertiesTable( new G4MaterialPropertiesTable() ); }
      StatusCode sc = 
        AddTabulatedProperties ( mixture->tabulatedProperties() ,
                                 NewMaterial->GetMaterialPropertiesTable() ) ;
      if( sc.isFailure() )
        { return Error("UpdateRep::could not add TabulatedProperties for " 
                       + mixtureName , sc  ); } 
    }
  /// 
  { MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << "UpdateRep::end for"+mixtureName << endreq; } 
  ///
  return StatusCode::SUCCESS; 
  /// 
};


















