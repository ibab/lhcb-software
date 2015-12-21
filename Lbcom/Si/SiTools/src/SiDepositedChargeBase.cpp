// $Id: SiDepositedChargeBase.cpp,v 1.1 2007-12-11 10:14:25 mneedham Exp $

// Gaudi

#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

// LHCbKernel
#include "Kernel/LHCbConstants.h"

// MCEvent
#include "Event/MCHit.h"

// local
#include "SiDepositedChargeBase.h"

using namespace LHCb;
using namespace Gaudi::Units;



SiDepositedChargeBase::SiDepositedChargeBase(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent) :
  GaudiTool( type, name, parent )
{
  // constructer
  declareProperty("delta2", m_delta2 = 1800*keV*keV/cm);

  // need a line here to get the interface correct
  declareInterface<ISiDepositedCharge>(this);
}

StatusCode SiDepositedChargeBase::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  /// initialize generators .
  auto tRandNumSvc = service<IRndmGenSvc>("RndmGenSvc", true);
  m_GaussDist = tRandNumSvc->generator(Rndm::Gauss(0.,1.0));
  if (!m_GaussDist) return Error( "Failed to init generator ", sc);

  return sc;
}

