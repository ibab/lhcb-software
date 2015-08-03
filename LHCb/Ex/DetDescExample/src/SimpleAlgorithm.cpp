// $Id: SimpleAlgorithm.cpp,v 1.23 2008-10-28 09:55:58 cattanem Exp $

/// Include files
#include <vector>

/// Gaudi interfaces
#include "GaudiKernel/IDataManagerSvc.h"

/// Detector description classes
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/TabulatedProperty.h"

/// Sub-detector detector elements
#include "CaloDet/DeCalorimeter.h"

/// Private classes to the example
#include "SimpleAlgorithm.h"
#include "DetDataAgent.h"

using namespace Gaudi::Units;

DECLARE_ALGORITHM_FACTORY( SimpleAlgorithm )

/////////////////////////
// Default constructor //
/////////////////////////
SimpleAlgorithm::SimpleAlgorithm (const std::string& name,
                                  ISvcLocator* pSvcLocator) :
  GaudiAlgorithm(name, pSvcLocator) {}


////////////////
// Initialize //
////////////////
StatusCode SimpleAlgorithm::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  info() << "Initialization starting..." << endmsg;
  info() << "///////////////////////////////////////////////////" << endmsg;
  info() << "// Very basic test of Detector Element retrieval //" << endmsg;
  info() << "///////////////////////////////////////////////////" << endmsg;

  IDetectorElement* cave = getDet<IDetectorElement>( "/dd/Structure/LHCb" );
  // No need to test validity of pointer, getDet throws exception if it fails

  info() << "Top detector element of LHCb was successfully retrieved." << endmsg;
  info() << "Here are some informations about it : " << endmsg;

  // Displays information about the volume associated with "/dd/Structure/LHCb"
  const ILVolume* stvol = cave->geometry()->lvolume();
  info() << "LHCb detector is made of " << stvol->materialName() << endmsg;

  dumpPVs (stvol, cave->name());

  info() << "/////////////////////////" << endmsg;
  info() << "// Usage of parameters //" << endmsg;
  info() << "/////////////////////////" << endmsg;
  info() << "This is the list of parameters "
         << "defined on the Ecal detector element : \n";

  IDetectorElement* ecal = getDet<IDetectorElement>( DeCalorimeterLocation::Ecal );

  info() << ecal->params()->printParams() << endmsg;

  info()
      << "/////////////////////////////////////////////////////////////////"
      << endmsg;
  info()
      << "// Usage of specific detector elements with generic converters //"
      << endmsg;
  info()
      << "/////////////////////////////////////////////////////////////////"
      << endmsg;
  info() << "In this example, the Ecal top detector element is "
         << "retrieved as a specific object (namely class DeCalorimeter). "
         << "However, a 'generic' converter is used." << endmsg;
  info() << "Here are once more the parameters of the Ecal "
         << "detector element, but retrieved using directly the DeCalorimeter "
         << "object : " << endmsg;

  DeCalorimeter* ecal2 = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );

  info() << "Ecal->maxEtInCenter(Outer) = " << ecal2->maxEtInCenter(0) << endmsg;
  info() << "Ecal->maxEtSlope(Outer) = "    << ecal2->maxEtSlope(0)    << endmsg;
  info() << "Ecal->adcMax() = "        << ecal2->adcMax()        << endmsg;
  info() << "Ecal->activeToTotal() = " << ecal2->activeToTotal() << endmsg;
  info() << "Ecal->zShowerMax() = "    << ecal2->zShowerMax()    << endmsg;

  info() <<
    "///////////////////////////////////////////////////////////////////////////"
         << endmsg;

  info() << "///////////////////////" << endmsg;
  info() << "// Testing Materials //" << endmsg;
  info() << "///////////////////////" << endmsg;

  info() << "Simple element : Oxygen" << endmsg;
  Material* elO = getDet<Material>( "/dd/Materials/Oxygen" );

  info() << "Material: " << elO->name() << endmsg;
  info() << "A       : " << elO->A()/(g/mole) << " g/mole" << endmsg;
  info() << "Z       : " << elO->Z() << endmsg;
  info() << "N       : " << elO->N() << endmsg;
  info() << "Density : " << elO->density()/(g/cm3) << " g/cm3" << endmsg;
  info() << "X0      : " << elO->radiationLength()/(cm) << " cm" << endmsg;
  info() << "Lambda  : " << elO->absorptionLength()/(cm) << " cm" << endmsg;
  info() << endmsg;

  info() << "Material composed of several isotopes : Boron" << endmsg;
  Material* elB = getDet<Material>( "/dd/Materials/Boron" );

  info() << "Material: " << elB->name() << endmsg;
  info() << "A       : " << elB->A()/(g/mole) << " g/mole" << endmsg;
  info() << "Z       : " << elB->Z() << endmsg;
  info() << "N       : " << elB->N() << endmsg;
  info() << "Density : " << elB->density()/(g/cm3) << " g/cm3" << endmsg;
  info() << "X0      : " << elB->radiationLength()/(cm) << " cm" << endmsg;
  info() << "Lambda  : " << elB->absorptionLength()/(cm) << " cm" << endmsg;
  info() << endmsg;

  info() << "Mixture of simple elements : Water" << endmsg;
  Material* mWater = getDet<Material>( "/dd/Materials/Water" );

  info() << "Material: " << mWater->name() << endmsg;
  info() << "A       : " << mWater->A()/(g/mole) << " g/mole" << endmsg;
  info() << "Z       : " << mWater->Z() << endmsg;
  info() << "N       : " << mWater->N() << endmsg;
  info() << "Density : " << mWater->density()/(g/cm3) << " g/cm3" << endmsg;
  info() << "X0      : " << mWater->radiationLength()/(cm) << " cm"  << endmsg;
  info() << "Lambda  : " << mWater->absorptionLength()/(cm) << " cm"  << endmsg;
  info() << endmsg;

  info() << "Mixture of mixtures : Argon_CF4_CO2" << endmsg;
  Material* mArCF4CO2 = getDet<Material>( "/dd/Materials/Argon_CF4_CO2" );

  info() << "Material: " << mArCF4CO2->name() << endmsg;
  info() << "A       : " << mArCF4CO2->A()/(g/mole) << " g/mole"  << endmsg;
  info() << "Z       : " << mArCF4CO2->Z() << endmsg;
  info() << "N       : " << mArCF4CO2->N() << endmsg;
  info() << "Density : " << mArCF4CO2->density()/(g/cm3) << " g/cm3"  << endmsg;
  info() << "X0      : " << mArCF4CO2->radiationLength()/(cm) << " cm" << endmsg;
  info() << "Lambda  : " << mArCF4CO2->absorptionLength()/(cm)<< " cm" << endmsg;
  info() << endmsg;

  info() << "///////////////////////////////////////////" << endmsg;
  info() << "// Testing the TabulatedProperty feature //" << endmsg;
  info() << "///////////////////////////////////////////" << endmsg;

  std::string path = "/dd/Materials/RichMaterialTabProperties/";
  path += "MirrorQuartzAbsLengthPT";
  TabulatedProperty* tab = getDet<TabulatedProperty>( path );

  TabulatedProperty::Table table = tab->table();
  TabulatedProperty::Table::iterator it;
  for (it = table.begin(); it != table.end(); it++) {
    info() << "new table Entry : "
           << "x = " << it->first << ", y = " << it->second << endmsg;
  }
  return StatusCode::SUCCESS;
}


/////////////
// Execute //
/////////////
StatusCode SimpleAlgorithm::execute() {
  info() << "execute" << endmsg;
  return StatusCode::SUCCESS;
}


//////////////
// Finalize //
//////////////
StatusCode SimpleAlgorithm::finalize() {

  info() << "///////////////////////////////////////////////" << endmsg;
  info() << "//   View of the Data Store at finalization  //" << endmsg;
  info() << "///////////////////////////////////////////////" << endmsg;

  DetDataAgent agent (msgSvc());

  // Get the IDataManagerSvc interface of the DetectorDataSvc
  IDataManagerSvc* storeManager = svc<IDataManagerSvc>( "DetectorDataSvc" );

  /// Let's traverse the transient detector data store
  storeManager->traverseTree( &agent );

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

void SimpleAlgorithm::dumpPVs (const ILVolume* lv, std::string de) {

  unsigned long noppv = lv->noPVolumes();
  info() << de << " log. volume has " << noppv    << " phys. volumes: ";

  unsigned int ppvc;
  for (ppvc = 0; ppvc < noppv; ppvc++) {
    info() << lv->pvolume(ppvc)->name() << " ";
  }

  info() << endmsg;
}
