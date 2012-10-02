#include "RichHpdProperties.h"
#include "RichG4AnalysisConstGauss.h"
#include "RichG4GaussPathNames.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/TabulatedProperty.h"
#include "boost/lexical_cast.hpp"
#include "RichG4SvcLocator.h"
#include "G4Material.hh"

#include "RichDet/DeRichSystem.h"
#include "RichDet/DeRichHPD.h"
#include "RichDet/DeRichPD.h"


// create with a large number of hpds and then adjust
// the size to the correct number of hpds in the constructer
// after reading from the number of hpds from the xml db.
// here the 200 as the size is just a randomly picked number,
// for initialization.
// the in the initialization the
// parameter is 2 and this is the number of rich detectors in lhcb.
//
RichHpdProperties* RichHpdProperties::RichHpdPropertiesInstance=0;

RichHpdProperties::RichHpdProperties( )
  : m_numHpdTotRich(std::vector<int>(2)),
    m_RichHpdQEList(2,std::vector<RichHpdQE*>(200)),
    m_RichHpdPSFList(2,std::vector<RichHpdPSF*>(200)),
    m_RichHpdDeMagList(2,std::vector<RichHpdDeMag*>(200)),
    m_HpdVerboseLevel(0),m_HpdActivateOverRideMaxQEFromDB(false),
    m_HpdDBOverRideMaxQEValue(0.45),
    m_HpdSmearValueVect(std::vector<double>(3))
{ }
//    m_hpdNumBegInHitCollection(std::vector<int>(4)),
//    m_hpdNumEndInHitCollection(std::vector<int>(4)),

void  RichHpdProperties::setHpdPropertiesVerboseLevel(int aLevel ) {
  m_HpdVerboseLevel=aLevel;

}
void RichHpdProperties::SetHpdSmearValueVect(double SmAgel, double SmR1, double SmR2){
  m_HpdSmearValueVect.clear();
  m_HpdSmearValueVect.resize(3);
  m_HpdSmearValueVect[0]= SmAgel;
  m_HpdSmearValueVect[1]= SmR1;
  m_HpdSmearValueVect[2]= SmR2;
}

