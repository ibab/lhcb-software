// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/L0mPadBuilder.cpp,v 1.1.1.1 2001-04-06 14:26:02 atsareg Exp $

/// Include files

/// Gaudi factories for algorithms and converters
#include "GaudiKernel/AlgFactory.h"

/// Gaudi base classes
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/SmartDataPtr.h"

/// Utility classes
#include "GaudiKernel/MsgStream.h"

/// Private classes to the example
#include "L0mPadBuilder.h"
#include "L0Muon/L0mPad.h"
#include "MuonPadLayout.h"
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
    declareProperty("BasicCellX",m_basicCellX);
    declareProperty("BasicCellY",m_basicCellY);
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
      m_layout = new MuonPadLayout(detSvc());
  } else {
      m_layout = new MuonPadLayout(m_zStation,
                                   m_basicCellX,
				   m_basicCellY,
				   m_regions);    
  }    
  m_layout->print(log);
  
  return StatusCode::SUCCESS;
}

StatusCode L0mPadBuilder::execute() {

  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << "execute" << endreq;
  
// create the collection of L0mPads

  StatusCode sc;
  
    
  ObjectVector<L0mPad>* pc = new ObjectVector<L0mPad>;
  log << MSG::DEBUG << "Registering pc ...  "  ;  
  sc = eventSvc()->registerObject(m_outputPads,pc);
  log << MSG::DEBUG << "done, status " << sc << endreq;  
    
// get Muon digitisation
    
  log << MSG::DEBUG << "Retrieving pObject MUPD...  " ;  
  
  DataObject* pObject;  
  sc = eventSvc()->retrieveObject("/Event/MC/MuonDigits",pObject);
  log << MSG::DEBUG << "done, status " << (int)sc.isSuccess() << endreq;  
    
  ObjectVector<MuonDigit>* mupdpc;
  mupdpc = dynamic_cast<ObjectVector<MuonDigit>* >(pObject);
  
  log << MSG::DEBUG << " MUPD size " << mupdpc->size() << endreq; 
  
  MuonDigit* pad;
  L0mPad* l0mpad;   
  
  ObjectVector<MuonDigit>::iterator impc;
//  SmartRefVector<MCParticle> vmc;
//  SmartRefVector<MCParticle>::iterator ivmc;
  
  for(impc = mupdpc->begin(); impc != mupdpc->end(); impc++) {

//      cout << "Muon Hit Z " << (*impc)->getZ() << endl;
//      vmc = (*impc)->mcParticles();
//      int ic = 1;
//      for(ivmc = vmc.begin(); ivmc != vmc.end(); ivmc++, ic++) {
//          cout << "MC track " << ic << " id: " 
//	       << (*ivmc)->particleID().id() << "\n" << endreq; 
//      }

      pad = *impc;
      l0mpad = m_layout->createPad(pad);
      l0mpad->setMCParticles(pad->mcParticles());
      pc->push_back(l0mpad);
/*      
      log << MSG::INFO << "Muon digit X/Y/Z: " 
                       << (*impc)->getX() << "/"
                       << (*impc)->getY() << "/"
		       << (*impc)->getZ() << endreq;
      l0mpad->print();                                   
*/    
     
  }
  

  return StatusCode::SUCCESS;
}

StatusCode L0mPadBuilder::finalize()  {
  return StatusCode::SUCCESS;
}
