// $Id: SelectB0JPsiKs.cpp,v 1.1 2002-03-27 20:54:06 gcorti Exp $
// Include files 

// from STL
#include <math.h>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiTools/IAssociator.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"

// from Event 
#include "LHCbEvent/Event.h"
#include "LHCbEvent/AxPartCandidate.h"
#include "LHCbEvent/MCParticle.h"
#include "LHCbEvent/RefTable.h"
#include "Event/Vertex.h"
#include "Event/Particle.h"
#include "Event/PhysSel.h"
#include "Event/L0DUReport.h"

// from DaVinci
#include "DaVinciSicb/IPhysSelTool.h"
#include "DaVinciSicb/SelComb.h"
#include "DaVinciSicb/FlavourTag.h"

// Tools
#include "DaVinciTools/IPhysDesktop.h"
#include "DaVinciTools/IMassVertexFitter.h"
#include "DaVinciTools/IVertexFitter.h"
#include "DaVinciTools/IGeomDispCalculator.h"
#include "DaVinciTools/IParticleFilter.h"
#include "DaVinciTools/IParticleStuffer.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/LorentzVector.h"

// local
#include "SelectB0JPsiKs.h"


//-----------------------------------------------------------------------------
// Implementation file for class : SelectB0JPsiKs
//
// 18/03/2002 : Claudia Pereira Nunes
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<SelectB0JPsiKs>          s_factory ;
const        IAlgFactory& SelectB0JPsiKsFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SelectB0JPsiKs::SelectB0JPsiKs( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ),
    m_pIDSearch(0),
    m_daugID(0),
    m_nEvents(0),
    m_B0Count(0),
    m_pAsct(0),
    m_pSelTool(0),
    m_ppSvc(0),
    m_pDesktop(0),
    m_pLagFit(0),
    m_pVertexFit(0),
    m_pGeomDispCalc(0),
    m_pFilter(0),
    m_pStuffer(0) {
  m_daugName.clear();
  declareProperty( "SearchParticle", m_pNameSearch = "B0" );
  declareProperty( "DecayProducts", m_daugName );
  declareProperty( "HistogramFlag", m_produceHistogram = false );
  declareProperty( "B0MassWindowLoose", m_b0masswinloo = 0.2 * GeV);
  declareProperty( "B0ZWindow", m_b0zwin = 50.0 * cm);
  declareProperty( "Chi2UncFitCut", m_chi2UncVtxCut = 5.0);  
  declareProperty( "B0MassWindowTight", m_b0masswinti = 0.021 * GeV); 
  declareProperty( "ImpParamB0" , m_impParamB0Cut = 0.06 * cm );
  declareProperty( "ImpParamKsJPsi" , m_impParamKsJPsiCut = 0.1 * cm );
  
}

