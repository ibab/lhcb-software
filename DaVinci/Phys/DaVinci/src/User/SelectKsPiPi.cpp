// $Id: SelectKsPiPi.cpp,v 1.1 2002-03-27 20:53:40 gcorti Exp $
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
#include "SelectKsPiPi.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SelectKsPiPi
//
// author : Claudia Nunes 
// 20/03/2002 : Modified from SelectJPsiMuMu made by Sandra Amato
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<SelectKsPiPi>          s_factory ;
const        IAlgFactory& SelectKsPiPiFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SelectKsPiPi::SelectKsPiPi( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator),
    m_pIDSearch(0),
    m_daugID(0),
    m_nEvents(0),
    m_KsCount(0),
    m_pAsct(0),
    m_pSelTool(0),
    m_ppSvc(0),
    m_pDesktop(0),
    m_pLagFit(0),
    m_pVertexFit(0),  
    m_pGeomDispCalc(0),
    m_pFilter(0),
    m_pStuffer(0)    {    
  m_daugName.clear();
  declareProperty( "SearchParticle", m_pNameSearch = "KS0" );
  declareProperty( "DecayProducts", m_daugName );
  declareProperty( "HistogramFlag", m_produceHistogram = false );
  declareProperty( "KsMassWindowLoose", m_ksmasswinloo = 0.2 * GeV);
  declareProperty( "KsZWindow", m_kszwin = 50.0 * cm);
  declareProperty( "Chi2UncFitCut", m_chi2UncVtxCut = 20.0);
  declareProperty( "ZDistanceCut", m_ZDistanceCut = 10.0 *  cm);
  declareProperty( "ZVertexCut", m_ZVertexCut = 220.0 * cm);
  declareProperty( "KsMassWindowTight", m_ksmasswinti = 0.01 * GeV);
  declareProperty( "Chi2ConFit", m_chi2ConVtxCut = 20.0);
  declareProperty( "Pt", m_ptCut = 1. * GeV );
  declareProperty( "ImpParam" , m_impParamCut = 0.9);
  
  
}

