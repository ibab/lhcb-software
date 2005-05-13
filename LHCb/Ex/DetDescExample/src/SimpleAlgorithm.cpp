// $Id: SimpleAlgorithm.cpp,v 1.16 2005-05-13 16:14:22 marcocle Exp $

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
  
  log << MSG::INFO << endmsg;
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
  log << MSG::INFO << "Initialization starting..." << endmsg;
  
  log << MSG::INFO
      << "///////////////////////////////////////////////////" << endmsg;
  log << MSG::INFO
      << "// Very basic test of Detector Element retrieval //" << endmsg;
  log << MSG::INFO
      << "///////////////////////////////////////////////////" << endmsg;
  SmartDataPtr<IDetectorElement> cave(detSvc(),"/dd/Structure/LHCb" );  
  // We test if the smart reference is non-zero to be sure
  // that the data object has been loaded properly
  if (!cave) {
    log << MSG::ERROR << "Can't retrieve /dd/Structure/LHCb" << endmsg;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO
      << "Top detector element of LHCb was successfully retrieved." << endmsg;
  log << MSG::INFO
      << "Here are some informations about it : " << endmsg;
  // Displays information about the volume associated with "/dd/Structure/LHCb"
  const ILVolume* stvol = cave->geometry()->lvolume();
  log << MSG::INFO << "LHCb detector is made of "
      << stvol->materialName() << endmsg;
    dumpPVs (msgSvc(), stvol, cave->name());
    

  log << MSG::INFO << "/////////////////////////" << endmsg;
  log << MSG::INFO << "// Usage of parameters //" << endmsg;
  log << MSG::INFO << "/////////////////////////" << endmsg;
  log << MSG::INFO << "This is the list of parameters " 
      << "defined on the Ecal detector element : " << endmsg;
  SmartDataPtr<IDetectorElement> ecal (detSvc(), "/dd/Structure/LHCb/Ecal");
  if (!ecal) {
    log << MSG::ERROR << "Can't retrieve /dd/Structure/LHCb/Ecal" << endmsg;
    return StatusCode::FAILURE;
  }
  std::vector<std::string> parameterList = ecal->params()->paramNames();
  std::vector<std::string>::iterator it2;
  for (it2 = parameterList.begin();
       it2 != parameterList.end();
       it2++) {
    log << MSG::INFO << *it2 << " = "
        << ecal->params()->paramToString(*it2)
        << endmsg;
  }

  
  log << MSG::INFO
      << "/////////////////////////////////////////////////////////////////"
      << endmsg;
  log << MSG::INFO
      << "// Usage of specific detector elements with generic converters //"
      << endmsg;
  log << MSG::INFO
      << "/////////////////////////////////////////////////////////////////"
      << endmsg;
  log << MSG::INFO << "In this example, the Ecal top detector element is "
      << "retrieved as a specific object (namely class DeCalorimeter). "
      << "However, a 'generic' converter is used." << endmsg;
  log << MSG::INFO << "Here are once more the parameters of the Ecal "
      << "detector element, but retrieved using directly the DeCalorimeter "
      << "object : " << endmsg;
  SmartDataPtr<DeCalorimeter> ecal2 (detSvc(), "/dd/Structure/LHCb/Ecal");
  if (!ecal2)                                                            {
    log << MSG::ERROR
        << "Can't retrieve /dd/Structure/LHCb/Ecal as DeCalorimeter" << endmsg;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Ecal->codingBit() = " << ecal2->codingBit() << endmsg;
  log << MSG::INFO << "Ecal->etInCenter() = " << ecal2->etInCenter() << endmsg;
  log << MSG::INFO << "Ecal->etSlope() = " << ecal2->etSlope() << endmsg;
  log << MSG::INFO << "Ecal->adcMax() = " << ecal2->adcMax() << endmsg;
  log << MSG::INFO << "Ecal->activeToTotal() = " << ecal2->activeToTotal()
      << endmsg;
  log << MSG::INFO << "Ecal->zShowerMax() = " << ecal2->zShowerMax() << endmsg;



  log << MSG::INFO << "/////////////////////////////////////////////////////" 
      << "//////////////////////" << endmsg;
  log << MSG::INFO
      << "// Usage of specific detector elements together with specific "
      << "converters //" << endmsg;
  log << MSG::INFO
      << "//////////////////////////////////////////////////////////////////"
      << "/////////" << endmsg;
  log << MSG::INFO << "In this example, one of the Muon Region is "
      << "retrieved as a specific object (namely class DeMuonRegion) by"
      << "using a specific converter (namely class XmlMuonRegionCnv). This "
      << "provides access to data contained in the <specific> tag of the "
      << "element" << endmsg;
  log << MSG::INFO << "Here are some parameters of the Muon Region detector "
      << "element :" << endmsg;
  SmartDataPtr<DeMuonRegion> region (detSvc(), "/dd/Structure/LHCb/Muon/M1/R1");
  if (!region) {
    log << MSG::ERROR << "Can't retrieve /dd/Structure/LHCb/Muon/M1/R1"
        << endmsg;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "region->chamberNum() = " << region->chamberNum()
      << endmsg;
  log << MSG::INFO << "region->gapsPerFE() = " << region->gapsPerFE() << endmsg;
  log << MSG::INFO << "region->FEAnodeX() = " << region->FEAnodeX() << endmsg;
  log << MSG::INFO << "region->FEAnodeY() = " << region->FEAnodeY() << endmsg;
  log << MSG::INFO << "region->FECathodeX() = " << region->FECathodeX()
      << endmsg;
  log << MSG::INFO << "region->FECathodeY() = " << region->FECathodeY()
      << endmsg;

 
  log << MSG::INFO << "///////////////////////" << endmsg;
  log << MSG::INFO << "// Testing Materials //" << endmsg;
  log << MSG::INFO << "///////////////////////" << endmsg;
  log << MSG::INFO << "Simple element : Oxygen" << endmsg;
  SmartDataPtr<Material> elO( detSvc(), "/dd/Materials/Oxygen" );
  if (!elO) {
    log << MSG::ERROR << "Can't retrieve /dd/Materials/Oxygen!" << endmsg;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Material: " << elO->name() << endmsg;
  log << MSG::INFO << "A       : " << elO->A()/(g/mole)
      << " g/mole" << endmsg;
  log << MSG::INFO << "Z       : " << elO->Z() << endmsg;
  log << MSG::INFO << "Density : " << elO->density()/(g/cm3)
      << " g/cm3" << endmsg;
  log << MSG::INFO << "X0      : " << elO->radiationLength()/(cm)
      << " cm" << endmsg;
  log << MSG::INFO << "Lambda  : " << elO->absorptionLength()/(cm)
      << " cm" << endmsg;
  log << MSG::INFO << endmsg;
  
  log << MSG::INFO << "Material composed of several isotopes : Boron" << endmsg;
  SmartDataPtr<Material> elB( detSvc(), "/dd/Materials/Boron" );
  if( !elB )                                                               {
    log << MSG::ERROR << "Can't retrieve /dd/Materials/Boron!" << endmsg;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Material: " << elB->name() << endmsg;
  log << MSG::INFO << "A       : " << elB->A()/(g/mole) << " g/mole"
      << endmsg;
  log << MSG::INFO << "Z       : " << elB->Z() << endmsg;
  log << MSG::INFO << "Density : " << elB->density()/(g/cm3) << " g/cm3"
      << endmsg;
  log << MSG::INFO << "X0      : " << elB->radiationLength()/(cm) << " cm"
      << endmsg;
  log << MSG::INFO << "Lambda  : " << elB->absorptionLength()/(cm) << " cm"
      << endmsg;
  log << MSG::INFO << endmsg;
  
  log << MSG::INFO << "Mixture of simple elements : Water" << endmsg;
  SmartDataPtr<Material> mWater( detSvc(), "/dd/Materials/Water" );
  if( !mWater ) {
    log << MSG::ERROR << "Can't retrieve /dd/Materials/Water!" << endmsg;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Material: " << mWater->name() << endmsg;
  log << MSG::INFO << "A       : " << mWater->A()/(g/mole) << " g/mole"
      << endmsg;
  log << MSG::INFO << "Z       : " << mWater->Z() << endmsg;
  log << MSG::INFO << "Density : " << mWater->density()/(g/cm3) << " g/cm3"
      << endmsg;
  log << MSG::INFO << "X0      : " << mWater->radiationLength()/(cm)
      << " cm"  << endmsg;
  log << MSG::INFO << "Lambda  : " << mWater->absorptionLength()/(cm)
      << " cm"  << endmsg;
  log << MSG::INFO << endmsg;
  
  log << MSG::INFO << "Mixture of mixtures : Argon_CF4_CO2" << endmsg;
  SmartDataPtr<Material> mArCF4CO2( detSvc(), "/dd/Materials/Argon_CF4_CO2" );
  if (!mArCF4CO2) {
    log << MSG::ERROR << "Can't retrieve /dd/Materials/Argon_CF4_CO2!"
        << endmsg;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Material: " << mArCF4CO2->name() << endmsg;
  log << MSG::INFO << "A       : " << mArCF4CO2->A()/(g/mole)
      << " g/mole"  << endmsg;
  log << MSG::INFO << "Z       : " << mArCF4CO2->Z() << endmsg;
  log << MSG::INFO << "Density : " << mArCF4CO2->density()/(g/cm3)
      << " g/cm3"  << endmsg;
  log << MSG::INFO << "X0      : " << mArCF4CO2->radiationLength()/(cm)
      << " cm"  << endmsg;
  log << MSG::INFO << "Lambda  : " << mArCF4CO2->absorptionLength()/(cm)
      << " cm"  << endmsg;
  log << MSG::INFO << endmsg;


  log << MSG::INFO << "///////////////////////////////////////////" << endmsg;
  log << MSG::INFO << "// Testing the TabulatedProperty feature //" << endmsg;
  log << MSG::INFO << "///////////////////////////////////////////" << endmsg;
  std::string path = "/dd/Geometry/Rich1/Rich1SurfaceTabProperties/";
  path += "Rich1Mirror1SurfaceReflectivityPT";
  SmartDataPtr<TabulatedProperty> tab (detSvc(), path);
  if (!tab) {
    log << MSG::ERROR
        << "Can't retrieve /dd/Geometry/Rich1/Rich1SurfaceTabProperties"
        << "/Rich1Mirror1SurfaceReflectivityPT" << endreq;
    return StatusCode::FAILURE;
  }  
  TabulatedProperty::Table table = tab->table();
  TabulatedProperty::Table::iterator it;
  for (it = table.begin();
       it != table.end();
       it++) {
    log << MSG::INFO << "new table Entry : "
        << "x = " << it->first << ", y = " << it->second << endmsg;
  }
  return StatusCode::SUCCESS;
}


/////////////
// Execute //
/////////////
StatusCode SimpleAlgorithm::execute() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "execute" << endmsg;
  return StatusCode::SUCCESS;
}


//////////////
// Finalize //
//////////////
StatusCode SimpleAlgorithm::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "///////////////////////////////////////////////"
      << endmsg;
  log << MSG::INFO << "//   View of the Data Store at finalization  //"
      << endmsg;
  log << MSG::INFO << "///////////////////////////////////////////////"
      << endmsg;
  DetDataAgent agent (msgSvc());

  IDataManagerSvc* storeManager = 0;
  StatusCode sc = service("DetectorDataSvc", storeManager);
  if (sc.isFailure()) {
    log << MSG::ERROR << "execute: Can't query IDataManagerSvc inteface..."
        << endmsg;
    return sc;
  }

  /// Let's traverse the transient detector data store
  storeManager->traverseTree( &agent );

  // Release the service when no longer needed
  storeManager->release();
  
  
  return StatusCode::SUCCESS;
}
