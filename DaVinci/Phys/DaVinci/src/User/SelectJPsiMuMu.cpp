// $Id: SelectJPsiMuMu.cpp,v 1.4 2002-04-03 17:24:00 gcorti Exp $
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
#include "Event/Vertex.h"
#include "Event/Particle.h"

// from DaVinci
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

// local
#include "SelectJPsiMuMu.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SelectJPsiMuMu
//
// 11/12/2001 : Sandra Amato
// 20/02/2002 : Modified to use DaVinci v3r0 (S. Amato)
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<SelectJPsiMuMu>          s_factory ;
const        IAlgFactory& SelectJPsiMuMuFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SelectJPsiMuMu::SelectJPsiMuMu( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator),
    m_nEvents(0),
    m_JPsiCount(0),
    m_pDesktop(0),
    m_pLagFit(0),
    m_pVertexFit(0),
    m_pGeomDispCalc(0),
    m_pFilter(0),
    m_pStuffer(0)     {
    
  declareProperty( "HistogramFlag", m_produceHistogram = false );
  declareProperty( "JPsiMassWindow", m_JPsiMassWin = 0.2 * GeV);
  declareProperty( "JPsiZWindow", m_JPsiZWin = 50.0 * cm);
  declareProperty( "Chi2UncFitCut", m_chi2UncVtxCut = 5.0);
  declareProperty( "ZDistanceCut", m_ZDistanceCut = 0.05 * cm);
  declareProperty( "ZVertexCut", m_ZVertexCut = 100.0 * cm);
  declareProperty( "JPsiMass", m_JPsiMassCut = 0.027 * GeV);
  declareProperty( "Chi2ConFit", m_chi2ConVtxCut = 5.0);
  
  
}

