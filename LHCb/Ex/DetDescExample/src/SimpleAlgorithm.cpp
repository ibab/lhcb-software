// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/SimpleAlgorithm.cpp,v 1.9 2001-06-25 08:59:30 sponce Exp $
#define DDEXAMPLE_SIMPLEALGORITHM_CPP

/// Include files

/// Gaudi interfaces
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IConverter.h"

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
#include "DeMuonStation.h"
#include "DetDataAgent.h"

#include "CLHEP/Geometry/Transform3D.h"


//------------------------------------------------------------------------------
// Implementation of class :  SimpleAlgorithm
//------------------------------------------------------------------------------
void dumpPVs( IMessageSvc* ms, ILVolume* lv, std::string de )
{

  MsgStream log(ms, "dumpPVs");

  unsigned long noppv5 = lv->noPVolumes();

  log << MSG::INFO << de << " log. volume has "
      << noppv5    << " phys. volumes: ";

  for( unsigned int ppvc = 0; ppvc < noppv5; ppvc++ ) {
    log << MSG::INFO << lv->pvolume(ppvc)->name() << " ";
  }

  log << MSG::INFO << endreq;
}

static const AlgFactory<SimpleAlgorithm>  Factory;
const IAlgFactory& SimpleAlgorithmFactory = Factory;

/// Algorithm parameters which can be set at run time must be declared.
/// This should be done in the constructor.
SimpleAlgorithm::SimpleAlgorithm (const std::string& name,
                                  ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  declareProperty("HistogramFlag", m_produceHistogram);
}

