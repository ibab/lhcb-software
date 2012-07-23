// $Id: OTCosmic.cpp,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $

// -------------------------------------------------------------
// File: OTCosmicGenerator/OTCosmicGenerator.cxx
// Description:

//    The output will be stored in the transient event store so it can be
//    passed to the simulation.
// 
// AuthorList:
//         W. Seligman: Initial Code 08-Nov-2002,
//         based on work by M. Shapiro and I. Hinchliffe
//

// Modification for increasing efficiency of muon hitting the detector:
//                     H. Ma.    March 17, 2006 
//   Property: ExzCut: 	
//	if true, the method exzCut(...) will be called to apply a 
//               energy dependent position cut on the surface.
//               This rejects low energy muons at large distance. 
//   Property: RMax
//               Used by exzCut to reject non-projective muons, which are 
//               too far out on the surface


// Modifications to accomodate Pixel EndCap C Cosmic Test needs
//      Marian Zdrazil   June 7, 2006   mzdrazil@lbl.gov
//
// Modifications to accomodate replacement of Pixel EndCap C by a Pixel EndCap A
//      Marian Zdrazil   November 24, 2006  mzdrazil@lbl.gov
//
//
// Description:
// ------------
// It is easier and actually more useful to leave the EndCap A
// in the vertical position (the way it is positioned in the ATLAS detector)
// instead of rotating it clockwise by 90deg which corresponds to the
// placement during the Pixel EndCap A cosmic test in SR1 in November 2006.
// This is why we will generate cosmic muons coming from the positive Z-axis 
// direction better than rotating the whole setup in PixelGeoModel.
//
// Modifications July 3rd 2007, Rob McPherson
//     - Fix mu+/mu- bug (always present in Athena versions)
//     - Fix sign of Py (since tag OTCosmicGenerator-00-00-21, muons only upward-going) 

// Local
#include "OTCosmic.h"

// From STL
#include <cmath>

// From Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h" 
#include "GaudiKernel/IRndmGenSvc.h"

// From DetDesc
#include "DetDesc/SolidBox.h"

DECLARE_TOOL_FACTORY( OTCosmic );

//--------------------------------------------------------------------------
OTCosmic::OTCosmic( const std::string& type , 
                    const std::string& name , 
                    const IInterface * parent )
  : GaudiTool( type , name, parent ) ,
    m_events( 0 ) ,
    m_generated( 0 ) , 
    m_rejectedbyenergycut( 0 ) , 
    m_rejectedbyscintacceptance( 0 ) ,
    m_muplus( 0 ) , m_muminus( 0 )
//--------------------------------------------------------------------------  
{
  declareInterface< IParticleGunTool >( this ) ;
  //
  // Migration to MeV and mm units: all conversions are done in this interface 
  // to the CosmicGun. The CosmicGun itself uses GeV units internally - to call
  // the fortran code.
  //
  declareProperty("emin",       m_emin =70.*Gaudi::Units::GeV );
  declareProperty("emax",       m_emax =10000.*Gaudi::Units::GeV );
  declareProperty("tmin",       m_tmin =0. );
  declareProperty("tmax",       m_tmax =0. );
  declareProperty("ctcut",      m_ctcut =0.35 );
  declareProperty("PrintEvent", m_printEvent=10);
  declareProperty("PrintMod",   m_printMod=100);
  declareProperty("ThetaMin",   m_thetamin = 0.);
  declareProperty("ThetaMax",   m_thetamax = 1.);
  declareProperty("PhiMin",     m_phimin = -1*Gaudi::Units::pi);
  declareProperty("PhiMax",     m_phimax = Gaudi::Units::pi);
}

//--------------------------------------------------------------------------
 OTCosmic::~OTCosmic()
//--------------------------------------------------------------------------
{ }

