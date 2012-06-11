// $Id: $
// Header file
#include "EvtGenInPhSpDecay.h"
//BOOST
#include "boost/filesystem.hpp"
//GaudiKernel
#include "Kernel/ParticleID.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"

// from Gaudi
#include "GaudiKernel/System.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmGen.h"
#include "GaudiKernel/GaudiException.h"

//STL
#include <iostream>
#include <fstream>

// from Event
#include "Event/HepMCEvent.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"
 
// from EvtGen
#include "EvtGen/EvtGen.hh"
#include "EvtGenModels/EvtPythia.hh"
#include "EvtGenBase/EvtParticleFactory.hh"
#include "EvtGenBase/EvtDecayTable.hh"
#include "EvtGenBase/EvtDecayBase.hh"

// from EvtGenExtras
#include "EvtGenModels/EvtModelRegExtras.hh"

#include "Generators/StreamForGenerator.h"
#include "Generators/IProductionTool.h"

// Calls to FORTRAN routines
#ifdef WIN32
extern "C" {
  void __stdcall PHOTOS_INIT( const char* filename, int length ) ;
  void __stdcall PYTHIAOUTPUT_INIT( int * ) ;
  void __stdcall PHOTOS_END () ;
}
#else
extern "C" {
  void photos_init__( const char* filename , int length ) ;
  void pythiaoutput_init__( int * ) ;
  void photos_end__()  ;
}
#endif


