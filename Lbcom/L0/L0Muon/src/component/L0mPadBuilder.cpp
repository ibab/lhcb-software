// $Id: L0mPadBuilder.cpp,v 1.2 2001-07-09 19:28:13 atsareg Exp $

/// Include files

/// Gaudi factories for algorithms and converters
#include "GaudiKernel/AlgFactory.h"

/// Gaudi base classes
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/SmartDataPtr.h"

/// Utility classes
#include "GaudiKernel/MsgStream.h"

/// Private classes to the example
#include "L0Muon/L0mPadBuilder.h"
#include "L0Muon/L0mPad.h"
#include "L0Muon/MuonLogicalPadLayout.h"
#include "LHCbEvent/MuonDigit.h"

//------------------------------------------------------------------------------
//
/// Implementation of class :  L0mPadBuilder
//
/// Author: A.Tsaregorodtsev
//
//------------------------------------------------------------------------------


static const AlgFactory<L0mPadBuilder>  Factory;
const IAlgFactory& L0mPadBuilderFactory = Factory;

/// Algorithm parameters which can be set at run time must be declared.
/// This should be done in the constructor.
L0mPadBuilder::L0mPadBuilder(const std::string& name, ISvcLocator* pSvcLocator)
: Algorithm(name, pSvcLocator)
{
    m_layout_xml=true;
    declareProperty("XMLdata",m_layout_xml);
    declareProperty("ZStations",m_zStation);
    declareProperty("Regions1",m_regions[0]);
    declareProperty("Regions2",m_regions[1]);
    declareProperty("Regions3",m_regions[2]);
    declareProperty("Regions4",m_regions[3]);
    declareProperty("Regions5",m_regions[4]);
    m_outputPads = "/Event/MC/L0mPads";
    declareProperty("outputPads",m_outputPads);
}


StatusCode L0mPadBuilder::initialize()   { 


  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << "Initialization starting..." << endreq;
  log << MSG::INFO << "Retrieving Muon Pad Layout" << endreq;
   
  if(m_layout_xml) {
      m_layout = new MuonLogicalPadLayout(detSvc(), log);
  } else {
      m_layout = new MuonLogicalPadLayout(m_zStation,m_regions);    
  }   
  
  // log << MSG::DEBUG << *m_layout << endreq; 
  
  return StatusCode::SUCCESS;
}

StatusCode L0mPadBuilder::execute() {

  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << "execute" << endreq;
   
//=============================
// get Muon digitisations
//=============================
    
  log << MSG::DEBUG << "Retrieving pObject MUPD...  " ;  
  
  std::string mcdigi = "/Event/MC/MuonDigits";
  SmartDataPtr< ObjectVector<MuonDigit> > mupdpc(eventSvc(),mcdigi);
  if(!mupdpc) {
    log << MSG::ERROR << " Can not retrieve MuonDigits from " 
        << mcdigi << endreq;
    return StatusCode::SUCCESS;
  }  
  
  log << MSG::DEBUG << " MUPD size " << mupdpc->size() << endreq; 
  
//=======================================  
// create the collection of L0mPads
//=======================================

  StatusCode sc;    
  ObjectVector<L0mPad>* pc = new ObjectVector<L0mPad>;
  log << MSG::DEBUG << "Registering pc ...  "  ;  
  sc = eventSvc()->registerObject(m_outputPads,pc);
  log << MSG::DEBUG << "done, status " << sc << endreq;  
  if( sc.isFailure() ) {
    if( 0 != pc ) { delete pc; }
    log << MSG::ERROR << "Unable to register the output container="
        << m_outputPads << endreq;
    return sc ;
  } 
   
//====================================
// Fill in the L0mPad's now
//====================================  
  
  MuonDigit* pad;
  L0mPad* l0mpad;     
  ObjectVector<MuonDigit>::iterator impc;
  
  for(impc = mupdpc->begin(); impc != mupdpc->end(); impc++) {
    pad = *impc;
    l0mpad = m_layout->createPad(pad);
    l0mpad->setMCParticles(pad->mcParticles());
    pc->push_back(l0mpad);     
    
  }
  return StatusCode::SUCCESS;
}

StatusCode L0mPadBuilder::finalize()  {
  return StatusCode::SUCCESS;
}


