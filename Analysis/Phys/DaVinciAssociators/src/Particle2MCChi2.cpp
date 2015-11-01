// $Id: Particle2MCChi2.cpp,v 1.14 2007-01-12 13:58:53 ranjard Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// event
#include "Event/MCParticle.h"
#include "Event/Particle.h"

// local
#include "Particle2MCChi2.h"

using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : Particle2MCChi2
//
// 17/05/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( Particle2MCChi2 )

#define _verbose if( msgLevel(MSG::VERBOSE) ) verbose()
#define _debug if( msgLevel(MSG::DEBUG) ) debug()
#define _info if( msgLevel(MSG::INFO) ) info()

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2MCChi2::Particle2MCChi2( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : AsctAlgorithm ( name , pSvcLocator )
  , m_chi2( 100. )
{
  declareProperty( "Chi2Cut", m_chi2 );
}

//=============================================================================
// Destructor
//=============================================================================
Particle2MCChi2::~Particle2MCChi2() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Particle2MCChi2::initialize() {

  _debug << "==> Initialise" << endmsg;

  StatusCode sc = GaudiAlgorithm::initialize();
  if( !sc.isSuccess() ) return sc;

  m_p2MCLink.reset( new Object2MCLinker<>( this,
                                      Particle2MCMethod::WithChi2,
                                      m_inputData) );
  return !m_p2MCLink ? Error("Cannot create Object2MCLinker helper")
                     : StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2MCChi2::execute() {

  _debug << "==> Execute" << endmsg;

  //////////////////////////////////
  // Loop on Particles containers //
  //////////////////////////////////

  for( const auto& inp : m_inputData ) {
    // Create a linker table
    const std::string linkContainer =
      inp + Particle2MCMethod::extension[Particle2MCMethod::Chi2];
    auto linkerTable = m_p2MCLink->linkerTable( linkContainer );
    if( !linkerTable) continue;

    // Get Particles
    SmartDataPtr<Particles> parts (eventSvc(), inp);
    if( !parts ) continue;
    int npp = parts->size();
    int nass = 0;
    int nrel = 0;
    _debug << "    " << npp
           << " Particles retrieved from " << inp << endmsg;

    for( const auto&  part : *parts) {
      _verbose << "    Particle " << objectName(part);
      const MCParticle* mcPart = m_p2MCLink->first( part);
      bool found = false;
      double minChi2 = 0;
      while( mcPart ) {
        double weight = m_p2MCLink->weight();
        if( weight <= m_chi2 ) {
          if( !found ) {
            _verbose << " associated to MCParts";
          }
          _verbose << " - " << mcPart->key();
          if( linkerTable)
            linkerTable->link( part, mcPart, weight);
          found = true;
          nrel++;
        }
        if( minChi2 == 0 || weight < minChi2) minChi2 = weight;
        mcPart = m_p2MCLink->next();
      }
      if( found) ++nass;
      else {
        _verbose << " not associated to any MCPart, minChi2 = "
                            << minChi2;
      }
      _verbose << endmsg;
    }
    _debug
      << "Out of " << npp << " Particles in " << inp << ", "
      << nass << " are associated, "
      << nrel << " relations found" << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Particle2MCChi2::finalize() {

  _debug << "==> Finalize" << endmsg;
  m_p2MCLink.reset();
  return GaudiAlgorithm::finalize();
}

//=============================================================================