//=============================================================================
// Destructor
//=============================================================================
SelectB0JPsiKs::~SelectB0JPsiKs() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode SelectB0JPsiKs::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  // Retrieve the tools used by this algorithm
  // Note that the ToolSvc is now available via the toolSvc() method
  // you can hardcode the type of a tool or set  in the job options  
  StatusCode sc = toolSvc()->retrieveTool("AxPart2MCParticleAsct", m_pAsct);

  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to retrieve Associator tool" << endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool("PhysSelTool", m_pSelTool);
  
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to retrieve PhysSel helper tool" <<endreq;
    return sc;
  }
  sc = toolSvc()->retrieveTool("UnconstVertexFitter", m_pVertexFit);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to retrieve Vertex  tool" << endreq;
    return sc;
  }


  sc = toolSvc()->retrieveTool("PhysDesktop", m_pDesktop, this);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to retrieve Desktop  tool" << endreq;
    return sc;
  }
  
  sc = toolSvc()->retrieveTool("LagrangeMassVertexFitter", m_pLagFit);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Unable to retrieve LagrangeMassVertexFitter tool"
        << endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool("GeomDispCalculator", m_pGeomDispCalc);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to retrieve Geometry tool" << endreq;
    return sc;
  }


  sc = toolSvc()->retrieveTool("ParticleFilter", m_pFilter, this);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to retrieve Filter tool" << endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool("ParticleStuffer", m_pStuffer);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to retrieve Stuffer tool" << endreq;
    return sc;
  }


  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  log << MSG::INFO << "    Looking for Particle Property Service." << endreq;

  sc = service("ParticlePropertySvc", m_ppSvc);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to locate Particle Property Service" 
        << endreq;
    return sc;
  }
  
  // Retrieve Geant3 code for particle to search for: this is what is
  // stored in MCParticle
  ParticleProperty* partProp = m_ppSvc->find( m_pNameSearch );
  m_pIDSearch = partProp->geantID();
    
  std::vector<std::string>::const_iterator idau;
  for ( idau = m_daugName.begin(); idau != m_daugName.end(); idau++ ) {
    partProp = m_ppSvc->find( *idau );
    if ( 0 == partProp )   {
      log << MSG::ERROR << "Cannot retrieve properties for particle \"" 
          << *idau << "\" " << endreq;
      return StatusCode::FAILURE;
    }
    m_daugID.push_back( partProp->geantID() );
  }
  

  partProp = m_ppSvc->find( "J/psi(1S)" );
  m_JPsiID = partProp->jetsetID();

  partProp = m_ppSvc->find( "KS0" );
  m_KsID = (*partProp).jetsetID();


  partProp = m_ppSvc->find( "B0" );
  m_B0Mass = (*partProp).mass();
  m_B0ID = (*partProp).jetsetID();

  
  // If histograms are required inintialize them
  if( m_produceHistogram )   {
    log << MSG::INFO << "    histograms will be produced" << endreq;

    // Create the histograms 

    m_hImpParamKsJPsi = histoSvc()->
      book("/stat/simple/40", " impact parameter of Ks wrt JPsi vertex ", 
      100,0.0,0.5);
    if( 0 == m_hImpParamKsJPsi ) {
      log << MSG::ERROR << "    Cannot register histogram 40" << endreq;
      return StatusCode::FAILURE;
    }

    m_hChi2UncFit = histoSvc()->
      book("/stat/simple/41", "Chi2 of unconstrained B0 vertex fit", 
      100,0.0,20.0);
    if( 0 == m_hChi2UncFit ) {
      log << MSG::ERROR << "    Cannot register histogram 41" << endreq;
      return StatusCode::FAILURE;
    }
    
    m_hImpParamB0 = histoSvc()->
      book("/stat/simple/42", " impact parameter of B0 ", 
      100,0.0,0.1);
    if( 0 == m_hImpParamB0 ) {
      log << MSG::ERROR << "    Cannot register histogram 42" << endreq;
      return StatusCode::FAILURE;
    }
    
    m_hB0Mass = histoSvc()->
      book("/stat/simple/43", " B0 Mass", 
      100,5.1,5.5);
    if( 0 == m_hB0Mass) {
      log << MSG::ERROR << "    Cannot register histogram 43" << endreq;
      return StatusCode::FAILURE;
    }


       
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode SelectB0JPsiKs::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  m_SelB0 = false;
  
  
  // Counter of events processed
  log << MSG::INFO << ">>> Execute" << endreq;
  log << MSG::INFO << "    processing event number " << ++m_nEvents << endreq;
  
  // Retrieve informations about event
  SmartDataPtr<Event> evt(eventSvc(), "/Event" );
  
  if ( evt ) {   
    log << MSG::INFO << "    retrieved EVENT: " << evt->event()
        << " RUN: " << evt->run() << endreq;
  }
  else {
    log << MSG::ERROR << "    not able to retrieve event" << endreq;
    return StatusCode::FAILURE;
  }
  
  // Fill the PhysDeskTop particle and vertex vectors. 
  // You have to give in the job options the name and confidence levels 
  // for the particles you want.
  StatusCode scDesktop = m_pDesktop->getInput();
  if (!scDesktop) {
    log << MSG::ERROR << "    not able to fill PhysDesktop " << endreq;
    return StatusCode::FAILURE;
  }
  
  const VertexVector& verts = m_pDesktop->vertices();
  const ParticleVector& parts = m_pDesktop->particles();
  
  log << MSG::DEBUG << "    Vertex Vector size " << verts.size()<< endreq;   
  
  log << MSG::DEBUG << "    Particle Vector size " <<parts.size()<< endreq;  


  // Start of Paul's ParticleFilter code (and debug)
  ParticleVector vJPsiKs, vJPsi, vKs;
  StatusCode scFilter = m_pFilter->filter( parts, vJPsiKs );

  if ( scFilter) {
    log << MSG::DEBUG << ">>> Particle filtration successfull  "  << endreq;
  }
  else {
    log << MSG::ERROR << ">>> Particle filtration failed " << endreq;
    return StatusCode::FAILURE;
  }

  ParticleVector::const_iterator icand = 0;
  for( icand = parts.begin(); icand != parts.end(); icand++ ) {
    if ( (*icand)->particleID().id() == m_JPsiID) {
      vJPsi.push_back(*icand);
    }
    if ( (*icand)->particleID().id() == m_KsID) {
      vKs.push_back(*icand);
    }
    
  }//for( icand = parts.begin()

  log << MSG::DEBUG << ">>> ParticleFilter DEBUG " << endreq;
  ParticleVector::const_iterator im;
  log << MSG::DEBUG << ">>> Number of JPsis plus Ks found " << vJPsiKs.size() 
      << endreq;
  log << MSG::DEBUG << ">>> " << endreq;    
  for( im = vJPsiKs.begin(); im != vJPsiKs.end(); im++ ) {
    log << MSG::DEBUG << ">>>   momentum         " 
        << (*im)->momentum().vect().mag() 
        << endreq;
    log << MSG::DEBUG << ">>>   trans momentum   "
        << (*im)->momentum().vect().perp() 
        << endreq;
    log << MSG::DEBUG << ">>>   ParticleID       " << (*im)->particleID().id() 
        << endreq;
    log << MSG::DEBUG << ">>>   Confidence Level " << (*im)->confLevel() 
        << endreq;
    log << MSG::DEBUG << ">>> " << endreq;    
  }  
  log << MSG::DEBUG << ">>> Number of JPsi found " << vJPsi.size() 
      << endreq; 
  log << MSG::DEBUG << ">>> Number of Ks found " << vKs.size() 
      << endreq;
  log << MSG::DEBUG << ">>> " << endreq;    
  // End of Paul's ParticleFilter code (and debug)



  


  // All JPsi/Ks combinations
  int b0CountPerEvent = 0;  
  //this vector (vJPsiKsComb) is supposed to be used as a stack to pass the 
  // JPsi/Ks combinations to be tried out
  ParticleVector vJPsiKsComb;
  
  ParticleVector::iterator iJPsi;
  ParticleVector::iterator iKs;
  
  for( iKs = vKs.begin(); iKs != vKs.end(); iKs++ ) {

    vJPsiKsComb.push_back(*iKs); 

    for (iJPsi = vJPsi.begin(); iJPsi != vJPsi.end(); iJPsi++) {
      

      vJPsiKsComb.push_back(*iJPsi);
      

      // Calculate the impact parameter for the Ks track wrt JPsi vertex
      
      double ip=0.;
      double ipErr=0;
      double dist=0.;
      double distErr=0;
      Vertex* JPsiVertex  = (*iJPsi)->endVertex();
      m_pGeomDispCalc->calcImpactPar(*(*iKs), (*JPsiVertex),ip,ipErr);
      
      // Check that Ks passes close to JPsi vertex
      if( m_produceHistogram ) { 
        m_hImpParamKsJPsi->fill(ip/cm , 1. );
      }      
      if( ip < m_impParamKsJPsiCut ){
        
        // Find invariant mass
        HepLorentzVector JPsiKsComb(0.0, 0.0, 0.0, 0.0);
        JPsiKsComb = (*iKs)->momentum() + (*iJPsi)->momentum();
       

        
        double mass = JPsiKsComb.m();
        
        //Remember SI is MeV/mm/nsec. I want to have histogram in GeV/cm to
        //compare  with SICbDST
        
        
        // Check that  their invariant mass is close to the Bd mass      

        if (fabs(mass - m_B0Mass) < m_b0masswinloo ){
       
          //Perform Unconstrained vertex fit 
          Vertex jPsiKsVertex;
          StatusCode scjPsiKsVertex =m_pVertexFit->fitVertex(vJPsiKsComb,
                                                             jPsiKsVertex);
          if( scjPsiKsVertex.isSuccess() ) { 
            HepPoint3D vertexpos =  jPsiKsVertex.position();       
            
            
            
            // Check that the z of the vertex well
            if ( fabs( vertexpos.z()) < m_b0zwin ) {
              
              
              // Cut on chi2
              if( m_produceHistogram ) { 
                m_hChi2UncFit->fill(jPsiKsVertex.chi2() , 1. );
              }
              if( jPsiKsVertex.chi2() < m_chi2UncVtxCut ) {
              

                // Create Particle B0 from Vertex 
                Particle candB0;       
                ParticleID B0PID( m_B0ID );
                StatusCode scStuff = m_pStuffer->fillParticle( jPsiKsVertex,
                                                               candB0, 
                                                               B0PID );
                if ( !scStuff ) {
                  log << MSG::ERROR << ">>> Particle stuffing failed" 
                      << endreq;
                  return StatusCode::FAILURE;
                }
                
                // Calculate impact parameter for B0 wrt primary vertex

                double ipB0=0.;
                double ipErr=0;
                  
                int count = 0;
                VertexVector::const_iterator ivert = 0;

                for( ivert = verts.begin(); ivert != verts.end(); 
                     ivert++ ) {
                  if((*ivert)->type() == Vertex::Primary ){
                    count++;
                    
                    if (count == 1) {
                       m_pGeomDispCalc->calcImpactPar(candB0,
                                                     *(*ivert),ipB0,ipErr);
                    }
                    
                  }
                }
                  
                if( m_produceHistogram ) { 
                  m_hImpParamB0->fill(ipB0/cm , 1. );
                }
                
                if ( ipB0 < m_impParamB0Cut ) {
                 
                
                  //  Cut on B0 mass

                  if( m_produceHistogram ) {
                    m_hB0Mass->fill(candB0.mass()/GeV, 1. );
                  }
                  if (fabs(candB0.mass()-m_B0Mass) < m_b0masswinti) {
                    
                    //Perform Constrained vertex fit

                    Vertex b0Vtx;
                    Particle b0; 

                    StatusCode scLagFit =m_pLagFit->lagrangeFitter("B0", 
                                                                   vJPsiKsComb,
                                                                   b0Vtx, b0);

                    if( scLagFit.isSuccess() ) {



                      log << MSG::DEBUG << " Passed all cuts   " << endreq;
                  
                      m_SelB0 = true;
                      
                      m_B0Count++;
                      b0CountPerEvent++;
                      
                      Particle* pInDesktop= m_pDesktop->createParticle(&b0);
                  
                      if( pInDesktop) {
                        log << MSG::DEBUG << "B0 added to PhysDesktop "
                            << "Pid and momentum = " 
                            << pInDesktop->particleID().id() << ", "
                            << pInDesktop->momentum().px() << ", "
                            << pInDesktop->momentum().py() << ", "
                            << pInDesktop->momentum().pz() << endreq;
                        log << MSG::DEBUG << "Its decay vertex position = " 
                            <<(pInDesktop->endVertex())->position().x() << ", "
                            <<(pInDesktop->endVertex())->position().y() << ", "
                            <<(pInDesktop->endVertex())->position().z()
                            << endreq;
                      }
                      else {
                        log << MSG::DEBUG << "not able to save B0 in desktop"
                            << endreq;  
                      }//pInDesktop
                    }// scLagfit
                    
                  } //B0MassCut
                } //B0 impact parameter
              } //Chi2Cut
            } //Z of the vertex
            
          } // scjpsiKsvertex
          // } //is success          
        } // Invariant mass close to Bd mass
      } // Ks close to JPsi
      
      // Remove this JPsi from "stack"
      vJPsiKsComb.pop_back();
      
    }//for(iJPsi = vJPsi.begin() ...
    
    
    vJPsiKsComb.pop_back(); 
    
    
    
  }//for( iKs = vKs.begin()...
      
  // Log number of Combinations found by previous step
  log << MSG::INFO << "  Number of JPsi/Ks combinations  = " 
      << b0CountPerEvent << endreq;

  log << MSG::INFO << "  Number of particles in desktop = " << parts.size() 
      << endreq;

  log << MSG::INFO << "  Number of vertices in desktop = " << verts.size()
      << endreq;
      
  // Gloria Now save desktop to TES in specified location in jobOptions
  // Notice that this delete particles from desktop at the moment
  scDesktop = m_pDesktop->saveDesktop();
  if (scDesktop) {
    log << MSG::INFO << " PhysDeskTop Saved to TES"<<endreq;
  }
  else {
    log << MSG::ERROR << "not able to save desktop in TE"
        << endreq;
    return StatusCode::FAILURE;
  }
  

  // End of execution for each event

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode SelectB0JPsiKs::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

 // Print out counters
  log << MSG::INFO << "    Number of events processed      = " 
      << m_nEvents << endreq;
  log << MSG::INFO << "    Number of selected     B0       = " 
      << m_B0Count << endreq;

  // End of finalization step



  return StatusCode::SUCCESS;
}
 

//=============================================================================
