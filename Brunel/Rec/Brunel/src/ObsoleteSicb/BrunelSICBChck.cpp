// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Rec/Brunel/src/ObsoleteSicb/BrunelSICBChck.cpp,v 1.1 2001-06-07 09:23:20 cattaneb Exp $

#include "BrunelSICBChck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

extern "C" {
#ifdef WIN32
  void __stdcall SUCHECK( void );
#else
  void sucheck_( void );
#define SUCHECK     sucheck_
#endif
}

//-----------------------------------------------------------------------------
// Implementation file for class : BrunelSICBChck
//
// 01/06/2001 : Marco Cattaneo
//-----------------------------------------------------------------------------

static const AlgFactory<BrunelSICBChck>    Factory;
const IAlgFactory& BrunelSICBChckFactory = Factory;

// Standard creator
BrunelSICBChck::BrunelSICBChck( const std::string& name,
                                ISvcLocator* pSvcLocator )
                              : Algorithm( name, pSvcLocator ) { }

StatusCode BrunelSICBChck::initialize() { return StatusCode::SUCCESS; }

StatusCode BrunelSICBChck::execute() {
  SUCHECK();
  return StatusCode::SUCCESS;
}

StatusCode BrunelSICBChck::finalize() { return StatusCode::SUCCESS; }

