// $Id: SmearVertexAlg.cpp,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
// Include files 

// This class
#include "SmearVertexAlg.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// from Event
#include "Event/HepMCEvent.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SmearVertexAlg
//
// 2003-02-02 : Witold POKORSKI
// 2005-05-20 : A.Carbone & G.Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<SmearVertexAlg>          s_factory ;
const        IAlgFactory& SmearVertexAlgFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SmearVertexAlg::SmearVertexAlg( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) {

  declareProperty("Xsigma",m_xsigma=0.07); // mm
  declareProperty("Ysigma",m_ysigma=0.07); // mm
  declareProperty("Zsigma",m_zsigma=50.0); // mm

  declareProperty("Xcut",m_xcut=4.0); // in terms of Xsigma
  declareProperty("Ycut",m_ycut=4.0); // in terms of Ysigma
  declareProperty("Zcut",m_zcut=4.0); // in terms of Zsigma

  declareProperty("FlatInZ",m_flat=false);
  declareProperty("Zmax",   m_zmax= 900); // mm
  declareProperty("Zmin",   m_zmin=-800); // mm

}

//=============================================================================
// Destructor
//=============================================================================
SmearVertexAlg::~SmearVertexAlg() {} 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode SmearVertexAlg::initialize() {

  debug() << "==> Initialize" << endreq ;

  StatusCode sc = GaudiAlgorithm::initialize( ) ;

  if ( sc.isFailure() ) return Error( "Base class not initialized" ) ;

  sc = m_gaudigen.initialize(randSvc(), Rndm::Gauss(0.0,1.0));
  if ( ! sc.isSuccess() )
    return Error( "Could not initialize random generator" ) ;

  if (m_flat) {
    if (m_zmin>m_zmax) return  Error("Zmin could not be greater then Zmax");
    sc=m_flatgen.initialize(randSvc(), Rndm::Flat(m_zmin,m_zmax));
    if ( !sc.isSuccess() )
      return  Error("Could not initialize flat random generator");
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SmearVertexAlg::execute() {
  debug() << "==> Execute" << endreq;
  
  HepMCEvents * hepMCptr = get< HepMCEvents >( HepMCEventLocation::Default ) ;
  
  if( 0 == hepMCptr ) info () << "No HepMCptr" << endreq ; 
  else {
    // loop over HepMC events
    HepMCEvents::iterator it;
    double dx, dy, dz;
    
    for( it=hepMCptr->begin() ; it!=hepMCptr->end() ; ++it ) {
      // generate random values for spread in x,y,z
      do { 
        dx=m_gaudigen();
      } while (fabs(dx)>m_xcut);
      dx=dx*m_xsigma;
      
      do {
        dy=m_gaudigen();
      } while (fabs(dy)>m_ycut);
      dy=dy*m_ysigma;
      
      if (m_flat) { // flat distribution in z
        dz=m_flatgen();
      } else {
        do {
          dz=m_gaudigen();
        } while (fabs(dz)>m_zcut);
        dz=dz*m_zsigma;
      }
      
      HepLorentzVector dpos(dx,dy,dz,0.0);
      
      // loop over all vertices in the event and 
      // shift them approprietely 
      HepMC::GenEvent::vertex_iterator vit;
      HepMC::GenEvent * pEvt = (*it)->pGenEvt() ;
      for ( vit = pEvt -> vertices_begin() ; 
            vit!= pEvt -> vertices_end() ; ++vit ) {
        HepLorentzVector pos=(*vit)->position();
        (*vit)->set_position(pos+dpos);
      }
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
