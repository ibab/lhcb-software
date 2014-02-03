
// This class
#include "MomentumSpectrum.h"

// From STL
#include <cmath>

// FromGaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IRndmGenSvc.h" 



//===========================================================================
// Implementation file for class: MomentumSpectrum
//
// 2014-01-03: Dan Johnson (adapted from Michel De Cian)
//===========================================================================

DECLARE_TOOL_FACTORY( MomentumSpectrum );

//===========================================================================
// Constructor
//===========================================================================
MomentumSpectrum::MomentumSpectrum( const std::string & type ,
                              const std::string & name , 
                              const IInterface * parent )
  : GaudiTool( type , name, parent ) {
    declareInterface< IParticleGunTool >( this ) ;
      
    declareProperty( "PdgCodes", m_pdgCodes);
    declareProperty( "InputFile", m_inputFileName);
    declareProperty( "HistogramPath", m_histoPath);
		declareProperty( "BinningVariables", m_binningVars);
    
}
//===========================================================================
// Destructor
//===========================================================================
MomentumSpectrum::~MomentumSpectrum() { }

//===========================================================================
// Initialize Particle Gun parameters
//===========================================================================
StatusCode MomentumSpectrum::initialize() {
  StatusCode sc = GaudiTool::initialize() ;
  if ( ! sc.isSuccess() ) return sc ;
  
  
  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  sc = m_flatGenerator.initialize( randSvc , Rndm::Flat( 0. , 1. ) ) ;
  
  if ( ! sc.isSuccess() ) return Error( "Cannot initialize flat generator" ) ;
  
  // Get the mass of the particle to be generated
  //
  LHCb::IParticlePropertySvc* ppSvc = 
    svc< LHCb::IParticlePropertySvc >( "LHCb::ParticlePropertySvc" , true ) ;

  // setup particle information
  m_masses.clear();

  info() << "Particle type chosen randomly from :";
  PIDs::iterator icode ;
  for ( icode = m_pdgCodes.begin(); icode != m_pdgCodes.end(); ++icode ) {
    const LHCb::ParticleProperty * particle = 
      ppSvc->find( LHCb::ParticleID( *icode ) ) ;
    m_masses.push_back( ( particle->mass() ) ) ;
    m_names.push_back( particle->particle() ) ;
    info() << " " << particle->particle() ;
  }
  
  info() << endmsg ;

  release( ppSvc ) ;

  // -- Open the file containing the spectrum
  TFile* file = TFile::Open( m_inputFileName.c_str(), "READ" );
  if( !file ){
    error() << "Could not find spectrum template file!" << endmsg;
    return StatusCode::FAILURE;
  }
  
  // -- Get the histogram template file for the particle momentum spectrum
  m_hist = (TH1*)file->Get( m_histoPath.c_str() );
  
  if( !m_hist ){
    error() << "Could not find spectrum histogram!" << endmsg;
    return StatusCode::FAILURE;
  }
	
	// -- Check the binning format specified
	// Make sure accept an empty binningVars variable for backwards-compatability;
	if(m_binningVars=="")	m_binningVars="pxpypz";
	if(m_binningVars!="ptpz" && m_binningVars!="pxpypz") {
		error() << "binningVariables set to unrecognised value: " << m_binningVars <<
							 ". Only \"pxpypz\" (default) or \"ptpz\" allowed)"   << endmsg;
	}

  info() << "Using file '" << m_inputFileName << "' with histogram '" 
	 << m_histoPath << "' for sampling the momentum spectrum" << endmsg;

  return sc ;
}

//===========================================================================
// Generate the particles
//===========================================================================
void MomentumSpectrum::generateParticle( Gaudi::LorentzVector & momentum , 
					 Gaudi::LorentzVector & origin , 
					 int & pdgId ) {  
  
  unsigned int currentType = 
    (unsigned int)( m_pdgCodes.size() * m_flatGenerator() );
  // protect against funnies
  if ( currentType >= m_pdgCodes.size() ) currentType = 0; 

  // -- Sample components of momentum according to template in histogram
  origin.SetCoordinates( 0. , 0. , 0. , 0. ) ;  
  // -- Cast m_hist to TH2D or TH3D depending on binning variables
	if ( m_binningVars == "pxpypz" ) {
		m_hist2d = 0;
		m_hist3d = (TH3D*) m_hist;
		// -- Sample components of momentum according to template in histogram
		double px(0), py(0), pz(0);
		m_hist3d->GetRandom3(px, py, pz);
		momentum.SetPx( px ) ; momentum.SetPy( py ) ; momentum.SetPz( pz ) ;
		momentum.SetE( std::sqrt( m_masses[currentType] * m_masses[currentType] +
					momentum.P2() ) ) ;
	}
	else if( m_binningVars == "ptpz" ) {
		m_hist2d = (TH2D*) m_hist;
		m_hist3d = 0;
		// -- Sample components of momentum according to template in histogram
		double pt(0), pz(0);
		m_hist2d->GetRandom2(pt,pz);
		// pick random azimuthal angle
		double phi = (-1.*Gaudi::Units::pi + m_flatGenerator() * Gaudi::Units::twopi) * Gaudi::Units::rad;
		momentum.SetPx( pt*cos(phi) );
		momentum.SetPy( pt*sin(phi) );
		momentum.SetPz( pz );
		momentum.SetE( std::sqrt( m_masses[currentType] * m_masses[currentType] +
					momentum.P2() ) ) ;
	}

                        
  pdgId = m_pdgCodes[ currentType ] ;                      
    
  debug() << " -> " << m_names[ currentType ] << endmsg 
          << "   P   = " << momentum << endmsg ;
}

