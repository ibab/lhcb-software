// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/SimpleAlgorithm.cpp,v 1.13 2002-04-29 17:05:54 sponce Exp $
#define DDEXAMPLE_SIMPLEALGORITHM_CPP

/// Include files

/// Gaudi interfaces
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/RegistryEntry.h"

/// Utility classes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/AlgFactory.h"

/// Detector description classes
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/TabulatedProperty.h"

/// Private classes to the example
#include "SimpleAlgorithm.h"
#include "DeCalorimeter.h"
#include "DeMuonRegion.h"
#include "DetDataAgent.h"

#include "CLHEP/Geometry/Transform3D.h"

static const AlgFactory<SimpleAlgorithm>  Factory;
const IAlgFactory& SimpleAlgorithmFactory = Factory;

/**
 * Displays the list of existing physical volumes in lv
 * @param ms the Message Service where to display the list
 * @param lv the logical volume concerned
 * @param de the name of the detector element associated to lv
 */
void dumpPVs (IMessageSvc* ms, const ILVolume* lv, std::string de) {
  MsgStream log(ms, "dumpPVs");

  unsigned long noppv = lv->noPVolumes();
  log << MSG::INFO << de << " log. volume has " 
      << noppv    << " phys. volumes: ";

  unsigned int ppvc;
  for (ppvc = 0; ppvc < noppv; ppvc++) {
    log << MSG::INFO << lv->pvolume(ppvc)->name() << " ";
  }
  
  log << MSG::INFO << endreq;
}

