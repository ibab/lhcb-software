// Include files
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IChronoStatSvc.h"

// from GSL
#include "gsl/gsl_math.h"

// from DetDesc
#include "DetDesc/ITransportSvc.h"
#include "DetDesc/Material.h"

// from TrackEvent
#include "Event/TrackParameters.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtraSelector.h"

// Local 
#include "TrackMasterExtrapolator.h"

// Needed for the creation of TrackMasterExtrapolator objects.
static const ToolFactory<TrackMasterExtrapolator> s_factory;
const IToolFactory& TrackMasterExtrapolatorFactory = s_factory;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackMasterExtrapolator::TrackMasterExtrapolator( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : TrackExtrapolator(type, name, parent),
    m_tMax(10.),
    m_eMax(100.0*MeV),
    m_shortFieldExtrapolator(0),
    m_longFieldExtrapolator(0)
{
  //job options
 
  declareProperty( "ShortDist"     , m_shortDist = 100.*mm);
  declareProperty( "ShortFieldExtrapolatorName",
                   m_shortFieldExtrapolatorName = "TrackParabolicExtrapolator" );
  declareProperty( "LongFieldExtrapolatorName",
                   m_longFieldExtrapolatorName = "TrackHerabExtrapolator" );
  declareProperty( "ExtraSelectorName",
                   m_extraSelectorName = "TrackDistanceExtraSelector" );
  declareProperty( "ApplyMultScattCorr"  , m_applyMultScattCorr  = true );
  declareProperty( "Fms2"                , m_fms2                = 1.0 );
  declareProperty( "ThickWall"           , m_thickWall           = 0.*mm );
  declareProperty( "ApplyEnergyLossCorr" , m_applyEnergyLossCorr = true );
  declareProperty( "EnergyLoss"          , 
                   m_energyLoss = 354.1 * MeV*mm2/mole );
  declareProperty( "MaxStepSize"         , m_maxStepSize         = 1000.*mm );
  declareProperty( "MinRadThreshold"     , m_minRadThreshold     = 1.0e-4 );
  //for electrons
  declareProperty( "ApplyElectronEnergyLossCorr",
                   m_applyElectronEnergyLossCorr = true );
  declareProperty( "StartElectronCorr",
                   m_startElectronCorr = 2500.*mm );
  declareProperty( "StopElectronCorr",
                   m_stopElectronCorr = 9000.*mm );
}

//=============================================================================
// Destructor
//=============================================================================
TrackMasterExtrapolator::~TrackMasterExtrapolator() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode TrackMasterExtrapolator::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize", sc );
 
  // request a short distance magnetic field extrapolator
  m_shortFieldExtrapolator = tool<ITrackExtrapolator>( m_shortFieldExtrapolatorName );

  // request extrapolator for going short distances in magnetic field
  m_longFieldExtrapolator = tool<ITrackExtrapolator>( m_longFieldExtrapolatorName );

  // selector
  m_extraSelector = tool<ITrackExtraSelector>( m_extraSelectorName ); // ,"selector", this);

  // initialize transport service
  m_transportSvc = svc<ITransportSvc>( "TransportSvc",true );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Main method: Extrapolate a State
//=========================================================================
StatusCode TrackMasterExtrapolator::propagate( State& state, 
                                               double zNew,
                                               ParticleID partId )
{
  StatusCode sc;

  // reset transport matrix
  //TODO: remove the work-around as soon as MathCore has something to replace
  //      m_F = HepMatrix(5, 5, 1);
  m_F = TransportMatrix();
  for ( unsigned int ind = 0; ind < 5; ++ind ) m_F(ind,ind) = 1.;

  //check not already at current
  double zStart = state.z();
  if (fabs(zNew-zStart) < TrackParameters::hiTolerance)
    {
      // already at required z position
      debug() << "already at required z position" << endreq;
      return StatusCode::SUCCESS;
    }

  // check whether upstream or downstream
  zStart > zNew ? m_upStream = true : m_upStream = false; 

  int    nbStep = (int)( fabs( zNew-zStart ) / m_maxStepSize ) + 1;
  double zStep  = ( zNew - zStart ) / nbStep;
  int    nWall;
  double maxSlope      = 5.;
  double maxTransverse = 10 * m;
  
  for ( int step=0 ; nbStep > step ; ++step )
    {
      ILVolume::Intersections intersept;
      TrackVector& tX = state.stateVector();
      XYZPoint start( tX[0], tX[1], state.z() );  // Initial position
      XYZVector vect( tX[2]*zStep, tX[3]*zStep, zStep );

      // protect against vertical or looping tracks
      if ( fabs(tX[0]) > maxTransverse )
	{
	  double pos = GSL_SIGN(tX[0])* maxTransverse ;
	  warning() << "Protect against absurd tracks: X " 
              << state.x() << " set to " << pos << ", abort. " << endreq;
	  state.setX( pos );
	  return StatusCode::FAILURE;
	}
    
      if ( fabs(tX[1]) > maxTransverse )
	{
	  double pos = GSL_SIGN(tX[1])*maxTransverse;
	  warning() << "Protect against absurd tracks: Y " 
              << state.y() << " set to " << pos << ", abort. " << endreq;
	  state.setY(pos);
	  return StatusCode::FAILURE;
	}

      if (fabs(state.tx()) > maxSlope)
	{
	  double slopeX = GSL_SIGN(state.tx())* maxSlope;
	  warning() << "Protect against looping tracks: Tx " 
              << state.tx() << " set to " << slopeX << ", abort. " << endreq;
	  state.setTx(slopeX);
	  return StatusCode::FAILURE;
	}
    
      if (fabs(state.ty()) > maxSlope)
	{
	  double slopeY = GSL_SIGN(state.ty())*maxSlope;
	  warning() << "Protect against looping tracks: Ty " 
              << state.ty() << " set to " << slopeY << ", abort. " << endreq;
	  state.setTy(slopeY);
	  return StatusCode::FAILURE;
	}

      // short cut if inside the same plane of the OT stations
      if ( (7.0*mm > fabs(zStep)) && (5000.0*mm < start.z() ) )
	{
	  nWall = 0;
	  debug() << "No transport between z= " << start.z() << " and " 
		  << start.z() + vect.z() << endreq;
	} 
      // check if transport is within LHCb
      else if (fabs(start.x()) > 25.*m || fabs(start.y()) > 25.*m ||
	       fabs(start.z()) > 25.*m ||
	       fabs(start.x()+vect.x()) > 25.*m ||
	       fabs(start.y()+vect.y()) > 25.*m ||
	       fabs(start.z()+vect.z()) > 25.*m )
	{
	  nWall = 0;
	  debug() << "No transport between z= " << start.z() << " and " 
		  << start.z() + vect.z() 
		  << ", since it reaches outside LHCb" << endreq;
	} 
      else
	{
	chronoSvc()->chronoStart("TransportSvcT");
	nWall = m_transportSvc->intersections( start, vect, 0., 1., 
                                         intersept, m_minRadThreshold );
	chronoSvc()->chronoStop("TransportSvcT");
	  
      }
  
      // local to global transformation
      transformToGlobal(zStep,start.z(),intersept);
 
      // add virtual wall at target
      ILVolume::Interval inter(start.z() + zStep, start.z() + zStep);
      const Material* dummyMat = 0;
      intersept.push_back(std::make_pair(inter,dummyMat));
      nWall = intersept.size();      
 
      // loop over the walls - last wall is `virtual one at target z'
      for (int iStep=0; iStep<nWall; ++iStep){

        double zWall = zScatter(intersept[iStep].first.first,intersept[iStep].first.second);
        ITrackExtrapolator* thisExtrapolator = m_extraSelector->select(state.z(), zWall);      
	sc = thisExtrapolator->propagate( state, zWall );
     
        // check for success
	if (sc.isFailure()){
	  warning() << "Transport to " << zWall
              << "using "+thisExtrapolator->name() << " FAILED" << endreq;
	}
	 
	//update f
	updateTransportMatrix( thisExtrapolator->transportMatrix() );
        
      
	// protect against vertical or looping tracks
	if (fabs(state.tx()) > maxSlope){
	  double slopeX = GSL_SIGN(  state.tx() )*maxSlope;
	  warning() << "Protect against looping tracks: Tx " 
              << state.tx() << " set to " << slopeX << ", abort. " << endreq;
	  state.setTx(slopeX);
	  return StatusCode::FAILURE;
	}
      
	if (fabs(state.ty()) > maxSlope){
	   double slopeY = GSL_SIGN(state.ty())*maxSlope;
	   warning() << "Protect against looping tracks: Ty " 
               << state.ty() << " set to " << slopeY << ", abort. " << endreq;
	    state.setTy(slopeY);
	    return StatusCode::FAILURE;
	}
      
        // number we need
        double tWall = fabs(intersept[iStep].first.first - intersept[iStep].first.second);
        const Material* theMaterial = intersept[iStep].second;

	// multiple scattering
	if ((m_applyMultScattCorr == true)&& (0 !=  theMaterial )){
	   if (tWall < m_thickWall){
	     sc = thinScatter( state, tWall/theMaterial->radiationLength() );
	    }
	   else{
	     sc = thickScatter( state, tWall, tWall/theMaterial->radiationLength()  );
	   }
	 }
      
	 // dE_dx energy loss
	 if (( m_applyEnergyLossCorr == true)&&(theMaterial != 0)){
	   sc = energyLoss( state, tWall, theMaterial );
	 }
      
	 // electron energy loss
	 if ((m_applyElectronEnergyLossCorr == true) 
            && (11 == partId.abspid())
            && (theMaterial != 0) ){
	   if ((state.z() > m_startElectronCorr) &&
	     (state.z() < m_stopElectronCorr)){
	     sc = electronEnergyLoss( state, theMaterial->radiationLength() );
	 }
       }
      
    } // loop over walls
    } // loop over steps

  verbose() << "State extrapolated succesfully" << endreq;

  return StatusCode::SUCCESS;
}

//============================================================================
// apply thick scatter Q/p state
//============================================================================
StatusCode TrackMasterExtrapolator::thinScatter( State& state,
                                                 double radLength )
{
  // apply multiple scattering - thin scatter

  double scatLength = 0.;
  double norm2 = 1.0+gsl_pow_2(state.tx())+gsl_pow_2(state.ty());

  if (radLength >TrackParameters::lowTolerance)
    {
      double radThick = sqrt(norm2)*radLength;
      //assumed GEV
      scatLength = radThick*gsl_pow_2(TrackParameters::moliereFactor*
				      (1.+0.038*log(radThick)));
    }

  // protect 0 momentum
  double p = GSL_MAX(state.p(),1.0*MeV);

  double cnoise = m_fms2 * scatLength/gsl_pow_2(p);

  // multiple scattering covariance matrix - initialized to 0
  TrackMatrix Q = TrackMatrix();

  Q(2,2) = norm2 * (1. + gsl_pow_2(state.tx())) * cnoise;
  Q(3,3) = norm2 * (1. + gsl_pow_2(state.ty())) * cnoise;
  Q(3,2) = norm2 * state.tx() * state.ty() * cnoise;

  // update covariance matrix C = C + Q
  TrackMatrix& tC = state.covariance();
  tC += Q;

  return StatusCode::SUCCESS;

}

//============================================================================
// apply thick scatter Q/p state
//============================================================================
StatusCode TrackMasterExtrapolator::thickScatter( State& state,
                                                  double tWall,
                                                  double radLength )
{
  // apply - thick scatter multiple scattering
  double scatLength = 0.;
  double norm2 = 1.0 + gsl_pow_2(state.tx()) + gsl_pow_2(state.ty());
  if (radLength > TrackParameters::lowTolerance)
    {
      double radThick = sqrt(norm2)*radLength;
      //assumed GEV
      scatLength = radThick * gsl_pow_2(TrackParameters::moliereFactor *
					(1. + 0.038*log(radThick)));
    }

  // protect zero momentum
  double p = GSL_MAX( state.p(), 1.0 * MeV );
  double cnoise = m_fms2 * scatLength / (p*p);

  // slope covariances
  double covTxTx = norm2 * (1. + gsl_pow_2(state.tx())) * cnoise;
  double covTyTy = norm2 * (1. + gsl_pow_2(state.ty())) * cnoise;
  double covTxTy = norm2 * state.tx() * state.ty() * cnoise;

  //D - depends on whether up or downstream
  double D = (m_upStream) ? -1. : 1. ;

  TrackMatrix Q = TrackMatrix();

  double tWall2 = gsl_pow_2(tWall);

  Q(0,0) = covTxTx * tWall2 / 3.;
  Q(1,0) = covTxTy * tWall2 / 3.;
  Q(2,0) = 0.5*covTxTx * D * tWall;
  Q(3,0) = 0.5*covTxTy * D * tWall;

  Q(1,1) = covTyTy * tWall2 / 3.;
  Q(2,1) = 0.5*covTxTy * D * tWall;
  Q(3,1) = 0.5*covTyTy * D * tWall;

  Q(2,2) = covTxTx;
  Q(3,2) = covTxTy;

  Q(3,3) = covTyTy;

  // update covariance matrix C = C + Q
  TrackMatrix& tC = state.covariance();
  tC += Q;

  return StatusCode::SUCCESS;

}

//============================================================================
// apply energy loss P state
//============================================================================
StatusCode TrackMasterExtrapolator::energyLoss( State& state,
                                                double tWall,
                                                const Material* aMaterial )
{
  // Apply correction for dE/dx energy loss (Bethe-Block)
  double norm = sqrt(1.0+gsl_pow_2(state.tx())+gsl_pow_2(state.ty()));
  double bbLoss = tWall * norm * aMaterial->density() * m_energyLoss * 
    aMaterial->Z() / aMaterial->A();
  bbLoss = GSL_MIN(m_eMax, bbLoss);
  if (m_upStream == false) { bbLoss *= -1.0; }

  // apply correction - note for now only correct the state vector
  TrackVector& tX = state.stateVector();

  if (tX[4]>0.) { tX[4] = 1./(1./tX[4]+(bbLoss)); }
  else { tX[4] = 1./(1./tX[4]-(bbLoss)); }

  return StatusCode::SUCCESS;

}

//============================================================================
// electron energy loss Q/p state
//============================================================================
StatusCode TrackMasterExtrapolator::electronEnergyLoss( State& state,
                                                        double radLength )
{
  //hard energy loss for electrons

  double t;
  double norm = sqrt(1.0+gsl_pow_2(state.tx())+gsl_pow_2(state.ty()));

  m_upStream == true ?  t = radLength*norm : t = -radLength*norm;

  // protect against t too big
  if (fabs(t)>m_tMax) { t = GSL_SIGN(t)*m_tMax; }

  // apply correction
  TrackVector& tX = state.stateVector();
  TrackMatrix& tC = state.covariance();

  tC(4,4) += gsl_pow_2(tX[4]) * (exp(-t*log(3.0)/log(2.0))-exp(-2.0*t));
  tX[4] *= exp(-t);

  return StatusCode::SUCCESS;
}

//=============================================================================
//
//=============================================================================
double TrackMasterExtrapolator::zScatter( const double z1, 
                                          const double z2 ) const
{
  double zS;
  if (fabs(z1-z2) <= m_thickWall){

    // thin scatter - at centre in z
    zS = 0.5*(z1+z2); 
  }
  else
    {
      // thick scatter
      if (m_upStream == true) {  
        zS = GSL_MIN(z1,z2); }
      else {  
        zS = GSL_MAX(z1,z2); }
    }
  return zS;
}

//=============================================================================
