/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#define GIGACNV_GIGAISOTOPECNV_CPP
/// ===========================================================================
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/MsgStream.h"
/// DetDesc
#include "DetDesc/Material.h"
#include "DetDesc/Isotope.h"
/// GiGa
#include "GiGa/IGiGaSetUpSvc.h"
#include "GiGa/GiGaException.h"
/// Geant4
#include "G4Isotope.hh"
#include "G4Material.hh"
/// local
#include "AddTabulatedProperties.h"
#include "GiGaIsotopeCnv.h"

///
static const CnvFactory<GiGaIsotopeCnv> s_GiGaIsotopeCnvFactory ;
const ICnvFactory& GiGaIsotopeCnvFactory = s_GiGaIsotopeCnvFactory ;

/// constructor
GiGaIsotopeCnv::GiGaIsotopeCnv( ISvcLocator* Locator )
  : GiGaCnvBase( storageType() , classID() , Locator )
{
  setNameOfGiGaConversionService( "GiGaGeomCnvSvc" );
  setConverterName              ( "GiGaIsotopeCnv" );
};
/// destructor
GiGaIsotopeCnv::~GiGaIsotopeCnv(){};
/// Class ID
const CLID&  GiGaIsotopeCnv::classID            () 
{ return Isotope::classID() ; }
/// StorageType
const unsigned char GiGaIsotopeCnv::storageType () 
{ return GiGaGeom_StorageType; }
/// Create representation
StatusCode GiGaIsotopeCnv::createRep( DataObject*     Object  , 
                                      IOpaqueAddress*& Address )
{
  ///
  Address = 0 ;
  ///
  if( 0 == Object      ) 
    { return Error("CreateRep::DataObject* points to NULL"); }
  Isotope* obj = 0 ;
  try        { obj = dynamic_cast<Isotope*>( Object ) ; }
  catch(...) { obj =                                        0 ; }
  if( 0 == obj         ) 
    { return Error("CreateRep::Bad cast to Isotope*"); }
  if( 0 == cnvSvc()    ) 
    { return Error("CreateRep::Conversion Service is unavailable"); }
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = 0 ;
  try        { addrCreator = dynamic_cast<IAddressCreator*> ( cnvSvc() ) ; }
  catch(...) { addrCreator =                                           0 ; }
  if( 0 == addrCreator ) 
    { return Error("CreateRep::Address Creator is unavailable"); }
  StatusCode st = 
    addrCreator->createAddress( repSvcType() , 
                                classID   () , 
                                "GiGaGeom"   , 
                                "GiGaIsotopeObject" , 
                                -1 , Address );
  if( st.isFailure()   ) 
    { return Error("CreateRep::Error in Address Creation", st); }
  if( 0 == Address     ) 
    { return Error("CreateRep::Error Address is created"        ); }
  ///
  return updateRep( Object , Address ) ;
  ///
};
///
StatusCode GiGaIsotopeCnv::updateRep( DataObject*     Object  , 
                                      IOpaqueAddress* /* Address */ )
{
  ///
  { MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << "UpdateRep::start" << endreq; }
  ///
  if( 0 == Object                 ) 
    { return Error("UpdateRep::DataObject* points to NULL"); }
  Isotope* isotope = 0 ;
  try        { isotope = dynamic_cast<Isotope*>( Object ) ; }
  catch(...) { isotope =                                        0 ; }
  if( 0 == isotope                ) 
    { return Error("UpdateRep::Bad cast to Isotope*"); }
  if( 0 == cnvSvc()               ) 
    { return Error("UpdateRep::Conversion Service is unavailable"); }
  /// if isotop is already converted?
  if( 0 != G4Isotope::GetIsotope( isotope->fullpath() ) ) 
    { return StatusCode::SUCCESS; }
  /// Here we should create the Isotop
  G4Isotope* NewIsotope = 0 ;
  NewIsotope = new G4Isotope( isotope->fullpath () ,
                              (int) isotope->Z  () ,
                              (int) isotope->N  () ,
                              isotope->A        () *g/mole);
  ///
  if( 0 != G4Material::GetMaterial( isotope->fullpath() ) ) 
    { return StatusCode::SUCCESS; }
  /// per each Isotope we could create the "simple material" with the same name
  G4Material* NewMaterial = 0 ;
  NewMaterial = new G4Material( isotope->fullpath        () ,
                                isotope->Z               () ,
                                isotope->A               () * g/mole ,
                                isotope->density         () ,
                                (G4State) isotope->state () ,
                                isotope->temperature     () ,
                                isotope->pressure        () );
  ///
  /// add tabulated properties
  if( !isotope->tabulatedProperties().empty() )
    {
      if( 0 == NewMaterial->GetMaterialPropertiesTable() )
        { NewMaterial->
            SetMaterialPropertiesTable( new G4MaterialPropertiesTable() ); }
      StatusCode sc = 
        AddTabulatedProperties ( isotope->tabulatedProperties() ,
                                 NewMaterial->GetMaterialPropertiesTable() ) ;
      if( sc.isFailure() )
        { return Error("UpdateRep::could not add TabulatedProperties for " + 
                       isotope->fullpath() , sc  ); }
    }
  ///
  { MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << "UpdateRep::end" << endreq; }
  ///
  return StatusCode::SUCCESS;
  ///
};















