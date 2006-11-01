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
    m_HpdVerboseLevel(0)
{ }

void  RichHpdProperties::setHpdPropertiesVerboseLevel(int aLevel ) {
  m_HpdVerboseLevel=aLevel;

}
void RichHpdProperties::InitializeHpdProperties( ) {

  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichHpdlog( msgSvc, "RichHpdProperties" );


  // First set the number of rich detectors.

  m_numberOfRichDetectors=2;
  if((int)m_numHpdTotRich.size() != m_numberOfRichDetectors)
    m_numHpdTotRich.resize(m_numberOfRichDetectors);

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

    //    m_numHpdTotRich[0]= Rich1DE->userParameterAsInt("Rich1TotNumHpd");

    RichHpdlog << MSG::INFO << "Total Number of hpds in Rich1 = "
               << m_numHpdTotRich[0] << endreq;

    //following line modified to be compatible with recent DetDesc. SE 16-6-2005. 
    m_HpdMaxQuantumEff=  Rich1DE->param<double>("RichHpdMaxQE");
    //    m_HpdMaxQuantumEff=  Rich1DE->userParameterAsDouble("RichHpdMaxQE");

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

    if( m_numHpdTotRich[ird] != (int) m_RichHpdQEList[ird].size() ){
      m_RichHpdDeMagList[ird].resize(m_numHpdTotRich[ird]);
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


RichHpdProperties::~RichHpdProperties() { ; }

void  RichHpdProperties::FillHpdQETablesAtInit( IDataProviderSvc* detSvc, IMessageSvc* msgSvc  ) {

  MsgStream RichHpdPropLogQE( msgSvc, "RichHpdProperties" );

  SmartDataPtr<TabulatedProperty>tabQE(detSvc,RichHpdQeffMatTabPropPath);
  std::vector<double>QeSingle;
  std::vector<double>EphotSingle;
  int numQEbin = 0;
  if(!tabQE) {

    RichHpdPropLogQE << MSG::ERROR
               <<"RichHpdPropertiesQE: Can't retrieve "
               << RichHpdQeffMatTabPropPath << endreq;

  }else {

    TabulatedProperty::Table table = tabQE->table();
    TabulatedProperty::Table::iterator it;
    for (it = table.begin(); it != table.end(); it++) {
      EphotSingle.push_back((it->first)*1000000.0);
      QeSingle.push_back((it->second)/100.0);
    }

    numQEbin=(int) QeSingle.size();

    // now for the printout
     if(m_HpdVerboseLevel >0) {
      RichHpdPropLogQE << MSG::INFO  <<"Number of QE bins "<<numQEbin<<endreq;
      RichHpdPropLogQE << MSG::INFO  <<"ephot and qe values: "<<endreq;
      for(int ii=0; ii< numQEbin; ii++ ) {
        RichHpdPropLogQE << MSG::INFO <<"energy QE   "
                   << EphotSingle[ii] <<"   "<<QeSingle[ii] <<endreq;

      }
    }
    // end of printout

  }

  for(int irichdet=0; irichdet<m_numberOfRichDetectors; irichdet++ ){
    int nHpd=m_numHpdTotRich[irichdet];

    if( nHpd != (int) m_RichHpdQEList[irichdet].size() ){
      m_RichHpdQEList[irichdet].resize(nHpd);
    }

    for(int ih=0; ih< nHpd ; ih++){
      
      m_RichHpdQEList[irichdet][ih] = new RichHpdQE(ih,irichdet);
      if(numQEbin > 0 ){
        m_RichHpdQEList[irichdet][ih]->setAnHpdQEen(numQEbin,QeSingle,EphotSingle);
      } else {
	// the following to be modified for non-existent hpds SE 26-10-2006.
        RichHpdPropLogQE << MSG::WARNING
                   <<" RichHpdProperties: Zero number of bins for hpd QE .Check db for "
		     << "Current richdet hpdInRichDet "<< irichdet<<"  "<<ih
                   <<endreq;
      }
    }
  }


}

void RichHpdProperties::FillHpdPSFTablesAtInit ( IDataProviderSvc* detSvc, IMessageSvc* msgSvc  ) {

  MsgStream RichHpdPropLogPSF( msgSvc, "RichHpdProperties" );


  //Now get the PSF values.
  SmartDataPtr<TabulatedProperty>tabPSF(detSvc,RichHpdPsfMatTabPropPath);
  SmartDataPtr<TabulatedProperty>tabPSFPhEn(detSvc,RichHpdPsfPhEnMatTabPropPath);
  SmartDataPtr<TabulatedProperty>tabPSFRadial(detSvc,RichHpdPsfRadialMatTabPropPath);
  double HpdPsfSingle = 0.0;
  std::vector<double> aPsfValueVect;
  std::vector<double> aPsfPhEnergyVect;
  std::vector<double> aPsfRadValueVect;
  aPsfValueVect.clear();
  aPsfPhEnergyVect.clear();
  aPsfRadValueVect.clear();
  
  
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
    // the following line can be removed in the future. Kept for backward compatibility.
    HpdPsfSingle=tablePsf.begin()->second;


    TabulatedProperty::Table::iterator it;
    TabulatedProperty::Table::iterator itp;
    TabulatedProperty::Table::iterator itr;
    for (it = tablePsf.begin(); it != tablePsf.end(); it++) {      
      aPsfValueVect.push_back(it->second);
    }

    for (itp = tablePsfPh.begin(); itp != tablePsfPh.end(); itp++) {      
      aPsfPhEnergyVect.push_back(itp->second);
    }

    for (itr = tablePsfRadial.begin(); itr != tablePsfRadial.end(); itr++) {      
      aPsfRadValueVect.push_back(itr->second);
    }
    


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

    }
  }
}

