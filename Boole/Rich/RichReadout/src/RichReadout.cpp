
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IToolSvc.h"

#include "RichReadout/RichReadout.h"
#include "RichReadout/RichFrontEndResponse.h"

#include "Event/MCHit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"


static const  AlgFactory<RichReadout>  s_factory ;
const         IAlgFactory& RichReadoutFactory = s_factory ;

RichReadout::RichReadout( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : Algorithm ( name, pSvcLocator ) {

}

RichReadout::~RichReadout() {};

StatusCode RichReadout::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize" << endreq;

  std::string mytype="RichFrontEndResponse";
  std::string myname="RichFrontEndResponse";

  StatusCode scAlgorithm = createSubAlgorithm(mytype, myname, m_response);

  if( scAlgorithm.isFailure()) {
    log << MSG::FATAL << "    Unable to create sub-algorithm" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

StatusCode RichReadout::execute() {

  MsgStream log(msgSvc(), name());

  m_response->execute();

  return StatusCode::SUCCESS;

}

StatusCode RichReadout::finalize()
{

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Finalize" << endreq;
  return StatusCode::SUCCESS;

}










