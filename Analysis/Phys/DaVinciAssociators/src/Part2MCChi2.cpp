// $Id: Part2MCChi2.cpp,v 1.1 2002-05-10 15:08:21 phicharp Exp $
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
#include "Part2MCChi2.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Part2MCChi2
//
// 11/04/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<Part2MCChi2>          s_factory ;
const        IAlgFactory& Part2MCChi2Factory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Part2MCChi2::Part2MCChi2( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ) 
  , m_inputData( ParticleLocation::Production )
  , m_outputData( Part2MCPartAsctLocation )
  , m_chi2( 100. )
  , m_histos( false )

{
  declareProperty( "InputData", m_inputData );
  declareProperty( "OutputData", m_outputData );
  declareProperty( "Chi2Cut", m_chi2 );
  declareProperty( "FillHistos", m_histos );
}

//=============================================================================
// Destructor
//=============================================================================
Part2MCChi2::~Part2MCChi2() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Part2MCChi2::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
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
StatusCode Part2MCChi2::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  // get Particles and MCParticles
  SmartDataPtr<Particles> parts (eventSvc(), m_inputData);
  if( 0 != parts ) {
    log << MSG::DEBUG << "    Particles retrieved" << endreq;
  }
  else {
    log << MSG::FATAL << "    *** Could not retrieve Particles" << endreq;
  }
  
  SmartDataPtr<MCParticles> 
    mcParts( eventSvc(), MCParticleLocation::Default );
  if( 0 != mcParts ) {
    log << MSG::DEBUG << "    MCPart retrieved" << endreq;
  }
  else {
    log << MSG::FATAL << "    *** Could not retrieve MCPart" << endreq;
  }

  // create an association table and register it in the TES
  Part2MCPartAsct::Table* table = new Part2MCPartAsct::Table();
  StatusCode sc = eventSvc()->registerObject( outputData(), table);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "     *** Could not register " << outputData()
        << endreq;
    delete table;
    return sc;
  }

  // loop on Parts and MCParts to match them
  int nax = 0;
  for( Particles::const_iterator pIt=parts->begin();
       parts->end() != pIt; pIt++) {
    // Info on the smallest chi2
    double minChi2=0;
    const MCParticle* minMCPart=0;
    HepVector minVector(6);
    // Get Part info...
    const Particle* part = *pIt;
    
    log << MSG::DEBUG << "    Part # " << ++nax << endreq;
    HepMatrix temp(6,6,0);
    HepMatrix m1(3,3,0);
    m1 = part->pointOnTrackErr();
    temp.sub(1,1,m1);
    m1 = part->slopesMomErr();
    temp.sub(4,4,m1);
    temp.sub(1,4,part->posSlopesCorr());
    temp.sub(4,1,part->posSlopesCorr().T());
    HepSymMatrix cov(6,0);
    cov.assign(temp);
    int fail;
    cov.invert( fail );
    if( fail ) {
      log << MSG::WARNING << "    Covariance matrix inversion failed" <<
        endreq;
    } else {
      HepVector pVector(6);
      get6Vector( part, pVector );
      
      double axz = pVector[2];
      log << MSG::DEBUG << "    part6Vector, p = " << pVector[5] << endreq;
  
      for( MCParticles::const_iterator mcIt=mcParts->begin();
         mcParts->end() != mcIt; mcIt++) {
        // Compute the chi2 between these two objects
        const MCParticle* mcPart = *mcIt;
        // Protect against missing information
        if( 0 == mcPart || 0 == mcPart->originVertex() ) continue;

        HepVector mcpVector(6);
        get6Vector( mcPart, mcpVector, axz);
        
        // Avoid long computations if momentum is too different
        //        if( fabs(mcpVector[5]-pVector[5]) > 10000. ) continue;

        double chi2 = Part2MCPartChi2( pVector, mcpVector, cov );

        if( chi2 > 0. && m_histos ) {
          m_hisChi2vsDiffP->fill( fabs(mcpVector[5]-pVector[5])/pVector[5], 
                                  log10(chi2) );
        }
        if( chi2 > 0 && (minChi2 == 0 || chi2 < minChi2) ) {
          minChi2 = chi2;
          minMCPart = mcPart;
          minVector = mcpVector;
        }
      }

      if( minChi2 > 0 && minChi2 < m_chi2 ) {
        table->relate( part, minMCPart);
      }
      if( m_histos ) {
        m_hisMinChi2vsDiffP->fill( fabs(minVector[5]-pVector[5])/pVector[5], 
                                   log10(minChi2) );
        m_hisChi2->fill( minChi2 );
      }
    }
  }
  
  log << MSG::DEBUG
      << parts->end() - parts->begin() << " Parts associated with "
      << mcParts->end() - mcParts->begin() << " MCParts" << endreq;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Part2MCChi2::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================


//=============================================================================
// Compute the chi2 between an part and an MCPart
//=============================================================================
double Part2MCChi2::Part2MCPartChi2(const HepVector& pVector, 
                                           const HepVector& mcpVector,
                                           const HepSymMatrix& cov)
{
  HepVector diff(6);

  diff = pVector - mcpVector;  
  HepVector v(6);
  v = cov * diff;
  return dot( v, diff );
}

void Part2MCChi2::get6Vector( const Particle* part, 
                                     HepVector& pVector)
{
  // Now copy the vector from the Part
  pVector[0] = part->pointOnTrack().x();
  pVector[1] = part->pointOnTrack().y();
  pVector[2] = part->pointOnTrack().z();
  pVector[3] = part->slopeX();
  pVector[4] = part->slopeY();
  pVector[5] = part->p();
  
}

void Part2MCChi2::get6Vector( const MCParticle* mcPart, 
                                     HepVector& mcpVector, const double axz)
{
  // Make the vector from the MCparticle
  double mcpzmom = mcPart->momentum().pz();
  if( 0. != mcpzmom ) {
    mcpVector[3] = mcPart->momentum().px() / mcpzmom;
    mcpVector[4] = mcPart->momentum().py() / mcpzmom;
  }
  mcpVector[5] = mcPart->momentum().vect().mag();
  
  // Now extrapolate the origin to the place where Part is given        
  mcpVector[2] = axz;

  double dz = axz;
  
  dz -= mcPart->originVertex()->position().z();
  mcpVector[0] = mcPart->originVertex()->position().x() +
    dz * mcpVector[3];
  mcpVector[1] = mcPart->originVertex()->position().y() +
    dz * mcpVector[4];
}