void  RichHpdProperties::FillHpdDemagTablesAtInit ( IDataProviderSvc* detSvc, IMessageSvc* msgSvc  ) {

  MsgStream RichHpdPropLogDemag( msgSvc, "RichHpdProperties" );

  /////////////////////////////////////
  // Now get the old demagnification values without any magfield distortion.
  // These are stored into the class as separate members for cross checks with
  // mag field distorted values . SE 26-10-2006.
  

  SmartDataPtr<TabulatedProperty> tabDemagDefault(detSvc, RichHpdDemagMatTabPropPath);
  std::vector<double> HpdDemagFacDefault;

  //    if(!m_UsingHpdMagneticFieldDistortion){

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
      if(HpdDemagFacDefault[0] != 0.0 )HpdDemagFacDefault[0] = -1.0*HpdDemagFacDefault[0];
    }

    //    }


  //Now populate the classes for each of the hpds.
  for(int irichdet=0; irichdet<m_numberOfRichDetectors; irichdet++ ){
    int nHpd=m_numHpdTotRich[irichdet];


    if( nHpd != (int) m_RichHpdQEList[irichdet].size() ){
      m_RichHpdDeMagList[irichdet].resize(nHpd);
    }

    for(int ih=0; ih< nHpd ; ih++){

      // now for the Demag
      m_RichHpdDeMagList[irichdet][ih]= new RichHpdDeMag(detSvc, ih, irichdet);


      if( m_UsingHpdMagneticFieldDistortion ) {
	     //Now get the demagnification table with the magfield distortions
	     int total_ih = ih + irichdet * m_numHpdTotRich[0];
	     std::string numb = "Sim_" + boost::lexical_cast<std::string>(total_ih);
	     SmartDataPtr<TabulatedProperty> tabdemag(detSvc, 
						 RichHpdDemagPathName+numb );
       TabulatedProperty::Table demag;
       RichHpdPropLogDemag << MSG::DEBUG<<"RichHpdProperties "<<numb<<endreq;
	     if(!tabdemag) {
	        RichHpdPropLogDemag << MSG::ERROR <<"RichHpdProperties Demag: "
		      <<"No " << RichHpdDemagPathName+numb <<endreq;
	     } else {
         RichHpdPropLogDemag << MSG::DEBUG<<"RichHpdProperties success"<<endreq;
          // RichHpdPropLogDemag << MSG::INFO<<"RichHpdProperties success"<<endreq;
	       demag = tabdemag->table();
	      }
      	m_RichHpdDeMagList[irichdet][ih]->setCurrentDemagnification( demag );
      }
      

      //  else {
      // the old Demagfactors without mag field distortion
    	m_RichHpdDeMagList[irichdet][ih]->setCurrentHPDDemag( HpdDemagFacDefault );
      // }
    }
  }


}


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