//=============================================================================
// Destructor
//=============================================================================
SelectJPsiMuMu::~SelectJPsiMuMu() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode SelectJPsiMuMu::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;




  // Retrieve the tools used by this algorithm
  // Note that the ToolSvc is now available via the toolSvc() method
  //           you can hardcode the type of a tool or set  in the job options  
  
  StatusCode sc = StatusCode::SUCCESS;
  
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

  IParticlePropertySvc* ppSvc = 0; 
  sc = service("ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to locate Particle Property Service" 
        << endreq;
    return sc;
  }
  
  // Retrieve information necessary for THIS algorithm
  // Note that these two lines can be removed using particleFilter
  ParticleProperty* partProp;
  
  partProp = ppSvc->find( "mu+" );
  m_muPlusID = partProp->jetsetID();

  partProp = ppSvc->find( "mu-" );
  m_muMinusID = (*partProp).jetsetID();

  // This is necessary
  partProp = ppSvc->find( "J/psi(1S)" );
  m_jpsiID = (*partProp).jetsetID();
  m_jpsiMass = (*partProp).mass();

  


  // If histograms are required inintialize them
  if( m_produceHistogram )   {
    log << MSG::INFO << "    histograms will be produced" << endreq;

    // Create the histograms 
    m_hChi2UncFit = histoSvc()->
      book("/stat/simple/1", "Chi2 of unconstrained J/Psi vertex fit", 
      100,0.0,20.0);
    if( 0 == m_hChi2UncFit ) {
      log << MSG::ERROR << "    Cannot register histogram 1" << endreq;
      return StatusCode::FAILURE;
    }
    
  

    m_hZDistance = histoSvc()->
      book("/stat/simple/2", "z of J/Psi Vtx - z of Primary Vtx", 
      100,-1.0,4.0);
    if( 0 == m_hZDistance ) {
      log << MSG::ERROR << "    Cannot register histogram 2" << endreq;
      return StatusCode::FAILURE;
    }
    
    m_hZVertex = histoSvc()->
      book("/stat/simple/3", "z of J/Psi vertex", 
      100,-10.0,10.0);
    if( 0 == m_hZVertex ) {
      log << MSG::ERROR << "    Cannot register histogram 3" << endreq;
      return StatusCode::FAILURE;
    }
    

    m_hJPsiMass = histoSvc()->
      book("/stat/simple/4", "J/Psi Mass", 
      100,3.05,3.15);
    if( 0 == m_hJPsiMass) {
      log << MSG::ERROR << "    Cannot register histogram 4" << endreq;
      return StatusCode::FAILURE;
    }
    

    m_hChi2ConFit = histoSvc()->
      book("/stat/simple/5", "Chi2 of Constrained J/Psi vertex Fit", 
      100,0.0,20.0);
    if( 0 == m_hChi2ConFit ) {
      log << MSG::ERROR << "    Cannot register histogram 5" << endreq;
      return StatusCode::FAILURE;
    }
    

    m_hSum4p = histoSvc()->
      book("/stat/simple/6", "Mass of 2 Muons Combination", 
      100,3.05,3.15);
    if( 0 == m_hSum4p ) {
      log << MSG::ERROR << "    Cannot register histogram 6" << endreq;
      return StatusCode::FAILURE;
    }


  }





  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode SelectJPsiMuMu::execute() {

  MsgStream  log( msgSvc(), name() );

  m_SelJPsi = false;

  
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

  // Fill the PhysDesktop particle and vertex vectors. 
  // You have to give in the job options the name and confidence levels 
  // for the particles you want.  
  StatusCode scDesktop = m_pDesktop->getInput();
  if (!scDesktop) {
    log << MSG::ERROR << "    not able to fill PhysDesktop " << endreq;
    return StatusCode::FAILURE;
  }
  
  // Retrieve the particles and vertices from PhysDesktop 
  const VertexVector& verts = m_pDesktop->vertices();
  const ParticleVector& parts = m_pDesktop->particles();
  
  log << MSG::DEBUG << "    Vertex Vector size " << verts.size()<< endreq;   
  VertexVector::const_iterator ivert = 0;
  double zPrim = 0.;
  for( ivert = verts.begin(); ivert != verts.end(); ivert++ ) {
    if( (*ivert)->type() == Vertex::Primary ){
      log << MSG::DEBUG << "    Primary Vertex coordinates =  " 
          << (*ivert)->position().x() 
          << " , " << (*ivert)->position().y() 
          << " , " << (*ivert)->position().z() << endreq;
      // Use just  one for the moment
      zPrim = (*ivert)->position().z(); 
    }
  }
  


  log << MSG::DEBUG << "    Particle Vector size " << parts.size()<< endreq;  

  // Start of Paul's ParticleFilter code (and debug)
  ParticleVector vMuons, vMuPlus, vMuMinus;
  StatusCode scFilter = m_pFilter->filter( parts, vMuons );
  StatusCode scFilPos = m_pFilter->filterPositive( vMuons, vMuPlus );
  StatusCode scFilNeg = m_pFilter->filterNegative( vMuons, vMuMinus );
  if ( scFilter && scFilPos && scFilNeg ) {
    log << MSG::DEBUG << ">>> Particle filtration successfull  "  << endreq;
  }
  else {
    log << MSG::ERROR << ">>> Particle filtration failed " << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG << ">>> ParticleFilter DEBUG " << endreq;
  ParticleVector::const_iterator im;
  log << MSG::DEBUG << ">>> Number of muons found " << vMuons.size() 
      << endreq;
  log << MSG::DEBUG << ">>> " << endreq;    
  for( im = vMuons.begin(); im != vMuons.end(); im++ ) {
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
  log << MSG::DEBUG << ">>> Number of mu+ found " << vMuPlus.size() 
      << endreq; 
  log << MSG::DEBUG << ">>> Number of mu- found " << vMuMinus.size() 
      << endreq;
  log << MSG::DEBUG << ">>> " << endreq;    
  // End of Paul's ParticleFilter code (and debug)

 
  // All mu+/mu- combinations
  int JPsiCountPerEvent = 0;  
  //this vector (vMuComb) is supposed to be used as a stack to pass the 
  // Mu+Mu- combinations to be tried out
  ParticleVector vMuComb;
  ParticleVector::iterator iMuPlus;
  ParticleVector::iterator iMuMinus;
  for( iMuMinus = vMuMinus.begin(); iMuMinus != vMuMinus.end(); iMuMinus++ ) {
    
    vMuComb.push_back(*iMuMinus);
  
    for (iMuPlus = vMuPlus.begin(); iMuPlus != vMuPlus.end(); iMuPlus++) {

      vMuComb.push_back(*iMuPlus);
      
      // Find invariant mass
      HepLorentzVector twoMuComb(0.0, 0.0, 0.0, 0.0);
      twoMuComb = (*iMuMinus)->momentum() + (*iMuPlus)->momentum();
      double mass = 0.0;
      mass = twoMuComb.m();
      
      //Remember SI is MeV/mm/nsec. I want to have histogram in GeV/cm to
      //compare  with SICbDST
      if( m_produceHistogram ) {
        m_hSum4p->fill( mass/GeV, 1. );
      }

      //Before performing vertex fit check that muon-antimuon invariant mass
      // is close to the J/Psi vertex
      if (fabs(mass - m_jpsiMass) < m_JPsiMassWin ){
        
        log << MSG::DEBUG << " pz mu- "<< (*iMuMinus)->momentum().pz()<<endreq;
        log << MSG::DEBUG << " pz mu+ "<< (*iMuPlus)->momentum().pz()<<endreq;

        //Perform Unconstrained vertex fit. 
        Vertex MuMuVertex;
        StatusCode scMuMuVertex =m_pVertexFit->fitVertex(vMuComb,MuMuVertex);
        if( scMuMuVertex.isSuccess() ) { 
          HepPoint3D vertexpos =  MuMuVertex.position();       
          
          log << MSG::DEBUG << "Unconstrained vertex position "
              << vertexpos.x() << "  " << vertexpos.y()  <<  " "   
              << vertexpos.z() << endreq;
          
          log << MSG::DEBUG << " Chisquare " << MuMuVertex.chi2()  
              << endreq;


          //Check that muon-antimuon vertex is within a reasonable 
          //window around z = 0.
          if ( fabs(vertexpos.z()) < m_JPsiZWin){
            
            if( m_produceHistogram ) { 
              m_hChi2UncFit->fill(MuMuVertex.chi2() , 1. );
            }
            // Cut on Chi2 of J/Psi unconstrained vertex fit
            if (MuMuVertex.chi2() < m_chi2UncVtxCut) { 

// Examples on how to use the geometrical Displacement Tools.
//Not actually doing anything with them here.
              double ip=0.;
              Hep3Vector ipVector;
              HepSymMatrix errMatrix;
              double ipErr=0;
              double dist=0.;
              double distErr=0;
              for( ivert = verts.begin(); ivert != verts.end(); ivert++ ) {
                if ((*ivert)->type()==1){
                  HepPoint3D priVertexPos = (*ivert)->position();
                  m_pGeomDispCalc->calcImpactPar(*(*iMuMinus),
                                                 *(*ivert),ip,ipErr);
                  // log << MSG::DEBUG << "  IP1 =  " << ip << endreq;  
                  m_pGeomDispCalc->calcImpactPar(*(*iMuMinus),*(*ivert),
                                                 ip,ipErr,
                                                 ipVector,errMatrix);
                  //log << MSG::DEBUG << "  IP1_vect =  " << ipVector.mag() << 
                  //  endreq;  
                  m_pGeomDispCalc->calcImpactPar(*(*iMuMinus),priVertexPos,
                                                 ip,ipErr);
                  //  log << MSG::DEBUG << "  IP2 =  " << ip << endreq;  
                  m_pGeomDispCalc->calcImpactPar(*(*iMuMinus),*(*ivert),
                                                 ip,ipErr,
                                                 ipVector,errMatrix);
                  // log << MSG::DEBUG << "  IP2_vect =  " << ipVector.mag() 
                  //    << endreq;  
                  m_pGeomDispCalc->calcCloseAppr(*(*iMuMinus),(*(*iMuPlus)),
                                                 dist,  distErr);
                  //  log << MSG::DEBUG << "  CDA =  " << dist << endreq;  
                  m_pGeomDispCalc->calcVertexDis(MuMuVertex,*(*ivert),
                                                 dist,distErr);
                  //  log << MSG::DEBUG << "  DIST =  " << dist << endreq;  
                }                
              }
// teste

              // Cut on Value of 'z of JPsi vertex - z of Primary vertex'(cm)
              double zDistance = vertexpos.z()  - zPrim;
              
              if( m_produceHistogram ) {
                m_hZDistance->fill(zDistance/cm , 1. );
              }
              if (zDistance > m_ZDistanceCut) {
                if( m_produceHistogram ) {
                  m_hZVertex->fill(vertexpos.z()/cm, 1. );
                }
               
                // Cut on z of J/Psi vertex
                if (vertexpos.z() < m_ZVertexCut) {
                  
                  if( m_produceHistogram ) {
                    m_hJPsiMass->fill(mass/GeV, 1. );
                  }
                  if (fabs(mass-m_jpsiMass) < m_JPsiMassCut) {
                    
                   
                    // Create Particle from Vertex or perform a mass 
                    // constrained fit
  // Start of Paul's ParticleStuffer code (and debug)
                    Particle candJpsi;       
                    ParticleID jpsiPID( m_jpsiID );
                    StatusCode scStuff = m_pStuffer->fillParticle( MuMuVertex,
                                                     candJpsi, jpsiPID );
                    if ( !scStuff )  {
                      log << MSG::ERROR << ">>> Particle stuffing failed" 
                          << endreq;
                      return StatusCode::FAILURE;
                    }
                    log << MSG::DEBUG << ">>> ParticleStuffer DEBUG " 
                        << endreq;
                    log << MSG::DEBUG 
                        << ">>> Comparison of input Vertex and output Particle"
                        << endreq;
                    log << MSG::DEBUG << ">>> Vertex attributes " << endreq;
                    log << MSG::DEBUG << ">>> position " 
                        << MuMuVertex.position().x()
                        << " " << MuMuVertex.position().y()
                        << " " << MuMuVertex.position().z() 
                        << endreq;       
                    log << MSG::DEBUG << ">>> error matrix " << endreq;       
                    HepSymMatrix mat = MuMuVertex.positionErr(); 
                    int i;
                    for ( i = 1; i != 4; i++ ) { 
                      log << MSG::DEBUG << ">>> ";         
                      for ( int j = 1; j != 4; j++ ) {          
                        log << MSG::DEBUG << mat( i, j ) << "  ";           
                      }       
                      log << MSG::DEBUG << endreq;       
                    }       
                    log << MSG::DEBUG << ">>> chi2 " 
                        << MuMuVertex.chi2() << endreq;       
                    log << MSG::DEBUG << ">>> no. of daughters " 
                        << MuMuVertex.products().size() << endreq;       

                    log << MSG::DEBUG << ">>> Particle attributes " << endreq;
                    log << MSG::DEBUG << ">>> position of point " 
                        << candJpsi.pointOnTrack().x()
                        << " " << candJpsi.pointOnTrack().y()
                        << " " << candJpsi.pointOnTrack().z() 
                        << endreq;       
                    log << MSG::DEBUG << ">>> point error matrix " << endreq;
                    HepSymMatrix matp = candJpsi.pointOnTrackErr();       
                    for ( i = 1; i != 4; i++ ) { 
                      log << MSG::DEBUG << ">>> ";         
                      for ( int j = 1; j != 4; j++ ) {          
                        log << MSG::DEBUG << matp( i, j ) << "  ";           
                      }       
                      log << MSG::DEBUG << endreq;       
                    }
                    log << MSG::DEBUG << ">>> four-momentum "
                        << candJpsi.momentum().px()
                        << " " << candJpsi.momentum().py()
                        << " " << candJpsi.momentum().pz() 
                        << " " << candJpsi.momentum().e() 
                         << endreq; 
                    log << MSG::DEBUG << ">>> four-momentum error matrix " 
                        << endreq;
                    HepSymMatrix matm = candJpsi.momentumErr();       
                    for ( i = 1; i != 5; i++ ) { 
                      log << MSG::DEBUG << ">>> ";         
                      for ( int j = 1; j != 5; j++ ) {          
                        log << MSG::DEBUG << matm( i, j ) << "  ";           
                      }       
                      log << MSG::DEBUG << endreq;       
                    }
                    log << MSG::DEBUG << ">>> four-momentum of daughters " 
                        << endreq;     
                    SmartRefVector<Particle>::const_iterator it;
                    for ( it = candJpsi.endVertex()->products().begin();
                          it != candJpsi.endVertex()->products().end(); it++ ){
                      log << MSG::DEBUG << ">>> ";
                      log << MSG::DEBUG << (*it)->momentum().px() << "  ";
                      log << MSG::DEBUG << (*it)->momentum().py() << "  ";
                      log << MSG::DEBUG << (*it)->momentum().pz() << "  ";
                      log << MSG::DEBUG << (*it)->momentum().e()  << "  ";
                      log << MSG::DEBUG << endreq;
                      
                    }
                    log << MSG::DEBUG << "daughters four-momentum error matrix"
                        << endreq;     
                    for ( it = candJpsi.endVertex()->products().begin();
                          it != candJpsi.endVertex()->products().end(); it++ ){
                      matm = (*it)->momentumErr();         
                      for ( int i = 1; i != 5; i++ ) { 
                        log << MSG::DEBUG << ">>> ";         
                        for ( int j = 1; j != 5; j++ ) {          
                          log << MSG::DEBUG << matm( i, j ) << "  ";           
                        }       
                        log << MSG::DEBUG << endreq;       
                      }
                    }
                    log << MSG::DEBUG << ">>> mass and error "
                        << candJpsi.mass() << "  " << candJpsi.massErr()
                        << endreq;  
  // End of Paul's ParticleStuffer code (and debug)
                    //Perform mass constrained vertex fit
                    
                    Vertex jpsiVtx;
                    Particle jpsi;
                   
                    StatusCode scLagFit =m_pLagFit->lagrangeFitter("J/psi(1S)",
                                                                   vMuComb, 
                                                                   jpsiVtx, 
                                                                   jpsi);
                    if( scLagFit.isSuccess() ) {
                    
                      //Cut on Chi2 of J/Psi constrained vertex fit      
                      if( m_produceHistogram ) {
                        m_hChi2ConFit->fill(jpsiVtx.chi2() , 1.);
                      }
                      if (jpsiVtx.chi2() < m_chi2ConVtxCut) {
                        
                        log << MSG::DEBUG << " Passed all cuts   " << endreq;
                        
                        m_SelJPsi = true;
                        
                        m_JPsiCount++;
                        JPsiCountPerEvent++;
                        
                        //Gloria saving THIS particle and its vertex to deskTop
                        //The pointer to the particle is returned: it is created
                        Particle* pInDesktop= m_pDesktop->createParticle(&jpsi);
                        if( pInDesktop) {
                          log << MSG::DEBUG << "J/Psi added to PhysDesktop "
                            << "Pid and momentum = " 
                            << pInDesktop->particleID().id() << ", "
                            << pInDesktop->momentum().px() << ", "
                            << pInDesktop->momentum().py() << ", "
                            << pInDesktop->momentum().pz() << endreq;
                          log << MSG::DEBUG << "Its decay vertex position = " 
                              <<(pInDesktop->endVertex())->position().x()<< ", "
                              <<(pInDesktop->endVertex())->position().y()<< ", "
                              <<(pInDesktop->endVertex())->position().z()
                              << endreq;
                        }
                        else {
                          log <<MSG::DEBUG<< "not able to save J/Psi in desktop"
                              << endreq;  
                        }

                        
                      }// chi2ConFit 
                    }//scLagFit.isSuccess
                    
                  }//mass() > m_JPsiMassCu
                }//unconsvertex_z < m_ZVertexCut
              }//ZDistance > ZDistanceCut
            }//chi2 < m_chi2UncVtxCut
          }// zvertex < m_jpsizwin
        }//scMuMuVertex.isSuccess()
      }//mass/GeV - m_JPsiMass
      
      // Remove this mu- from "stack"
    vMuComb.pop_back(); 

   }//for(iMuMinus = vMuPlus.begin() ...
   
   vMuComb.pop_back(); 
  }//for( iMuPlus = vMuMinus.begin()...
  
  // Log number of Combinations found by previous step
  log << MSG::INFO << "  Number of two muons combinations  = " 
      << JPsiCountPerEvent  << endreq;
  
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
    log << MSG::ERROR << "not able to save desktop in TES"
        << endreq;
    return StatusCode::FAILURE;
  }

  

  // End of execution for each event
  return StatusCode::SUCCESS;

}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode SelectJPsiMuMu::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  // Print out counters
  log << MSG::INFO << "    Number of events processed      = " 
      << m_nEvents << endreq;

  log << MSG::INFO << "    Number of selected   JPsi       = " 
      << m_JPsiCount << endreq;
  // End of finalization step




  return StatusCode::SUCCESS;
}

//=============================================================================
