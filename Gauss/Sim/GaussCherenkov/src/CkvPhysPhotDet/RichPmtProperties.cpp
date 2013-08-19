#include "GaussCherenkov/RichPmtProperties.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussCherenkov/CkvG4GaussPathNames.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/TabulatedProperty.h"
#include "boost/lexical_cast.hpp"
#include "GaussRICH/RichG4SvcLocator.h"
#include "G4Material.hh"
#include "GaussCherenkov/CherenkovG4Counters.h"

#include "RichDet/DeRichSystem.h"
#include "RichDet/DeRichHPD.h"
#include "GaussCherenkov/CkvGeometrySetupUtil.h"



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
  : m_numPmtTotRich(std::vector<int>(3)),
    m_numPmtTotUsedRich(std::vector<int>(3)),
    m_numModuleTotRich(std::vector<int>(3)),
    m_RichPmtQEList(3,std::vector<RichPmtQE*>(200)),
    m_RichPmtPSFList(3,std::vector<RichPmtPSF*>(200)),
    m_PmtVerboseLevel(0),m_PmtActivateOverRideMaxQEFromDB(false),
    m_PmtDBOverRideMaxQEValue(0.60),
    m_CurQEMatPathname(RichPmtQeffMatTabPropPath ),
    m_CurQETableSourceOption(0),
    m_PmtQEScaleFactor( 1.0 ),
    m_ActivatePmtModuleSuppressSet3(false),
    m_ActivatePmtModuleSuppressSet4(false),
    m_ActivatePmtModuleSuppressSet5(false),
    m_ActivatePmtModuleSuppressSet6(false),
    m_SuperRichFlag(false) 
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
  

  m_SuperRichFlag=  getSuperRichFlag() ;

  
  // First set the number of rich detectors.

  m_numberOfRichDetectors= m_SuperRichFlag ? 3 :  2;

  if((int)m_numPmtTotRich.size() != m_numberOfRichDetectors)
    m_numPmtTotRich.resize(m_numberOfRichDetectors);
  if((int)m_numPmtTotUsedRich.size() != m_numberOfRichDetectors)
    m_numPmtTotUsedRich.resize(m_numberOfRichDetectors);

  if((int) m_numModuleTotRich.size() != m_numberOfRichDetectors)
    m_numModuleTotRich.resize(m_numberOfRichDetectors);
   
  
  m_numberOfRichHitCollections= m_SuperRichFlag ? 6: 4;

  // RichPmtlog<<MSG::INFO<< "Num Rich NumCol "<< m_numberOfRichDetectors<<"   "<<m_numberOfRichHitCollections<<endreq;
  
  std::vector<DetectorElement*> R_DE = getRichMasterDetectorElements ();
  // RichPmtlog<<MSG::INFO<< "Detelem for Rich  "<< R_DE<<endreq;
  
  if  ( (!m_SuperRichFlag)  && ( R_DE[0] ) ) {
      

    //following line modified to be compatible with recent DetDesc. SE 16-6-2005. 
    // the following is commented out to get the flag from the options file. SE 26-10-2006
    //    m_UsePmtMagDistortions = Rich1DE->param<int>("UsePmtMagDistortions");
   
    
    m_numPmtTotRich[0]= R_DE[0]->param<int>("Rich1TotNumPmt");
    m_numPmtTotRich[1]= R_DE[0]->param<int>("Rich2TotNumPmt");
    if(R_DE[0]->exists("Rich1TotUsedNumPmt") ) {      
      m_numPmtTotUsedRich[0]= R_DE[0]->param<int>("Rich1TotUsedNumPmt");
    }else {
      m_numPmtTotUsedRich[0] = m_numPmtTotRich[0];
    }

    m_numModuleTotRich[0]= R_DE[0]->param<int>("Rich1TotNumModules");
    m_numModuleTotRich[1]= R_DE[0]->param<int>("Rich2TotNumModules");
    


    //  RichPmtlog << MSG::DEBUG<<" Rich Pmt Copy Num In Panels Begin Values  "<<m_pmtNumBegInHitCollection[0]
    //           <<"  "<<m_pmtNumBegInHitCollection[1]<<"   "<<m_pmtNumBegInHitCollection[2]<<"  "
    //            <<m_pmtNumBegInHitCollection[3]<<endreq;
    //  RichPmtlog << MSG::DEBUG<<" Rich Pmt Copy Num In Panels End Values  "<<m_pmtNumEndInHitCollection[0]
    //           <<"  "<<m_pmtNumEndInHitCollection[1]<<"   "<<m_pmtNumEndInHitCollection[2]<<"  "
    //           <<m_pmtNumEndInHitCollection[3]<<endreq;
    

    m_PmtMaxQuantumEffFromDB=  R_DE[0]->param<double>("RichPmtMaxQE");
    
    setPmtMaximumQuantumEfficiency();
    
    //if( m_PmtActivateOverRideMaxQEFromDB) m_PmtMaxQuantumEff=m_PmtDBOverRideMaxQEValue;

     m_MinPhotonEnergyInRICH=  R_DE[0]->param<double> ("RichMinPhotonEnergy");
     m_MaxPhotonEnergyInRICH=  R_DE[0]->param<double> ("RichMaxPhotonEnergy");
     
    // Now get the MaxZHit coordinate in Rich1.
    m_Rich1MaxZHitCoord = R_DE[0]->param<double>("Rich1MaxDownstreamZHitCoord");
  
    if ( m_PmtVerboseLevel >0 ) {
       RichPmtlog << MSG::INFO <<"Max Z Hit coord in Rich1 = "<<m_Rich1MaxZHitCoord  <<endreq;
    }
    
  

  

    // the following line to be un commented after
    // the rich2 structure.xml file has this info.

    // m_numPmtTotRich[1]= Rich2DE->userParameterAsInt("Rich2TotNumPmt");
    //    m_numPmtTotRich[1]= Rich1DE->userParameterAsInt("Rich2TotNumPmt");
     if( R_DE[1] ) {
       

       m_numPmtTotRich[1]= R_DE[0]->param<int>("Rich2TotNumPmt");
      if(R_DE[0]->exists("Rich2TotNumPmt")){
        m_numPmtTotUsedRich[1]= R_DE[0]->param<int>("Rich2TotNumPmt");
      }else {
        m_numPmtTotUsedRich[1]= m_numPmtTotRich[1];
     }
    
    
       
    RichPmtlog << MSG::INFO << "Classic RICH1 Total Number of pmts used   MaxNumPmt  = "
               << m_numPmtTotUsedRich[0] <<"   "<< m_numPmtTotRich[0]  <<endreq;
       
    RichPmtlog << MSG::INFO
               << "Classic RICH2: Total Number of pmts Used MaxNumPmt = "
               << m_numPmtTotUsedRich[1] << "   "<<  m_numPmtTotRich[1] <<endreq;
    
     } // end test R_DE[1]
     
     
  }    // end test R_DE[0]
  else if(  (m_SuperRichFlag)  && ( R_DE[2] )  ) {
    // now with super Rich


    
    
    m_numPmtTotRich[2]= R_DE[2]->param<int>("SuperRichTotNumPmt");

    if(R_DE[2]->exists("SuperRichTotUsedNumPmt") ) {      
      m_numPmtTotUsedRich[2]= R_DE[2]->param<int>("SuperRichTotUsedNumPmt");
    }else {
      m_numPmtTotUsedRich[2] = m_numPmtTotRich[2];
    }
    
    //    RichPmtlog<<MSG::INFO<<"SR tot num pmt "<<m_numPmtTotRich[2]<<endreq;
    

    m_PmtMaxQuantumEffFromDB=  R_DE[2]->param<double>("RichPmtMaxQE");

    //    RichPmtlog<<MSG::INFO<<"SR max QE "<<   m_PmtMaxQuantumEffFromDB <<endreq;
    
    setPmtMaximumQuantumEfficiency();
         
    //if( m_PmtActivateOverRideMaxQEFromDB) m_PmtMaxQuantumEff=m_PmtDBOverRideMaxQEValue;

     m_MinPhotonEnergyInRICH=  R_DE[2]->param<double> ("RichMinPhotonEnergy");
     m_MaxPhotonEnergyInRICH=  R_DE[2]->param<double> ("RichMaxPhotonEnergy");

     //     RichPmtlog<<MSG::INFO<<"SR max QE ph en min max  "<< m_PmtMaxQuantumEffFromDB<<"   "
     //           << m_MinPhotonEnergyInRICH<<"  "<< m_MaxPhotonEnergyInRICH  <<endreq;
    


    

    
  }
  
  
  
  // skip out if there is neither Rich1 nor SuperRICH

  if(  R_DE[0] || R_DE[2] ) { 
  
  
    
       
  
    CherenkovG4Counters* aCherenkovG4Counters=  CherenkovG4Counters::getInstance();
    if( R_DE[0] ) {
      
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
    
    }else if ( R_DE[2] ) {
      
   


      aCherenkovG4Counters->setNumPmtsSuperRich(m_numPmtTotRich[2] );
      aCherenkovG4Counters->InitCherenkovG4CountersSR() ;
      
     if( m_numPmtTotRich[2] != (int) m_RichPmtQEList[2].size() ){
       m_RichPmtQEList[2].resize(m_numPmtTotRich[2]);
     }
     if( m_numPmtTotRich[2] != (int) m_RichPmtPSFList[2].size() ){
       m_RichPmtPSFList[2].resize(m_numPmtTotRich[2]);
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

    //    RichPmtlog << MSG::INFO<<"Now fill QE  for SR "<<endreq;

   FillPmtQETablesAtInit ( detSvc, msgSvc );

  // FillPmtPSFTablesAtInit (detSvc, msgSvc );

  //  FillPmtDemagTablesAtInit ( detSvc, msgSvc );

   //   RichPmtlog << MSG::INFO
   //   << "Filled the PMT QE, PSF and Demag tables for  RICH  "<<endreq;


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
  //  if(m_PmtVerboseLevel >0) {
    RichPmtlog << MSG::INFO
               <<"Pmt HighVoltage value = "<<PmtHVSingle<<endreq;
    // }
  m_RichPmtHighVoltage=PmtHVSingle;

  //Now get the PhCathode to Si Surface max Distance
  if(m_SuperRichFlag ) {
    m_RichPmtQWToSiDist  = R_DE[2]->param<double>("RichPmtQWToSiMaxDist");
    
  }else {
    m_RichPmtQWToSiDist  = R_DE[0]->param<double>("RichPmtQWToSiMaxDist");
  }
  
  if(m_PmtVerboseLevel >0 ){
    RichPmtlog << MSG::INFO
               <<"Pmt QW to Si Max Dist = "<< m_RichPmtQWToSiDist <<endreq;
  }




   setQWPhCathNames ();


  
  } // end test R_DE[0]
  
  
}

void RichPmtProperties::InitializeSiDetParam() {
  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichPmtlog( msgSvc, "RichPmtProperties" );

  std::string aRDePath = m_SuperRichFlag ? SuperRichDeStructurePathName : Rich1DeStructurePathName;

  SmartDataPtr<DetectorElement> R_DE(detSvc, aRDePath );


 if( !R_DE ){
    RichPmtlog << MSG::ERROR<< "Can't retrieve  "<< aRDePath <<" for sidet param "<< endreq;
  }
  else
  {
    m_siDetXSize= R_DE->param<double>("RichPmtAnodeXSize");
    m_siDetYSize= R_DE->param<double>("RichPmtAnodeYSize");
    m_siDetZSize= R_DE->param<double>("RichPmtAnodeZSize");    
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
  std::vector<double> QeZero;

  //New stuff: QE tables from condDB (M.Musy 27/01/08)
  //  SmartDataPtr<DeRichSystem> richsys( detSvc, DeRichLocations::RichSystem );

  //  DeRichSystem* richsys = getRichSystemDetectorElement ();

  //  if(!richsys) {
  //   RichPmtPropLogQE << MSG::ERROR << "Could not get RichSystem!"<<endreq;
  //  return;
  // }

  /// Returns a list of all active PMTs identified by their RichSmartID
  //  const LHCb::RichSmartID::Vector pmtlist = richsys->activePMTRichSmartIDs();
  //RichPmtPropLogQE << MSG::INFO <<"QE pmtlist.size()="<<pmtlist.size()<<endreq;

  //  LHCb::RichSmartID::Vector::const_iterator i;
  //  for( i=pmtlist.begin(); i!=pmtlist.end(); ++i ) {
  //  std::string location = richsys->getDePMTLocation(*i);
  //  int irichdet = (*i).rich();

  //  m_numberOfRichDetectors= m_SuperRichFlag ? 1 :  2;

  int aTotNumPmtRICH =0;
  int nPmtInRich1 =0;
  int nPmtInRich2 =0;
  if(m_SuperRichFlag ) {
    aTotNumPmtRICH =(int)  m_RichPmtQEList[2].size();
  }else {  
    nPmtInRich1 = m_RichPmtQEList[0].size();
    nPmtInRich2 = m_RichPmtQEList[1].size();
    aTotNumPmtRICH= nPmtInRich1+nPmtInRich2;
  }
  
    //int ih = richsys->copyNumber(*i).data() - nPmtInRich1*irichdet;//runs 0->195 and 0->287
    //if(irichdet>1) RichPmtPropLogQE<<MSG::ERROR<<"Error in PMT numbering scheme!"<<endreq;
//     RichPmtPropLogQE <<MSG::INFO<<"QE DePMTLocation="<<location
//                      <<"  CopyNr="<<richsys->copyNumber(*i)
//                      << "  Nr.in Rich"<<irichdet+1<<"="<<ih<<endreq;

    TabulatedProperty::Table table;
    TabulatedProperty::Table tableSupSet3;
    TabulatedProperty::Table tableSupSet4;
    TabulatedProperty::Table tableSupSet5;
    TabulatedProperty::Table tableSupSet6;

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

     //RichPmtPropLogQE<<MSG::INFO<<"QE source: " << m_CurQETableSourceOption << endreq;
     RichPmtPropLogQE<<MSG::INFO<<"Now getting the QE from "<<m_CurQEMatPathname<<endreq;
     //RichPmtPropLogQE<<MSG::INFO<<"QE overall scale factor: " << m_PmtQEScaleFactor << endreq;

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

      // if( (aPhotonEnergy >= (double) m_MinPhotonEnergyInRICH) && 
          //	  (aPhotonEnergy <= (double) m_MaxPhotonEnergyInRICH ) ) {
          //	aQeCorrected = getPmtCorrectedQEFromPhotonEnergy(aPhotonEnergy, aQeOrig);
          // }

      double aQeCorrected = aQeOrig * m_PmtQEScaleFactor ;

      double aPhotonEnergyIneV = aPhotonEnergy*1000000.0; // mult to get from MeV to eV
      EphotSingle.push_back(aPhotonEnergyIneV);
      QeSingle.push_back(aQeCorrected );
      QeZero.push_back(0.000000001);
    }
    
    

    int numQEbin = (int) QeSingle.size();


    int aNumPmtInModule=16;
    
    std::vector<bool> PmtSupFlag(aTotNumPmtRICH,false);
    G4cout<<" Pmt supress flags set3 set4 set5 set6 "<< m_ActivatePmtModuleSuppressSet3<<"   "
          <<m_ActivatePmtModuleSuppressSet4<<"  "<< m_ActivatePmtModuleSuppressSet5
         <<"   "<< m_ActivatePmtModuleSuppressSet6<< G4endl;
    
    if(m_ActivatePmtModuleSuppressSet3) {
      SmartDataPtr<TabulatedProperty>  PmtModuleSupressSet3Table( detSvc,RichPmtModuleSuppressSet3Path  );
      if( PmtModuleSupressSet3Table ){
        tableSupSet3= PmtModuleSupressSet3Table->table();
        
        TabulatedProperty::Table::const_iterator ita;

        
        for(ita=tableSupSet3.begin(); ita != tableSupSet3.end(); ita++) {
          int aSupModuleA = (int) (ita->second);
          for (int iP=(aSupModuleA*aNumPmtInModule); iP< ((aSupModuleA+1)*aNumPmtInModule) ; iP++) {
            PmtSupFlag[iP]=true; 
          }
          
        } 
      }
      
    }

    if ( m_ActivatePmtModuleSuppressSet4) {
      // for this set only one of row pmts kept. The other three rows are annuled. 6-3-2013
      SmartDataPtr<TabulatedProperty>  PmtModuleSupressSet4Table( detSvc,RichPmtModuleSuppressSet4Path  );
      if( PmtModuleSupressSet4Table ){
        tableSupSet4= PmtModuleSupressSet4Table->table();
        
        TabulatedProperty::Table::const_iterator itb;
        for(itb=tableSupSet4.begin(); itb != tableSupSet4.end(); itb++) {
          int aSupModuleB = (int) (itb->second);
          int  aPmtSupStartSet4= (aSupModuleB*aNumPmtInModule);
          int  aPmtSupEndSet4=aPmtSupStartSet4+aNumPmtInModule;            
 
          //          int aPmtSupEndSet4= aPmtSupStartSet4+12;
          //int aPmtSupEndSet4= aPmtSupStartSet4+12;
          //if( aSupModuleB >= (m_numModuleTotRich[0])/2  ) {
            //            aPmtSupStartSet4=aPmtSupStartSet4+4;
            // aPmtSupEndSet4=aPmtSupStartSet4+aNumPmtInModule;            
          // }
          
          
          for (int iPb=aPmtSupStartSet4; iPb<aPmtSupEndSet4 ; iPb++) {
            PmtSupFlag[iPb]=true; 
          }
          
        } 
      
        
      }
    }
    
    if ( m_ActivatePmtModuleSuppressSet5) {
      SmartDataPtr<TabulatedProperty>  PmtModuleSupressSet5Table( detSvc,RichPmtModuleSuppressSet5Path  );
      if( PmtModuleSupressSet5Table ){
        tableSupSet5= PmtModuleSupressSet5Table->table();
        
        TabulatedProperty::Table::const_iterator itb;
        for(itb=tableSupSet5.begin(); itb != tableSupSet5.end(); itb++) {
          int aSupModuleC = (int) (itb->second);
          for (int iPb=(aSupModuleC*aNumPmtInModule); iPb< ((aSupModuleC+1)*aNumPmtInModule) ; iPb++) {
            PmtSupFlag[iPb]=true; 
          }
          
        } 
      
        
      }
    }
    

      if ( m_ActivatePmtModuleSuppressSet6) {
      SmartDataPtr<TabulatedProperty>  PmtModuleSupressSet6Table( detSvc,RichPmtModuleSuppressSet6Path  );
      if( PmtModuleSupressSet6Table ){
        tableSupSet6= PmtModuleSupressSet6Table->table();
        
        TabulatedProperty::Table::const_iterator itb;
        for(itb=tableSupSet6.begin(); itb != tableSupSet6.end(); itb++) {
          int aSupModuleD = (int) (itb->second);
          for (int iPb=(aSupModuleD*aNumPmtInModule); iPb< ((aSupModuleD+1)*aNumPmtInModule) ; iPb++) {
            PmtSupFlag[iPb]=true; 
          }
          
        } 
      
        
      }
      }
      
    
    
    

   for(int i=0; i<(aTotNumPmtRICH) ; ++i ) {
      int ih= i;
      int irichdet=0;
      if(i >= nPmtInRich1) {
         ih=i-nPmtInRich1;
         irichdet=1;
      }
      if(m_SuperRichFlag) irichdet=2;
      
    m_RichPmtQEList[irichdet][ih] = new RichPmtQE(ih,irichdet);
    if(numQEbin > 0 ){

      if( !( PmtSupFlag[i]) ) {

          m_RichPmtQEList[irichdet][ih]->setAnPmtQEen( numQEbin, QeSingle, EphotSingle);
      }else {
        // G4cout<<" Supressing QE of pmt module "<< i <<"  "<<(i/16) <<G4endl;
        
         m_RichPmtQEList[irichdet][ih]->setAnPmtQEen( numQEbin, QeZero, EphotSingle);
      }

    } else {
      // the following to be modified for non-existent pmts SE 26-10-2006.
      RichPmtPropLogQE << MSG::WARNING
		       <<" RichPmtProperties: Zero number of bins for pmt QE .Check db for "
		       << "Current richdet pmtInRichDet "<< irichdet<<"  "<<ih
                       <<endreq;
    }
    

    
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
              thisPCRI= RindexPC->Value(photonenergy); //G4.95
              //thisPCRI= RindexPC->GetProperty(photonenergy);
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
                      thisQWRI= RindexQW->Value(photonenergy); //G4.95
                      //  thisQWRI= RindexQW->GetProperty(photonenergy);
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

  if(richdetnum > m_numberOfRichDetectors  || richdetnum < 0) {
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

  if( (richdetnum >=  m_numberOfRichDetectors) || (richdetnum < 0) ) {
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

std::vector<DetectorElement*> RichPmtProperties::getRichMasterDetectorElements ( ) 
{ 

  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichPmtlog( msgSvc, "RichPmtProperties" );
  


  std::vector<DetectorElement*> a_DE;
  a_DE.clear();
  
  if(  getSuperRichFlag() ) {
    a_DE.resize(3);
    SmartDataPtr<DetectorElement> SuperRDE(  detSvc,SuperRichDeStructurePathName );
    a_DE[2]= SuperRDE;
    if(!SuperRDE) {
       RichPmtlog << MSG::ERROR << "Can't retrieve  SuperRichDE  from "<< SuperRichDeStructurePathName << endreq;
    }    


    
  }else {   
    a_DE.resize(2);
    SmartDataPtr<DetectorElement> Rich1DE(detSvc,Rich1DeStructurePathName );
    SmartDataPtr<DetectorElement> Rich2DE(detSvc,Rich2DeStructurePathName );
    a_DE[0]=     Rich1DE;
    a_DE[1]=     Rich2DE;

    if( !Rich1DE ){
       RichPmtlog << MSG::ERROR << "Can't retrieve  Rich1DE  from "<< Rich1DeStructurePathName << endreq;
     }
    if( !Rich2DE ){
       RichPmtlog << MSG::WARNING << "Can't retrieve  Rich1DE  from "<< Rich2DeStructurePathName << endreq;
     }


    
  }
  
  return a_DE;  
}
DeRichSystem* RichPmtProperties::getRichSystemDetectorElement (){
  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichPmtlog( msgSvc, "RichPmtProperties" );
  
  DeRichSystem*  aDE;
  
  if(  getSuperRichFlag() ) {
     
     SmartDataPtr<DeRichSystem> SuperRSystemDE(  detSvc,SuperRichSystemDeStructurePathName );
     aDE = SuperRSystemDE;
     
  }else {
    SmartDataPtr<DeRichSystem> RichClassicSystemDE (  detSvc, DeRichLocations::RichSystem ) ;
     aDE = RichClassicSystemDE;
    
    
  }
  
  return aDE;
  
}



void   RichPmtProperties::setQWPhCathNames () {
   IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichPmtlog( msgSvc, "RichPmtProperties" );
   


  // Now get the quartz window name and the photocathode name.
  // also the lens name.


  std::string aRDePath = m_SuperRichFlag ? SuperRichDeStructurePathName : Rich1DeStructurePathName;

  std::string aPmtPhcathSubPathName =m_SuperRichFlag ? SuperRichLPmtPhCathDeSubPathname :  RichPmtPhCathDeSubPathname;
  

  SmartDataPtr<DetectorElement> R_DET (detSvc, aRDePath);  

  // temporarily comment out the superrich option

  //  if(! m_SuperRichFlag ) {
    

  if(m_SuperRichFlag) {
    
    SmartDataPtr<DetectorElement> SRichLPmtQWDE(detSvc, ( aRDePath+ SuperRichLPmtQwDeSubPathName));
    
    SmartDataPtr<DetectorElement> SRichLPmtPCDE(detSvc, (aRDePath + SuperRichLPmtPhCathDeSubPathname  ));
     
    SmartDataPtr<DetectorElement> SRichLPmtLensDE(detSvc, ( aRDePath + SuperRichLPmtLensDeSubPathName ));
    if(!SRichLPmtQWDE) {
     RichPmtlog << MSG::ERROR
                <<"SuperRich Can't retrieve "+aRDePath + SuperRichLPmtQwDeSubPathName <<endreq;
    }else {
           if(SRichLPmtQWDE->geometry()->hasLVolume()){
             m_LPmtQWLogVolName =SRichLPmtQWDE->geometry()->lvolumeName();

             
           }else {
                 RichPmtlog << MSG::ERROR <<"Erroneous Log Vol for SuperRich  Pmt QW log vol " <<endreq;
            
           }       
    }
    
    if(!SRichLPmtPCDE) {
      RichPmtlog << MSG::ERROR
                <<"SuperRich Can't retrieve "+aRDePath + SuperRichLPmtPhCathDeSubPathname <<endreq;
       
    }else {
      if(  SRichLPmtPCDE->geometry()->hasLVolume()){ 
          m_LPmtPhCathodeLogVolName = SRichLPmtPCDE->geometry()->lvolumeName();     

      }else {
                 RichPmtlog << MSG::ERROR <<"Erroneous Log Vol for SuperRich  Pmt PhCath log vol " <<endreq;
                 
      }  
      
    }
    if( !SRichLPmtLensDE ) {
       
      RichPmtlog << MSG::ERROR
                <<"SuperRich Can't retrieve "+aRDePath + SuperRichLPmtLensDeSubPathName <<endreq;
       
    }else {
      if( SRichLPmtLensDE ->geometry()->hasLVolume()){ 
       m_LPmtLensLogVolName    =SRichLPmtLensDE  ->geometry()->lvolumeName();     
      }else {
        RichPmtlog << MSG::ERROR <<"Erroneous Log Vol for SuperRich  Pmt Lens log vol " <<endreq;
                 
      }  
      
    }
    
    
  }else {  // now for Two Rich options
    
     SmartDataPtr<DetectorElement> RichPmtQWDE(detSvc, ( aRDePath+RichPmtQwDeSubPathName ));
    
    SmartDataPtr<DetectorElement> RichPmtPCDE(detSvc, (aRDePath + RichPmtPhCathDeSubPathname ));

    SmartDataPtr<DetectorElement> RichLPmtQWDE(detSvc, ( aRDePath+RichLPmtQwDeSubPathName ));
    
    SmartDataPtr<DetectorElement> RichLPmtPCDE(detSvc, (aRDePath + RichLPmtPhCathDeSubPathname ));
     
    SmartDataPtr<DetectorElement> RichLPmtLensDE(detSvc, ( aRDePath + RichLPmtLensDeSubPathName ));
    
    if( RichPmtQWDE->geometry()->hasLVolume())  m_PmtQWLogVolName =RichPmtQWDE->geometry()->lvolumeName();
    if(RichPmtPCDE->geometry()->hasLVolume())  m_PmtPhCathodeLogVolName = RichPmtPCDE->geometry()->lvolumeName();

    if(! RichLPmtQWDE || !RichLPmtPCDE || !RichLPmtLensDE  ) {

    if( R_DET ){
      if(R_DET->exists("Rich1PmtLensPresence") ) {         
        if( ( R_DET->param<int> ("Rich1PmtLensPresence")) == 1 ) {
         RichPmtlog << MSG::ERROR<<"Expecting Rich1PmtLens,  but Can't retrieve "+aRDePath + RichLPmtQwDeSubPathName<<endreq;
         
        }
        
      }
      
    }
    
    }else {    

    if(RichLPmtQWDE->geometry()->hasLVolume())     m_LPmtQWLogVolName =RichLPmtQWDE->geometry()->lvolumeName();
    if(RichLPmtPCDE->geometry()->hasLVolume())   m_LPmtPhCathodeLogVolName  = RichLPmtPCDE->geometry()->lvolumeName();
    if(RichLPmtLensDE->geometry()->hasLVolume())   m_LPmtLensLogVolName =RichLPmtLensDE->geometry()->lvolumeName();
    
    }
    
    
  }  

  
  //  }// end temporary avoidance of superrich option.
  
}


CkvGeometrySetupUtil * RichPmtProperties::getCkvGeometrySetup () 
{return CkvGeometrySetupUtil::getCkvGeometrySetupUtilInstance(); }

bool RichPmtProperties::getSuperRichFlag() {
  CkvGeometrySetupUtil * aCkvGeometrySetup=getCkvGeometrySetup () ;
  return  (aCkvGeometrySetup->isSuperRich()); 
}