void RichHpdProperties::InitializeHpdProperties( ) {
 
  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichHpdlog( msgSvc, "RichHpdProperties" );
  

  // First set the number of rich detectors.

  m_numberOfRichDetectors=2;
  if((int)m_numHpdTotRich.size() != m_numberOfRichDetectors)
    m_numHpdTotRich.resize(m_numberOfRichDetectors);
  m_numberOfRichHitCollections=4;
  // if((int)  m_hpdNumBegInHitCollection.size() != m_numberOfRichHitCollections)
  //  m_hpdNumBegInHitCollection.resize(m_numberOfRichHitCollections);
  // if((int)  m_hpdNumEndInHitCollection.size() != m_numberOfRichHitCollections)
  //  m_hpdNumEndInHitCollection.resize(m_numberOfRichHitCollections);
  

  // RichHpdlog << MSG::INFO
  //           << " Test of Printout from RichHpdProperties" << endreq;

  SmartDataPtr<DetectorElement> Rich1DE(detSvc,Rich1DeStructurePathName );
  if( !Rich1DE ){
    RichHpdlog << MSG::ERROR
               << "Can't retrieve  "<< Rich1DeStructurePathName << endreq;
  }
  else
  {

    //following line modified to be compatible with recent DetDesc. SE 16-6-2005. 
    // the following is commented out to get the flag from the options file. SE 26-10-2006
    //    m_UseHpdMagDistortions = Rich1DE->param<int>("UseHpdMagDistortions");
   

    m_numHpdTotRich[0]= Rich1DE->param<int>("Rich1TotNumHpd");

    // m_hpdNumBegInHitCollection = Rich1DE->paramVect<int>("RichHpdNumBeginInPanels");
    // m_hpdNumEndInHitCollection = Rich1DE->paramVect<int>("RichHpdNumEndInPanels");


    RichHpdlog << MSG::INFO << "Total Number of hpds in Rich1 = "
               << m_numHpdTotRich[0] << endreq;

    //  RichHpdlog << MSG::DEBUG<<" Rich Hpd Copy Num In Panels Begin Values  "<<m_hpdNumBegInHitCollection[0]
    //           <<"  "<<m_hpdNumBegInHitCollection[1]<<"   "<<m_hpdNumBegInHitCollection[2]<<"  "
    //            <<m_hpdNumBegInHitCollection[3]<<endreq;
    //  RichHpdlog << MSG::DEBUG<<" Rich Hpd Copy Num In Panels End Values  "<<m_hpdNumEndInHitCollection[0]
    //           <<"  "<<m_hpdNumEndInHitCollection[1]<<"   "<<m_hpdNumEndInHitCollection[2]<<"  "
    //           <<m_hpdNumEndInHitCollection[3]<<endreq;
    
    //following line modified to be compatible with recent DetDesc. SE 16-6-2005. 
    m_HpdMaxQuantumEffFromDB=  Rich1DE->param<double>("RichHpdMaxQE");
    
    setHpdMaximumQuantumEfficiency();
    
    //if( m_HpdActivateOverRideMaxQEFromDB) m_HpdMaxQuantumEff=m_HpdDBOverRideMaxQEValue;

     m_MinPhotonEnergyInRICH=  Rich1DE->param<double> ("RichMinPhotonEnergy");
     m_MaxPhotonEnergyInRICH=  Rich1DE->param<double> ("RichMaxPhotonEnergy");
    //temporary test
    // m_MinPhotonEnergyInRICH=1.5*eV;
    // m_MaxPhotonEnergyInRICH=7.0*eV;
    //end temporary test
  }

  SmartDataPtr<DetectorElement> Rich2DE(detSvc,Rich2DeStructurePathName );
  if( !Rich2DE )
  {

    //    RichHpdlog << MSG::ERROR
    //              << "Can't retrieve "<< Rich2DeStructurePathName << endreq;

  }else if(!Rich1DE) {
    RichHpdlog <<MSG::ERROR<<"Can't retrieve " << Rich1DeStructurePathName<<"   for Rich2"
               <<endreq;

  }else {

    // the following line to be un commented after
    // the rich2 structure.xml file has this info.

    // m_numHpdTotRich[1]= Rich2DE->userParameterAsInt("Rich2TotNumHpd");
    //    m_numHpdTotRich[1]= Rich1DE->userParameterAsInt("Rich2TotNumHpd");
    m_numHpdTotRich[1]= Rich1DE->param<int>("Rich2TotNumHpd");
    RichHpdlog << MSG::INFO
               << "Total Number of hpds in Rich2 = "
               << m_numHpdTotRich[1]<<endreq;

  }

  // to resize the vectors. Just test on the size of one of the vectorlists
  // and adjust all the three vectorlists.
  for (int ird=0; ird<m_numberOfRichDetectors; ird++ ) {

    if( m_numHpdTotRich[ird] != (int) m_RichHpdDeMagList[ird].size() ){
      m_RichHpdDeMagList[ird].resize(m_numHpdTotRich[ird]);
    }
    if( m_numHpdTotRich[ird] != (int) m_RichHpdQEList[ird].size() ){
      m_RichHpdQEList[ird].resize(m_numHpdTotRich[ird]);
    }
    if( m_numHpdTotRich[ird] != (int) m_RichHpdPSFList[ird].size() ){
      m_RichHpdPSFList[ird].resize(m_numHpdTotRich[ird]);
    }

  }



  // Now fill in the hpd properties from the db.
  // for now the same properties are filled for all the hpds since there is
  // only one set available. This is to be changed in the future. The (single) set of properties
  // now read from the xml db.

  // First for the hpd QE
  // the photon energy is converted from MeV to eV and the qeff is
  // converted from percentage to absolute values.
  // the future the following may be transferred to the hpdqe class.

  FillHpdQETablesAtInit ( detSvc, msgSvc );

  FillHpdPSFTablesAtInit (detSvc, msgSvc );

  FillHpdDemagTablesAtInit ( detSvc, msgSvc );

  RichHpdlog << MSG::INFO
	     << "Filled the HPD QE, PSF and Demag tables for  RICH  "<<endreq;


  //Now get the HPD High Voltage
  SmartDataPtr<TabulatedProperty>tabHV(detSvc,RichHpdHVMatTabPropPath);
  double HpdHVSingle = 0.0;

  if(!tabHV) {
    RichHpdlog << MSG::ERROR
               <<"RichHpdProperties: "
               <<" Can't retrieve "+ RichHpdHVMatTabPropPath <<endreq;


  }else {
    TabulatedProperty::Table tableHV = tabHV->table();
    HpdHVSingle=tableHV.begin()->second;

  }
  if(m_HpdVerboseLevel >0) {
    RichHpdlog << MSG::INFO
               <<"Hpd HighVoltage value = "<<HpdHVSingle<<endreq;
  }
  m_RichHpdHighVoltage=HpdHVSingle;

  //Now get the PhCathode to Si Surface max Distance
  double hpdQwtoSiDist = 0.0;
  if( !Rich1DE ){
    RichHpdlog << MSG::ERROR
               << "Can't retrieve "<< Rich1DeStructurePathName<<"   forHpdQw toSiDist "
               << endreq;
  }else {


    hpdQwtoSiDist = Rich1DE->param<double>("RichHpdQWToSiMaxDist");

  }
  if(m_HpdVerboseLevel >0 ){
    RichHpdlog << MSG::INFO
               <<"Hpd QW to Si Max Dist = "<<hpdQwtoSiDist <<endreq;
  }
  m_RichHpdQWToSiDist= hpdQwtoSiDist;
  if(m_HpdVerboseLevel >1 ){

    RichHpdlog << MSG::INFO << "Hpd QW to Si Max Dist =  "
               <<m_RichHpdQWToSiDist << endreq;
  }


  // Now get the MaxZHit coordinate in Rich1.
  double MaxZHitInRich1 = 0.0;
  if( !Rich1DE ){
    RichHpdlog << MSG::ERROR
               << "Can't retrieve "<< Rich1DeStructurePathName << " for MaxZhit in Rich1 "
               << endreq;
  }else {

    // modif to to be comaptible with recent Detdesc SE 16-6-2005.
    MaxZHitInRich1 = Rich1DE->param<double>("Rich1MaxDownstreamZHitCoord");

  }
  if ( m_HpdVerboseLevel >0 ) {
    RichHpdlog << MSG::INFO
               <<"Max Z Hit coord in Rich1 = "<<MaxZHitInRich1 <<endreq;
  }
  m_Rich1MaxZHitCoord=  MaxZHitInRich1;

  if(m_HpdVerboseLevel >1 ){

    RichHpdlog << MSG::INFO << "Max Z coord for a hit in Rich1 =  "
               << m_Rich1MaxZHitCoord << endreq;
  }

  // Now get the quartz window name and the photocathode name.
  std::string hpdQWlvname;
  std::string hpdPhCathlvname;
  double phcathRinn = 0.0;
  SmartDataPtr<DetectorElement> RichHpdQWDE(detSvc, (Rich1DeStructurePathName+RichHpdQwDeSubPathName));
  if(!RichHpdQWDE) {
    RichHpdlog << MSG::ERROR
               <<"Can't retrieve "+Rich1DeStructurePathName+RichHpdQwDeSubPathName<<endreq;

  }else{

    if(RichHpdQWDE->geometry()->hasLVolume()){
      hpdQWlvname=RichHpdQWDE->geometry()->lvolumeName();
    }else{
      RichHpdlog << MSG::ERROR
                 <<"Erroneous Log Vol for Hpd QW log vol " <<endreq;
    }
  }
  if(m_HpdVerboseLevel >0 ){

    RichHpdlog << MSG::INFO
               <<"Hpd Qw Log Volname =  "<<hpdQWlvname<<endreq;
  }
  m_HpdQWLogVolName = hpdQWlvname;
  SmartDataPtr<DetectorElement> RichHpdPCDE(detSvc, (Rich1DeStructurePathName+RichHpdPaCathDeSubPathname));
  if(!RichHpdPCDE) {
    RichHpdlog << MSG::ERROR
               <<"Can't retrieve " +Rich1DeStructurePathName+RichHpdPaCathDeSubPathname
               <<endreq;

  }else{


    if(RichHpdPCDE->geometry()->hasLVolume()){
      hpdPhCathlvname = RichHpdPCDE->geometry()->lvolumeName();
    }else{
      RichHpdlog << MSG::ERROR
                 <<"Erroneous Log Vol for Hpd PhCathode log vol " <<endreq;
    }

    // modif to be compatible with recent Detdesc. SE 16-6-2005.
    phcathRinn= RichHpdPCDE->param<double>( "HpdPhCathodeRadInner");


  }
  if(m_HpdVerboseLevel >0 ){

    RichHpdlog << MSG::INFO
               <<"Hpd Ph Cathode Log Volname =  "<<hpdPhCathlvname<<endreq;
    RichHpdlog << MSG::INFO
               <<"Hpd Ph Cathode Rinner = "<< phcathRinn<<endreq;
  }
  m_HpdPhCathodeLogVolName = hpdPhCathlvname;

  m_HpdPhCathodeInnerRadius= phcathRinn;


  
}

