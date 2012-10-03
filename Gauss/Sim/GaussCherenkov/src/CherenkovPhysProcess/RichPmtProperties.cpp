#include "RichPmtProperties.h"
#include "RichG4AnalysisConstGauss.h"
#include "RichG4GaussPathNames.h"
#include "CkvG4GaussPathNames.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/TabulatedProperty.h"
#include "boost/lexical_cast.hpp"
#include "RichG4SvcLocator.h"
#include "G4Material.hh"
#include "CherenkovG4Counters.h"

#include "RichDet/DeRichSystem.h"
#include "RichDet/DeRichHPD.h"

// create with a large number of hpds and then adjust
// the size to the correct number of hpds in the constructer
// after reading from the number of hpds from the xml db.
// here the 200 as the size is just a randomly picked number,
// for initialization.
// the in the initialization the
// parameter is 2 and this is the number of rich detectors in lhcb.
//
RichPmtProperties* RichPmtProperties::RichPmtPropertiesInstance=0;

RichPmtProperties::RichPmtProperties( )
  : m_numPmtTotRich(std::vector<int>(2)),
    m_RichPmtQEList(2,std::vector<RichPmtQE*>(200)),
    m_RichPmtPSFList(2,std::vector<RichPmtPSF*>(200)),
    m_PmtVerboseLevel(0),m_PmtActivateOverRideMaxQEFromDB(false),
    m_PmtDBOverRideMaxQEValue(0.60),
    m_CurQEMatPathname(RichPmtQeffMatTabPropPath ),
    m_CurQETableSourceOption(0) 
{ }
//    m_RichPmtDeMagList(2,std::vector<RichPmtDeMag*>(200)),
//    m_pmtNumBegInHitCollection(std::vector<int>(4)),
//    m_pmtNumEndInHitCollection(std::vector<int>(4)),

void  RichPmtProperties::setPmtPropertiesVerboseLevel(int aLevel ) {
  m_PmtVerboseLevel=aLevel;

}

