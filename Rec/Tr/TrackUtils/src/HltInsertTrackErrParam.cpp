// Include files 
#include <math.h>
#include "gsl/gsl_sys.h"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/construct.hpp"

// event
#include "Event/Track.h"

// local
#include "HltInsertTrackErrParam.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltInsertTrackErrParam
//
// 2007-07-16 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltInsertTrackErrParam )
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltInsertTrackErrParam::HltInsertTrackErrParam( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_newLocation(false)
{
  m_xParams.push_back(  0.0214*Gaudi::Units::mm );
  m_xParams.push_back( -0.000801*Gaudi::Units::mm);
  m_xParams.push_back(  0.0108*Gaudi::Units::mm);
  m_xParams.push_back( -0.00122*Gaudi::Units::mm );
  m_xParams.push_back(  0.0000547*Gaudi::Units::mm);
    
  m_yParams = m_xParams ; // y is equal to x by default

  declareProperty( "XParams",  m_xParams );
  declareProperty( "YParams",  m_yParams );
  declareProperty( "InputLocation" , m_inputLocation = "Undefined" );
  declareProperty( "OutputLocation" , m_outputLocation = "" );
  declareProperty( "KeepMomentumCovariance" , m_keepMomentum = true, "Keep Momentum terms of the covariance matrix" );

}
//=============================================================================
// Destructor
//=============================================================================
HltInsertTrackErrParam::~HltInsertTrackErrParam() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltInsertTrackErrParam::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  printParams( m_xParams, "X");
  printParams( m_yParams, "Y");

  m_newLocation = ( m_outputLocation!="" );

  info() << "Tracks will be taken from " << m_inputLocation << endmsg ;
  info() << " and stored in " ;
  if ( m_newLocation ) info() << m_outputLocation << endmsg ;
  else info() << "the same location" << endmsg ;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  
//=========================================================================
void HltInsertTrackErrParam::printParams( std::vector<double>& params, 
                                          std::string coord ) {
  info() << "Your Pt-dependent " << coord << " error parameterisation will be " 
         << endmsg ;
  int p = 0 ;
  info() << "Error on " << coord << " = " ;
  for ( std::vector<double>::const_iterator i = params.begin() ;
        i!=params.end() ; ++i){
    if ( (p>0) && (*i>0.) ) info() << " +" ;
    info() << *i ;
    if ( p>0 ) info() << "*pt^(-" << p << ")"  ;
    ++p;
  }
  info() << endmsg ;

  return ;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode HltInsertTrackErrParam::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  /// @todo Should use some HltAlgorithm method to get access to tracks anywhere

  LHCb::Tracks* tracks = getIfExists<LHCb::Tracks>(m_inputLocation);
  if ( NULL == tracks ){
    setFilterPassed(false);
    Warning("No tracks at "+m_inputLocation).ignore();
    return StatusCode::SUCCESS;
  }
  LHCb::Tracks* newTracks =  (  m_newLocation ?  new LHCb::Tracks() 
                                              : (LHCb::Tracks*)0    );

  if (msgLevel(MSG::DEBUG)) 
    debug() << "Loaded " << tracks->size() << " tracks"  << endmsg ;

  for (  LHCb::Tracks::iterator it = tracks->begin(); it != tracks->end(); ++it) {
    if (msgLevel(MSG::VERBOSE)) 
      verbose() << "Looping on track " << (*it)->key() << endmsg ;
    
    LHCb::Track* tk =  ( newTracks ? (*it)->clone() : *it );
    
    StatusCode sc = insertParamInTrack( tk );
    if (!sc) {
      Warning(" call to insertParamInTrack failed -- abandoning all new tracks").ignore();
      if (newTracks!=0) { 
        std::for_each(newTracks->begin(),
                      newTracks->end(),
                      boost::lambda::delete_ptr());
        newTracks->clear();
        
      }
      return sc;    
    } 
    if ( newTracks !=0 ) newTracks->insert(tk) ;
    
  }
  if ( newTracks!=0 ) put(newTracks,  m_outputLocation);

  return StatusCode::SUCCESS;
}
//=========================================================================
//  Loop over states
//=========================================================================
StatusCode HltInsertTrackErrParam::insertParamInTrack( LHCb::Track* tr )
{
  for ( std::vector< LHCb::State* >::const_iterator s = tr->states().begin() ;
        s != tr->states().end() ; ++s ){ // const
    LHCb::State::Location loc = (*s)->location() ;
    if (( loc == LHCb::State::ClosestToBeam ) || 
        ( loc == LHCb::State::FirstMeasurement ) ||
        ( loc == LHCb::State::EndVelo )){
      if ( msgLevel(MSG::DEBUG)) debug() << "State at " << loc << " " << (*s)->position().z() << endmsg ;
      /// @todo add counter later
      StatusCode sc = insertParamInState( tr->stateAt(loc) );
      if (!sc) return sc;
    }
  }
  return StatusCode::SUCCESS ;
}  
//=========================================================================
//  The actual thing
//=========================================================================
StatusCode HltInsertTrackErrParam::insertParamInState( LHCb::State* state ){
  
  if ( 0 == gsl_fcmp( state->qOverP() , 0 , 1.e-9 ) ){ // that needs a 1000 TeV track
    err() << "Track state at " << state->position() << " (" << state->location() 
          << ") has q/P = " << state->qOverP() << endmsg ;
    return StatusCode::FAILURE;
  }

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << state->position().x() << " " 
            << state->position().y() << " "
            << state->position().z() << " "
            << 1./ state->qOverP() 
            << endmsg;
    debug() << "Pt: " << state->pt() << endmsg;  
    debug() << "Existent matrix: \n" << state->covariance() << endmsg;
  }
  
  double invPt=1./(fabs(state->pt()/Gaudi::Units::GeV));
  
  if ( msgLevel( MSG::DEBUG ) ) debug() << "Inverse of Pt: " << invPt << endmsg;
  
  double sigmaX = 0 ;  
  for ( unsigned int p = 0 ; p<m_xParams.size() ; ++p ){
    sigmaX += m_xParams[p]*std::pow(invPt, static_cast<int>(p));}
  double sigmaY = 0 ;  
  for ( unsigned int p = 0 ; p<m_yParams.size() ; ++p ){
    sigmaY += m_yParams[p]*std::pow(invPt, static_cast<int>(p));}
    
  if ( msgLevel( MSG::DEBUG ) ) debug() << "sigmaX, sigmaY: " << sigmaX << " " << sigmaY << endmsg;
  
  Gaudi::TrackSymMatrix newMatrix = Gaudi::TrackSymMatrix() ;
  if ( m_keepMomentum ) newMatrix = state->covariance() ;
  
  newMatrix(0,0) = sigmaX*sigmaX;
  newMatrix(1,1) = sigmaY*sigmaY;
  newMatrix(1,0) = 0;   // xy term
  newMatrix(0,1) = 0;   // xy term

  /// @todo could be done with insert...
  if ( m_keepMomentum ) { // set all cros terms to 0
    newMatrix(0,2) = 0; newMatrix(0,3) = 0 ; newMatrix(0,4) ;// x - tx, ty, q/P
    newMatrix(1,2) = 0; newMatrix(1,3) = 0 ; newMatrix(1,4) ;// y - tx, ty, q/P
    newMatrix(2,0) = 0; newMatrix(3,0) = 0 ; newMatrix(4,0) ;// x - tx, ty, q/P
    newMatrix(2,1) = 0; newMatrix(3,1) = 0 ; newMatrix(4,1) ;// y - tx, ty, q/P
  } else { // keep diagonals
    newMatrix(2,2) = state->covariance()(2,2);
    newMatrix(3,3) = state->covariance()(3,3);
    newMatrix(4,4) = state->covariance()(4,4);
  }

  if ( msgLevel( MSG::DEBUG ) ) debug() << "New matrix: \n" << newMatrix << endmsg;
  
  state->setCovariance(newMatrix);

  return StatusCode::SUCCESS ;
}
