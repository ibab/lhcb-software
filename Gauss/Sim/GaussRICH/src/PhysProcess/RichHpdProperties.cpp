#include "RichHpdProperties.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/TabulatedProperty.h"

// create with a large number of hpds and then adjust
// the size to the correct number of hpds in the constructer
// after reading from the number of hpds from the xml db.
// here the 200 as the size is just a randomly picked number,
// for initialization.
// the in the initialization the
// parameter is 2 and this is the number of rich detectors in lhcb.
//
RichHpdProperties::RichHpdProperties(IDataProviderSvc* detSvc,
                                     IMessageSvc* msgSvc)
  : m_numHpdTotRich(std::vector<int>(2)),
    m_RichHpdQEList(2,std::vector<RichHpdQE*>(200)),
    m_RichHpdPSFList(2,std::vector<RichHpdPSF*>(200)),
    m_RichHpdDeMagList(2,std::vector<RichHpdDeMag*>(200))
{

  MsgStream RichHpdlog( msgSvc, "RichHpdProperties" );

  // the following Msgstream and verbose level may be
  // modified inthe future.

  HpdVerboseLevel=0;

  // First set the number of rich detectors.

  m_numberOfRichDetectors=2;
  if((int)m_numHpdTotRich.size() != m_numberOfRichDetectors)
    m_numHpdTotRich.resize(m_numberOfRichDetectors);

  // RichHpdlog << MSG::INFO
  //           << " Test of Printout from RichHpdProperties" << endreq;
  // First get the number of hpds in rich1 and rich2.
  // following modif to be compatible with recent Detdesc. SE 16-6-2005.

  //  SmartDataPtr<IDetectorElement> Rich1DE(detSvc, "/dd/Structure/LHCb/Rich1");
  SmartDataPtr<DetectorElement> Rich1DE(detSvc, "/dd/Structure/LHCb/Rich1");
  if( !Rich1DE ){
    RichHpdlog << MSG::ERROR
               << "Can't retrieve /dd/Structure/LHCb/Rich1" << endreq;
  }
  else
  {

    //following line modified to be compatible with recent DetDesc. SE 16-6-2005. 
    m_numHpdTotRich[0]= Rich1DE->param<int>("Rich1TotNumHpd");

    //    m_numHpdTotRich[0]= Rich1DE->userParameterAsInt("Rich1TotNumHpd");

    RichHpdlog << MSG::INFO << "Total Number of hpds in Rich1 = "
               << m_numHpdTotRich[0] << endreq;

    //following line modified to be compatible with recent DetDesc. SE 16-6-2005. 
    m_HpdMaxQuantumEff=  Rich1DE->param<double>("RichHpdMaxQE");
    //    m_HpdMaxQuantumEff=  Rich1DE->userParameterAsDouble("RichHpdMaxQE");

  }

  SmartDataPtr<DetectorElement> Rich2DE(detSvc, "/dd/Structure/LHCb/Rich2");
  if( !Rich2DE )
  {

    //    RichHpdlog << MSG::ERROR
    //              << "Can't retrieve /dd/Structure/LHCb/Rich2" << endreq;

  }else if(!Rich1DE) {
    RichHpdlog <<MSG::ERROR<<"Can't retrieve /dd/Structure/Rich1 for Rich2"
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

      m_RichHpdQEList[ird].resize(m_numHpdTotRich[ird]);
      m_RichHpdPSFList[ird].resize(m_numHpdTotRich[ird]);
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

  SmartDataPtr<TabulatedProperty>tabQE(detSvc,"/dd/Materials/RichMaterialTabProperties/HpdQuantumEff");
  std::vector<double>QeSingle;
  std::vector<double>EphotSingle;
  int numQEbin = 0;
  if(!tabQE) {

    RichHpdlog << MSG::ERROR
               <<"RichHpdProperties: Can't retrieve "
               <<"/dd/Materials/RichMaterialTabProperties/HpdQuantumEff " << endreq;

  }else {

    TabulatedProperty::Table table = tabQE->table();
    TabulatedProperty::Table::iterator it;
    for (it = table.begin(); it != table.end(); it++) {
      EphotSingle.push_back((it->first)*1000000.0);
      QeSingle.push_back((it->second)/100.0);
    }

    numQEbin=(int) QeSingle.size();

    if(HpdVerboseLevel >0) {
      RichHpdlog << MSG::INFO  <<"Number of QE bins "<<numQEbin<<endreq;
      RichHpdlog << MSG::INFO  <<"ephot and qe values: "<<endreq;
      for(int ii=0; ii< numQEbin; ii++ ) {
        RichHpdlog << MSG::INFO <<"energy QE   "
                   << EphotSingle[ii] <<"   "<<QeSingle[ii] <<endreq;

      }
    }
  }

  //Now get the PSF values.
  SmartDataPtr<TabulatedProperty>tabPSF(detSvc,"/dd/Materials/RichMaterialTabProperties/HpdPointSpreadFunction");
  double HpdPsfSingle = 0.0;
  if(!tabPSF) {
    RichHpdlog << MSG::ERROR
               <<"RichHpdProperties: "
               <<" Can't retrieve /dd/Materials/RichMaterialTabProperties/HpdPSF "
               <<endreq;


  }else {
    TabulatedProperty::Table tablePsf = tabPSF->table();
    HpdPsfSingle=tablePsf.begin()->second;

  }
  if(HpdVerboseLevel >0 ) {
    RichHpdlog << MSG::INFO <<"Hpd PSF value = "<<HpdPsfSingle<<endreq;
  }
  //Now get the demagnification values
  SmartDataPtr<TabulatedProperty>tabDemag(detSvc,"/dd/Materials/RichMaterialTabProperties/HpdDemagnification");
  std::vector<double>HpdDemagFac;
  if(!tabDemag) {
    RichHpdlog << MSG::ERROR <<"RichHpdProperties: "
               <<" Can't retrieve /dd/Materials/RichMaterialTabProperties/HpdDemagnification "<<endreq;

  }else{
    TabulatedProperty::Table tableDem = tabDemag->table();
    TabulatedProperty::Table::iterator itd;

    for (itd = tableDem.begin(); itd != tableDem.end(); itd++) {

      HpdDemagFac.push_back(itd->second);
    }

    // Now for the cross-focussing effect.
    // the sign of the second term in the vector
    // is to be verified in the future. For now it is left as postive.

    if(HpdDemagFac[0] != 0.0 )HpdDemagFac[0] = -1.0*HpdDemagFac[0];


  }
  if(HpdVerboseLevel >0 ){
    RichHpdlog << MSG::INFO <<" Hpd demag factors "
               <<HpdDemagFac[0]<<"   "<<HpdDemagFac[1]<<endreq;
  }
  //Now populate the classes for each of the hpds.
  for(int irichdet=0; irichdet<m_numberOfRichDetectors; irichdet++ ){
    int nHpd=m_numHpdTotRich[irichdet];
    for(int ih=0; ih< nHpd ; ih++){
      //first for the QE
      m_RichHpdQEList[irichdet][ih] = new RichHpdQE(ih,0);
      if(numQEbin > 0 ){
        m_RichHpdQEList[irichdet][ih]->setAnHpdQEen(numQEbin,QeSingle,EphotSingle);
      }else{
        RichHpdlog << MSG::ERROR
                   <<" RichHpdProperties: Zero number of bins for hpd QE .Check db "
                   <<endreq;
      }
      // now for the PSF
      m_RichHpdPSFList[irichdet][ih]= new  RichHpdPSF(ih,0);
      m_RichHpdPSFList[irichdet][ih]->setCurrentHpdPSF(HpdPsfSingle);
      // now for the Demagfactors
      m_RichHpdDeMagList[irichdet][ih]= new RichHpdDeMag(ih,0);
      if((int) HpdDemagFac.size() >= 2 ) {
        m_RichHpdDeMagList[irichdet][ih]->setCurrentHPDDemag(HpdDemagFac);
      }
    }
  }


  //Now get the HPD High Voltage
  SmartDataPtr<TabulatedProperty>tabHV(detSvc,"/dd/Materials/RichMaterialTabProperties/HpdHighVoltage");
  double HpdHVSingle = 0.0;

  if(!tabHV) {
    RichHpdlog << MSG::ERROR
               <<"RichHpdProperties: "
               <<" Can't retrieve /dd/Materials/RichMaterialTabProperties/HpdHighVoltage "<<endreq;


  }else {
    TabulatedProperty::Table tableHV = tabHV->table();
    HpdHVSingle=tableHV.begin()->second;

  }
  if(HpdVerboseLevel >0) {
    RichHpdlog << MSG::INFO
               <<"Hpd HighVoltage value = "<<HpdHVSingle<<endreq;
  }
  m_RichHpdHighVoltage=HpdHVSingle;

  //Now get the PhCathode to Si Surface max Distance
  double hpdQwtoSiDist = 0.0;
  if( !Rich1DE ){
    RichHpdlog << MSG::ERROR
               << "Can't retrieve /dd/Structure/LHCb/Rich1 forHpdQw toSiDist "
               << endreq;
  }else {

    // modif to be compatible with recent Detdesc, SE 16-6-2005.
    hpdQwtoSiDist = Rich1DE->param<double>("RichHpdQWToSiMaxDist");
    //    hpdQwtoSiDist = Rich1DE->userParameterAsDouble("RichHpdQWToSiMaxDist");
  }
  if(HpdVerboseLevel >0 ){
    RichHpdlog << MSG::INFO
               <<"Hpd QW to Si Max Dist = "<<hpdQwtoSiDist <<endreq;
  }
  m_RichHpdQWToSiDist= hpdQwtoSiDist;
  if(HpdVerboseLevel >1 ){

    RichHpdlog << MSG::INFO << "Hpd QW to Si Max Dist =  "
               <<m_RichHpdQWToSiDist << endreq;
  }


  // Now get the MaxZHit coordinate in Rich1.
  double MaxZHitInRich1 = 0.0;
  if( !Rich1DE ){
    RichHpdlog << MSG::ERROR
               << "Can't retrieve /dd/Structure/LHCb/Rich1 for MaxZhit in Rich1 "
               << endreq;
  }else {

    // modif to to be comaptible with recent Detdesc SE 16-6-2005.
    MaxZHitInRich1 = Rich1DE->param<double>("Rich1MaxDownstreamZHitCoord");
    //    MaxZHitInRich1 = Rich1DE->userParameterAsDouble("Rich1MaxDownstreamZHitCoord");
  }
  if ( HpdVerboseLevel >0 ) {
    RichHpdlog << MSG::INFO
               <<"Max Z Hit coord in Rich1 = "<<MaxZHitInRich1 <<endreq;
  }
  m_Rich1MaxZHitCoord=  MaxZHitInRich1;

  if(HpdVerboseLevel >1 ){

    RichHpdlog << MSG::INFO << "Max Z coord for a hit in Rich1 =  "
               << m_Rich1MaxZHitCoord << endreq;
  }

  // Now get the quartz window name and the photocathode name.
  std::string hpdQWlvname;
  std::string hpdPhCathlvname;
  double phcathRinn = 0.0;
  SmartDataPtr<DetectorElement> RichHpdQWDE(detSvc, "/dd/Structure/LHCb/Rich1/Rich1FirstHpdQW");
  if(!RichHpdQWDE) {
    RichHpdlog << MSG::ERROR
               <<"Can't retrieve /dd/Structure/LHCb/Rich1/Rich1FirstHpdQW "<<endreq;

  }else{

    if(RichHpdQWDE->geometry()->hasLVolume()){
      hpdQWlvname=RichHpdQWDE->geometry()->lvolumeName();
    }else{
      RichHpdlog << MSG::ERROR
                 <<"Erroneous Log Vol for Hpd QW log vol " <<endreq;
    }
  }
  if(HpdVerboseLevel >0 ){

    RichHpdlog << MSG::INFO
               <<"Hpd Qw Log Volname =  "<<hpdQWlvname<<endreq;
  }
  m_HpdQWLogVolName = hpdQWlvname;
  SmartDataPtr<DetectorElement> RichHpdPCDE(detSvc, "/dd/Structure/LHCb/Rich1/Rich1FirstHpdPhCathode");
  if(!RichHpdPCDE) {
    RichHpdlog << MSG::ERROR
               <<"Can't retrieve /dd/Structure/LHCb/Rich1/Rich1FirstHpdPhCathode "
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

    //    phcathRinn= RichHpdPCDE->userParameterAsDouble( "HpdPhCathodeRadInner");

  }
  if(HpdVerboseLevel >0 ){

    RichHpdlog << MSG::INFO
               <<"Hpd Ph Cathode Log Volname =  "<<hpdPhCathlvname<<endreq;
    RichHpdlog << MSG::INFO
               <<"Hpd Ph Cathode Rinner = "<< phcathRinn<<endreq;
  }
  m_HpdPhCathodeLogVolName = hpdPhCathlvname;

  m_HpdPhCathodeInnerRadius= phcathRinn;

}


RichHpdProperties::~RichHpdProperties() { ; }

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
  }else if(hpdnum > m_numHpdTotRich[richdetnum] || hpdnum <0 ) {
    G4cout <<"Rich Hpd Properties: Unknown hpd detector number for Demag =  "
           << hpdnum<<"  for richdetnum=  "<<richdetnum<<G4endl;

  } else {

    curDem  = m_RichHpdDeMagList[richdetnum][hpdnum];
  }

  return curDem;
}

