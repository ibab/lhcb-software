///
///
/// from Gaudi 
#include "GaudiKernel/AddrFactory.h" 
#include "GaudiKernel/SvcFactory.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DetectorElement.h"
#include"GaudiKernel/IDataSelector.h"
#include"GaudiKernel/IConverter.h"
/// from GiGa  
#include "GiGa/GiGaGeomCnvSvc.h" 


///
///
/// Implementation of GiGaGeomCnvSvc 
///
///
/// Author: Vanya Belyaev
/// Date  : 7 Aug 2000
///

// Include G4 Stuff
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Box.hh"

///
///
///

extern unsigned char GiGaGeom_StorageType        ; 
extern const IAddrFactory& GiGaGeomAddressFactory ; 


static const  SvcFactory<GiGaGeomCnvSvc>                         s_GiGaGeomCnvSvcFactory ; 
const        ISvcFactory&                GiGaGeomCnvSvcFactory = s_GiGaGeomCnvSvcFactory ; 


///
/// constructor
///

GiGaGeomCnvSvc::GiGaGeomCnvSvc( const std::string&   ServiceName          , 
  				ISvcLocator*         ServiceLocator       ) 
  : GiGaCnvSvc(                                      ServiceName          , 
		                                     ServiceLocator       , 
						     GiGaGeom_StorageType )
  ///
{
  setAddressFactory(&GiGaGeomAddressFactory) ;
  setNameOfDataProviderSvc("DetectorDataSvc"); 
};

///
/// 

StatusCode GiGaGeomCnvSvc::initialize() { 
  StatusCode sc = GiGaCnvSvc::initialize();
  MsgStream log( msgSvc(), name() );  
  if(!sc.isSuccess()) {return Error(" Failed to initialize GiGaCnvSvc", sc);}
  // create Air in G4 to create the G4 World volume
  sc = createAir();
  if(!sc.isSuccess()) {return Error(" Failed to create Air in G4", sc);}
  // Create G4 World Volume
  sc = createG4WorldVol();
  if(!sc.isSuccess()) {return Error("Failed to create G4 World Volume", sc);}
  return StatusCode::SUCCESS;
}

StatusCode GiGaGeomCnvSvc::finalize()   { 
  delete m_G4WorldSd;
  delete m_G4WorldLV;
  delete m_G4WorldPV;
  return GiGaCnvSvc::finalize(); 
}


StatusCode GiGaGeomCnvSvc::createAir(){

  int nelem;
  double iz, a;

  std::string name, symbol;
  a = 14.01*g/mole;
  G4Element* elN = new G4Element(name="Nitrogen", symbol="N", iz=7., a);

  a = 16.00*g/mole;
  G4Element* elO = new G4Element(name="Oxygen", symbol="O", iz=8., a);

  double density = 1.29e-03*g/cm3;
  G4Material* air = new G4Material(name="air", density, nelem=2);
  air->AddElement(elN, .7);
  air->AddElement(elO, .3);
  return StatusCode::SUCCESS;
}

StatusCode GiGaGeomCnvSvc::createG4WorldVol(){
  // Initialize G4 World physical volume
  // It is a big box with no mother volume
  G4Material* air = G4Material::GetMaterial("air");
  if(air == 0){
    return Error(" Failed to create G4 World Volume because air does not exist", StatusCode::FAILURE);
  }
  try {
    m_G4WorldSd = new G4Box("G4World_box", 1000*m,1000*m,1000*m);
    m_G4WorldLV = new G4LogicalVolume(m_G4WorldSd,air, "G4World_LV", 0,0,0);
    //sc = insertLV ("G4World_LV", m_G4WorldLV);
    m_G4WorldPV = new G4PVPlacement(0,Hep3Vector(),"G4World_PV",m_G4WorldLV,0,false,0);
  } catch(...) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}




// rewrite the createReps function.
// Has to identify DetElements and subclasses of Det Elements

/// Convert a collection of transient data objects into another representation.
StatusCode GiGaGeomCnvSvc::createReps(IDataSelector* pSelector)  {
  Context::iterator       i;
  IDataSelector::iterator j;
  Context                 context;
  StatusCode              status = NO_CONVERTER;
  StatusCode              iret   = StatusCode::SUCCESS;
  // Create context
  for ( j = pSelector->begin(); j != pSelector->end(); j++ )   {
    const CLID& obj_class = (*j)->clID();
    // check if it is a Det Elem.
    try {
      DetectorElement* de = dynamic_cast<DetectorElement*>(*j);
       const CLID& obj_class = de->clID();
    } catch(...) { 
    }
    IConverter*    cnv = converter(obj_class);
    context.push_back(ContextEntry(cnv, 0, (*j)));
    if ( 0 == cnv )   {
      iret = NO_CONVERTER;
    }
  }
  // Update primitive representation
  for ( i = context.begin(); i != context.end(); i++ )   {
    ContextEntry& e = (*i);
    if ( 0 != e.converter() )  {
      status = e.converter()->createRep(e.object(), e.address());
      if ( status.isSuccess() )  {
        status = m_dataSvc->updateRegistryEntry(e.address(), e.object());
        if ( !status.isSuccess() )  {
          iret = status;
        }
      }
      else  {
        iret = status;
      }
    }
  }
  // Finally update local representation references which are unknown to the data model
  for ( i = context.begin(); i != context.end(); i++ )   {
    ContextEntry& e = (*i);
    // e.address()==0 -> representation was NOT created in the first go!
    if ( 0 != e.converter() && 0 != e.address() )  {
      status = e.converter()->fillRepRefs(e.address(), e.object());
      if ( !status.isSuccess() )  {
        iret = status;
      }
    }
  }
  return iret;
}