void RichPmtProperties::InitializePmtProperties( ) {
 
  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichPmtlog( msgSvc, "RichPmtProperties" );
  

  // First set the number of rich detectors.

  m_numberOfRichDetectors=2;
  if((int)m_numPmtTotRich.size() != m_numberOfRichDetectors)
    m_numPmtTotRich.resize(m_numberOfRichDetectors);
  m_numberOfRichHitCollections=4;
  // if((int)  m_pmtNumBegInHitCollection.size() != m_numberOfRichHitCollections)
  //  m_pmtNumBegInHitCollection.resize(m_numberOfRichHitCollections);
  // if((int)  m_pmtNumEndInHitCollection.size() != m_numberOfRichHitCollections)
  //  m_pmtNumEndInHitCollection.resize(m_numberOfRichHitCollections);
  

  // RichPmtlog << MSG::INFO
  //           << " Test of Printout from RichPmtProperties" << endreq;

  SmartDataPtr<DetectorElement> Rich1DE(detSvc,Rich1DeStructurePathName );
  if( !Rich1DE ){
    RichPmtlog << MSG::ERROR
               << "Can't retrieve  "<< Rich1DeStructurePathName << endreq;
  }
  else
  {

    //following line modified to be compatible with recent DetDesc. SE 16-6-2005. 
    // the following is commented out to get the flag from the options file. SE 26-10-2006
    //    m_UsePmtMagDistortions = Rich1DE->param<int>("UsePmtMagDistortions");
   

    m_numPmtTotRich[0]= Rich1DE->param<int>("Rich1TotNumPmt");



    // m_pmtNumBegInHitCollection = Rich1DE->paramVect<int>("RichPmtNumBeginInPanels");
    // m_pmtNumEndInHitCollection = Rich1DE->paramVect<int>("RichPmtNumEndInPanels");


    RichPmtlog << MSG::INFO << "Total Number of pmts in Rich1 = "
               << m_numPmtTotRich[0] <<endreq;

    //  RichPmtlog << MSG::DEBUG<<" Rich Pmt Copy Num In Panels Begin Values  "<<m_pmtNumBegInHitCollection[0]
    //           <<"  "<<m_pmtNumBegInHitCollection[1]<<"   "<<m_pmtNumBegInHitCollection[2]<<"  "
    //            <<m_pmtNumBegInHitCollection[3]<<endreq;
    //  RichPmtlog << MSG::DEBUG<<" Rich Pmt Copy Num In Panels End Values  "<<m_pmtNumEndInHitCollection[0]
    //           <<"  "<<m_pmtNumEndInHitCollection[1]<<"   "<<m_pmtNumEndInHitCollection[2]<<"  "
    //           <<m_pmtNumEndInHitCollection[3]<<endreq;
    
    //following line modified to be compatible with recent DetDesc. SE 16-6-2005. 
    m_PmtMaxQuantumEffFromDB=  Rich1DE->param<double>("RichPmtMaxQE");
    
    setPmtMaximumQuantumEfficiency();
    
    //if( m_PmtActivateOverRideMaxQEFromDB) m_PmtMaxQuantumEff=m_PmtDBOverRideMaxQEValue;

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

    //    RichPmtlog << MSG::ERROR
    //              << "Can't retrieve "<< Rich2DeStructurePathName << endreq;

  }else if(!Rich1DE) {
    RichPmtlog <<MSG::ERROR<<"Can't retrieve " << Rich1DeStructurePathName<<"   for Rich2"
               <<endreq;

  }else {

    // the following line to be un commented after
    // the rich2 structure.xml file has this info.

    // m_numPmtTotRich[1]= Rich2DE->userParameterAsInt("Rich2TotNumPmt");
    //    m_numPmtTotRich[1]= Rich1DE->userParameterAsInt("Rich2TotNumPmt");
    m_numPmtTotRich[1]= Rich1DE->param<int>("Rich2TotNumPmt");
    RichPmtlog << MSG::INFO
               << "Total Number of pmts in Rich2 = "
               << m_numPmtTotRich[1]<<endreq;
    
  }
  
  CherenkovG4Counters* aCherenkovG4Counters=  CherenkovG4Counters:: getInstance();
  aCherenkovG4Counters-> setNumPmtsRich(m_numPmtTotRich[0],m_numPmtTotRich[1]);
  aCherenkovG4Counters->InitCherenkovG4Counters();
  
  
  // to resize the vectors. Just test on the size of one of the vectorlists
  // and adjust all the three vectorlists.
  for (int ird=0; ird<m_numberOfRichDetectors; ird++ ) {

    //    if( m_numPmtTotRich[ird] != (int) m_RichPmtDeMagList[ird].size() ){
    //  m_RichPmtDeMagList[ird].resize(m_numPmtTotRich[ird]);
    // }
    if( m_numPmtTotRich[ird] != (int) m_RichPmtQEList[ird].size() ){
      m_RichPmtQEList[ird].resize(m_numPmtTotRich[ird]);
    }
    if( m_numPmtTotRich[ird] != (int) m_RichPmtPSFList[ird].size() ){
      m_RichPmtPSFList[ird].resize(m_numPmtTotRich[ird]);
    }

  }



  // Now fill in the pmt properties from the db.
  // for now the same properties are filled for all the pmts since there is
  // only one set available. This is to be changed in the future. The (single) set of properties
  // now read from the xml db.

  // First for the pmt QE
  // the photon energy is converted from MeV to eV and the qeff is
  // converted from percentage to absolute values.
  // the future the following may be transferred to the pmtqe class.

  FillPmtQETablesAtInit ( detSvc, msgSvc );

  // FillPmtPSFTablesAtInit (detSvc, msgSvc );

  //  FillPmtDemagTablesAtInit ( detSvc, msgSvc );

  RichPmtlog << MSG::INFO
	     << "Filled the PMT QE, PSF and Demag tables for  RICH  "<<endreq;


  //Now get the PMT High Voltage
  SmartDataPtr<TabulatedProperty>tabHV(detSvc,RichPmtHVMatTabPropPath);
  double PmtHVSingle = 0.0;

  if(!tabHV) {
    RichPmtlog << MSG::ERROR
               <<"RichPmtProperties: "
               <<" Can't retrieve "+ RichPmtHVMatTabPropPath <<endreq;


  }else {
    TabulatedProperty::Table tableHV = tabHV->table();
    PmtHVSingle=tableHV.begin()->second;

  }
  if(m_PmtVerboseLevel >0) {
    RichPmtlog << MSG::INFO
               <<"Pmt HighVoltage value = "<<PmtHVSingle<<endreq;
  }
  m_RichPmtHighVoltage=PmtHVSingle;

  //Now get the PhCathode to Si Surface max Distance
  double pmtQwtoSiDist = 0.0;
  if( !Rich1DE ){
    RichPmtlog << MSG::ERROR
               << "Can't retrieve "<< Rich1DeStructurePathName<<"   forPmtQw toSiDist "
               << endreq;
  }else {


    pmtQwtoSiDist = Rich1DE->param<double>("RichPmtQWToSiMaxDist");

  }
  if(m_PmtVerboseLevel >0 ){
    RichPmtlog << MSG::INFO
               <<"Pmt QW to Si Max Dist = "<<pmtQwtoSiDist <<endreq;
  }
  m_RichPmtQWToSiDist= pmtQwtoSiDist;
  if(m_PmtVerboseLevel >1 ){

    RichPmtlog << MSG::INFO << "Pmt QW to Si Max Dist =  "
               <<m_RichPmtQWToSiDist << endreq;
  }


  // Now get the MaxZHit coordinate in Rich1.
  double MaxZHitInRich1 = 0.0;
  if( !Rich1DE ){
    RichPmtlog << MSG::ERROR
               << "Can't retrieve "<< Rich1DeStructurePathName << " for MaxZhit in Rich1 "
               << endreq;
  }else {

    // modif to to be comaptible with recent Detdesc SE 16-6-2005.
    MaxZHitInRich1 = Rich1DE->param<double>("Rich1MaxDownstreamZHitCoord");

  }
  if ( m_PmtVerboseLevel >0 ) {
    RichPmtlog << MSG::INFO
               <<"Max Z Hit coord in Rich1 = "<<MaxZHitInRich1 <<endreq;
  }
  m_Rich1MaxZHitCoord=  MaxZHitInRich1;

  if(m_PmtVerboseLevel >1 ){

    RichPmtlog << MSG::INFO << "Max Z coord for a hit in Rich1 =  "
               << m_Rich1MaxZHitCoord << endreq;
  }

  // Now get the quartz window name and the photocathode name.
  std::string pmtQWlvname;
  std::string pmtPhCathlvname;
  //  double phcathRinn = 0.0;
  SmartDataPtr<DetectorElement> RichPmtQWDE(detSvc, (Rich1DeStructurePathName+RichPmtQwDeSubPathName));
  if(!RichPmtQWDE) {
    RichPmtlog << MSG::ERROR
               <<"Can't retrieve "+Rich1DeStructurePathName+RichPmtQwDeSubPathName<<endreq;

  }else{

    if(RichPmtQWDE->geometry()->hasLVolume()){
      pmtQWlvname=RichPmtQWDE->geometry()->lvolumeName();
    }else{
      RichPmtlog << MSG::ERROR
                 <<"Erroneous Log Vol for Pmt QW log vol " <<endreq;
    }
  }
  if(m_PmtVerboseLevel >0 ){

    RichPmtlog << MSG::INFO
               <<"Pmt Qw Log Volname =  "<<pmtQWlvname<<endreq;
  }
  m_PmtQWLogVolName = pmtQWlvname;
  SmartDataPtr<DetectorElement> RichPmtPCDE(detSvc, (Rich1DeStructurePathName+RichPmtPhCathDeSubPathname));
  if(!RichPmtPCDE) {
    RichPmtlog << MSG::ERROR
               <<"Can't retrieve " +Rich1DeStructurePathName+RichPmtPhCathDeSubPathname
               <<endreq;

  }else{


    if(RichPmtPCDE->geometry()->hasLVolume()){
      pmtPhCathlvname = RichPmtPCDE->geometry()->lvolumeName();
    }else{
      RichPmtlog << MSG::ERROR
                 <<"Erroneous Log Vol for Pmt PhCathode log vol " <<endreq;
    }

  }
  if(m_PmtVerboseLevel >0 ){

    RichPmtlog << MSG::INFO
               <<"Pmt Ph Cathode Log Volname =  "<<pmtPhCathlvname<<endreq;

  }
  m_PmtPhCathodeLogVolName = pmtPhCathlvname;
  
}