//-----------------------------------------------------------------------------
// Implementation file for class : EvtGenInPhSpDecay
//
// 2011-08-16 : Alex Shires
//-----------------------------------------------------------------------------
// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( EvtGenInPhSpDecay );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EvtGenInPhSpDecay::EvtGenInPhSpDecay( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : EvtGenDecay ( type, name , parent ),
    // name of q2 parameter file
    m_q2mapfile ( "" )  ,
    // statistics
    m_tot_calls ( 0.0 ) ,
    m_num_evts ( 0.0 ) ,
    m_ave_eff ( 0.0 ) 
{
    // Declare IEvtGenInPhSpDecay interface
    declareInterface<IDecayTool>( this ) ;
    //number of bins for q2
    declareProperty("NumQ2Bins" , m_q2nbins = 100 ) ;
    declareProperty("MaxQ2Bins" , m_q2nmax  = 19 ) ;
    declareProperty("MaxEvtGenCalls" , m_limit  = 1000 ) ;
    declareProperty("ParamFile", m_prefix = "Q2Param" ) ;
}
//=============================================================================
// Destructor
//=============================================================================
EvtGenInPhSpDecay::~EvtGenInPhSpDecay( ) { }
//=============================================================================
// Initialize method
//=============================================================================
StatusCode EvtGenInPhSpDecay::initialize( ) {
  StatusCode sc = EvtGenDecay::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  std::stringstream ssfilename ;
  ssfilename << m_prefix << "_" << m_q2nbins << "_" << m_q2nmax << ".txt" ;
  m_q2mapfilename = ssfilename.str() ;
  //do the same for the Q2 binning file
  if ( "UNKNOWN" != System::getEnv("DECFILESROOT") ) 
    m_q2mapfile  = System::getEnv( "DECFILESROOT" ) + 
      "/EvtData/" + m_q2mapfilename ;
  else   return Error( "DECFILESROOT is UNKNOWN" ) ;
 
  if ( msgLevel ( MSG::INFO) )  info() << "Parameter file is "<< m_q2mapfile << endmsg;

  // Check if file exists:
  if ( ! boost::filesystem::exists( m_q2mapfile ) ) 
    return Error( "The specified parameter file does not exist" ) ;

  m_q2mapvec.reserve( m_q2nbins ) ;
  //load map
  loadMap();
  
  if ( msgLevel ( MSG::DEBUG ) )  debug() << "EvtGenInPhSpDecay initialized" << endmsg ;
  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Finalize method
//=============================================================================
StatusCode EvtGenInPhSpDecay::finalize() {
  
  if ( msgLevel ( MSG::DEBUG ) )   debug() << "EvtGenInPhSpDecay finalized" << endmsg ;

  boost::filesystem::remove( m_q2mapfilename ) ;

  //if ( msgLevel ( MSG::INFO ) ) {
  //    info() << "Total calls to EvtGen:   " << m_tot_calls << endmsg ;
  //    info() << "Average number of calls: "<<  m_tot_calls / m_num_evts << endmsg;
  //    info() << "Average efficiency:      " << m_ave_eff / m_num_evts  << endmsg;
  //}
  return EvtGenDecay::finalize( ) ;
}

//=============================================================================
// load q2 parameters from file
//=============================================================================

StatusCode EvtGenInPhSpDecay::loadMap( ) {
    StatusCode sc = StatusCode::SUCCESS ;
    //check whether file exists
    std::ifstream infile ( m_q2mapfile.string().c_str() ) ;
    //
    if ( ! infile.good() ) {
        error() << "parameter file " << m_q2mapfile << " failed to open" << endmsg ;
        return StatusCode::FAILURE ;
    }
    else if ( msgLevel( MSG::DEBUG ) ) {
        debug() << "parameter file " << m_q2mapfile << " opened" << endmsg ;
    }
    m_q2mapvec.reserve(m_q2nbins);
    std::string line ;
    double q2val, dummy;
    std::stringstream ssline ;
    while ( infile.good() ) {
        //read line
        getline( infile , line ) ;
        ssline.str( line ) ;
        ssline >> dummy >> q2val;
        //info() << "line: " << line << " q2: " << dummy << " value: " << q2val << endmsg ;
        if ( msgLevel( MSG::DEBUG ) ) debug() << "line: " << line <<" q2 " << dummy <<" val "<< q2val << endmsg ;
        m_q2mapvec.push_back( q2val ) ;  
        ssline.clear() ;
    }
    infile.close() ;
    return sc;
}

//=============================================================================
// Generate with EvtGen until conditions are satisfied
//=============================================================================
StatusCode EvtGenInPhSpDecay::doEvtGenLoop( EvtParticle* & part , 
                                            HepMC::GenParticle * &theMother ,
                                            const EvtId &pid ) const
{
    //loop to re-decay particle until phase space conditions are satisfied
    //
    double qsq ( 0.0 ) , passval ( 0.0 ) , random ( 0.0 ) ;
    double delta ( 20.0 / m_q2nbins ) ;
    int bin ( 0 ) , ncalls ( 0 ) , pdgid ( 0 ) ;
    bool bmum, bmup ;
    EvtVector4R mum (0,0,0,0) , mup (0,0,0,0) ;
    EvtParticle * tmppart ( 0 ) ;
    bool pass = false ;
    if ( msgLevel ( MSG::DEBUG ) ) debug() << "Starting EvtGen do-while loop " << endmsg ;
    do {
        //generateDecay
        StatusCode sc = callEvtGen( part , theMother , pid ) ;
        if ( ! sc.isSuccess( ) ) {
            part->deleteTree( ) ;
            return sc ;
        }
        pass = sc.isFailure( ) ;
        ++ncalls ;  
        //check for B
        if ( abs( EvtPDL::getStdHep( part -> getId() ) ) != 511 ) return sc ; 
        //check for two oscillations - else use particle - must be a better way of doing this
        tmppart = part ;
        while ( tmppart -> getNDaug() == 1 ) {
            tmppart = tmppart -> getDaug( 0 ) ; 
        }
        //loop over daughters, find muminus and mu plus
        bmum = false ;
        bmup = false ;
        for ( unsigned int it = 0 ; it < tmppart -> getNDaug() ; ++it ) {
            pdgid = EvtPDL::getStdHep( tmppart -> getDaug( it ) -> getId() ) ;
            // go for daughter index
            if ( pdgid == 13) { 
                mup = tmppart -> getDaug( it ) -> getP4Lab() ;
                bmup = true ;
            }
            else if (pdgid == -13 ) {
                mum = tmppart -> getDaug( it ) -> getP4Lab() ;
                bmum = true ;
            }
        }
        if (!bmum && !bmup ) {
            error() << "DecayTool used for B decay without muons" << endmsg ;
            return sc ;
        }
        // qsq is invariant mass of mum + mup
        qsq = ( mum + mup ) . mass2 ( ) ;
        //calculate bin
        bin = qsq / delta ;
        //get value
        passval = 1 ;
        if ( bin < m_q2nbins ) {
            passval = m_q2mapvec[bin] ; 
        }
        random  =  m_randomEngine->random() ;
        //compare with rando numberm
        pass = ( random < passval ) ;
        if ( msgLevel ( MSG::DEBUG ) ) {
            debug() << "Pass: " << pass << " Q2: " << qsq << " Bin: "<<bin ;
            debug() << " Value: " << passval << " Random: " << random << endmsg ;
        }
        //limit - keep particle
        if (ncalls > m_limit ) { 
            warning() << "too many calls to EvtGen " << endmsg ;
            pass = true ;
        }
        if ( !pass ) {
            part -> deleteTree( ) ;
            part = 0 ;
        }
// */
    } while ( !pass ) ;
    if ( msgLevel ( MSG::DEBUG ) ) {
        debug() << "End of EvtGen loop " << endmsg;
        debug() << "Number of calls: " << ncalls << endmsg;
    }
    return StatusCode::SUCCESS ; 
}

//=============================================================================
// Generate a Decay tree from a particle theMother in the event theEvent
//=============================================================================
StatusCode EvtGenInPhSpDecay::generateSignalDecay( HepMC::GenParticle * theMother ,
                                             bool & flip) const {
  // If particle already has daughters, return now
  if ( 0 != theMother -> end_vertex() ) {
    flip = false ;
    return StatusCode::SUCCESS ;
  }
  
  // Call EvtGen for the particle to generate
  checkParticle( theMother ) ;

  // get signal alias
  EvtId decayId = getSignalAlias( theMother -> pdg_id() ) ;

  EvtParticle * part( 0 ) ;
  //StatusCode sc = callEvtGen( part , theMother , decayId ) ;
  StatusCode sc = doEvtGenLoop( part , theMother , decayId ) ;
  if ( ! sc.isSuccess( ) ) return sc ;

  // Update HepMCEvent theEvent and HepMCParticle theMother

  // sets PDG Id of theMother
  // because EvtGen might have asked to change the original one
  // for CP modes (if flip is enabled)
  theMother -> set_pdg_id( EvtPDL::getStdHep( part->getId() ) ) ;
  flip = ( decayId != part -> getId() ) ;

  // Get reference position in space and time to be able to assign
  // correct vertex for daughter particles of theMother
  // because EvtGen gives position with respect to the "root" particle
  // This reference position is production vertex of theMother
  HepMC::GenVertex * PV = theMother -> production_vertex() ;
  Gaudi::LorentzVector theOriginPosition( PV -> position() ) ;
  
  // Fill HepMC event theEvent with EvtGen decay tree part
  // starting from theMother
  makeHepMC( part , theMother , theOriginPosition , -999 ) ;

  // delete EvtGen particle and all its daughters
  part -> deleteTree ( ) ;

  // Set status to "signal in lab frame"
  theMother -> set_status( LHCb::HepMCEvent::SignalInLabFrame ) ;

  return StatusCode::SUCCESS ;
}