void RichHpdProperties::InitializeSiDetParam() {
  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  SmartDataPtr<DetectorElement> Rich1DE(detSvc,Rich1DeStructurePathName );
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichHpdlog( msgSvc, "RichHpdProperties" );
  if( !Rich1DE ){
    RichHpdlog << MSG::ERROR
               << "Can't retrieve  "<< Rich1DeStructurePathName <<" for sidet param "<< endreq;
  }
  else
  {
    m_siDetXSize= Rich1DE->param<double>("RichHpdSiliconDetectorXSize");
    m_siDetYSize= Rich1DE->param<double>("RichHpdSiliconDetectorYSize");
    m_siDetZSize= Rich1DE->param<double>("RichHpdSiliconDetectorZSize");    
  }  
}
void  RichHpdProperties::setHpdMaximumQuantumEfficiency(){

  if( m_HpdActivateOverRideMaxQEFromDB) {
    m_HpdMaxQuantumEff=m_HpdDBOverRideMaxQEValue;
  }else {
    m_HpdMaxQuantumEff= m_HpdMaxQuantumEffFromDB;    
  }
  
  
}

RichHpdProperties::~RichHpdProperties() { ; }

//////////////////////////////////////////////////////////////////////////
void  RichHpdProperties::FillHpdQETablesAtInit( IDataProviderSvc* detSvc, 
						IMessageSvc* msgSvc  ) {

  MsgStream RichHpdPropLogQE( msgSvc, "RichHpdProperties" );
  std::vector<double> QeSingle;
  std::vector<double> EphotSingle;

  //New stuff: QE tables from condDB (M.Musy 27/01/08)
  SmartDataPtr<DeRichSystem> richsys( detSvc, DeRichLocations::RichSystem );
  if(!richsys) {
    RichHpdPropLogQE << MSG::ERROR << "Could not get RichSystem!"<<endreq;
    return;
  }

  /// Returns a list of all active HPDs identified by their RichSmartID
  //const LHCb::RichSmartID::Vector hpdlist = richsys->activeHPDRichSmartIDs(); 
  // this is chaged to allHPDSmartIds to take care of disabled HPDs as well. SE Feb10-2011
  const LHCb::RichSmartID::Vector& hpdlist = richsys->allPDRichSmartIDs();   
  //RichHpdPropLogQE << MSG::INFO <<"QE hpdlist.size()="<<hpdlist.size()<<endreq;

  LHCb::RichSmartID::Vector::const_iterator i;
  for( i=hpdlist.begin(); i!=hpdlist.end(); ++i ) {
    std::string location = richsys->getDePDLocation(*i);
    int irichdet = (*i).rich();
    int nHpdInRich1 = m_RichHpdQEList[0].size();
    int ih = richsys->copyNumber(*i).data() - nHpdInRich1*irichdet;//runs 0->195 and 0->287
    if(irichdet>1) RichHpdPropLogQE<<MSG::ERROR<<"Error in HPD numbering scheme!"<<endreq;
//     RichHpdPropLogQE <<MSG::INFO<<"QE DeHPDLocation="<<location
//                      <<"  CopyNr="<<richsys->copyNumber(*i)
//                      << "  Nr.in Rich"<<irichdet+1<<"="<<ih<<endreq;

    TabulatedProperty::Table table;

    //    SmartDataPtr<DeRichHPD> iDeHpd( detSvc, location );
    SmartDataPtr<DeRichPD> iDeHpd( detSvc, location );
    if(  ( !iDeHpd ) || (m_UseNominalHpdQE)  ) {
      RichHpdPropLogQE << MSG::WARNING << "No HPD in Conditions DB at "<<location
		       <<"Using Old QE default table for this HPD!"<<endreq;
      SmartDataPtr<TabulatedProperty> tabQE(detSvc, RichHpdQeffMatTabPropPath);
      if(!tabQE) RichHpdPropLogQE << MSG::ERROR
				  <<"RichHpdPropertiesQE: Can't retrieve "
				  << RichHpdQeffMatTabPropPath << endreq;
      else {
          table = tabQE->table();
      }  
    } else {
          table = iDeHpd->pdQuantumEff()->tabProperty()->table();
    }
    
    TabulatedProperty::Table::const_iterator it;
    for (it = table.begin(); it != table.end(); it++) {
      double aPhotonEnergy= (it->first);
      double aQeOrig      = (it->second)/100.0; // division by 100 to get from percent to prob 
      double aQeCorrected = aQeOrig;

      if( (aPhotonEnergy >= (double) m_MinPhotonEnergyInRICH) && 
	  (aPhotonEnergy <= (double) m_MaxPhotonEnergyInRICH ) ) {
	aQeCorrected = getHpdCorrectedQEFromPhotonEnergy(aPhotonEnergy, aQeOrig);
      }
      double aPhotonEnergyIneV = aPhotonEnergy*1000000.0; // mult to get from MeV to eV
      EphotSingle.push_back(aPhotonEnergyIneV);
      QeSingle.push_back(aQeCorrected );
    }

    int numQEbin = (int) QeSingle.size();

    m_RichHpdQEList[irichdet][ih] = new RichHpdQE(ih,irichdet);
    if(numQEbin > 0 ){
      m_RichHpdQEList[irichdet][ih]->setAnHpdQEen( numQEbin, QeSingle, EphotSingle);
    } else {
      // the following to be modified for non-existent hpds SE 26-10-2006.
      RichHpdPropLogQE << MSG::WARNING
		       <<" RichHpdProperties: Zero number of bins for hpd QE .Check db for "
		       << "Current richdet hpdInRichDet "<< irichdet<<"  "<<ih
		       <<endreq;
    }

    //debug stuff
//       for( double ene=1.75; ene<7.0;  ene+=0.2 ) {
//         const Rich::TabulatedProperty1D* qe_table = iDeHpd->pdQuantumEff();
//         double qe = qe_table->value(ene/1000000.0);
//         RichHpdPropLogQE <<MSG::INFO<<"energy="<<ene<<" wavel="<<1243.125/ene
//                          <<" ---> " << qe <<endreq;
//       }

  }

}

