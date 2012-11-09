#include "TorchTBMcpProperties.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussCherenkov/CkvG4GaussPathNames.h"
#include "TorchTBGaussPathNames.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/TabulatedProperty.h"
#include "boost/lexical_cast.hpp"
#include "GaussRICH/RichG4SvcLocator.h"
#include "G4Material.hh"


// create with a large number of hpds and then adjust
// the size to the correct number of hpds in the constructer
// after reading from the number of hpds from the xml db.
// here the 200 as the size is just a randomly picked number,
// for initialization.
// the in the initialization the
// parameter is 2 and this is the number of rich detectors in lhcb.
//
TorchTBMcpProperties* TorchTBMcpProperties::TorchTBMcpPropertiesInstance=0;

TorchTBMcpProperties::TorchTBMcpProperties( )
  : m_numMcpInTorchTB(2),
    m_TorchTBMcpQEList(std::vector<RichPmtQE*>(2)),
    m_TorchTBMcpVerboseLevel(0),
    m_TorchCurQEMatPathname(TorchTBMcpQeffMatTabPropPath )
{ }

void  TorchTBMcpProperties::setTorchTBMcpPropertiesVerboseLevel(int aLevel ) {
  m_TorchTBMcpVerboseLevel=aLevel;

}

void TorchTBMcpProperties::InitializeTorchTBMcpProperties() {
 
  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream TorchTBMcplog( msgSvc, "TorchTBMcpProperties" );
  
  SmartDataPtr<DetectorElement> TorchTBDE(detSvc,TorchTBDeStructurePathName);
  

  m_numberOfTorchHitCollections=1;


  //  m_numMcpInTorchTB  to be accessed from the TorchTB detector element
  TorchTBMcplog<<MSG::INFO <<" Number of Mcps in Torch Testbeam "<<m_numMcpInTorchTB <<endreq;

    m_TorchTBMcpMaxQuantumEff=1.0;
  
  // m_TorchTBMcpMaxQuantumEff=TorchTBDE->param<double> ("TorchTestBeamMcpMaxQE");
  

  //  SmartDataPtr<DetectorElement> Rich1DE(detSvc,Rich1DeStructurePathName );
  //  if( !Rich1DE ){
  //  TorchTBMcplog << MSG::ERROR
  //             << "Can't retrieve  "<< Rich1DeStructurePathName << endreq;
  // }
  
  //
  //   m_MinPhotonEnergyInRICH=  Rich1DE->param<double> ("RichMinPhotonEnergy");
  //   m_MaxPhotonEnergyInRICH=  Rich1DE->param<double> ("RichMaxPhotonEnergy");
    //temporary test
    // m_MinPhotonEnergyInRICH=1.5*eV;
    // m_MaxPhotonEnergyInRICH=7.0*eV;
    //end temporary test
  // }

  
  // to resize the vectors. 

  m_TorchTBMcpQEList.resize(m_numMcpInTorchTB);

  // Now fill in the Mcp properties from the db.
  // for now the same properties are filled for all the mcpss since there is
  // only one set available. The (single) set of properties
  // now read from the xml db.

  // First for the pmt QE
  // the photon energy is converted from MeV to eV and the qeff is
  // converted from percentage to absolute values.
  // the future the following may be transferred to the pmtqe class.

  TorchTBMcplog << MSG::INFO<<" Now filling Torch Tb QE tables at init "<<endreq;
  
  
  FillTBMcpQETablesAtInit ( detSvc, msgSvc );

  TorchTBMcplog << MSG::INFO
	     << "Filled the MCP QE tables for Torch TB  "<<endreq;

 TorchTBMcplog << MSG::INFO
	     << "Now get  the MCP HV for Torch TB  "<<endreq;

  //Now get the PMT High Voltage
  SmartDataPtr<TabulatedProperty>tabHV(detSvc,TorchTBMcpHVMatTabPropPath);
  double McpHVSingle = 0.0;

  if(!tabHV) {
    TorchTBMcplog << MSG::ERROR
               <<"TorchTBMcpProperties: "
               <<" Can't retrieve "+ TorchTBMcpHVMatTabPropPath <<endreq;


  }else {
    TabulatedProperty::Table tableHV = tabHV->table();
    McpHVSingle=tableHV.begin()->second;

  }
  if(m_TorchTBMcpVerboseLevel >0) {
    TorchTBMcplog << MSG::INFO
               <<"Mcp HighVoltage value = "<<McpHVSingle<<endreq;
  }
  m_TorchTBMcpHighVoltage=McpHVSingle;

  // Now get the MaxZHit coordinate in Rich1.


  // Now get the quartz window name and the photocathode name.
  std::string mcpQWlvname = "/dd/Geometry/AfterMagnetRegion/TorchTBMaster/lvMCPQuartz";
  
  std::string mcpPhCathlvname= "/dd/Geometry/AfterMagnetRegion/TorchTBMaster/lvMCPPhCathode";
  
  //  double phcathRinn = 0.0;
  //SmartDataPtr<DetectorElement> TorchTBMcpQWDE(detSvc, (TorchTBDeStructurePathName+TorchTBMcpQwDeSubPathName));
  // if(!TorchTBMcpQWDE) {
  //  TorchTBMcplog << MSG::ERROR
  //             <<"Can't retrieve "+TorchTBDeStructurePathName+TorchTBMcpQwDeSubPathName<<endreq;

  //  }else{

  //    if(TorchTBMcpQWDE->geometry()->hasLVolume()){
  //    mcpQWlvname=TorchTBMcpQWDE->geometry()->lvolumeName();
  //  }else{
  //    TorchTBMcplog << MSG::ERROR
  //               <<"Erroneous Log Vol for Mcp QW log vol " <<endreq;
  //  }
  //  }
  //  if(m_TorchTBMcpVerboseLevel >0 ){

    TorchTBMcplog << MSG::INFO
               <<"Mcp Qw Log Volname =  "<<mcpQWlvname<<endreq;
    //  }
  
    m_TorchTBMcpQWLogVolName = mcpQWlvname;
    //  SmartDataPtr<DetectorElement> TorchTBMcpPCDE(detSvc, (TorchTBDeStructurePathName+TorchTBMcpPhCathDeSubPathname));
    // if(!TorchTBMcpPCDE) {
    // TorchTBMcplog << MSG::ERROR
    //           <<"Can't retrieve " +TorchTBDeStructurePathName+TorchTBMcpPhCathDeSubPathname
    //           <<endreq;

    //  }else{


    //    if(TorchTBMcpPCDE->geometry()->hasLVolume()){
    //   mcpPhCathlvname = TorchTBMcpPCDE->geometry()->lvolumeName();
    // }else{
    //  TorchTBMcplog << MSG::ERROR
    //             <<"Erroneous Log Vol for Mcp PhCathode log vol " <<endreq;
      // }
    //
    //  }
//  if(m_TorchTBMcpVerboseLevel >0 ){

    TorchTBMcplog << MSG::INFO
               <<"Pmt Ph Cathode Log Volname =  "<<mcpPhCathlvname<<endreq;

    //  }
  m_TorchTBMcpPhCathodeLogVolName = mcpPhCathlvname;
  
}

