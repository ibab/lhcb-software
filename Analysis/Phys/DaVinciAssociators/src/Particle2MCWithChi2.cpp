// $Id: Particle2MCWithChi2.cpp,v 1.7 2004-06-11 15:26:17 phicharp Exp $
// Include files 
#include <math.h>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// histograms
#include  "GaudiKernel/IHistogramSvc.h"

// event
#include "Event/MCParticle.h"
#include "Event/Particle.h"

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

#define ifMsg(sev) msg << sev; if( msg.level() <= (sev) ) msg

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

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;
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

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;

  // Create an association table and register it in the TES
  Particle2MCWithChi2Asct::Table* table = 
    "" != outputTable() ? new Particle2MCWithChi2Asct::Table() : NULL;

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
    Object2MCLink p2MCLink(this);
    Object2MCLink::Linker*
      linkerTable = p2MCLink.linkerTable( linkContainer );
    if( NULL != linkerTable ) {
      ifMsg(MSG::VERBOSE) << "    Created Linker table for container "
                          << linkContainer << endreq;
    } else {
      ifMsg(MSG::VERBOSE) << "    Linker table for container "
                          << linkContainer << " already exists"
                          << endreq;
    }
    if( NULL == table && NULL == linkerTable ) continue;
    
    // Get Particles
    SmartDataPtr<Particles> parts (eventSvc(), *inp);
    if( 0 == parts ) continue;
    int npp = parts->size();
    int nass = 0;
    ifMsg(MSG::VERBOSE) << "    " << npp 
                        << " Particles retrieved from " << *inp << endreq;
    
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
        msg << MSG::INFO << "    Covariance matrix inversion failed" <<
          endreq;
      } else {
        HepVector pVector(6);
        get6Vector( part, pVector );
        
        double axz = pVector[2];
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
          
          HepVector mcpVector(6);
          get6Vector( mcPart, mcpVector, axz);
          
          // Avoid long computations if momentum is too different
          if( fabs(mcpVector[5]-pVector[5]) > 1000. ) continue;
          
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
          if( NULL != table ) 
            table->relate( part, minMCPart, minChi2);
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
    ifMsg(MSG::DEBUG)
      << npp << " Particles in " << *inp << ", "
      << nass << " are associated, " << endreq;
  } // End of loop on input data locations
  
  // Now register the table in the TES
  if( NULL != table ) put( table, outputTable() );
  ifMsg(MSG::VERBOSE) << "     Registered table " << outputTable() << endreq;
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Particle2MCWithChi2::finalize() {

  MsgStream msg(msgSvc(), name());
  ifMsg(MSG::DEBUG) << "==> Finalize" << endreq;
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
