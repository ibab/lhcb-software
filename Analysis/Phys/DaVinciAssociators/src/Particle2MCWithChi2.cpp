// $Id: Particle2MCWithChi2.cpp,v 1.18 2009-03-02 15:26:34 jpalac Exp $
// Include files 
#include <math.h>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// histograms
#include  "GaudiKernel/IHistogramSvc.h"

// properties
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IAlgorithm.h"

// event
#include "Event/MCParticle.h"
#include "Event/Particle.h"

// local
#include "Particle2MCWithChi2.h"

using namespace LHCb;

// Uncomment for activating covariance matrices printout
//#define DEBUG_COV

//-----------------------------------------------------------------------------
// Implementation file for class : Particle2MCWithChi2
//
// 11/04/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( Particle2MCWithChi2 )

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
  , m_chi2SpeedUpCut(-1.)
{
  declareProperty( "FillHistos", m_histos );
}

//=============================================================================
// Destructor
//=============================================================================
Particle2MCWithChi2::~Particle2MCWithChi2() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Particle2MCWithChi2::initialize() {

  _debug << "==> Initialise" << endmsg;
  // Book histograms...
  if( m_histos ) {    
    m_hisChi2 = histoSvc()->book( "asct", 1, "Chi2", 100, 0., 1000. );
    m_hisChi2vsDiffP = histoSvc()->book( "asct", 2, "log(Chi2) vs dP/P",
                                         100, 0., 1., 35, 0., 7);
    m_hisMinChi2vsDiffP = histoSvc()->book( "asct", 3, "log(Chi2) vs dP/P",
                                            100, 0., 1., 35, 0., 7);
  }

  // check if parent is Particle2MCChi2 and steal chi2 property
  _debug << "Looking for parents of " << name() << endmsg ;
  int pos = name().find_last_of(".");
  std::string pname = name().substr(0,pos);  
  _verbose << "Name is " << name() << " last . is at " << pos 
           << " -> parent is " << pname << endmsg ;

  IAlgManager* algMgr = svc<IAlgManager>("ApplicationMgr");
  IAlgorithm* algo;
  if (NULL==algMgr) {
    warning() << "No Algorithm manager found" << endmsg ;
    return StatusCode::SUCCESS; // ignore
  }
  StatusCode sc = algMgr->getAlgorithm( pname, algo );
  if ( NULL!=algo ){
    SmartIF<IProperty> prop(algo);
    if ( !prop ){
      DoubleProperty value;
      value.assign(prop->getProperty( "Chi2Cut" ));
      m_chi2SpeedUpCut = value ;
      _info << "Will be using Chi2Cut of " << pname << " = " << m_chi2SpeedUpCut << endmsg ;
    } else {
      _info << "Algorithm " << algo << " has no property Chi2Cut" << endmsg ;
    }
    } else _info << "No algorithm " << algo << " found " << endmsg ;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2MCWithChi2::execute() {

  _debug << "==> Execute" << endmsg;

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
    Object2MCLinker<> p2MCLink(this);
    Object2MCLinker<>::Linker*
      linkerTable = p2MCLink.linkerTable( linkContainer );
    if( NULL != linkerTable ) {
      _verbose << "    Created Linker table for container "
                          << linkContainer << endmsg;
    } else {
      _verbose << "    Linker table for container "
                          << linkContainer << " already exists"
                          << endmsg;
    }
    if( NULL == linkerTable ) continue;
    
    // Get Particles
    SmartDataPtr<Particles> parts (eventSvc(), *inp);
    if( 0 == parts ) continue;
    int npp = parts->size();
    int nass = 0;
    _verbose << "    " << npp 
                        << " Particles retrieved from " << *inp << endmsg;
    
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
      // Set the sigma(z) to non-zero
      cov(2,2) = 1.;
#ifdef DEBUG_COV
      _verbose << "Particle :\n" << *part << "\n" 
               << cov << endmsg;
      if( 0 != part->proto() ) {
        _verbose << "Covariance matrix of the track " << "\n"
                 << part->proto()->track()->firstState().posMomCovariance() << endmsg;
        _verbose << "Here is the state " << "\n"
                 << part->proto()->track()->firstState() << endmsg;
      }
#endif
      
      double cov55 = cov(5,5);
      bool ok = cov.Invert();
      
      if( !ok ) {
        _info << "    Covariance matrix inversion failed" << endmsg;
        _verbose << "Covariance matrix:\n" << cov << endmsg;
      } else {
#ifdef DEBUG_COV
        _verbose << "Inverse matrix for Particle " << part->key() << "\n" 
                 << cov << endmsg;
#endif
        Gaudi::Vector6 pVector( part->referencePoint().x(), 
                                part->referencePoint().y(), 
                                part->referencePoint().z(),
                                part->momentum().Px(), 
                                part->momentum().Py(), 
                                part->momentum().Pz());
        
        double axz =  part->referencePoint().z();
        double pCharge = part->charge();
        _verbose << "Particle " << part->key() << " " << part->momentum() << endmsg;
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
          
          Gaudi::Vector6 mcpVector;
          get6Vector( mcPart, axz, mcpVector);
          
          // Avoid long computations if momentum is too much different
          if( m_chi2SpeedUpCut<=0 && fabs(mcpVector[5]-pVector[5]) > 1000. && 
              fabs(mcpVector[5]-pVector[5])>0.1*mcpVector[5] ) continue;
          if( m_chi2SpeedUpCut>0 && 
              (mcpVector[5]-pVector[5])*(mcpVector[5]-pVector[5])/cov55 > m_chi2SpeedUpCut  )
            continue ;
          
          double chi2 = ROOT::Math::Similarity( pVector - mcpVector, cov);

          _verbose << "     & MCPart " << mcPart->key() << " " << mcPart->momentum() 
                   << "-> Chi2 = " << chi2  << endmsg;

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
          _verbose << "Particle " << part->key() << " associated with MCPart " << minMCPart->key()
                   << "- Chi2 = " << minChi2 << endmsg;
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
           << nass << " are associated, " << endmsg;
  } // End of loop on input data locations
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Particle2MCWithChi2::finalize() {

  _debug << "==> Finalize" << endmsg;
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

  double dz = axz - mcPart->originVertex()->position().z();
  
  mcpVector[0] = mcPart->originVertex()->position().x() +
    dz * mcpVector[3]/mcpVector[5];
  mcpVector[1] = mcPart->originVertex()->position().y() +
    dz * mcpVector[4]/mcpVector[5];
}
