#include "GaudiKernel/CnvFactory.h" 
#include "GaudiKernel/IAddressCreator.h" 
#include "GaudiKernel/IOpaqueAddress.h" 
#include "GaudiKernel/IGiGaSetUpSvc.h" 

#include "GaudiKernel/MsgStream.h"


#include "DetDesc/Material.h"
#include "DetDesc/Isotope.h"

#include "GiGa/GiGaException.h" 
#include "GiGa/GiGaIsotopeCnv.h" 

///
/// Geant4 includes
///


#include "G4Isotope.hh"
#include "G4Material.hh"

///
///
///

extern unsigned char GiGaGeom_StorageType ; 

static const  CnvFactory<GiGaIsotopeCnv>                                      s_GiGaIsotopeCnvFactory ;
const        ICnvFactory&                             GiGaIsotopeCnvFactory = s_GiGaIsotopeCnvFactory ;

///
/// constructor 
///

GiGaIsotopeCnv::GiGaIsotopeCnv( ISvcLocator* Locator ) 
  : GiGaCnv( storageType() , classID() , Locator ) 
{
  setNameOfGiGaConversionService( "GiGaGeomCnvSvc" ); 
  setConverterName              ( "GiGaIsotopeCnv" ); 
}; 

///
/// destructor 
///

GiGaIsotopeCnv::~GiGaIsotopeCnv(){}; 

///
/// Class ID
///

const CLID&  GiGaIsotopeCnv::classID            () { return Isotope::classID() ; }


///
/// StorageType 
///

const unsigned char GiGaIsotopeCnv::storageType () { return GiGaGeom_StorageType; } 

///
/// Create representation 
///

StatusCode GiGaIsotopeCnv::createRep( DataObject*     Object  , IOpaqueAddress*& Address ) 
{
  ///
  Address = 0 ; 
  ///
  if( 0 == Object                 ) { return Error("CreateRep::DataObject* points to NULL"); } 
  ///
  Isotope* obj = 0 ; 
  try        { obj = dynamic_cast<Isotope*>( Object ) ; } 
  catch(...) { obj =                                        0 ; } 
  ///
  if( 0 == obj                    ) { return Error("CreateRep::Bad cast to Isotope*"); }
  ///
  
  if( 0 == cnvSvc()    ) { return Error("CreateRep::Conversion Service is unavailable"); } 
  
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = 0 ; 
  try        { addrCreator = dynamic_cast<IAddressCreator*> ( cnvSvc() ) ; } 
  catch(...) { addrCreator =                                           0 ; } 
  if( 0 == addrCreator ) { return Error("CreateRep::Address Creator is unavailable"); } 
  
  StatusCode status = addrCreator->createAddress( repSvcType() , classID() , "GiGaGeom" , "GiGaIsotopeObject" , -1 , Address );   
  ///
  if( status.isFailure() ) { return Error("CreateRep::Error in Address Creation",status); }
  ///
  if( 0 == Address       ) { return Error("CreateRep::Error Address is created"        ); }
  ///
  return updateRep( Object , Address ) ; 
  /// 
}; 

///
///
///

StatusCode GiGaIsotopeCnv::updateRep( DataObject*     Object  , IOpaqueAddress* Address ) 
{
  ///
  IOpaqueAddress* aux = 0 ; aux = Address ; /// just to please the compiler 
  ///
  ///
  { MsgStream log( msgSvc() , ConverterName() ); log << MSG::VERBOSE << "UpdateRep::start" << endreq; } 
  ///
  ///
  if( 0 == Object                 ) { return Error("UpdateRep::DataObject* points to NULL"); } 
  ///
  Isotope* isotope = 0 ; 
  try        { isotope = dynamic_cast<Isotope*>( Object ) ; } 
  catch(...) { isotope =                                        0 ; } 
  ///
  if( 0 == isotope                ) { return Error("UpdateRep::Bad cast to Isotope*"); }
  ///
  if( 0 == cnvSvc()               ) { return Error("UpdateRep::Conversion Service is unavailable"); } 
  
  ///
  /// if isotop is already converted? 
  ///
  
  if( 0 != G4Isotope::GetIsotope( isotope->fullpath() ) ) { return StatusCode::SUCCESS; } 
  
  ///
  /// Here we should create the Isotop
  ///
  const unsigned int N = 
    isotope->N() < isotope->Z() ? 
    (const unsigned int) isotope->A() / g * mole : (const unsigned int) isotope->N() ;  
  std::cerr << " check for parameters:"
	    << " \tname=" << isotope->fullpath()
	    << " \tZ=" << isotope->Z()
	    << " \tN=" << N
	    << " \tA=" << isotope->A() 
            << "\tg/mole=" << g/mole << std::endl; 
  ///
  G4Isotope* NewIsotope = 0 ; 
  NewIsotope = new G4Isotope( isotope->fullpath () , 
			      (int) isotope->Z  () , 
			      N   , 
			      //			      (int) isotope->N  () , 
			      isotope->A        () *g/mole);
  ///

  ///
  ///
  ///
  
  if( 0 != G4Material::GetMaterial( isotope->fullpath() ) ) { return StatusCode::SUCCESS; } 
  
  ///
  /// per each Isotope we could create the "simple material" with the same name
  ///
  
  G4Material* NewMaterial = 0 ; 
  NewMaterial = new G4Material( isotope->fullpath        () , 
                                isotope->Z               () , 
                                isotope->A               () * g/mole , 
                                isotope->density         () , 
				(G4State) isotope->state () ,  
				isotope->temperature     () , 
				isotope->pressure        () ); 
  
  ///
  ///  
  { MsgStream log( msgSvc() , ConverterName() ); log << MSG::VERBOSE << "UpdateRep::end" << endreq; } 
  ///
  return StatusCode::SUCCESS; 
  /// 
}; 















