#include "GaudiKernel/CnvFactory.h" 
#include "GaudiKernel/IAddressCreator.h" 
#include "GaudiKernel/IOpaqueAddress.h" 
#include "GaudiKernel/IGiGaSetUpSvc.h" 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataSelector.h"

#include "DetDesc/Element.h"
#include "DetDesc/Isotope.h"

#include "GiGa/GiGaException.h" 
#include "GiGa/GiGaElementCnv.h" 

///
/// Geant4 includes
///

#include "G4Element.hh" 
#include "G4Material.hh" 


extern unsigned char GiGaGeom_StorageType ; 

static const  CnvFactory<GiGaElementCnv>                                      s_GiGaElementCnvFactory ;
const        ICnvFactory&                             GiGaElementCnvFactory = s_GiGaElementCnvFactory ;

///
/// constructor 
///

GiGaElementCnv::GiGaElementCnv( ISvcLocator* Locator ) 
  : GiGaCnv( storageType() , classID() , Locator ) 
{
  setNameOfGiGaConversionService( "GiGaGeomCnvSvc" ); 
  setConverterName              ( "GiGaElementCnv" );
}; 

///
/// destructor 
///

GiGaElementCnv::~GiGaElementCnv(){}; 

///
/// Class ID
///

const CLID&  GiGaElementCnv::classID            () { return Element::classID() ; }


///
/// StorageType 
///

const unsigned char GiGaElementCnv::storageType () { return GiGaGeom_StorageType; } 

///
/// Create representation 
///

StatusCode GiGaElementCnv::createRep( DataObject*     Object  , IOpaqueAddress*& Address ) 
{
  ///
  Address = 0 ; 
  ///
  if( 0 == Object        ) { return Error("CreateRep::DataObject* point to NULL!");} 
  ///
  ///
  Element* obj = 0 ; 
  try        { obj = dynamic_cast<Element*>( Object ) ; } 
  catch(...) { obj =                                0 ; } 
  ///
  if( 0 == obj           ) { return Error("CreateRep::Bad cast to Element*");}
  ///
  if( 0 == cnvSvc()      ) { return Error("CreateRep::Conversion Service is unavailble ") ;} 
  ///
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = 0 ; 
  try        { addrCreator = dynamic_cast<IAddressCreator*> ( cnvSvc() ) ; } 
  catch(...) { addrCreator =                                           0 ; } 
  ///
  if( 0 == addrCreator   ) { return Error("CreateRep::Address Creator is unavailable   ") ;} 
  ///
  StatusCode status = addrCreator->createAddress( repSvcType() , classID() , "GiGaGeom" , "GiGaElementObject" , -1 , Address );   
  ///
  if( status.isFailure() ) { return Error("CreateRep::Error in Address Creator"); }
  ///
  if( 0 == Address       ) { return Error("CreateRep::Address is not created" ); }   
  ///
  return updateRep( Object , Address ) ; 
  /// 
}; 

///
///
///

StatusCode GiGaElementCnv::updateRep( DataObject*     Object  , IOpaqueAddress* Address ) 
{
  ///
  { MsgStream log( msgSvc() , ConverterName() ); log << MSG::VERBOSE << "UpdateRep::start" << endreq; } 
  ///
  IOpaqueAddress* aux = 0 ; aux = Address ; /// just to please the compiler
  ///
  if( 0 == Object   ) { return Error("UpdateRep::DataObject* points to NULL"); } 
  ///
  Element* element = 0 ; 
  try        { element = dynamic_cast<Element*>(Object) ; } 
  catch(...) { element =                                    0 ; } 
  ///
  if( 0 == element ) { return Error("UpdateRep::Bad cast to Element*");} 
  ///
  

  ///
  /// Check if Element is already known to Geant4 
  ///
  
  if( 0 != G4Element::GetElement( element->fullpath() ) ) { return StatusCode::SUCCESS; } 
  
  ///
  /// Here we should create the element 
  ///
  
  G4Element*  NewElement = 0 ; 
  if( 0 == element->nOfIsotopes() )                                 /// simple element 
    { NewElement = new G4Element( element->fullpath () , 
				  element->name     () , 
				  element->Z        () , 
				  element->A        () ); } 
  else                                                              /// compound element (from isotopes)
    {                                                               
      
      ///
      /// convert all isotopes first
      ///
      {
	IDataSelector dataSelector; 
	for( unsigned int index = 0 ; index < (unsigned int) element->nOfIsotopes() ; ++index ) 
	  { 
	    Isotope* isotope = element->isotope( index ); 
	    if( 0 != isotope ) { dataSelector.push_back( isotope );} 
	  } 
	///
        if( dataSelector.size() != (unsigned int) element->nOfIsotopes() )
	  { return Error("UpdateRep::wrong Isotope number for Element="+element->fullpath()); } 
	///
	StatusCode status = cnvSvc()->createReps( &dataSelector );  
	if( status.isFailure() ) { return Error("UpdateRep::could not convert isotopes for "+element->fullpath(),status); } 
	///
      }      
      ///
      ///
      /// 
      NewElement = new G4Element( element->fullpath() , element->name() , element->nOfIsotopes() ) ;
      for( unsigned int index = 0 ; index < (unsigned int) element->nOfIsotopes() ; ++index ) 
	{
	  G4Isotope* isotope = G4Isotope::GetIsotope( element->isotope( index )->fullpath() ); 
          if( 0 == isotope ) 
	    { return Error("UpdateRep::could not extract isotope="+element->isotope( index )->fullpath() ); }
          double fraction = 0.10 ; // NB!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  NewElement->AddIsotope( isotope , fraction ); // 
	}
    }
  

  ///
  /// Check if Material is already known to Geant4
  ///
  
  if( 0 != G4Material::GetMaterial( element->fullpath() ) ) { return StatusCode::SUCCESS; } 
  
  ///
  /// Here we could define simple materials 
  ///
  
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
      /// convert all isotopes first
      ///
      {
	IDataSelector dataSelector; 
	for( unsigned int index = 0 ; index < (unsigned int) element->nOfIsotopes() ; ++index ) 
	  { 
	    Isotope* isotope = element->isotope( index ); 
	    if( 0 != isotope ) { dataSelector.push_back( isotope );} 
	  } 
	///
        if( dataSelector.size() != (unsigned int) element->nOfIsotopes() )
	  { return Error("UpdateRep::wrong Isotope number for Element="+element->fullpath()); } 
	///
	StatusCode status = cnvSvc()->createReps( &dataSelector );  
	if( status.isFailure() ) { return Error("UpdateRep::could not convert isotopes for "+element->fullpath(),status); }
	/// 
      }      
      ///
      ///
      /// 
      NewMaterial = new G4Material( element->fullpath        () , 
				    element->density         () , 
				    element->nOfIsotopes     () ,
				    (G4State) element->state () ,  
				    element->temperature     () , 
				    element->pressure        () ); 
      
      for( unsigned int index = 0 ; index < (unsigned int) element->nOfIsotopes() ; ++index ) 
	{
	  G4Element* el = G4Element::GetElement(element->isotope( index )->fullpath() ); 
          if( 0 == el ) 
	    { return Error("UpdateRep::could not extract element="+element->isotope( index )->fullpath() ); }
          double fraction = 0.10 ; // NB!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  NewMaterial->AddElement( el , fraction ); // 
	}
    }  
  ///
  ///
  ///
  { MsgStream log( msgSvc() , ConverterName() ); log << MSG::VERBOSE << "UpdateRep::end" << endreq; } 
  ///
  ///
  return StatusCode::SUCCESS; 
  /// 
}; 


















