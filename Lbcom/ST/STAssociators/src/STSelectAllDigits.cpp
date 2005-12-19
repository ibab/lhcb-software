// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"

#include "STSelectAllDigits.h"


static const ToolFactory<STSelectAllDigits>  s_factory;
const IToolFactory& STSelectAllDigitsFactory = s_factory;


STSelectAllDigits::STSelectAllDigits(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  GaudiTool(type, name, parent)
{

 // interfaces
 
 declareInterface<ISTDigitSelector>(this);
}

STSelectAllDigits::~STSelectAllDigits(){
  // destructer
}


StatusCode STSelectAllDigits::initializeEvent(){

  // intialize once per event

  return StatusCode::SUCCESS;
}

bool STSelectAllDigits::execute(const LHCb::STDigit* ){

  // search for digit in cluster container
  return true;
}