void RichPmtProperties::InitializeSiDetParam() {
  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  SmartDataPtr<DetectorElement> Rich1DE(detSvc,Rich1DeStructurePathName );
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichPmtlog( msgSvc, "RichPmtProperties" );
  if( !Rich1DE ){
    RichPmtlog << MSG::ERROR
               << "Can't retrieve  "<< Rich1DeStructurePathName <<" for sidet param "<< endreq;
  }
  else
  {
    m_siDetXSize= Rich1DE->param<double>("RichPmtAnodeXSize");
    m_siDetYSize= Rich1DE->param<double>("RichPmtAnodeYSize");
    m_siDetZSize= Rich1DE->param<double>("RichPmtAnodeZSize");    
  }  
}
void  RichPmtProperties::setPmtMaximumQuantumEfficiency(){

  if( m_PmtActivateOverRideMaxQEFromDB) {
    m_PmtMaxQuantumEff=m_PmtDBOverRideMaxQEValue;
  }else {
    m_PmtMaxQuantumEff= m_PmtMaxQuantumEffFromDB;    
  }
  
  
}

RichPmtProperties::~RichPmtProperties() { ; }

//////////////////////////////////////////////////////////////////////////
void  RichPmtProperties::FillPmtQETablesAtInit( IDataProviderSvc* detSvc, 
						IMessageSvc* msgSvc  ) {

  MsgStream RichPmtPropLogQE( msgSvc, "RichPmtProperties" );
  std::vector<double> QeSingle;
  std::vector<double> EphotSingle;

  //New stuff: QE tables from condDB (M.Musy 27/01/08)
  SmartDataPtr<DeRichSystem> richsys( detSvc, DeRichLocations::RichSystem );
  if(!richsys) {
    RichPmtPropLogQE << MSG::ERROR << "Could not get RichSystem!"<<endreq;
    return;
  }

  /// Returns a list of all active PMTs identified by their RichSmartID
  //  const LHCb::RichSmartID::Vector pmtlist = richsys->activePMTRichSmartIDs();
  //RichPmtPropLogQE << MSG::INFO <<"QE pmtlist.size()="<<pmtlist.size()<<endreq;

  //  LHCb::RichSmartID::Vector::const_iterator i;
  //  for( i=pmtlist.begin(); i!=pmtlist.end(); ++i ) {
  //  std::string location = richsys->getDePMTLocation(*i);
  //  int irichdet = (*i).rich();

    int nPmtInRich1 = m_RichPmtQEList[0].size();
    int nPmtInRich2 = m_RichPmtQEList[1].size();
      
    //int ih = richsys->copyNumber(*i).data() - nPmtInRich1*irichdet;//runs 0->195 and 0->287
    //if(irichdet>1) RichPmtPropLogQE<<MSG::ERROR<<"Error in PMT numbering scheme!"<<endreq;
//     RichPmtPropLogQE <<MSG::INFO<<"QE DePMTLocation="<<location
//                      <<"  CopyNr="<<richsys->copyNumber(*i)
//                      << "  Nr.in Rich"<<irichdet+1<<"="<<ih<<endreq;

    TabulatedProperty::Table table;

    //    SmartDataPtr<DeRichPMT> iDePmt( detSvc, location );
    // if(  ( !iDePmt ) || (m_UseNominalPmtQE)  ) {
    // if( (m_UseNominalPmtQE)  ) {
      //      RichPmtPropLogQE << MSG::INFO << "No PMT in Conditions DB at "<<location
		  //     <<"Using Old QE default table for this PMT!"<<endreq;

    if(m_CurQETableSourceOption != 0 ) {
      if( m_CurQETableSourceOption == 1 ) { m_CurQEMatPathname= RichPmtCBABorosilicateQeffMatTabPropPath;
      }else if (m_CurQETableSourceOption == 2)  {m_CurQEMatPathname=RichHpdQeffMatTabPropPath;    
      }else if (m_CurQETableSourceOption == 3)  {m_CurQEMatPathname=RichPmtCBAUVGlassQeffMatTabPropPath;    
      }else if (m_CurQETableSourceOption == 4)  {m_CurQEMatPathname=RichPmtUBABorosilicateQeffMatTabPropPath;    
      }else if (m_CurQETableSourceOption == 5)  {m_CurQEMatPathname=RichPmtUBAUVGlassQeffMatTabPropPath;
      }else if (m_CurQETableSourceOption == 6)  {m_CurQEMatPathname=RichPmtSBABorosilicateQeffMatTabPropPath;
      }else if (m_CurQETableSourceOption == 7)  {m_CurQEMatPathname=RichPmtSBAUVGlassQeffMatTabPropPath;
      }
      
    }
    
      
    





     SmartDataPtr<TabulatedProperty> tabQE(detSvc, m_CurQEMatPathname );
     RichPmtPropLogQE<<MSG::INFO<<" Now getting the QE from "<<m_CurQEMatPathname<<endreq;

    //  SmartDataPtr<TabulatedProperty> tabQE(detSvc, RichPmtBorosilicateQeffMatTabPropPath);
    // RichPmtPropLogQE<<MSG::INFO<<" Now getting QE from "<<RichPmtBorosilicateQeffMatTabPropPath<<endreq;    

      if(!tabQE) { 
         RichPmtPropLogQE << MSG::ERROR
				  <<"RichPmtPropertiesQE: Can't retrieve "
      				  << m_CurQEMatPathname << endreq;
      //				  <<RichPmtBorosilicateQeffMatTabPropPath  << endreq;
     } else {
        table = tabQE->table();
      }
      
      // } else {
      //    table = iDePmt->pmtQuantumEff()->tabProperty()->table();
      // }
    
    
    TabulatedProperty::Table::const_iterator it;
    for (it = table.begin(); it != table.end(); it++) {
      double aPhotonEnergy= (it->first);
      double aQeOrig      = (it->second)/100.0; // division by 100 to get from percent to prob 
      double aQeCorrected = aQeOrig;

      // if( (aPhotonEnergy >= (double) m_MinPhotonEnergyInRICH) && 
          //	  (aPhotonEnergy <= (double) m_MaxPhotonEnergyInRICH ) ) {
          //	aQeCorrected = getPmtCorrectedQEFromPhotonEnergy(aPhotonEnergy, aQeOrig);
          // }

      double aPhotonEnergyIneV = aPhotonEnergy*1000000.0; // mult to get from MeV to eV
      EphotSingle.push_back(aPhotonEnergyIneV);
      QeSingle.push_back(aQeCorrected );
    }
    
    

    int numQEbin = (int) QeSingle.size();


    for(int i=0; i<( nPmtInRich1+nPmtInRich2) ; ++i ) {
      int ih= i;
      int irichdet=0;
      if(i >= nPmtInRich1) {
         ih=i-nPmtInRich1;
         irichdet=1;
      }
    m_RichPmtQEList[irichdet][ih] = new RichPmtQE(ih,irichdet);
    if(numQEbin > 0 ){
      m_RichPmtQEList[irichdet][ih]->setAnPmtQEen( numQEbin, QeSingle, EphotSingle);
    } else {
      // the following to be modified for non-existent pmts SE 26-10-2006.
      RichPmtPropLogQE << MSG::WARNING
		       <<" RichPmtProperties: Zero number of bins for pmt QE .Check db for "
		       << "Current richdet pmtInRichDet "<< irichdet<<"  "<<ih
                       <<endreq;
    }
    

    //debug stuff
//       for( double ene=1.75; ene<7.0;  ene+=0.2 ) {
//         const Rich::TabulatedProperty1D* qe_table = iDePmt->pmtQuantumEff();
//         double qe = qe_table->value(ene/1000000.0);
//         RichPmtPropLogQE <<MSG::INFO<<"energy="<<ene<<" wavel="<<1243.125/ene
//                          <<" ---> " << qe <<endreq;
//       }
    
    }// end loop over pmts
    
    
}





