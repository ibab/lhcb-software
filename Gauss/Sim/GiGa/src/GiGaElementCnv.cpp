#include "GaudiKernel/CnvFactory.h" 
#include "GaudiKernel/IAddressCreator.h" 
#include "GaudiKernel/IOpaqueAddress.h" 
#include "GaudiKernel/IGiGaSetUpSvc.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataSelector.h"
// DetDesc
#include "DetDesc/Element.h"
#include "DetDesc/Isotope.h"
// GiGa 
#include "GiGa/GiGaException.h" 
/// Geant4 includes
#include "G4Element.hh" 
#include "G4Material.hh" 
// Local
#include "AddTabulatedProperties.h" 
#include "GiGaElementCnv.h" 


static const  CnvFactory<GiGaElementCnv>                                      s_GiGaElementCnvFactory ;
const        ICnvFactory&                             GiGaElementCnvFactory = s_GiGaElementCnvFactory ;

/// constructor 
GiGaElementCnv::GiGaElementCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
{
  setNameOfGiGaConversionService( "GiGaGeomCnvSvc" ); 
  setConverterName              ( "GiGaElementCnv" );
}; 
/// destructor 
GiGaElementCnv::~GiGaElementCnv(){}; 
/// Class ID
const CLID&  GiGaElementCnv::classID            () { return Element::classID() ; }
/// StorageType 
const unsigned char GiGaElementCnv::storageType () { return GiGaGeom_StorageType; } 
/// Create representation 
StatusCode GiGaElementCnv::createRep( DataObject*     Object  , IOpaqueAddress*& Address ) 
{
  Address = 0 ; 
  if( 0 == Object        ) { return Error("CreateRep::DataObject* point to NULL!");} 
  Element* obj = 0 ; 
  try        { obj = dynamic_cast<Element*>( Object ) ; } 
  catch(...) { obj =                                0 ; } 
  if( 0 == obj           ) { return Error("CreateRep::Bad cast to Element*");}
  if( 0 == cnvSvc()      ) { return Error("CreateRep::Conversion Service is unavailble ") ;} 
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = 0 ; 
  try        { addrCreator = dynamic_cast<IAddressCreator*> ( cnvSvc() ) ; } 
  catch(...) { addrCreator =                                           0 ; } 
  if( 0 == addrCreator   ) { return Error("CreateRep::Address Creator is unavailable   ") ;} 
  StatusCode status = addrCreator->createAddress( repSvcType() , classID() , "GiGaGeom" , "GiGaElementObject" , -1 , Address );   
  if( status.isFailure() ) { return Error("CreateRep::Error in Address Creator"); }
  if( 0 == Address       ) { return Error("CreateRep::Address is not created" ); }   
  ///
  return updateRep( Object , Address ) ; 
  /// 
}; 
///
StatusCode GiGaElementCnv::updateRep( DataObject*     Object  , IOpaqueAddress* Address ) 
{
  ///
  typedef Element::Isotopes::iterator Iterator;
  ///
  { MsgStream log( msgSvc() , name() ); log << MSG::VERBOSE << "UpdateRep::start" << endreq; } 
  ///
  IOpaqueAddress* aux = 0 ; aux = Address ; /// just to please the compiler
  if( 0 == Object   ) { return Error("UpdateRep::DataObject* points to NULL"); } 
  Element* element = 0 ; 
  try        { element = dynamic_cast<Element*>(Object) ; } 
  catch(...) { element =                                    0 ; } 
  if( 0 == element ) { return Error("UpdateRep::Bad cast to Element*");} 
  /// Check if Element is already known to Geant4 
  if( 0 != G4Element::GetElement( element->fullpath() ) ) { return StatusCode::SUCCESS; } 
  /// create isotopes first
  {
    IDataSelector ds;
    for( Iterator it = element->isotopes().begin() ; element->isotopes().end() != it ; ++it )
      { ds.push_back( it->second ); }
    StatusCode sc = cnvSvc()->createReps( &ds ); 
    if( sc.isFailure()   ) { return Error("updateRep:: could not convert isotopes for "+element->fullpath() );}
  }
  /// Here we should create the element 
  G4Element*  NewElement = 0 ; 
  if( 0 == element->nOfIsotopes() )                                 /// simple element 
    { NewElement = new G4Element( element->fullpath () , 
                                  element->name     () , 
                                  element->Z        () , 
                                  element->A        () ); } 
  else                                                              /// compound element (from isotopes)
    {                                                               
      /// 
      NewElement = new G4Element( element->fullpath() , element->name() , element->nOfIsotopes() ) ;
      for( Iterator it = element->isotopes().begin() ; element->isotopes().end() != it ; ++it )
        {
          G4Isotope* isotope = G4Isotope::GetIsotope( it->second->fullpath() ); 
          if( 0 == isotope ) 
            { return Error("UpdateRep::could not extract isotope="+it->second->fullpath() ); }
          NewElement->AddIsotope( isotope , it->first ); // 
        } 
    }
  /// Check if Material is already known to Geant4
  if( 0 != G4Material::GetMaterial( element->fullpath() ) ) { return StatusCode::SUCCESS; } 
  /// Here we could define simple materials 
  G4Material*  NewMaterial = 0 ; 
  if( 0 == element->nOfIsotopes() ) 
    { 
      NewMaterial = new G4Material( element->fullpath        () , 
                                    element->Z               () , 
                                    element->A               () , 
                                    element->density         () , 
                                    (G4State) element->state () ,  
                                    element->temperature     () , 
                                    element->pressure        () ); 
    } 
  else
    { 
      /// 
      NewMaterial = new G4Material( element->fullpath        () , 
                                    element->density         () , 
                                    element->nOfIsotopes     () ,
                                    (G4State) element->state () ,  
                                    element->temperature     () , 
                                    element->pressure        () ); 
      for( Iterator it = element->isotopes().begin() ; element->isotopes().end() != it ; ++it )
        {
          G4Element* el = G4Element::GetElement( it->second->fullpath() ); 
          if( 0 == el ) 
            { return Error("UpdateRep::could not extract element="+it->second->fullpath() ); }
          NewMaterial->AddElement( el , it->first ); // 
        } 
    }  
  /// add tabulated properties
  if( !element->tabulatedProperties().empty() )
    {
      if( 0 == NewMaterial->GetMaterialPropertiesTable() )
        { NewMaterial->SetMaterialPropertiesTable( new G4MaterialPropertiesTable() ); }
      StatusCode sc = AddTabulatedProperties ( element->tabulatedProperties()  ,
                                               NewMaterial->GetMaterialPropertiesTable() ) ;
      if( sc.isFailure() )
        { return Error("UpdateRep::could not add TabulatedProperties for "+element->fullpath() , sc  ); } 
    }
  ///
  { MsgStream log( msgSvc() , name() ); log << MSG::VERBOSE << "UpdateRep::end" << endreq; } 
  ///
  return StatusCode::SUCCESS; 
  /// 
}; 


















