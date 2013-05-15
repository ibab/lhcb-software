// $Id: GiGaIsotopeCnv.cpp,v 1.9 2009-10-15 10:01:01 silviam Exp $ 
#define GIGACNV_GIGAISOTOPECNV_CPP
// ============================================================================
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

// ============================================================================
///
// ============================================================================
DECLARE_CONVERTER_FACTORY( GiGaIsotopeCnv );
// ============================================================================

// ============================================================================
/// constructor
// ============================================================================
GiGaIsotopeCnv::GiGaIsotopeCnv( ISvcLocator* Locator )
  : GiGaCnvBase( storageType() , classID() , Locator )
  , m_leaf     ( "" , classID() )
{
  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Geo ) ; 
  setConverterName              ( "GiGaIsotopeCnv"         ) ;
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
GiGaIsotopeCnv::~GiGaIsotopeCnv(){};
// ============================================================================

// ============================================================================
/// Class ID
// ============================================================================
const CLID&  GiGaIsotopeCnv::classID() { return Isotope::classID() ; }
// ============================================================================

// ============================================================================
/// StorageType
// ============================================================================
unsigned char GiGaIsotopeCnv::storageType () 
{ return GiGaGeom_StorageType; }
// ============================================================================

// ============================================================================
/// Create representation
// ============================================================================
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
    addrCreator->createAddress( repSvcType  () , 
                                classID     () , 
                                m_leaf.par  () , 
                                m_leaf.ipar () ,
                                Address        );
  if( st.isFailure()   ) 
    { return Error("CreateRep::Error in Address Creation", st); }
  if( 0 == Address     ) 
    { return Error("CreateRep::Error Address is created"        ); }
  ///
  return updateRep( Object , Address ) ;
  ///
};
// ============================================================================

// ============================================================================
///
// ============================================================================
StatusCode GiGaIsotopeCnv::updateRep
( DataObject*         Object    , 
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
  if( 0 != G4Isotope::GetIsotope( isotope->registry()->identifier() ) ) 
    { return StatusCode::SUCCESS; }
  /// Here we should create the Isotop
  // G4Isotope* NewIsotope = 0 ;
  new G4Isotope( isotope->registry()->identifier () ,
                 (int) isotope->Z                () ,
                 (int) isotope->N                () ,
                 isotope->A                      () );
  ///
  G4bool warning = false;
  if( 0 != G4Material::GetMaterial( isotope->registry()->identifier(),warning ) ) 
    { return StatusCode::SUCCESS; }
  /// per each Isotope we could create the "simple material" with the same name
  G4Material* NewMaterial = 0 ;
  NewMaterial = new G4Material( isotope->registry()->identifier() ,
                                isotope->Z                     () ,
                                isotope->A                     () ,
                                isotope->density               () ,
                                (G4State) isotope->state       () ,
                                isotope->temperature           () ,
                                isotope->pressure              () );
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
                       isotope->registry()->identifier() , sc  ); }
    }
  ///
  { MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << "UpdateRep::end" << endreq; }
  ///
  return StatusCode::SUCCESS;
  ///
};

// ============================================================================
// The END 
// ============================================================================















