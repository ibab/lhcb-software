// $Id: GiGaElementCnv.cpp,v 1.8 2009-10-15 10:00:43 silviam Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2007/01/12 15:45:56  ranjard
// v18r0 - use GAUDI v19r0
//
// Revision 1.6  2002/12/07 14:36:26  ibelyaev
//  see $GIGACNVROOT/doc/release.notes
//
// Revision 1.5  2002/01/22 18:24:43  ibelyaev
//  Vanya: update for newer versions of Geant4 and Gaudi
//
// Revision 1.4  2001/08/12 17:24:52  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.3  2001/07/15 20:45:10  ibelyaev
// the package restructurisation
// 
// ============================================================================
#define GIGACNV_GIGAELEMENTCNV_CPP 1 
// ============================================================================
#include "GaudiKernel/CnvFactory.h" 
#include "GaudiKernel/IAddressCreator.h" 
#include "GaudiKernel/IOpaqueAddress.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataSelector.h"
// DetDesc
#include "DetDesc/Element.h"
#include "DetDesc/Isotope.h"
// GiGa 
#include "GiGa/GiGaException.h" 
// GiGaCnv
#include "GiGa/IGiGaSetUpSvc.h" 
#include "GiGaCnv/GiGaCnvUtils.h" 
/// Geant4 includes
#include "G4Element.hh" 
#include "G4Material.hh" 
// Local
#include "AddTabulatedProperties.h" 
#include "GiGaElementCnv.h" 


// ============================================================================
DECLARE_CONVERTER_FACTORY( GiGaElementCnv )
// ============================================================================

// ============================================================================
/// constructor 
// ============================================================================
GiGaElementCnv::GiGaElementCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
  , m_leaf     ( "" , classID() )
{
  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Geo ) ; 
  setConverterName              ( "GiGaElementCnv"         ) ;
}
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaElementCnv::~GiGaElementCnv(){}

// ============================================================================
/// Class ID
// ============================================================================
const CLID&  GiGaElementCnv::classID () { return Element::classID() ; }
// ============================================================================

// ============================================================================
/// StorageType 
// ============================================================================
unsigned char GiGaElementCnv::storageType ()
{ return GiGaGeom_StorageType; } 
// ============================================================================

// ============================================================================
/// Create representation 
// ============================================================================
StatusCode GiGaElementCnv::createRep
( DataObject*      Object  , 
  IOpaqueAddress*& Address ) 
{
  Address = 0 ; 
  if( 0 == Object        ) 
    { return Error("CreateRep::DataObject* point to NULL!");} 
  Element* obj = 0 ; 
  try        { obj = dynamic_cast<Element*>( Object ) ; } 
  catch(...) { obj =                                0 ; } 
  if( 0 == obj           ) 
    { return Error("CreateRep::Bad cast to Element*");}
  if( 0 == cnvSvc()      )
    { return Error("CreateRep::Conversion Service is unavailble ") ;} 
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = addressCreator() ;
  if( 0 == addrCreator   ) 
    { return Error("CreateRep::Address Creator is unavailable   ") ;} 
  StatusCode status = 
    addrCreator->createAddress( repSvcType  () , 
                                classID     () , 
                                m_leaf.par  () , 
                                m_leaf.ipar () , 
                                Address        );
  if( status.isFailure() )
    { return Error("CreateRep::Error in Address Creator"); }
  if( 0 == Address       ) 
    { return Error("CreateRep::Address is not created" ); }   
  ///
  return updateRep( Object , Address ) ; 
  /// 
}
// ============================================================================

// ============================================================================
///
// ============================================================================
StatusCode GiGaElementCnv::updateRep
( DataObject*      Object       , 
  IOpaqueAddress* /* Address */) 
{
  ///
  typedef Element::Isotopes::iterator Iterator;
  ///
  { MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << "UpdateRep::start" << endreq; } 
  ///
  if( 0 == Object   ) 
    { return Error("UpdateRep::DataObject* points to NULL"); } 
  /// extract the name of the element 
  const std::string& elementPath = Object->registry()->identifier();
  Element* element = 0 ; 
  try        { element = dynamic_cast<Element*>(Object) ; } 
  catch(...) { element =                                    0 ; } 
  if( 0 == element )
    { return Error("UpdateRep::Bad cast to Element*");} 
  /// Check if Element is already known to Geant4 
  G4bool warning = false;
  if( 0 != G4Element::GetElement( elementPath , warning ) ) 
    { return StatusCode::SUCCESS; } 
  
  /// create isotopes first
  {
    for( Iterator it = element->isotopes().begin() ; 
         element->isotopes().end() != it ; ++it )
      { 
        StatusCode sc = GiGaCnvUtils::createRep( cnvSvc() , it->second ) ;
        if( sc.isFailure()   ) 
          { return Error("updateRep:: could not convert isotopes for "
                         + elementPath );}
      }
    
  }
  
  /// Here we should create the element 
  G4Element*  NewElement = 0 ; 
  if( 0 == element->nOfIsotopes() ) /// simple element 
    { NewElement = new G4Element( elementPath      , 
                                  element->name () , 
                                  element->Z    () , 
                                  element->A    () ); } 
  else                        /// compound element (from isotopes)
    {                                                               
      /// 
      NewElement = new G4Element( elementPath             , 
                                  element->name        () , 
                                  element->nOfIsotopes () ) ;
      for( Iterator it = element->isotopes().begin() ;
           element->isotopes().end() != it ; ++it )
        {
          G4Isotope* isotope = 
            G4Isotope::GetIsotope( it->second->registry()->identifier() ); 
          if( 0 == isotope ) 
            { return Error("UpdateRep::could not extract isotope=" + 
                           it->second->registry()->identifier() ); }
          NewElement->AddIsotope( isotope , it->first ); // 
        } 
    }
  
  /// Check if Material is already known to Geant4
  
  if( 0 != G4Material::GetMaterial( elementPath , warning ) )
    { return StatusCode::SUCCESS; } 
  
  /// Here we could define simple materials 
  G4Material*  NewMaterial = 0 ; 
  if( 0 == element->nOfIsotopes() ) 
    { 
      NewMaterial = new G4Material( elementPath                 , 
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
      NewMaterial = new G4Material( elementPath                 , 
                                    element->density         () , 
                                    element->nOfIsotopes     () ,
                                    (G4State) element->state () ,  
                                    element->temperature     () , 
                                    element->pressure        () ); 
      for( Iterator it = element->isotopes().begin() ; 
           element->isotopes().end() != it ; ++it )
        {
          G4Element* el = 
            G4Element::GetElement( it->second->registry()->identifier() ); 
          if( 0 == el ) 
            { return Error("UpdateRep::could not extract element=" + 
                           it->second->registry()->identifier() ); }
          NewMaterial->AddElement( el , it->first ); // 
        } 
    }  
  /// add tabulated properties
  if( !element->tabulatedProperties().empty() )
    {
      if( 0 == NewMaterial->GetMaterialPropertiesTable() )
        { NewMaterial->
            SetMaterialPropertiesTable( new G4MaterialPropertiesTable() ); }
      StatusCode sc = 
        AddTabulatedProperties ( element->tabulatedProperties()  ,
                                 NewMaterial->GetMaterialPropertiesTable() ) ;
      if( sc.isFailure() )
        { return Error("UpdateRep::could not add TabulatedProperties for " + 
                       elementPath , sc  ); } 
    }
  ///
  { MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << "UpdateRep::end" << endreq; } 
  ///
  return StatusCode::SUCCESS; 
  /// 
}


// ============================================================================
// End 
// ============================================================================
