double RichHpdProperties::getHpdCorrectedQEFromPhotonEnergy(double photonenergy, double originalQE )
{
  //  double thisPCRI=getHpdPCRIFromPhotEnergy( photonenergy );
  // double thisQWRI=getHpdQWRIFromPhotEnergy( photonenergy );
  // the ref index is directly accessed through the G4 methods since
  // it is the property of the material bulk. G4 automatically does the interpolation
  // efficiently , of this property and hence no need to have our own direct access and interpolation.
  // Also this correction is applied to the QE before the event loop starts, to save cpu time.
  // SE 26-10-2007


    double thisPCRI=1.0;
    double thisQWRI=1.0;
    static const G4MaterialTable* theMaterialTable = G4Material::GetMaterialTable();
    G4int imata=0; G4bool matafound=false; 
    G4int imatb=0; G4bool matbfound=false;
    
    while( (imata<  (G4int) theMaterialTable->size()) &&  (! matafound) ) {

      if(RichHpdPhCathMatName == ((*theMaterialTable)[imata]->GetName())) {
        G4Material* aMatPC = (*theMaterialTable)[imata];
        G4MaterialPropertyVector* RindexPC=
                      aMatPC->GetMaterialPropertiesTable()->GetProperty("RINDEX");
          if(   RindexPC ) {
            thisPCRI= RindexPC->Value(photonenergy); //G4.95 onwards
          } 
          matafound=true;    
      }      
      imata++;      
    }    

    while( (imatb<  (G4int) theMaterialTable->size()) &&  (! matbfound) ) {
      if( RichHpdQWMatName == ((*theMaterialTable)[imatb]->GetName())) {
        G4Material* aMatQW = (*theMaterialTable)[imatb];
        G4MaterialPropertyVector* RindexQW=
                      aMatQW->GetMaterialPropertiesTable()->GetProperty("RINDEX");
        if(   RindexQW ) {
          thisQWRI= RindexQW->Value(photonenergy); //G4.95 onwards
        } 

        matbfound=true;        
      }      
      imatb++;
      
    }

    // the division by 100 from percent to real probability is 
    // done in FillHpdQETablesAtInit of RichHpdProperties. SE 26-10-2007
    //  double maxQEguess=1.0;
    // if(originalQE > 1.0) maxQEguess=100.0; //QE by now should be in real probablitity, 
    // this will catch the error if it isn't
  
    //originalQE = originalQE/maxQEguess;
  //  double thisQWrefl=0.0;
  // double thisPCrefl=0.0;
    // end of modif by SE

  G4double thisQWrefl=(thisQWRI-1)/(thisQWRI+1);
  thisQWrefl=thisQWrefl*thisQWrefl;
  //test print
  //G4cout<< " Hpd QW property  energy refindex refl "<<photonenergy<<"   "<<thisQWRI<<"  "<<thisQWrefl<<G4endl;

  //end of testprint
  
  G4double thisPCrefl=(thisPCRI-thisQWRI)/(thisPCRI+thisQWRI);
  thisPCrefl=thisPCrefl*thisPCrefl;

  //test print
  //G4cout<< " Hpd PC property  energy refindex refl "<<photonenergy<<"   "<<thisPCRI<<"  "<<thisPCrefl<<G4endl;
  //end of testprint
  
  
  //double thisTrans=1.0;
  
  //G4double thisTrans= (1+thisQWrefl*thisPCrefl)*(1-thisQWrefl)*(1-thisPCrefl);   //first reflections
  G4double thisTrans= (1-thisQWrefl)*(1-thisPCrefl)/(1-thisPCrefl*thisQWrefl); //Geometric series
  //G4double thisTrans= (1.0-thisQWrefl); //QW refl only
  // G4double thisTrans= (1.0-thisQWrefl)*(1.0-thisPCrefl*thisPCrefl); //Fudge factor -:)S

//test print
  //G4cout<< " Hpd QW-PC transmission "<<photonenergy<<"  "<< thisTrans<<G4endl;


//end test print
  //double thiscorrQE=1.0;
  
  G4double thiscorrQE=(originalQE/(thisTrans)); //first order
  //thiscorrQE=(thiscorrQE/                      //second order
  //                     (1+(1-thiscorrQE)*(thisPCRI-1)*(thisPCRI-1)
  //    		                         /((thisPCRI+1)*(thisPCRI+1))));

  if(thiscorrQE > 1.0) thiscorrQE=1.0;
  
  // test printout

  //G4cout<< "HPD QETable  Photon energy QeOrig QeCorrected "<<photonenergy<<"  "<<originalQE<<"  "
  //      << thiscorrQE<<G4endl;

  // end of testprintout

  return thiscorrQE;

}


