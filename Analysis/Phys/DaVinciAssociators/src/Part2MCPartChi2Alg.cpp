// $Id: Part2MCPartChi2Alg.cpp,v 1.1.1.1 2002-04-25 15:57:35 phicharp Exp $
// Include files 
#include <math.h>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"


// histograms
#include  "GaudiKernel/IHistogramSvc.h"

// local
#include "Part2MCPartChi2Alg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Part2MCPartChi2Alg
//
// 11/04/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<Part2MCPartChi2Alg>          s_factory ;
const        IAlgFactory& Part2MCPartChi2AlgFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Part2MCPartChi2Alg::Part2MCPartChi2Alg( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ) 
  , m_outputData( "/Phys/Part2MC")
  , m_chi2( 100. )
{

  declareProperty( "OutputData", m_outputData );
  declareProperty( "Chi2Cut", m_chi2 );
  

}

//=============================================================================
// Destructor
//=============================================================================
Part2MCPartChi2Alg::~Part2MCPartChi2Alg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Part2MCPartChi2Alg::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  // Book histograms...
  m_hisChi2 = histoSvc()->book( "asct", 1, "Chi2", 100, 0., 1000. );
  m_hisChi2vsDiffP = histoSvc()->book( "asct", 2, "log(Chi2) vs dP/P",
                                       100, 0., 1., 35, 0., 7);
  m_hisMinChi2vsDiffP = histoSvc()->book( "asct", 3, "log(Chi2) vs dP/P",
                                       100, 0., 1., 35, 0., 7);

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode Part2MCPartChi2Alg::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  // get Particles and MCParticles
  SmartDataPtr<AxPartCandidateVector> 
    axParts (eventSvc(), "Anal/AxPartCandidates");
  if( 0 != axParts ) log << MSG::DEBUG << "    AxPart retrieved" << endreq;
  else log << MSG::FATAL << "    *** Could not retrieve AxPart" << endreq;
  SmartDataPtr<MCParticleVector> 
    mcParts( eventSvc(), "MC/MCParticles");
  if( 0 != mcParts ) log << MSG::DEBUG << "    MCPart retrieved" << endreq;
  else log << MSG::FATAL << "    *** Could not retrieve MCPart" << endreq;

  // create an association table and register it in the TES
  Part2MCPartAsct::Table* table = new Part2MCPartAsct::Table();
  StatusCode sc = eventSvc()->registerObject( outputData(), table);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "     *** Could not register " << outputData()
        << endreq;
    delete table;
    return sc;
  }

  // loop on AxParts and MCParts to match them
  int nax = 0;
  for( AxPartCandidateVector::const_iterator axIt=axParts->begin();
       axParts->end() != axIt; axIt++) {
    // Info on the smallest chi2
    double minChi2=0;
    const MCParticle* minMCPart=0;
    HepVector minVector(6);
    // Get AxPart info...
    const AxPartCandidate* axPart = *axIt;
    
    log << MSG::DEBUG << "    AxPart # " << ++nax << endreq;
    HepSymMatrix cov = axPart->fitCovariance();
    int fail;
    cov.invert( fail );
    if( fail ) {
      log << MSG::WARNING << "    Covariance matrix inversion failed" <<
        endreq;
    } else {
      HepVector pVector(6);
      get6Vector( axPart, pVector );
      
      double axz = pVector[2];
      log << MSG::DEBUG << "    AxPart6Vector, p = " << pVector[5] << endreq;
  
      for( MCParticleVector::const_iterator mcIt=mcParts->begin();
         mcParts->end() != mcIt; mcIt++) {
        // Compute the chi2 between these two objects
        const MCParticle* mcPart = *mcIt;
        // Protect against missing information
        if( 0 == mcPart || 0 == mcPart->originMCVertex() ) continue;

        HepVector mcpVector(6);
        get6Vector( mcPart, mcpVector, axz);
        
        // Avoid long computations if momentum is too different
        //        if( fabs(mcpVector[5]-pVector[5]) > 10000. ) continue;

        double chi2 = Part2MCPartChi2( pVector, mcpVector, cov );

        if( chi2 > 0. )
          m_hisChi2vsDiffP->fill( fabs(mcpVector[5]-pVector[5])/pVector[5], 
                                  log10(chi2) );
        if( chi2 > 0 && (minChi2 == 0 || chi2 < minChi2) ) {
          minChi2 = chi2;
          minMCPart = mcPart;
          minVector = mcpVector;
        }
      }

      if( minChi2 > 0 && minChi2 < m_chi2 ) {
        table->relate( axPart, minMCPart);
      }
      m_hisMinChi2vsDiffP->fill( fabs(minVector[5]-pVector[5])/pVector[5], 
                              log10(minChi2) );
      m_hisChi2->fill( minChi2 );
    }
  }
  
  log << MSG::DEBUG <<  
    axParts->end() - axParts->begin() << " AxParts associated with " <<
    mcParts->end() - mcParts->begin() << " MCParts" << endreq;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Part2MCPartChi2Alg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================


//=============================================================================
// Compute the chi2 between an AxPart and an MCPart
//=============================================================================
double Part2MCPartChi2Alg::Part2MCPartChi2(const HepVector& pVector, 
                                           const HepVector& mcpVector,
                                           const HepSymMatrix& cov)
{
  HepVector diff(6);

  diff = pVector - mcpVector;  
  HepVector v(6);
  v = cov * diff;
  return dot( v, diff );
}

void Part2MCPartChi2Alg::get6Vector( const AxPartCandidate* axPart, 
                                     HepVector& pVector)
{
  // Now copy the vector from the Part
  pVector[0] = axPart->firstPoint().x();
  pVector[1] = axPart->firstPoint().y();
  pVector[2] = axPart->firstPoint().z();
  pVector[3] = axPart->fitSlopeX();
  pVector[4] = axPart->fitSlopeY();
  pVector[5] = axPart->fitMomentum();
  
}

void Part2MCPartChi2Alg::get6Vector( const MCParticle* mcPart, 
                                     HepVector& mcpVector, const double axz)
{
  // Make the vector from the MCparticle
  double mcpzmom = mcPart->fourMomentum().pz();
  if( 0. != mcpzmom ) {
    mcpVector[3] = mcPart->fourMomentum().px() / mcpzmom;
    mcpVector[4] = mcPart->fourMomentum().py() / mcpzmom;
  }
  mcpVector[5] = mcPart->fourMomentum().vect().mag();
  
  // Now extrapolate the origin to the place where Part is given        
  mcpVector[2] = axz;

  double dz = axz;
  
  dz -= mcPart->originMCVertex()->position().z();
  mcpVector[0] = mcPart->originMCVertex()->position().x() +
    dz * mcpVector[3];
  mcpVector[1] = mcPart->originMCVertex()->position().y() +
    dz * mcpVector[4];
}