StatusCode OTCosmic::initialize() {
  StatusCode sc = GaudiTool::initialize() ;
  if ( ! sc.isSuccess() ) return sc ;

  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  sc = m_flatgenerator.initialize( randSvc , Rndm::Flat( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() ) return Error( "Cannot initialize flat generator" ) ;

  // Initialize event count.
  m_events=m_rejectedbyenergycut=m_rejectedbyscintacceptance=0 ;

  // position the two sintillators
  m_scintsolid = 
    std::auto_ptr<SolidBox>(new SolidBox( "OTScintSolid",60*Gaudi::Units::cm,0.5*135*Gaudi::Units::cm,5*Gaudi::Units::mm)) ;
  m_toptransform = Gaudi::Transform3D(Gaudi::Transform3D::Vector(100*Gaudi::Units::cm,200*Gaudi::Units::cm,1000*Gaudi::Units::cm)) ;
  m_bottransform = Gaudi::Transform3D(Gaudi::RotationX(M_PI/2),
				      Gaudi::Transform3D::Vector(100*Gaudi::Units::cm,-300*Gaudi::Units::cm,600*Gaudi::Units::cm)) ;

  LHCb::IParticlePropertySvc* ppSvc = 
    svc< LHCb::IParticlePropertySvc >( "LHCb::ParticlePropertySvc" , true ) ;
  m_muplus  = ppSvc->find( LHCb::ParticleID(  13 ) );
  m_muminus = ppSvc->find( LHCb::ParticleID( -13 ) );
  release( ppSvc ) ;
  
  return StatusCode::SUCCESS;
}

void OTCosmic::generateParticle( Gaudi::LorentzVector & fourMomentum , 
                                 Gaudi::LorentzVector & origin , 
                                 int & pdgId ) {
  ++m_events;
  debug() << "Event #" << m_events << endmsg;
  bool accepted=false;
  
  while(!accepted){
	
    // Generate a cosmics (energy, angles) at the surface
    //HepLorentzVector pp = m_cosmicgun->GenerateEvent();
    //double theta1=pp.theta();
    //double phi1=pp.phi();
    //double mag1=pp.vect().mag();

    // Generate the energy
    const float alpha = 1.-2.7 ;
    float erndm = m_flatgenerator() ;
    double Emu0  = std::pow( (1-erndm) * std::pow(m_emin,alpha) + erndm * std::pow(m_emax,alpha), 1/alpha ) ;
    // Generate phi and theta (z-axis perp earth surface
    double phi1   = Gaudi::Units::pi * (2*m_flatgenerator()-1.) ;
    double theta1 = std::pow( acos( m_flatgenerator() ), 1./3. ) ;
    
    // apply the correction for energy loss in 100 m rock
    //double Emu0          = pp.t() * Gaudi::Units::GeV ;
    const double a       = 2.5 * Gaudi::Units::MeV ; // GeV * g^{-1} * cm^2
    const double b       = 4.e-6 ; // g^{-1} * cm^2
    const double eps     = a/b ;  // ~ 600 GeV
    const double depth   = 1.e4 ;  // cm
    const double density = 3. ;    //  g * cm^{-3}
    double X             = density * depth / cos(theta1) ; // for theta==0 --> Xb = 0.12
    double Emu           = ( Emu0 + eps ) * exp( - X*b ) - eps ;
    //std::cout << "energy, energy loss: " << Emu << " " << Emu0 - Emu << std::endl 
    //	      << "threshold: " << (exp(density*depth*b ) - 1) * eps << std::endl ;

    if(Emu > 200.*Gaudi::Units::MeV) {
      //std::cout << "accepted energy: " << Emu << std::endl ;
      // rotate to the lhcb frame
      fourMomentum.SetCoordinates( Emu*sin(theta1)*cos(phi1),-Emu*cos(theta1),Emu*sin(theta1)*sin(phi1),Emu );
      //p4 *= Gaudi::Units::GeV ;
      Gaudi::XYZVector globaldir(fourMomentum.x(),fourMomentum.y(),fourMomentum.z()) ;
      
      // Generate a position inside the 1st scintillator
      double x = (2.*m_flatgenerator()-1) * m_scintsolid->xHalfLength() ;
      double y = (2.*m_flatgenerator()-1) * m_scintsolid->yHalfLength() ;
      double z = (2.*m_flatgenerator()-1) * m_scintsolid->zHalfLength() ;
      Gaudi::XYZPoint localpostop(x,y,z) ;
      
      // Transform this to global coordinates
      Gaudi::XYZPoint globalpostop = m_toptransform * localpostop ;
      
      // Now test that this cosmic hits the 2nd scintillator
      ISolid::Ticks ticks ;
      int n = m_scintsolid->intersectionTicks( m_bottransform.Inverse() * globalpostop,
					       m_bottransform.Inverse() * globaldir, ticks) ;
      if(n==2) {
	info() << "accepted trigger: " << Emu << " " << theta1 << " " << phi1 << endreq ;
	// still need to generate the phase 
	double time = m_tmin ;
	if( m_tmin < m_tmax ) time += m_flatgenerator() * (m_tmax - m_tmin) ;
	origin.SetCoordinates(globalpostop.X(), globalpostop.Y(), globalpostop.Z(), time ) ;
	accepted = true ;
      } else ++m_rejectedbyscintacceptance ;
    } else ++m_rejectedbyenergycut ;
  }    

  int charge = int(m_flatgenerator() * 2 - 1);//m_cosmicgun->GetMuonCharge();
  pdgId = charge > 0 ? m_muplus->pid().pid()  : m_muminus->pid().pid() ;
}


//---------------------------------------------------------------------------
void OTCosmic::printCounters() {
  int ntot = m_events + m_rejectedbyenergycut + m_rejectedbyscintacceptance ;
  info() << "Total cosmic rays generated:           " << ntot << std::endl 
	 << "Fraction rejected by energy threshold: " 
	 << m_rejectedbyenergycut/double(ntot) << std::endl
	 << "Fraction rejected by acceptance:       " 
	 << m_rejectedbyscintacceptance/double(m_events +m_rejectedbyscintacceptance) << endreq ;
}