//=============================================================================
// Destructor
//=============================================================================
SelectKsPiPi::~SelectKsPiPi() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode SelectKsPiPi::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  // Retrieve the tools used by this algorithm
  // Note that the ToolSvc is now available via the toolSvc() method
  //  you can hardcode the type of a tool or set  in the job options  
  
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
  

  partProp = m_ppSvc->find( "pi+" );
  m_PiPlusID = partProp->jetsetID();

  partProp = m_ppSvc->find( "pi-" );
  m_PiMinusID = (*partProp).jetsetID();


  partProp = m_ppSvc->find( "KS0" );
  m_KsMass = (*partProp).mass();
  m_ksID = (*partProp).jetsetID();
  
  // If histograms are required inintialize them
  if( m_produceHistogram )   {
    log << MSG::INFO << "    histograms will be produced" << endreq;

    // Create the histograms 

   m_hImpParam = histoSvc()->
      book("/stat/simple/20", "Ks impact par variable", 
      100,0.0,1.0);
    if( 0 == m_hImpParam ) {
      log << MSG::ERROR << "    Cannot register histogram 20" << endreq;
      return StatusCode::FAILURE;
    }

    m_hChi2UncFit = histoSvc()->
      book("/stat/simple/21", "Chi2 of unconstrained Ks vertex fit", 
      100,0.0,20.0);
    if( 0 == m_hChi2UncFit ) {
      log << MSG::ERROR << "    Cannot register histogram 21" << endreq;
      return StatusCode::FAILURE;
    }
    
    m_hZDistance = histoSvc()->
      book("/stat/simple/22", "z of Ks Vtx - z of Primary Vtx", 
      100,-50.0,450.0);
    if( 0 == m_hZDistance ) {
      log << MSG::ERROR << "    Cannot register histogram 22" << endreq;
      return StatusCode::FAILURE;
    }
    
    m_hPt = histoSvc()->
      book("/stat/simple/23", "pt of Ks", 
      100,0.0,10.0);
    if( 0 == m_hPt ) {
      log << MSG::ERROR << "    Cannot register histogram 23" << endreq;
      return StatusCode::FAILURE;
    }

    m_hZVertex = histoSvc()->
      book("/stat/simple/24", "z of Ks vertex", 
      100,0.0,500.0);
    if( 0 == m_hZVertex ) {
      log << MSG::ERROR << "    Cannot register histogram 24" << endreq;
      return StatusCode::FAILURE;
    }
    
    m_hKsMass = histoSvc()->
      book("/stat/simple/25", "Ks Mass", 
      100,0.45,0.55);
    if( 0 == m_hKsMass) {
      log << MSG::ERROR << "    Cannot register histogram 25" << endreq;
      return StatusCode::FAILURE;
    }
    
    m_hChi2ConFit = histoSvc()->
      book("/stat/simple/26", "Chi2 of Constrained Ks vertex Fit", 
      100,0.0,20.0);
    if( 0 == m_hChi2ConFit ) {
      log << MSG::ERROR << "    Cannot register histogram 26" << endreq;
      return StatusCode::FAILURE;
    }

    m_hSum4p = histoSvc()->
      book("/stat/simple/27", "Mass of 2 Pions Combination", 
      100,3.05,3.15);
    if( 0 == m_hSum4p ) {
      log << MSG::ERROR << "    Cannot register histogram 27" << endreq;
      return StatusCode::FAILURE;
    }
    
  }


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SelectKsPiPi::execute() {

  MsgStream  log( msgSvc(), name() );

  m_SelKs = false;
  

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
  
  // Retrieve the particles and vertices from PhysDeskTop 
  const VertexVector& verts = m_pDesktop->vertices();
  const ParticleVector& parts = m_pDesktop->particles();
  
  log << MSG::DEBUG << "    Vertex Vector size " << verts.size()<< endreq;   
  VertexVector::const_iterator ivert = 0;
  double zPrim = 0.;
  for( ivert = verts.begin(); ivert != verts.end(); ivert++ ) {
    if( (*ivert)->type() == Vertex::Primary ){
      log << MSG::DEBUG << "    Primary Vertex coordinates = ( " 
          << (*ivert)->position().x() 
          << " , " << (*ivert)->position().y() 
          << " , " << (*ivert)->position().z() << endreq;
      // Use just  one for the moment
      zPrim = (*ivert)->position().z(); 
    }
  }
  
  log << MSG::DEBUG << "    Particle Vector size " <<parts.size()<< endreq;  


  // Start of Paul's ParticleFilter code (and debug)
  ParticleVector vPions, vPiPlus, vPiMinus;
  StatusCode scFilter = m_pFilter->filter( parts, vPions );
  StatusCode scFilPos = m_pFilter->filterPositive( parts, vPiPlus );
  StatusCode scFilNeg = m_pFilter->filterNegative( parts, vPiMinus );
  if ( scFilter && scFilPos && scFilNeg ) {
    log << MSG::DEBUG << ">>> Particle filtration successfull  "  << endreq;
  }
  else {
    log << MSG::ERROR << ">>> Particle filtration failed " << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG << ">>> ParticleFilter DEBUG " << endreq;
  ParticleVector::const_iterator im;
  log << MSG::DEBUG << ">>> Number of pions found " << vPions.size() 
      << endreq;
  log << MSG::DEBUG << ">>> " << endreq;    
  for( im = vPions.begin(); im != vPions.end(); im++ ) {
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
  log << MSG::DEBUG << ">>> Number of pi+ found " << vPiPlus.size() 
      << endreq; 
  log << MSG::DEBUG << ">>> Number of pi- found " << vPiMinus.size() 
      << endreq;
  log << MSG::DEBUG << ">>> " << endreq;    
  // End of Paul's ParticleFilter code (and debug)


 
  // All pi+/pi- combinations
  int ksCountPerEvent = 0;  
  //this vector (vPiComb) is supposed to be used as a stack to pass the 
  // Pi+Pi- combinations to be tried out
  ParticleVector vPiComb;
  
  ParticleVector::iterator iPiPlus;
  ParticleVector::iterator iPiMinus;

  for( iPiPlus = vPiPlus.begin(); iPiPlus != vPiPlus.end(); iPiPlus++ ) {
    
    vPiComb.push_back(*iPiPlus);
    
    for (iPiMinus = vPiMinus.begin(); iPiMinus != vPiMinus.end(); iPiMinus++) {
      
      vPiComb.push_back(*iPiMinus);    
      
      // Find invariant mass
      HepLorentzVector twoPiComb(0.0, 0.0, 0.0, 0.0);
      twoPiComb = (*iPiMinus)->momentum() + (*iPiPlus)->momentum();
      double mass = 0.0;
      mass = twoPiComb.m();
      
      
      //Remember SI is MeV/mm/nsec. I want to have histogram in GeV/cm to
      //compare  with SICbDST
      
      if( m_produceHistogram ) {
        m_hSum4p->fill( mass/GeV, 1. );
      }
      
      
      //Before performing vertex fit check that pion-antipion invariant mass
      // is close to the Ks mass	      
      if (fabs(mass - m_KsMass) < m_ksmasswinloo ){
        
        //Perform Unconstrained Ks vertex Fit
        
        Vertex piPiVertex;
        StatusCode scpiPiVertex =m_pVertexFit->fitVertex(vPiComb,
                                                         piPiVertex);
        if( scpiPiVertex.isSuccess() ) { 
          
          // Check that the pion-antipion
          //vertex is within a reasonable window around z = 0.
          HepPoint3D vertexpos =  piPiVertex.position();       
          if ( vertexpos.z() > -m_kszwin){
            
            // Create Particle Ks from Vertex 
            Particle candKs;       
            ParticleID ksPID( m_ksID );
            StatusCode scStuff = m_pStuffer->fillParticle( piPiVertex,
                                                           candKs, 
                                                           ksPID );
            if ( !scStuff ) {
              log << MSG::ERROR << ">>> Particle stuffing failed" 
                  << endreq;
              return StatusCode::FAILURE;
            }
            
            // calculate the Ks impact parameter
            
            double ip1=0.;
            double ipErr1=0.;
            double ipErr2=0.;
            double ip2=0.;
            double ipKs=0.;
            int count =0;
            
            for( ivert = verts.begin(); ivert != verts.end(); ivert++ ) {
              if((*ivert)->type() == Vertex::Primary ){
                count++;
                
                m_pGeomDispCalc->calcImpactPar(*(*iPiMinus),
                                               *(*ivert),ip1,ipErr1);
                m_pGeomDispCalc->calcImpactPar(*(*iPiPlus),
                                               *(*ivert),ip2,ipErr2);
                
                if (count == 1) {
                  ipKs = fabs((ip2 - ip1))/(ip2 + ip1);
                }
              }
            }
            if( m_produceHistogram ) { 
              m_hImpParam->fill(ipKs, 1. );
            }
            
            if ( ipKs < m_impParamCut ) {
              
              // Cut on Chi2 of Ks unconstrained vertex fit
              if( m_produceHistogram ) { 
                m_hChi2UncFit->fill(piPiVertex.chi2() , 1. );
              }
              if (piPiVertex.chi2() < m_chi2UncVtxCut) {
                
                // Cut on Value of 'z of Ks vertex - z of Primary vertex'(cm)
                double zDistance = vertexpos.z()  - zPrim;
                
                if( m_produceHistogram ) {
                  m_hZDistance->fill(zDistance/cm , 1.);
                }
                if (zDistance > m_ZDistanceCut) {
                  
                  //Cut  on the Ks pt
                  if( m_produceHistogram ) {
                    m_hPt->fill(candKs.momentum().vect().perp()/GeV, 1. );
                  }
                  if ( candKs.momentum().vect().perp()  > m_ptCut) {
                    
                    // Cut on z of Ks vertex
                    if( m_produceHistogram ) {
                      m_hZVertex->fill(vertexpos.z()/cm, 1. );
                    }
                    
                    if (vertexpos.z() < m_ZVertexCut) {
                      
                      if( m_produceHistogram ) {
                        m_hKsMass->fill(candKs.mass()/GeV, 1. );
                      }
                      if ((fabs(candKs.mass() - m_KsMass)) < m_ksmasswinti) {
                        
                        
                        
                        //Perform mass constrained vertex fit
                        Vertex ksVtx;
                        Particle ks;
                        
                        StatusCode scLagFit =m_pLagFit->lagrangeFitter("KS0", 
                                                                       vPiComb,
                                                                       ksVtx,
                                                                       ks);
                        
                        if( scLagFit.isSuccess() ) {

                          //Cut on Chi2 of Ks constrained vertex fit      
                          if( m_produceHistogram ) {
                            m_hChi2ConFit->fill(ksVtx.chi2(), 1.);
                          }

                          if (ksVtx.chi2() < m_chi2ConVtxCut) {
                            
                            log << MSG::DEBUG << " Passed all cuts " << endreq;
                            
                            m_SelKs = true;
                            
                            m_KsCount++;
                            ksCountPerEvent++;
                            
                            
                            Particle* pInDesktop= 
                              m_pDesktop->createParticle(&ks);
                        
                            if( pInDesktop) {
                              log << MSG::DEBUG << "Ks added to PhysDesktop "
                                  << "Pid and momentum = " 
                                  << pInDesktop->particleID().id() << ", "
                                  << pInDesktop->momentum().px() << ", "
                                  << pInDesktop->momentum().py() << ", "
                                  << pInDesktop->momentum().pz() << endreq;
                              log << MSG::DEBUG 
                                  << "Its decay vertex position = "
                                  <<(pInDesktop->endVertex())->position().x()
                                  <<", "
                                  <<(pInDesktop->endVertex())->position().y() 
                                  << ", "
                                  << (pInDesktop->endVertex())->position().z()
                                  << endreq;
                            }
                            else {
                              log << MSG::DEBUG 
                                  << "not able to save Ks in desktop"<<endreq;
                            }
                            
                            
                            
                          } //chi2ConFit 
                          
                        } //is success
                        
                      } // m_KsMassCut
                    } // m_ZVertexCut
                  } //KsptCut
                } //ZDistance > ZDistanceCut
              } //chi2 < m_chi2UncVtxCut
            } //ip Cut
          } // zvertex < m_kszwin
        } //scpiPiVertex.isSuccess()
      } //mass/GeV - m_ksmasswinloo
      
      // Remove this pi- from "stack"     
      vPiComb.pop_back();
      
      
    } //for(iPiPlus = vPiPlus.begin() ...
    
    
    vPiComb.pop_back(); 
    
    
    
  } //for( iPiMinus = vPiMinus.begin()...
  
  // Log number of Combinations found by previous step
  log << MSG::INFO << "  Number of two pions combinations  = " 
      << ksCountPerEvent << endreq;
  
  
  log << MSG::INFO << "  Number of particles in desktop = " << parts.size() 
      << endreq;
  log << MSG::INFO << "  Number of vertices in desktop = " << verts.size()
      << endreq;

  // Gloria Now save desktop to TES in specified location in jobOptions
  // Notice that this delete particles from desktop at the moment
  scDesktop = m_pDesktop->saveDesktop();
  if (scDesktop) {
    log << MSG::INFO << " PhysDesktop Saved to TES"<<endreq;
  }
  else {
    log << MSG::ERROR << "not able to save Desktop in TES"
        << endreq;
    return StatusCode::FAILURE;
  }







    return StatusCode::SUCCESS;

}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode SelectKsPiPi::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  // Print out counters
  log << MSG::INFO << "    Number of events processed      = " 
      << m_nEvents << endreq;
  log << MSG::INFO << "    Number of selected     Ks       = " 
      << m_KsCount << endreq;

  // End of finalization step




  return StatusCode::SUCCESS;
}

//=============================================================================