/// The "initialization" of the algorithm. It creates the environment needed
/// for processing the events. In this example it declares the specific
/// converters needed for that specific application.
StatusCode SimpleAlgorithm::initialize() {

  MsgStream log(msgSvc(), name());

  StatusCode sc;
  
  log << MSG::INFO << "Initialization starting..." << endreq;

  // This is a quick test of the TabulatedProperty feature
  log << MSG::INFO << "Testing the TabulatedProperty feature" << endreq;
  SmartDataPtr<TabulatedProperty> tab
    (detSvc(), "/dd/Geometry/Rich1/Rich1Surfaces/MirrorSurfaceReflectivityPT");
  if (!tab) {
    log << MSG::ERROR
        << "Can't retrieve /dd/Geometry/Rich1/Rich1Surfaces"
        << "/MirrorSurfaceReflectivityPT" << endreq;
    return StatusCode::FAILURE;
  }

  TabulatedProperty::Table table = tab->table();
  for (TabulatedProperty::Table::iterator it = table.begin();
       it != table.end();
       it++) {
    log << MSG::INFO << "new table Entry : "
        << "x = " << it->first << ", y = " << it->second << endreq;
  }

  /// Now we retrieve the top level detector element, e.g. LHCb detector
  /// "Structure" is the top level catalog holding detector logical structure
  log << MSG::INFO << "Retrieving now detector elements" << endreq;
  SmartDataPtr<IDetectorElement> cave(detSvc(),"/dd/Structure/LHCb" );
  
  /// We test if the smart reference is non-zero to be sure
  /// that the data object has been loaded properly
  if (!cave) {
    log << MSG::ERROR << "Can't retrieve /dd/Structure/LHCb" << endreq;
    return StatusCode::FAILURE;
  }
  
  /// Fill the pointer with the information about volume
  /// associated with "/dd/Structure/LHCb"
  ILVolume* stvol = cave->geometry()->lvolume();
  log << MSG::INFO << "LHCb detector is made of " << stvol->materialName()
      << endreq;
  
  dumpPVs( msgSvc(), stvol, cave->name() );

  //---------------------------------------------------------------------------
  // This is an example of usage of userParameter tag
  SmartDataPtr<IDetectorElement> ecal( detSvc(), "/dd/Structure/LHCb/Ecal" );
  if( !ecal )                                                            {
    log << MSG::ERROR << "Can't retrieve /dd/Structure/LHCb/Ecal" << endreq;
    return StatusCode::FAILURE;
  }
  
  std::vector<std::string> parameterList = ecal->userParameters();
  for (std::vector<std::string>::iterator it = parameterList.begin();
       it != parameterList.end();
       it++) {
    log << MSG::INFO << "ECAL " << *it << " = "
        << ecal->userParameterValue(*it)
        << endreq;
  }
  
  dumpPVs( msgSvc(), ecal->geometry()->lvolume(), ecal->name() );
  
  //---------------------------------------------------------------------------
  SmartDataPtr<IDetectorElement> ecalouter
    (detSvc(), "/dd/Structure/LHCb/Ecal/EcalOuter");
  if (!ecalouter) {
    log << MSG::ERROR << "Can't retrieve /dd/Structure/LHCb/Ecal/EcalOuter"
        << endreq;
    return StatusCode::FAILURE;
  }
  //---------------------------------------------------------------------------

  ILVolume* ecalo_vol = ecalouter->geometry()->lvolume();
  if (ecalo_vol) {
    std::string ecalo_mat = ecalo_vol->materialName();
    log << MSG::INFO << "EcalOuter has volume "
        << ecalouter->geometry()->lvolumeName() << endreq;
    log << MSG::INFO << "EcalOuter module made of " << ecalo_mat << endreq;
  }

  dumpPVs( msgSvc(), ecalo_vol, ecalouter->name() );

  /// Now retrieve the custom, user defined detector element "Velo" with
  /// "specific" data defined in vertex.xml file.
  /// In our case the specific data holds information about number of stations
  SmartDataPtr<IDetectorElement> vertex( cave, "Velo" );
  if( !vertex ) {
    log << MSG::ERROR << "Can't retrieve /dd/Structure/LHCb/Velo" << endreq;
    return StatusCode::FAILURE;
  }
  
  ILVolume* ivol = vertex->geometry()->lvolume();
  const ISolid*   vsolid = ivol->solid();
  HepTransform3D vtrans = vertex->geometry()->matrixInv();
  /// HepRotation vrot  = vtrans.getRotation();
  Hep3Vector vvec   = vtrans.getTranslation();
  log << MSG::INFO << "Vertex detector is made of " << ivol->materialName()
      << endreq;
  std::cout << "Vertex detector has solid  " << vsolid << std::endl;
  std::cout << "Vertex detector has position  " << vvec << std::endl;

  dumpPVs( msgSvc(), ivol, vertex->name() );

  /*
  SmartDataPtr<IDetectorElement> vs( vertex, "Stations/Station05" );
  if (!vs) {
    log << MSG::ERROR
        << "Can't retrieve /dd/Structure/LHCb/Velo/Stations/Station05!"
        << endreq;
    return StatusCode::FAILURE;
  }
  
  /// Report the material and its radiation length
  ILVolume* stvol5 = vs->geometry()->lvolume();
  /// const ISolid*   vsolid5 = stvol5->solid();
  HepTransform3D vtrans5 = vs->geometry()->matrixInv();
  HepRotation vrot5 = vtrans5.getRotation();
  Hep3Vector v5axis; HepDouble v5angle; vrot5.getAngleAxis (v5angle, v5axis);
  Hep3Vector vvec5   = vtrans5.getTranslation();

  dumpPVs (msgSvc(), stvol5, vs->name());

  log << MSG::INFO
      << vs->name()
      << " is made of "            << stvol5->materialName()
      << " with radiation length " << stvol5->material()->radiationLength()
      << endreq;
  */
  SmartDataPtr<DetectorElement> stations (detSvc(),
 					  "/dd/Structure/LHCb/Muon/Stations");
  if (!stations) {
    log << MSG::ERROR << "Can't retrieve /dd/Structure/LHCb/Muon/Stations!"
        << endreq;
    return StatusCode::FAILURE;
  }
  
  // Loop over all the muon stations found in the detector model
  // This is an example of how to use specific converters
  for (DataObject::DirIterator d =  stations->dirBegin();
       d != stations->dirEnd();
       d++) {
    SmartDataPtr<DeMuonStation> s (detSvc(), (*d)->fullpath());
    if (!s) {
      log << MSG::ERROR << "Unable to retrieve: " << (*d)->fullpath() << endreq;
      return sc;
    }
    m_stations.push_back( s );
  }
  
  log << MSG::INFO << "Found " << m_stations.size()
      << " Muon Stations in detector model" << endreq;
  
  /// Printing the information found
  for ( unsigned int i = 0; i < m_stations.size(); i++ ) {
    log << MSG::INFO << "MUON STATION: "
        << m_stations[i]->localPath() << endreq;
    log << MSG::INFO << "Aluminium plate thickness: "
        << m_stations[i]->thickness() << endreq;
    log << MSG::INFO << "X pad dimension:           "
        << m_stations[i]->padx() << endreq;
    log << MSG::INFO << "Y pad dimension:           "
        << m_stations[i]->pady() << endreq;
  }
  
  log << MSG::INFO << "Retrieving now materials..." << endreq;
  
  /// The best way to work with materials is using the Material class.
  /// This class defines the interface shared by all sorts of material
  /// e.g. isotopes, elements or mixtures

  /// Materials are located inside the "/dd/Materials" catalog
  
  /// Now retrieve simple material element Oxygen
  SmartDataPtr<Material> elO( detSvc(), "/dd/Materials/Oxygen" );
  if( !elO )                                                               {
    log << MSG::ERROR << "Can't retrieve /dd/Materials/Oxygen!" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "\nMaterial: " << elO->name() << endreq;
  log << MSG::INFO << "A       : "   << elO->A()/(g/mole)
      << " g/mole" << endreq;
  log << MSG::INFO << "Z       : "   << elO->Z() << endreq;
  log << MSG::INFO << "Density : "   << elO->density()/(g/cm3)
      << " g/cm3" << endreq;
  log << MSG::INFO << "X0      : "   << elO->radiationLength()/(cm)
      << " cm" << endreq;
  log << MSG::INFO << "Lambda  : "   << elO->absorptionLength()/(cm)
      << " cm" << endreq;
  
  /// Now retrieve the material element composed of a few isotopes
  SmartDataPtr<Material> elB( detSvc(), "/dd/Materials/Boron" );
  if( !elB )                                                               {
    log << MSG::ERROR << "Can't retrieve /dd/Materials/Boron!" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "\nMaterial: " << elB->name() << endreq;
  log << MSG::INFO << "A       : "   << elB->A()/(g/mole) << " g/mole"
      << endreq;
  log << MSG::INFO << "Z       : "   << elB->Z() << endreq;
  log << MSG::INFO << "Density : "   << elB->density()/(g/cm3) << " g/cm3"
      << endreq;
  log << MSG::INFO << "X0      : "   << elB->radiationLength()/(cm) << " cm"
      << endreq;
  log << MSG::INFO << "Lambda  : "   << elB->absorptionLength()/(cm) << " cm"
      << endreq;
  
  /// Now retrieve simple material mixture
  SmartDataPtr<Material> mWater( detSvc(), "/dd/Materials/Water" );
  if( !mWater )                                                            {
    log << MSG::ERROR << "Can't retrieve /dd/Materials/Water!" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "\nMaterial: " << mWater->name() << endreq;
  log << MSG::INFO << "A       : " << mWater->A()/(g/mole) << " g/mole"
      << endreq;
  log << MSG::INFO << "Z       : " << mWater->Z() << endreq;
  log << MSG::INFO << "Density : " << mWater->density()/(g/cm3) << " g/cm3"
      << endreq;
  log << MSG::INFO << "X0      : " << mWater->radiationLength()/(cm)
      << " cm"  << endreq;
  log << MSG::INFO << "Lambda  : " << mWater->absorptionLength()/(cm)
      << " cm"  << endreq;
  
  /// And finally retrieve mixture of mixtures,
  /// e.g. mixture of Argon, CF4 and CO2
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

  SmartDataPtr<Material> mSiO2( detSvc(), "/dd/Materials/SiO2" );
  if( !mSiO2 )                                                         {
    log << MSG::ERROR << "Can't retrieve /dd/Materials/SiO2!" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "\nMaterial: " << mSiO2->name() << endreq;
  log << MSG::INFO << "A       : " << mSiO2->A()/(g/mole) << " g/mole"
      << endreq;
  log << MSG::INFO << "Z       : " << mSiO2->Z() << endreq;
  log << MSG::INFO << "Density : " << mSiO2->density()/(g/cm3) << " g/cm3"
      << endreq;
  log << MSG::INFO << "X0      : " << mSiO2->radiationLength()/(cm) << " cm"
      << endreq;
  log << MSG::INFO << "Lambda  : " << mSiO2->absorptionLength()/(cm) << " cm"
      << endreq;
  
  SmartDataPtr<Material> mCO2( detSvc(), "/dd/Materials/CO2" );
  if( !mCO2 )                                                         {
    log << MSG::ERROR << "Can't retrieve /dd/Materials/CO2!" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "\nMaterial: " << mCO2->name() << endreq;
  log << MSG::INFO << "A       : " << mCO2->A()/(g/mole)
      << " g/mole"  << endreq;
  log << MSG::INFO << "Z       : " << mCO2->Z() << endreq;
  log << MSG::INFO << "Density : " << mCO2->density()/(g/cm3)
      << " g/cm3"  << endreq;
  log << MSG::INFO << "X0      : " << mCO2->radiationLength()/(cm)
      << " cm"  << endreq;
  log << MSG::INFO << "Lambda  : " << mCO2->absorptionLength()/(cm)
      << " cm"  << endreq;

  SmartDataPtr<Material> mCF4( detSvc(), "/dd/Materials/CF4" );
  if( !mCF4 )                                                         {
    log << MSG::ERROR << "Can't retrieve /dd/Materials/CF4!" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "\nMaterial: " << mCF4->name() << endreq;
  log << MSG::INFO << "A       : " << mCF4->A()/(g/mole)
      << " g/mole"  << endreq;
  log << MSG::INFO << "Z       : " << mCF4->Z() << endreq;
  log << MSG::INFO << "Density : " << mCF4->density()/(g/cm3)
      << " g/cm3"  << endreq;
  log << MSG::INFO << "X0      : " << mCF4->radiationLength()/(cm)
      << " cm"  << endreq;
  log << MSG::INFO << "Lambda  : " << mCF4->absorptionLength()/(cm)
      << " cm"  << endreq;

  return StatusCode::SUCCESS;
}

StatusCode SimpleAlgorithm::execute() {

  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << "execute" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode SimpleAlgorithm::finalize() {

  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << "finalize" << endreq;
  
  DetDataAgent agent( msgSvc() );

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