void RichHpdProperties::FillHpdPSFTablesAtInit ( IDataProviderSvc* detSvc, IMessageSvc* msgSvc  ) {

  MsgStream RichHpdPropLogPSF( msgSvc, "RichHpdProperties" );


  //Now get the PSF values.
  SmartDataPtr<TabulatedProperty>tabPSF(detSvc,RichHpdPsfMatTabPropPath);
  SmartDataPtr<TabulatedProperty>tabPSFPhEn(detSvc,RichHpdPsfPhEnMatTabPropPath);
  SmartDataPtr<TabulatedProperty>tabPSFRadial(detSvc,RichHpdPsfRadialMatTabPropPath);
  //the following moved to the options file. SE 1-9-11
  // SmartDataPtr<TabulatedProperty>tabHpdSmear(detSvc,RichHpdHitSmearPath);
  
  double HpdPsfSingle = 0.0;
  std::vector<double> aPsfValueVect;
  std::vector<double> aPsfPhEnergyVect;
  std::vector<double> aPsfRadValueVect;
  //  std::vector<double> aSmearValueVect= m_HpdSmearValueVect;
  
  
  
  aPsfValueVect.clear();
  aPsfPhEnergyVect.clear();
  aPsfRadValueVect.clear();
  //  aSmearValueVect.clear();
  
  
  if( (!tabPSF) || (!tabPSFPhEn) || (!tabPSFRadial) ) {
    RichHpdPropLogPSF << MSG::ERROR
               <<"RichHpdProperties PSF: "
                      <<" Can't retrieve  "+RichHpdPsfMatTabPropPath << " or " <<RichHpdPsfPhEnMatTabPropPath
                      <<"  or "<<RichHpdPsfRadialMatTabPropPath               
               <<endreq;


  }else {

    TabulatedProperty::Table tablePsf = tabPSF->table();
    TabulatedProperty::Table tablePsfPh = tabPSFPhEn->table();
    TabulatedProperty::Table tablePsfRadial = tabPSFRadial->table();
    // the following moved to options file.
    //    TabulatedProperty::Table tableHitSmear ;
    // if( tabHpdSmear != 0 ) tableHitSmear= tabHpdSmear ->table();

    // the following line can be removed in the future. Kept for backward compatibility.
    HpdPsfSingle=tablePsf.begin()->second;


    TabulatedProperty::Table::iterator it;
    TabulatedProperty::Table::iterator itp;
    TabulatedProperty::Table::iterator itr;
    TabulatedProperty::Table::iterator its;
    for (it = tablePsf.begin(); it != tablePsf.end(); it++) {      
      aPsfValueVect.push_back(it->second);
    }

    for (itp = tablePsfPh.begin(); itp != tablePsfPh.end(); itp++) {      
      aPsfPhEnergyVect.push_back(itp->second);
    }

    for (itr = tablePsfRadial.begin(); itr != tablePsfRadial.end(); itr++) {      
      aPsfRadValueVect.push_back(itr->second);
    }

    // moved to options file
    //    if( tabHpdSmear != 0 ) {
    //  
    //  for (its = tableHitSmear.begin(); its != tableHitSmear.end(); its++) {      
    //    aSmearValueVect.push_back(its->second);
    //  }
    // }else {
      // the following is for backward compatibility for old DB.
    //  aSmearValueVect.resize(3);
    //  aSmearValueVect[0]=0.0;
    //  aSmearValueVect[1]=0.0;
    //  aSmearValueVect[2]=0.0;
    // }
    
    

    
  }
  
  if(m_HpdVerboseLevel >0 ) {
    RichHpdPropLogPSF << MSG::INFO <<"Hpd PSF value =  "<<HpdPsfSingle<<endreq;
  }

  //Now populate the classes for each of the hpds.
  for(int irichdet=0; irichdet<m_numberOfRichDetectors; irichdet++ ){

    int nHpd=m_numHpdTotRich[irichdet];

    if( nHpd != (int) m_RichHpdPSFList[irichdet].size() ){
      m_RichHpdPSFList[irichdet].resize(nHpd);

    }

    for(int ih=0; ih< nHpd ; ih++){
      // now for the PSF
      
      m_RichHpdPSFList[irichdet][ih]= new  RichHpdPSF(ih,irichdet);
      // the following line kept for backward compatibility for now. 26-10-2006
      m_RichHpdPSFList[irichdet][ih]->setCurrentHpdPSF(HpdPsfSingle);
      m_RichHpdPSFList[irichdet][ih]->sethpdPointSpreadFunctionVect(aPsfValueVect );
      m_RichHpdPSFList[irichdet][ih]->sethpdPSFPhoEnergyVect(aPsfPhEnergyVect );
      m_RichHpdPSFList[irichdet][ih]->sethpdPSFRadialPosVect(aPsfRadValueVect );
      // m_RichHpdPSFList[irichdet] [ih]->sethitSmearValueVect(aSmearValueVect);
      m_RichHpdPSFList[irichdet] [ih]->sethitSmearValueVect(m_HpdSmearValueVect);
      
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void  RichHpdProperties::FillHpdDemagTablesAtInit ( IDataProviderSvc* detSvc, 
						    IMessageSvc* msgSvc )
{ 
  MsgStream RichHpdPropLogDemag( msgSvc, "RichHpdProperties" );

  if( m_UsingHpdMagneticFieldDistortion ) {//demag tables from condDB (M.Musy 27/01/08)

    //New stuff
    SmartDataPtr<DeRichSystem> richsys(detSvc, DeRichLocations::RichSystem );
    if(!richsys) {
      RichHpdPropLogDemag << MSG::ERROR << "Could not get RichSystem!"<<endreq;
      return;
    }

    /// Returns a list of all active HPDs identified by their RichSmartID
    const LHCb::RichSmartID::Vector& hpdlist = richsys->activePDRichSmartIDs();
    for(LHCb::RichSmartID::Vector::const_iterator i=hpdlist.begin(); i!=hpdlist.end(); ++i)
      {
	std::string location = richsys->getDePDLocation(*i);
	int irichdet = (*i).rich();
	int nHpdInRich = m_RichHpdDeMagList[0].size();
	int ih       = richsys->copyNumber(*i).data() - nHpdInRich*irichdet;//runs 0->195 and 0->288
	if(irichdet>1) RichHpdPropLogDemag<<MSG::ERROR<<"Error in numbering scheme!"<<endreq;
// 	RichHpdPropLogDemag<<MSG::INFO<<"DeHPDLocation="<<location
// 			   <<"  CopyNr="<<richsys->copyNumber(*i)
// 			   << "  Nr.in Rich"<<irichdet+1<<"="<<ih<<endreq;

	m_RichHpdDeMagList[irichdet][ih]= new RichHpdDeMag(detSvc, ih, irichdet);

  SmartDataPtr<DeRichHPD> iDeHpd( detSvc, location );

	if(iDeHpd) {

	  const Rich1DTabFunc* r_demag   = iDeHpd->demagnification_RtoR();
	  const Rich1DTabFunc* phi_demag = iDeHpd->demagnification_RtoPhi();

	  m_RichHpdDeMagList[irichdet][ih]->setCurrentDemagnification( r_demag, phi_demag  );

	  //debug stuff:
// 	  for( double r_cathode=0.0; r_cathode<36.0;  r_cathode+=5.0 ) {
// 	    double r_anode = r_demag->value(r_cathode);
// 	    const Rich1DTabFunc* r_mag     = iDeHpd->  magnification_RtoR();
// 	    const Rich1DTabFunc* phi_mag   = iDeHpd->  magnification_RtoPhi();
// 	    double rback_cathode = r_mag->value(r_anode);
// 	    double deltaphi_anode       = phi_demag->value(r_cathode);
// 	    double deltaphi_back_cathode= phi_mag  ->value(r_anode);
// 	    RichHpdPropLogDemag <<MSG::INFO<<"r_cathode="<<r_cathode<<" r_anode="<<r_anode
// 				<<" ---> " <<"back to r_cathode="<<rback_cathode
// 	      //        <<"  Dphi_anode="<<deltaphi_anode
// 	      //<<" ---> " <<"back to Dphi_cathode="<<deltaphi_back_cathode
// 				<<endreq;
// 	  }

	} else { RichHpdPropLogDemag << MSG::ERROR << "No HPD in "<<location<<endreq; }

      }


  } else { //old table:
    
    SmartDataPtr<TabulatedProperty> tabDemagDefault(detSvc, RichHpdDemagMatTabPropPath);
    std::vector<double> HpdDemagFacDefault;
    if(!tabDemagDefault) {
      RichHpdPropLogDemag << MSG::ERROR <<"RichHpdProperties Demagnification:   "
			  <<" Can't retrieve" +RichHpdDemagMatTabPropPath <<endreq;
    } else {
      TabulatedProperty::Table tableDem = tabDemagDefault->table();
      TabulatedProperty::Table::iterator itd;

      for (itd = tableDem.begin(); itd != tableDem.end(); itd++) {
	HpdDemagFacDefault.push_back(itd->second);
      }
      // Now for the cross-focussing effect, change the sign of the linear term.
      // The sign of the second (quadratic) term in the vector
      // is to be verified in the future. For now it is left as postive.
      HpdDemagFacDefault[0] = -1.0*HpdDemagFacDefault[0];
    }
    
    for(int irichdet=0; irichdet<m_numberOfRichDetectors; irichdet++ ){
      for(int ih=0; ih< m_numHpdTotRich[irichdet] ; ih++){
	m_RichHpdDeMagList[irichdet][ih]= new RichHpdDeMag(detSvc, ih, irichdet);
	m_RichHpdDeMagList[irichdet][ih]->setCurrentHPDDemag( HpdDemagFacDefault );
      }
    }

  }

}

////////////////////////////////////////////////////////////////////////////
RichHpdQE* RichHpdProperties::getRichHpdQE(int  hpdnum, int richdetnum ) {
  RichHpdQE* curRQE = 0;

  if(richdetnum > 1 || richdetnum < 0) {
    G4cout <<"Rich Hpd Properties: Unknown rich detector number for QE=  "
           <<richdetnum<<G4endl;
  }else if(hpdnum > m_numHpdTotRich[richdetnum] || hpdnum <0 ) {
    G4cout <<"Rich Hpd Properties: Unknown hpd detector number for QE =  "
           << hpdnum<<"  for richdetnum=  "<<richdetnum<<G4endl;

  } else {

    curRQE = m_RichHpdQEList[richdetnum][hpdnum];
  }

  return curRQE;
}

RichHpdPSF* RichHpdProperties::getRichHpdPSF(int hpdnum, int richdetnum ){
  RichHpdPSF* curRPS = 0;

  if(richdetnum > 1 || richdetnum < 0) {
    G4cout <<"Rich Hpd Properties: Unknown rich detector number for PSF =  "
           <<richdetnum<<G4endl;
  }else if(hpdnum > m_numHpdTotRich[richdetnum] || hpdnum <0 ) {
    G4cout <<"Rich Hpd Properties: Unknown hpd detector number for PSF =  "
           << hpdnum<<"  for richdetnum=  "<<richdetnum<<G4endl;

  } else {

    curRPS  = m_RichHpdPSFList[richdetnum][hpdnum];
  }


  return curRPS;
}
RichHpdDeMag* RichHpdProperties::getRichHpdDeMag(int hpdnum, int richdetnum){
  RichHpdDeMag* curDem = 0;

  if(richdetnum > 1 || richdetnum < 0) {
    G4cout <<"Rich Hpd Properties: Unknown rich detector number for Demag =  "
           <<richdetnum<<G4endl;
  }else if(hpdnum >= m_numHpdTotRich[richdetnum] || hpdnum <0 ) {
    G4cout <<"Rich Hpd Properties: Unknown hpd detector number for Demag =  "
           << hpdnum<<"  for richdetnum=  "<<richdetnum<<G4endl;

  } else {

    curDem  = m_RichHpdDeMagList[richdetnum][hpdnum];
  }

  return curDem;
}

RichHpdProperties*  RichHpdProperties::getRichHpdPropertiesInstance() {
  if( RichHpdPropertiesInstance == 0 ) {
    RichHpdPropertiesInstance = new RichHpdProperties();

  }

  return RichHpdPropertiesInstance;
}
