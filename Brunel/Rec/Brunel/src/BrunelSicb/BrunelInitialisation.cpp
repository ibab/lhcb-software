// $Id: BrunelInitialisation.cpp,v 1.3 2001-07-27 09:54:17 cattaneb Exp $

// Include files
#include "BrunelInitialisation.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "LHCbEvent/MCEvent.h"
#include "SicbCnv/SicbFortran.h"

//----------------------------------------------------------------------------
//
// Implementation of class :  BrunelInitialisation
//
// Author:      Marco Cattaneo
// Created:     15th February 2000
// Modified: MC 2001/05/31 - to fix NPILEUP for RAWH2 data
// Modified: MC 2001/07/27 - to make sure Raw and Rec event roots are created
//-----------------------------------------------------------------------------
extern "C" {
#ifdef WIN32
  void __stdcall BRUNELINIT( void );
#else
  void brunelinit_( void );
#define BRUNELINIT brunelinit_
#endif
}

static const AlgFactory<BrunelInitialisation>  s_factory;
const IAlgFactory& BrunelInitialisationFactory = s_factory;

// Necessary to instantiate factory method
 IAlgorithm* BrunelInitialisation::create(const std::string& name, 
                                          ISvcLocator* loc)   {
  IAlgorithm *alg = new BrunelInitialisation( name,loc);
  return alg;
}

BrunelInitialisation::BrunelInitialisation( const std::string& name, 
                                            ISvcLocator* pSvcLocator )
                     : Algorithm( name, pSvcLocator ) { }

BrunelInitialisation::~BrunelInitialisation() { }

StatusCode BrunelInitialisation::initialize() { return StatusCode::SUCCESS; }

StatusCode BrunelInitialisation::execute() {

  MsgStream log( msgSvc(), name() );

  // Initialise SICB stuff
  BRUNELINIT();

  // Update the common block variable NPILEUP with number of piled up events.
  // Necessary for generator level pileup.
  // May break some SICB DST code if pileup has been done in Brunel
  
  std::string mcPath = "/Event/MC";
  SmartDataPtr<MCEvent> mcEvt(eventSvc(),mcPath);
  if (0 == mcEvt){
    log <<MSG::ERROR << mcPath << " doesn't exist" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    int npile = mcEvt->pileUp();
    SicbFortran::SetNPileUp( npile );
    log << MSG::DEBUG << "npile = " << npile << endreq;
  }
  
  // ensure Raw and Rec sub-event roots exist 
  std::string rawPath = "/Event/Raw";
  SmartDataPtr<DataObject> rawEvt(eventSvc(),rawPath);
  if (0 == rawEvt){
    log <<MSG::ERROR << rawPath << " doesn't exist" << endreq;
    return StatusCode::FAILURE;
  }

  std::string recPath = "/Event/Rec";
  SmartDataPtr<DataObject> recEvt(eventSvc(),recPath);
  if (0 == recEvt){
    log <<MSG::ERROR << recPath << " doesn't exist" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode BrunelInitialisation::finalize() { return StatusCode::SUCCESS; }
