// $Id: Particle2MCWithChi2.cpp,v 1.3 2002-07-15 15:17:17 phicharp Exp $
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
#include "Particle2MCWithChi2.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle2MCWithChi2
//
// 11/04/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<Particle2MCWithChi2>          s_factory ;
const        IAlgFactory& Particle2MCWithChi2Factory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2MCWithChi2::Particle2MCWithChi2( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_outputTable( Particle2MCWithChi2AsctLocation )
  , m_histos( false )
{
  m_inputData.push_back( ParticleLocation::Production );
  declareProperty( "InputData", m_inputData );
  declareProperty( "OutputTable", m_outputTable );
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
StatusCode Particle2MCWithChi2::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  // Create an association table and register it in the TES
  Particle2MCWithChi2Asct::Table* table = new Particle2MCWithChi2Asct::Table();

  // Get the MCParticles
  SmartDataPtr<MCParticles> 
    mcParts( eventSvc(), MCParticleLocation::Default );
  if( 0 != mcParts ) {
    log << MSG::DEBUG << "    MCParts retrieved" << endreq;
  }
  else {
    log << MSG::ERROR << "    *** Could not retrieve MCPart from " 
        << MCParticleLocation::Default << endreq;
    return StatusCode::FAILURE;
  }


  //////////////////////////////////
  // Loop on Particles containers //
  //////////////////////////////////

  for( vector<std::string>::iterator inp = m_inputData.begin(); 
       m_inputData.end()!= inp; inp++) {
    // Get Particles
    SmartDataPtr<Particles> parts (eventSvc(), *inp);
    if( 0 != parts ) {
      log << MSG::DEBUG << "    Particles retrieved from " << *inp 
          << endreq;
    }
    else {
      log << MSG::FATAL 
          << "    *** Could not retrieve Particles from " << *inp
          << endreq;
      continue;
    }
    
    // loop on Parts and then on MCParts to match them
    for( Particles::const_iterator pIt=parts->begin();
         parts->end() != pIt; pIt++) {
      // Info on the smallest chi2
      double minChi2=0;
      const MCParticle* minMCPart=0;
      HepVector minVector(6);
      // Get Part info...
      const Particle* part = *pIt;

      // Fill in the 6-matrix for the position+momentum correlation matrix
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
        // Loop on MCParticles
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
          
          double chi2 = Chi2ofParticle2MC( pVector, mcpVector, cov );

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
          table->relate( part, minMCPart, minChi2);
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
  } // End of loop on input data locations
  
  // Now register the table in the TES
  StatusCode sc = eventSvc()->registerObject( outputTable(), table);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "     *** Could not register table " << outputTable()
        << endreq;
    delete table;
    return sc;
  } else {
    log << MSG::DEBUG << "     Registered table " << outputTable() << endreq;
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Particle2MCWithChi2::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================


//=============================================================================
// Compute the chi2 between an part and an MCPart
//=============================================================================
double Particle2MCWithChi2::Chi2ofParticle2MC(const HepVector& pVector, 
                                           const HepVector& mcpVector,
                                           const HepSymMatrix& cov)
{
  HepVector diff(6);

  diff = pVector - mcpVector;  
  HepVector v(6);
  v = cov * diff;
  return dot( v, diff );
}

void Particle2MCWithChi2::get6Vector( const Particle* part, 
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

void Particle2MCWithChi2::get6Vector( const MCParticle* mcPart, 
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