/////////////////////////
// Default constructor //
/////////////////////////
SimpleAlgorithm::SimpleAlgorithm (const std::string& name,
                                  ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {}


////////////////
// Initialize //
////////////////
StatusCode SimpleAlgorithm::initialize() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialization starting..." << endreq;
  
  log << MSG::INFO
      << "///////////////////////////////////////////////////" << endreq;
  log << MSG::INFO
      << "// Very basic test of Detector Element retrieval //" << endreq;
  log << MSG::INFO
      << "///////////////////////////////////////////////////" << endreq;
  SmartDataPtr<IDetectorElement> cave(detSvc(),"/dd/Structure/LHCb" );  
  // We test if the smart reference is non-zero to be sure
  // that the data object has been loaded properly
  if (!cave) {
    log << MSG::ERROR << "Can't retrieve /dd/Structure/LHCb" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO
      << "Top detector element of LHCb was successfully retrieved." << endreq;
  log << MSG::INFO
      << "Here are some informations about it : " << endreq;
  // Displays information about the volume associated with "/dd/Structure/LHCb"
  const ILVolume* stvol = cave->geometry()->lvolume();
  log << MSG::INFO << "LHCb detector is made of "
      << stvol->materialName() << endreq;
    dumpPVs (msgSvc(), stvol, cave->name());
    

  log << MSG::INFO << "/////////////////////////////" << endreq;
  log << MSG::INFO << "// Usage of userParameters //" << endreq;
  log << MSG::INFO << "/////////////////////////////" << endreq;
  log << MSG::INFO << "This is the list of userParameters " 
      << "defined on the Ecal detector element : " << endreq;
  SmartDataPtr<IDetectorElement> ecal (detSvc(), "/dd/Structure/LHCb/Ecal");
  if (!ecal) {
    log << MSG::ERROR << "Can't retrieve /dd/Structure/LHCb/Ecal" << endreq;
    return StatusCode::FAILURE;
  }
  std::vector<std::string> parameterList = ecal->userParameters();
  std::vector<std::string>::iterator it2;
  for (it2 = parameterList.begin();
       it2 != parameterList.end();
       it2++) {
    log << MSG::INFO << *it2 << " = "
        << ecal->userParameterAsString(*it2)
        << endreq;
  }

  
  log << MSG::INFO
      << "/////////////////////////////////////////////////////////////////"
      << endreq;
  log << MSG::INFO
      << "// Usage of specific detector elements with generic converters //"
      << endreq;
  log << MSG::INFO
      << "/////////////////////////////////////////////////////////////////"
      << endreq;
  log << MSG::INFO << "In this example, the Ecal top detector element is "
      << "retrieved as a specific object (namely class DeCalorimeter). "
      << "However, a 'generic' converter is used." << endreq;
  log << MSG::INFO << "Here are once more the parameters of the Ecal "
      << "detector element, but retrieved using directly the DeCalorimeter "
      << "object : " << endreq;
  SmartDataPtr<DeCalorimeter> ecal2 (detSvc(), "/dd/Structure/LHCb/Ecal");
  if (!ecal2)                                                            {
    log << MSG::ERROR
        << "Can't retrieve /dd/Structure/LHCb/Ecal as DeCalorimeter" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Ecal->codingBit() = " << ecal2->codingBit() << endreq;
  log << MSG::INFO << "Ecal->etInCenter() = " << ecal2->etInCenter() << endreq;
  log << MSG::INFO << "Ecal->etSlope() = " << ecal2->etSlope() << endreq;
  log << MSG::INFO << "Ecal->adcMax() = " << ecal2->adcMax() << endreq;
  log << MSG::INFO << "Ecal->activeToTotal() = " << ecal2->activeToTotal()
      << endreq;
  log << MSG::INFO << "Ecal->zShowerMax() = " << ecal2->zShowerMax() << endreq;



  log << MSG::INFO << "/////////////////////////////////////////////////////" 
      << "//////////////////////" << endreq;
  log << MSG::INFO
      << "// Usage of specific detector elements together with specific "
      << "converters //" << endreq;
  log << MSG::INFO
      << "//////////////////////////////////////////////////////////////////"
      << "/////////" << endreq;
  log << MSG::INFO << "In this example, one of the Muon Region is "
      << "retrieved as a specific object (namely class DeMuonRegion) by"
      << "using a specific converter (namely class XmlMuonRegionCnv). This "
      << "provides access to data contained in the <specific> tag of the "
      << "element" << endreq;
  log << MSG::INFO << "Here are some parameters of the Muon Region detector "
      << "element :" << endreq;
  SmartDataPtr<DeMuonRegion> region (detSvc(), "/dd/Structure/LHCb/Muon/M1/R1");
  if (!region) {
    log << MSG::ERROR << "Can't retrieve /dd/Structure/LHCb/Muon/M1/R1"
        << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "region->chamberNum() = " << region->chamberNum()
      << endreq;
  log << MSG::INFO << "region->gapsPerFE() = " << region->gapsPerFE() << endreq;
  log << MSG::INFO << "region->FEAnodeX() = " << region->FEAnodeX() << endreq;
  log << MSG::INFO << "region->FEAnodeY() = " << region->FEAnodeY() << endreq;
  log << MSG::INFO << "region->FECathodeX() = " << region->FECathodeX()
      << endreq;
  log << MSG::INFO << "region->FECathodeY() = " << region->FECathodeY()
      << endreq;

 
  log << MSG::INFO << "///////////////////////" << endreq;
  log << MSG::INFO << "// Testing Materials //" << endreq;
  log << MSG::INFO << "///////////////////////" << endreq;
  log << MSG::INFO << "Simple element : Oxygen" << endreq;
  SmartDataPtr<Material> elO( detSvc(), "/dd/Materials/Oxygen" );
  if (!elO) {
    log << MSG::ERROR << "Can't retrieve /dd/Materials/Oxygen!" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Material: " << elO->name() << endreq;
  log << MSG::INFO << "A       : " << elO->A()/(g/mole)
      << " g/mole" << endreq;
  log << MSG::INFO << "Z       : " << elO->Z() << endreq;
  log << MSG::INFO << "Density : " << elO->density()/(g/cm3)
      << " g/cm3" << endreq;
  log << MSG::INFO << "X0      : " << elO->radiationLength()/(cm)
      << " cm" << endreq;
  log << MSG::INFO << "Lambda  : " << elO->absorptionLength()/(cm)
      << " cm" << endreq;
  log << MSG::INFO << endreq;
  
  log << MSG::INFO << "Material composed of several isotopes : Boron" << endreq;
  SmartDataPtr<Material> elB( detSvc(), "/dd/Materials/Boron" );
  if( !elB )                                                               {
    log << MSG::ERROR << "Can't retrieve /dd/Materials/Boron!" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Material: " << elB->name() << endreq;
  log << MSG::INFO << "A       : " << elB->A()/(g/mole) << " g/mole"
      << endreq;
  log << MSG::INFO << "Z       : " << elB->Z() << endreq;
  log << MSG::INFO << "Density : " << elB->density()/(g/cm3) << " g/cm3"
      << endreq;
  log << MSG::INFO << "X0      : " << elB->radiationLength()/(cm) << " cm"
      << endreq;
  log << MSG::INFO << "Lambda  : " << elB->absorptionLength()/(cm) << " cm"
      << endreq;
  log << MSG::INFO << endreq;
  
  log << MSG::INFO << "Mixture of simple elements : Water" << endreq;
  SmartDataPtr<Material> mWater( detSvc(), "/dd/Materials/Water" );
  if( !mWater ) {
    log << MSG::ERROR << "Can't retrieve /dd/Materials/Water!" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Material: " << mWater->name() << endreq;
  log << MSG::INFO << "A       : " << mWater->A()/(g/mole) << " g/mole"
      << endreq;
  log << MSG::INFO << "Z       : " << mWater->Z() << endreq;
  log << MSG::INFO << "Density : " << mWater->density()/(g/cm3) << " g/cm3"
      << endreq;
  log << MSG::INFO << "X0      : " << mWater->radiationLength()/(cm)
      << " cm"  << endreq;
  log << MSG::INFO << "Lambda  : " << mWater->absorptionLength()/(cm)
      << " cm"  << endreq;
  log << MSG::INFO << endreq;
  
  log << MSG::INFO << "Mixture of mixtures : Argon_CF4_CO2" << endreq;
  SmartDataPtr<Material> mArCF4CO2( detSvc(), "/dd/Materials/Argon_CF4_CO2" );
  if (!mArCF4CO2) {
    log << MSG::ERROR << "Can't retrieve /dd/Materials/Argon_CF4_CO2!"
        << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "\nMaterial: " << mArCF4CO2->name() << endreq;
  log << MSG::INFO << "A       : " << mArCF4CO2->A()/(g/mole)
      << " g/mole"  << endreq;
  log << MSG::INFO << "Z       : " << mArCF4CO2->Z() << endreq;
  log << MSG::INFO << "Density : " << mArCF4CO2->density()/(g/cm3)
      << " g/cm3"  << endreq;
  log << MSG::INFO << "X0      : " << mArCF4CO2->radiationLength()/(cm)
      << " cm"  << endreq;
  log << MSG::INFO << "Lambda  : " << mArCF4CO2->absorptionLength()/(cm)
      << " cm"  << endreq;
  log << MSG::INFO << endreq;


  log << MSG::INFO << "///////////////////////////////////////////" << endreq;
  log << MSG::INFO << "// Testing the TabulatedProperty feature //" << endreq;
  log << MSG::INFO << "///////////////////////////////////////////" << endreq;
  std::string path = "/dd/Geometry/Rich1/Rich1SurfaceTabProperties/";
  path += "Rich1MirrorSurfaceReflectivityPT";
  SmartDataPtr<TabulatedProperty> tab
    (detSvc(), path);
  if (!tab) {
    log << MSG::ERROR
        << "Can't retrieve /dd/Geometry/Rich1/Rich1Surfaces"
        << "/MirrorSurfaceReflectivityPT" << endreq;
    return StatusCode::FAILURE;
  }  
  TabulatedProperty::Table table = tab->table();
  TabulatedProperty::Table::iterator it;
  for (it = table.begin();
       it != table.end();
       it++) {
    log << MSG::INFO << "new table Entry : "
        << "x = " << it->first << ", y = " << it->second << endreq;
  }
  return StatusCode::SUCCESS;
}


/////////////
// Execute //
/////////////
StatusCode SimpleAlgorithm::execute() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "execute" << endreq;
  return StatusCode::SUCCESS;
}


//////////////
// Finalize //
//////////////
StatusCode SimpleAlgorithm::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "///////////////////////////////////////////////"
      << endreq;
  log << MSG::INFO << "// View of the Data Store from the finalize  //"
      << endreq;
  log << MSG::INFO << "///////////////////////////////////////////////"
      << endreq;
  DetDataAgent agent (msgSvc());

  IDataManagerSvc* storeManager = 0;
  StatusCode sc = service("DetectorDataSvc", storeManager);
  if (sc.isFailure()) {
    log << MSG::ERROR << "execute: Can't query IDataManagerSvc inteface..."
        << endreq;
    return sc;
  }

  /// Let's traverse the transient detector data store
  storeManager->traverseTree( &agent );
  
  return StatusCode::SUCCESS;
}
