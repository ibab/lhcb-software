// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2001/07/15 20:45:12  ibelyaev
/// the package restructurisation
///
// ============================================================================
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
/// Geant4 
#include "G4Element.hh"
#include "G4Material.hh"
/// local 
#include "AddTabulatedProperties.h"
#include "GiGaMixtureCnv.h" 

static const  CnvFactory<GiGaMixtureCnv> s_GiGaMixtureCnvFactory ;
const ICnvFactory& GiGaMixtureCnvFactory = s_GiGaMixtureCnvFactory ;
/// constructor 
GiGaMixtureCnv::GiGaMixtureCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
{
  setNameOfGiGaConversionService( "GiGaGeomCnvSvc" ); 
  setConverterName              ( "GiGaMixtureCnv" );
}; 
/// destructor 
GiGaMixtureCnv::~GiGaMixtureCnv(){}; 
/// Class ID
const CLID&  GiGaMixtureCnv::classID            () 
{ return Mixture::classID() ; }
/// StorageType 
const unsigned char GiGaMixtureCnv::storageType () 
{ return GiGaGeom_StorageType; } 
/// Create representation 
StatusCode GiGaMixtureCnv::createRep( DataObject*     Object  , 
                                      IOpaqueAddress*& Address ) 
{
  ///
  Address = 0 ; 
  ///
  if( 0 == Object        ) 
    { return Error("CreateRep::DataObject* points to NULL"); } 
  Mixture* mixture = 0 ; 
  try        { mixture = dynamic_cast<Mixture*>( Object ) ; } 
  catch(...) { mixture =                                0 ; } 
  if( 0 == mixture       ) 
    { return Error("CreateRep::Bad cast to Mixture*"); }
  if( 0 == cnvSvc()      ) 
    { return Error("CreateRep::Conversion Service is unavailable"); }
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = 0 ; 
  try        { addrCreator = dynamic_cast<IAddressCreator*> ( cnvSvc() ) ; } 
  catch(...) { addrCreator =                                           0 ; } 
  if( 0 == addrCreator   ) 
    { return Error("CreateRep::AddressCreator is unavailable"); }
  StatusCode status = 
    addrCreator->createAddress( repSvcType() , 
                                classID   () , 
                                "GiGaGeom"   , 
                                "GiGaIsotopeObject" , -1 , Address );   
  if( status.isFailure() )
    { return Error("CreateRep::Error in Addres creation"); }
  if( 0 == Address       )
    { return Error("CreateRep::Created Address is invalid"); }
  ///
  return updateRep( Object , Address ) ; 
  /// 
}; 

StatusCode GiGaMixtureCnv::updateRep( DataObject*     Object  , 
                                      IOpaqueAddress*  /* Address */ ) 
{
  ///
  { MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << "UpdateRep::start" << endreq; } 
  ///
  if( 0 == Object        ) 
    { return Error("UpdateRep::DataObject* points to NULL"); } 
  Mixture* mixture = 0 ; 
  try        { mixture = dynamic_cast<Mixture*>( Object ) ; } 
  catch(...) { mixture =                                0 ; } 
  if( 0 == mixture       ) 
    { return Error("UpdateRep::Ban cast to Mixture*"); }
  if( 0 == cnvSvc()      ) 
    { return Error("UpdateRep::Conversion Service is unavailable"); }
  /// check if the mixture is already converted
  if( 0 !=  G4Material::GetMaterial( mixture->fullpath()  ) )
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
                         mixture->fullpath()); } 
        dataSelector.push_back( element ); 
      } 
    StatusCode status = cnvSvc()->createReps( &dataSelector );  
    if( status.isFailure() )
      { return Error("UpdateRep::could not convert elements for " + 
                     mixture->fullpath(),status); }
  }      
  /// create new material
  G4Material* NewMaterial = 0 ; 
  if( 0 == mixture->nOfItems() )
    {
      NewMaterial = new G4Material( mixture->fullpath        () , 
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
      NewMaterial = new G4Material( mixture->fullpath        () , 
                                    mixture->density         () , 
                                    mixture->nOfItems        () ,
                                    (G4State) mixture->state () ,  
                                    mixture->temperature     () , 
                                    mixture->pressure        () ); 
      
      for( unsigned int index = 0 ; 
           index < (unsigned int) mixture->nOfItems() ; ++index ) 
        {
          G4Material* mat = 
            G4Material::GetMaterial( mixture->element( index )->fullpath() ); 
          if( 0 == mat ) 
            { return Error("UpdateRep::could not extract material=" + 
                           mixture->element( index )->fullpath() ); }
          NewMaterial->AddMaterial( mat ,
                                    mixture->elementFraction( index ) ); // 
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
        { return Error("UpdateRep::could not add TabulatedProperties for " + 
                       mixture->fullpath() , sc  ); } 
    }
  /// 
  { MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << "UpdateRep::end for"+Object->fullpath() << endreq; } 
  ///
  return StatusCode::SUCCESS; 
  /// 
}; 


