double RichPmtProperties::getPmtCorrectedQEFromPhotonEnergy(double photonenergy, double originalQE )
{
  //  double thisPCRI=getPmtPCRIFromPhotEnergy( photonenergy );
  // double thisQWRI=getPmtQWRIFromPhotEnergy( photonenergy );
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

      if(RichPmtPhCathMatName == ((*theMaterialTable)[imata]->GetName())) {
        G4Material* aMatPC = (*theMaterialTable)[imata];
        G4MaterialPropertyVector* RindexPC=
                      aMatPC->GetMaterialPropertiesTable()->GetProperty("RINDEX");
          if(   RindexPC ) {
            thisPCRI= RindexPC->Value(photonenergy);
          } 
          matafound=true;    
      }      
      imata++;      
    }    

    while( (imatb<  (G4int) theMaterialTable->size()) &&  (! matbfound) ) {
      if( RichPmtQWMatName == ((*theMaterialTable)[imatb]->GetName())) {
        G4Material* aMatQW = (*theMaterialTable)[imatb];
        G4MaterialPropertyVector* RindexQW=
                      aMatQW->GetMaterialPropertiesTable()->GetProperty("RINDEX");
        if(   RindexQW ) {
            thisQWRI= RindexQW->Value(photonenergy);
        } 

        matbfound=true;        
      }      
      imatb++;
      
    }

    // the division by 100 from percent to real probability is 
    // done in FillPmtQETablesAtInit of RichPmtProperties. SE 26-10-2007
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
  //G4cout<< " Pmt QW property  energy refindex refl "<<photonenergy<<"   "<<thisQWRI<<"  "<<thisQWrefl<<G4endl;

  //end of testprint
  
  G4double thisPCrefl=(thisPCRI-thisQWRI)/(thisPCRI+thisQWRI);
  thisPCrefl=thisPCrefl*thisPCrefl;

  //test print
  //G4cout<< " Pmt PC property  energy refindex refl "<<photonenergy<<"   "<<thisPCRI<<"  "<<thisPCrefl<<G4endl;
  //end of testprint
  
  
  //double thisTrans=1.0;
  
  //G4double thisTrans= (1+thisQWrefl*thisPCrefl)*(1-thisQWrefl)*(1-thisPCrefl);   //first reflections
  G4double thisTrans= (1-thisQWrefl)*(1-thisPCrefl)/(1-thisPCrefl*thisQWrefl); //Geometric series
  //G4double thisTrans= (1.0-thisQWrefl); //QW refl only
  // G4double thisTrans= (1.0-thisQWrefl)*(1.0-thisPCrefl*thisPCrefl); //Fudge factor -:)S

