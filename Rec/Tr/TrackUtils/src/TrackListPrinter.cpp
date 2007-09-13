// $Id: TrackListPrinter.cpp,v 1.1 2007-09-13 08:35:07 wouter Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// track interfaces
#include "Event/Track.h"

#include "TrackListPrinter.h"
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackListPrinter );

TrackListPrinter::TrackListPrinter(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
  declareProperty( "inputLocation",  m_inputLocation  = TrackLocation::Default );
}

TrackListPrinter::~TrackListPrinter()
{
  // destructor
}


StatusCode TrackListPrinter::execute()
{
  Tracks* inCont = get<Tracks>(m_inputLocation);
  
  if(inCont)
    for (Tracks::const_iterator iterT = inCont->begin(); iterT != inCont->end(); ++iterT) 
      info() << **iterT << endmsg ;

  return StatusCode::SUCCESS;
};

