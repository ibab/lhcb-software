// $Id: LumiHistoCurrents.cpp
// Include files 
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"
#include "LumiHistoCurrents.h"
#include "Event/ODIN.h"
//-----------------------------------------------------------------------------
// Implementation file for class : LumiHistoCurrents
//
// 2009-05-05 : Antonio Perez-Calero
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiHistoCurrents );



LumiHistoCurrents::LumiHistoCurrents( const std::string& name,
                            ISvcLocator* pSvcLocator )
  : HltBaseAlg  ( name , pSvcLocator )
{
  
}

LumiHistoCurrents::~LumiHistoCurrents()
{
  //delete m_outputSelection;
} 

//---------------------------------------------------------

StatusCode LumiHistoCurrents::initialize() {

  StatusCode sc = HltBaseAlg::initialize(); 
  if ( sc.isFailure() ) return sc;

  // create the histograms
    
  m_HistoCurrents.push_back(TH2F("Beam1Current","Beam1 current vs BX type", 4, 0.,4.,16,0.,15.));
  m_HistoCurrents.push_back(TH2F("Beam2Current","Beam2 current vs BX type", 4, 0.,4.,16,0.,15.));
  
  debug() << "Initialised Lumi Histo Currents" << endmsg ;
  return sc;

}

//---------------------------------------------------------

StatusCode LumiHistoCurrents::execute() {

  StatusCode sc = StatusCode::SUCCESS;  
  
  // get ODIN info
  LHCb::ODIN* odin;
  if( exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
  }else{
    StatusCode sc = Error("ODIN cannot be loaded",StatusCode::FAILURE);
    return sc;
  }
  
  double m_bxtype = 0;
  unsigned int OdinCurrent = 0;

  double m_current1 = 0;
  double m_current2 = 0;

  std::stringstream bxType("");
  bxType << (LHCb::ODIN::BXTypes) odin->bunchCrossingType();
  m_bxtype = odin->bunchCrossingType();

  //debug() << " Bunch Crossing Type : " << bxType.str() << "code"<< m_bxtype << endreq;
  info() << " Bunch Crossing Type : " << bxType.str() << " code: "<< m_bxtype << endreq;

  OdinCurrent = odin->bunchCurrent();
  m_current1 = OdinCurrent>>4;
  m_current2 = OdinCurrent - ((OdinCurrent>>4)<<4);

  info() << " Found currents : "<< OdinCurrent << " Beam 1: "<< m_current1 << " Beam 2: "<< m_current2 << endreq;
  //debug() << " Found currents : "<< OdinCurrent << " Beam 1: "<< m_current1 << " Beam 2: "<< m_current2 << endreq;
  
    
  m_HistoCurrents[0].Fill(m_bxtype, m_current1);
  m_HistoCurrents[1].Fill(m_bxtype, m_current2);

  return sc;
}

//---------------------------------------------------------

StatusCode LumiHistoCurrents::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  return HltBaseAlg::finalize();  // must be called after all other actions
  
}