void TorchTBMcpProperties::InitializeTorchTBAnodeDetParam() {
  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  SmartDataPtr<DetectorElement> TorchTBDE(detSvc, TorchTBDeStructurePathName);
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();

  MsgStream TorchTBMcplog( msgSvc, "TorchTBMcpProperties" );
  if( !TorchTBDE ){
    TorchTBMcplog << MSG::ERROR
               << "Can't retrieve  "<< TorchTBDeStructurePathName <<" for anode param "<< endreq;
  }
  else
  {
    m_TorchAnodeDetXSize= TorchTBDE ->param<double>("TorchTBMcpAnodeXSize");
    m_TorchAnodeDetYSize= TorchTBDE ->param<double>("TorchTBMcpAnodeYSize");
    m_TorchAnodeDetZSize= TorchTBDE ->param<double>("TorchTBMcpAnodeZSize");    
  }  
}


TorchTBMcpProperties::~TorchTBMcpProperties() { ; }

//////////////////////////////////////////////////////////////////////////
void  TorchTBMcpProperties::FillTBMcpQETablesAtInit( IDataProviderSvc* detSvc, 
						IMessageSvc* msgSvc  ) {

  MsgStream TorchTBMcpPropLogQE( msgSvc, "TorchTBMcpProperties" );
  std::vector<double> QeSingle;
  std::vector<double> EphotSingle;      

    TabulatedProperty::Table table;
     SmartDataPtr<TabulatedProperty> tabQE(detSvc, m_TorchCurQEMatPathname );
     TorchTBMcpPropLogQE<<MSG::INFO<<" Now getting the QE from "<<m_TorchCurQEMatPathname<<endreq;

      if(!tabQE) { 
         TorchTBMcpPropLogQE << MSG::ERROR
				  <<"TorchTBMcpPropertiesQE: Can't retrieve "
      				  <<  m_TorchCurQEMatPathname << endreq;

      } else {
        table = tabQE->table();
      }
      
    
    TabulatedProperty::Table::const_iterator it;
    for (it = table.begin(); it != table.end(); it++) {
      double aPhotonEnergy= (it->first);
      double aQeOrig      = (it->second)/100.0; // division by 100 to get from percent to prob 
      double aQeCorrected = aQeOrig;

      double aPhotonEnergyIneV = aPhotonEnergy*1000000.0; // mult to get from MeV to eV
      EphotSingle.push_back(aPhotonEnergyIneV);
      QeSingle.push_back(aQeCorrected );
    }

    int numQEbin = (int) QeSingle.size();
    for(int i=0; i< m_numMcpInTorchTB ; ++i ) {
      m_TorchTBMcpQEList [i] = new RichPmtQE(i,0);
      if(numQEbin > 0 ){
        m_TorchTBMcpQEList[i]->setAnPmtQEen( numQEbin, QeSingle, EphotSingle);
      } else {

        TorchTBMcpPropLogQE << MSG::WARNING
		       <<" TorchTBMcpProperties: Zero number of bins for Mcp QE .Check db for "
                          << "Current Mcp "<< i<<endreq;
      }
    
    }// end loop over mcps
    
    
}
RichPmtQE* TorchTBMcpProperties::getTorchTBMcpQE(int  mcpnum ) {
  RichPmtQE* curRQE = 0;
  if(mcpnum >= m_numMcpInTorchTB) {
    
    G4cout<<" Unknown MCP number for getting the QE. Max current MCp "
    << m_numMcpInTorchTB<<"  "<<mcpnum<<G4endl;
    
  } else {

    curRQE = m_TorchTBMcpQEList [mcpnum];
  }

  return curRQE;
}

TorchTBMcpProperties*  TorchTBMcpProperties::getTorchTBMcpPropertiesInstance() {
  if( TorchTBMcpPropertiesInstance == 0 ) {
    TorchTBMcpPropertiesInstance = new TorchTBMcpProperties();

  }

  return TorchTBMcpPropertiesInstance;
}