//test print
  //G4cout<< " Pmt QW-PC transmission "<<photonenergy<<"  "<< thisTrans<<G4endl;


//end test print
  //double thiscorrQE=1.0;
  
  G4double thiscorrQE=(originalQE/(thisTrans)); //first order
  //thiscorrQE=(thiscorrQE/                      //second order
  //                     (1+(1-thiscorrQE)*(thisPCRI-1)*(thisPCRI-1)
  //    		                         /((thisPCRI+1)*(thisPCRI+1))));

  if(thiscorrQE > 1.0) thiscorrQE=1.0;
  
  // test printout

  //G4cout<< "PMT QETable  Photon energy QeOrig QeCorrected "<<photonenergy<<"  "<<originalQE<<"  "
  //      << thiscorrQE<<G4endl;

  // end of testprintout

  return thiscorrQE;

}


void RichPmtProperties::FillPmtPSFTablesAtInit ( IDataProviderSvc* detSvc, IMessageSvc* msgSvc  ) {

  MsgStream RichPmtPropLogPSF( msgSvc, "RichPmtProperties" );


  //Now get the PSF values.
  SmartDataPtr<TabulatedProperty>tabPSF(detSvc,RichPmtPsfMatTabPropPath);
  // SmartDataPtr<TabulatedProperty>tabPSFPhEn(detSvc,RichPmtPsfPhEnMatTabPropPath);
  // SmartDataPtr<TabulatedProperty>tabPSFRadial(detSvc,RichPmtPsfRadialMatTabPropPath);
  double PmtPsfSingle = 0.0;
  std::vector<double> aPsfValueVect;
  aPsfValueVect.clear();
  
  if( (!tabPSF) ) {
    RichPmtPropLogPSF << MSG::ERROR
               <<"RichPmtProperties PSF: "
                      <<" Can't retrieve  "+RichPmtPsfMatTabPropPath <<endreq;
    
      //<< " or " <<RichPmtPsfPhEnMatTabPropPath
      //                <<"  or "<<RichPmtPsfRadialMatTabPropPath               
    //           <<endreq;


  }else {

    TabulatedProperty::Table tablePsf = tabPSF->table();
    //    TabulatedProperty::Table tablePsfPh = tabPSFPhEn->table();
    // TabulatedProperty::Table tablePsfRadial = tabPSFRadial->table();
    // the following line can be removed in the future. Kept for backward compatibility.
    PmtPsfSingle=tablePsf.begin()->second;


    TabulatedProperty::Table::iterator it;
    TabulatedProperty::Table::iterator itp;
    TabulatedProperty::Table::iterator itr;
    for (it = tablePsf.begin(); it != tablePsf.end(); it++) {      
      aPsfValueVect.push_back(it->second);
    }

    //    for (itp = tablePsfPh.begin(); itp != tablePsfPh.end(); itp++) {      
    //   aPsfPhEnergyVect.push_back(itp->second);
    // }

    //    for (itr = tablePsfRadial.begin(); itr != tablePsfRadial.end(); itr++) {      
    //  aPsfRadValueVect.push_back(itr->second);
    //  }
    


  }
  if(m_PmtVerboseLevel >0 ) {
    RichPmtPropLogPSF << MSG::INFO <<"Pmt PSF value =  "<<PmtPsfSingle<<endreq;
  }

  //Now populate the classes for each of the pmts.
  for(int irichdet=0; irichdet<m_numberOfRichDetectors; irichdet++ ){

    int nPmt=m_numPmtTotRich[irichdet];

    if( nPmt != (int) m_RichPmtPSFList[irichdet].size() ){
      m_RichPmtPSFList[irichdet].resize(nPmt);

    }

    for(int ih=0; ih< nPmt ; ih++){
      // now for the PSF
      
      m_RichPmtPSFList[irichdet][ih]= new  RichPmtPSF(ih,irichdet);
      // the following line kept for backward compatibility for now. 26-10-2006
      m_RichPmtPSFList[irichdet][ih]->setCurrentPmtPSF(PmtPsfSingle);
      m_RichPmtPSFList[irichdet][ih]->setpmtPointSpreadFunctionVect(aPsfValueVect );
      //      m_RichPmtPSFList[irichdet][ih]->setpmtPSFPhoEnergyVect(aPsfPhEnergyVect );
      // m_RichPmtPSFList[irichdet][ih]->setpmtPSFRadialPosVect(aPsfRadValueVect );

    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//void  RichPmtProperties::FillPmtDemagTablesAtInit ( IDataProviderSvc* detSvc, //
//						    IMessageSvc* msgSvc )
//{ 
  // dummy method
//}
//
////////////////////////////////////////////////////////////////////////////
RichPmtQE* RichPmtProperties::getRichPmtQE(int  pmtnum, int richdetnum ) {
  RichPmtQE* curRQE = 0;

  if(richdetnum > 1 || richdetnum < 0) {
    G4cout <<"Rich Pmt Properties: Unknown rich detector number for QE=  "
           <<richdetnum<<G4endl;
  }else if(pmtnum > m_numPmtTotRich[richdetnum] || pmtnum <0 ) {
    G4cout <<"Rich Pmt Properties: Unknown pmt detector number for QE =  "
           << pmtnum<<"  for richdetnum=  "<<richdetnum<<G4endl;

  } else {

    curRQE = m_RichPmtQEList[richdetnum][pmtnum];
  }

  return curRQE;
}

RichPmtPSF* RichPmtProperties::getRichPmtPSF(int pmtnum, int richdetnum ){
  RichPmtPSF* curRPS = 0;

  if(richdetnum > 1 || richdetnum < 0) {
    G4cout <<"Rich Pmt Properties: Unknown rich detector number for PSF =  "
           <<richdetnum<<G4endl;
  }else if(pmtnum > m_numPmtTotRich[richdetnum] || pmtnum <0 ) {
    G4cout <<"Rich Pmt Properties: Unknown pmt detector number for PSF =  "
           << pmtnum<<"  for richdetnum=  "<<richdetnum<<G4endl;

  } else {

    curRPS  = m_RichPmtPSFList[richdetnum][pmtnum];
  }


  return curRPS;
}
//RichPmtDeMag* RichPmtProperties::getRichPmtDeMag(int pmtnum, int richdetnum){
//  RichPmtDeMag* curDem = 0;
//
//
//  return curDem;
//}

RichPmtProperties*  RichPmtProperties::getRichPmtPropertiesInstance() {
  if( RichPmtPropertiesInstance == 0 ) {
    RichPmtPropertiesInstance = new RichPmtProperties();

  }

  return RichPmtPropertiesInstance;
}
