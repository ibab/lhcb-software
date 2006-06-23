// $Id: Particle2MCWithChi2.cpp,v 1.9 2006-06-23 14:54:39 phicharp Exp $
// Include files 
#include <math.h>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// histograms
#include  "GaudiKernel/IHistogramSvc.h"

// event
#include "Event/MCParticle.h"
#include "Event/Particle.h"

// local
#include "Particle2MCWithChi2.h"

using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : Particle2MCWithChi2
//
// 11/04/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<Particle2MCWithChi2>          s_factory ;
const        IAlgFactory& Particle2MCWithChi2Factory = s_factory ; 

#define _verbose if( msgLevel(MSG::VERBOSE) ) verbose()
#define _debug if( msgLevel(MSG::DEBUG) ) debug()
#define _info if( msgLevel(MSG::INFO) ) info()

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2MCWithChi2::Particle2MCWithChi2( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : AsctAlgorithm ( name , pSvcLocator )
  , m_histos( false )
{
  declareProperty( "FillHistos", m_histos );
}

//=============================================================================
// Destructor
//=============================================================================
Particle2MCWithChi2::~Particle2MCWithChi2() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Particle2MCWithChi2::initialize() {

  _debug << "==> Initialise" << endreq;
  // Book histograms...
  if( m_histos ) {    
    m_hisChi2 = histoSvc()->book( "asct", 1, "Chi2", 100, 0., 1000. );
    m_hisChi2vsDiffP = histoSvc()->book( "asct", 2, "log(Chi2) vs dP/P",
                                         100, 0., 1., 35, 0., 7);
    m_hisMinChi2vsDiffP = histoSvc()->book( "asct", 3, "log(Chi2) vs dP/P",
                                            100, 0., 1., 35, 0., 7);
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2MCWithChi2::execute() {

  _debug << "==> Execute" << endreq;

  // Get the MCParticles
  MCParticles* mcParts = get<MCParticles>( MCParticleLocation::Default );

  //////////////////////////////////
  // Loop on Particles containers //
  //////////////////////////////////

  for( std::vector<std::string>::iterator inp = m_inputData.begin(); 
       m_inputData.end()!= inp; inp++) {
    // Create a linker table
    const std::string linkContainer = 
      *inp + Particle2MCMethod::extension[Particle2MCMethod::WithChi2];
    // Just a fake helper class
    Object2MCLinker p2MCLink(this);
    Object2MCLinker::Linker*
      linkerTable = p2MCLink.linkerTable( linkContainer );
    if( NULL != linkerTable ) {
      _verbose << "    Created Linker table for container "
                          << linkContainer << endreq;
    } else {
      _verbose << "    Linker table for container "
                          << linkContainer << " already exists"
                          << endreq;
    }
    if( NULL == linkerTable ) continue;
    
    // Get Particles
    SmartDataPtr<Particles> parts (eventSvc(), *inp);
    if( 0 == parts ) continue;
    int npp = parts->size();
    int nass = 0;
    _verbose << "    " << npp 
                        << " Particles retrieved from " << *inp << endreq;
    
    // loop on Parts and then on MCParts to match them
    for( Particles::const_iterator pIt=parts->begin();
         parts->end() != pIt; pIt++) {
      // Info on the smallest chi2
      double minChi2=0;
      const MCParticle* minMCPart=0;
      Gaudi::Vector6 minVector;
      // Get Part info...
      const Particle* part = *pIt;

      // Get the 6x6 covariance matrix (pos, momentum)
      Gaudi::SymMatrix6x6 cov = part->covMatrix().Sub<Gaudi::SymMatrix6x6>(0,0);
      bool ok = cov.Sinvert();
      
      if( !ok ) {
        _info << "    Covariance matrix inversion failed" << endreq;
      } else {
        Gaudi::Vector6 pVector( part->referencePoint().x(), 
                                part->referencePoint().y(), 
                                part->referencePoint().z(),
                                part->momentum().Px(), 
                                part->momentum().Py(), 
                                part->momentum().Pz());
        
        double axz =  part->referencePoint().z();
        double pCharge = part->charge();
        // Loop on MCParticles
        for( MCParticles::const_iterator mcIt=mcParts->begin();
             mcParts->end() != mcIt; mcIt++) {
          // Compute the chi2 between these two objects
          const MCParticle* mcPart = *mcIt;
          // Protect against missing information
          if( NULL == mcPart || NULL == mcPart->originVertex() ) continue;
          
          // Associate only charge2charged and neutral2neutral
          if( 0. == pCharge && 
              0. != mcPart->particleID().threeCharge() ) continue;
          if( 0. != pCharge && 
              0. == mcPart->particleID().threeCharge() ) continue;
          
          Gaudi::Vector6 mcpVector(6);
          get6Vector( mcPart, axz, mcpVector);
          
          // Avoid long computations if momentum is too different
          if( fabs(mcpVector[5]-pVector[5]) > 100. ) continue;
          
          const Gaudi::Matrix6x6 cov1(cov);
          double chi2 = ROOT::Math::Similarity( pVector - mcpVector, cov1);

          if( m_histos && chi2 > 0. ) {
            m_hisChi2vsDiffP->fill( fabs(mcpVector[5]-pVector[5])/pVector[5], 
                                    log10(chi2) );
          }
          if( chi2 > 0 && (minChi2 == 0 || chi2 < minChi2) ) {
            minChi2 = chi2;
            minMCPart = mcPart;
            minVector = mcpVector;
          }
        }

        if( minChi2 > 0 ) {
          // Establish the relation with the minChi2 as a "weight"
          if( NULL != linkerTable ) 
            linkerTable->link( part, minMCPart, minChi2);
          nass++;
        }
        if( m_histos ) {
          m_hisMinChi2vsDiffP->fill( fabs(minVector[5]-pVector[5])/pVector[5], 
                                     log10(minChi2) );
          m_hisChi2->fill( minChi2 );
        }
      }
    }
    _debug << npp << " Particles in " << *inp << ", "
           << nass << " are associated, " << endreq;
  } // End of loop on input data locations
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Particle2MCWithChi2::finalize() {

  _debug << "==> Finalize" << endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================

void Particle2MCWithChi2::get6Vector( const MCParticle* mcPart, const double axz, 
                                      Gaudi::Vector6 & mcpVector)
{
  // Make the vector from the MCparticle
  mcpVector[3] = mcPart->momentum().Px();
  mcpVector[4] = mcPart->momentum().Py();
  mcpVector[5] = mcPart->momentum().Pz();
  
  // Now extrapolate the origin to the place where Part is given        
  mcpVector[2] = axz;

  double dz = axz;
  
  dz -= mcPart->originVertex()->position().z();
  mcpVector[0] = mcPart->originVertex()->position().x() +
    dz * mcpVector[3]/mcpVector[5];
  mcpVector[1] = mcPart->originVertex()->position().y() +
    dz * mcpVector[4]/mcpVector[5];
}
