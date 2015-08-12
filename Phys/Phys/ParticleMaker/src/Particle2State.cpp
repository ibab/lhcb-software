// $Id: Particle2State.cpp,v 1.3 2009-09-11 17:14:21 jonrob Exp $
// Include files

// LHCb
#include "GaudiKernel/GenericMatrixTypes.h"
#include "LHCbMath/MatrixManip.h"
#include "LHCbMath/MatrixTransforms.h"
#include "LHCbMath/Similarity.h"

// local
#include "Particle2State.h"


namespace Gaudi {
  typedef ROOT::Math::SMatrix<double, 2, 3> Matrix2x3;
  typedef ROOT::Math::SMatrix<double, 3, 2> Matrix3x2;
}
//-----------------------------------------------------------------------------
// Implementation file for class : Particle2State
//
// 2006-05-16 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Particle2State )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2State::Particle2State( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IParticle2State>(this);
}

//=============================================================================
// Destructor
//=============================================================================
Particle2State::~Particle2State() {}

//=============================================================================
//=============================================================================
/// Fill Particle from a state
//=============================================================================
StatusCode Particle2State::state2Particle( const LHCb::State& state,
                                           LHCb::Particle& particle ) const 
{
  // point on the track and error
  particle.setReferencePoint( state.position() ) ;

  // momentum
  const Gaudi::XYZVector mom = state.momentum();
  const double mass = particle.measuredMass();
  const double p = mom.R() ;
  const double e = std::sqrt( p*p + mass*mass );
  particle.setMomentum( Gaudi::XYZTVector(mom.X(),mom.Y(),mom.Z(),e) ) ;

  // get the jacobian for dp4/d(tx,ty,qop)
  Gaudi::Matrix4x3 dP4dMom;
  Gaudi::Math::JacobdP4dMom(state.stateVector().Sub<Gaudi::Vector3>(2),mass,dP4dMom);

  // now fill the total jacobia, but only nonzero elements
  // TODO/FIXME: utilize the fact that the top 2 rows/columns are diagonal...
  ROOT::Math::SMatrix<double,7,5> Jacob ;
  Jacob(0,0) = Jacob(1,1) = 1 ; 
  Jacob.Place_at(dP4dMom,3,2) ;
  const Gaudi::SymMatrix7x7 cov = LHCb::Math::Similarity( Jacob, state.covariance() );

  // CRJ : const casts used here to allow direct manipulation of the Particle matrices
  //       avoiding the need for local temporaries. Ugly but speeds up the HLT so close
  //       our eyes and live with it ...

  // error on position
  Gaudi::SymMatrix3x3& errPos = *(const_cast<Gaudi::SymMatrix3x3*>(&(particle.posCovMatrix())));
  errPos.Place_at(cov.Sub<Gaudi::SymMatrix3x3>( 0, 0 ), 0, 0 );

  // error on momentum
  Gaudi::SymMatrix4x4& errMom = *(const_cast<Gaudi::SymMatrix4x4*>(&(particle.momCovMatrix())));
  errMom.Place_at(cov.Sub<Gaudi::SymMatrix4x4>( 3, 3 ), 0, 0 );

  // correlation
  Gaudi::Matrix4x3& errPosMom = *(const_cast<Gaudi::Matrix4x3*>(&(particle.posMomCovMatrix())));
  errPosMom.Place_at(cov.Sub<Gaudi::Matrix4x3>( 3, 0 ), 0, 0 );

  if ( msgLevel(MSG::VERBOSE) )
  { 
    verbose() << "New particle " << particle.particleID().pid() << endmsg ;
    verbose() << "    Ref Point is " << particle.referencePoint() << endmsg ;  
    verbose() << "    Momentum is " << particle.momentum() << endmsg ;
    verbose() << "    E=  " << e << " q/P = " << state.qOverP() << endmsg ;
    verbose() << "    Jacob 7 is : \n" << Jacob << endmsg ;
    verbose() << "    state.covariance() is \n" << state.covariance() << endmsg ;
    verbose() << "    Gets cov \n" << cov << endmsg ;
    verbose() << "    Position Covariance Matrix is \n"
              << particle.posCovMatrix() << endmsg ;
    verbose() << "    Momentum Covariance Marix is \n"
              << particle.momCovMatrix() << endmsg ;
    verbose() << "    Position x Momentum Covariance Matrix is \n"
              << particle.posMomCovMatrix() << endmsg ;
  }

  return StatusCode::SUCCESS ;

}
//=============================================================================
// make a state from a Particle
//=============================================================================
StatusCode Particle2State::particle2State( const LHCb::Particle& part,
                                           LHCb::State& s ) const 
{

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Making a state for " << part.key() << " a "
              << part.particleID().pid() << endmsg ;
    verbose() << "Particle " << part << "\n  slopes " << part.slopes()
              << " Q/p " << part.charge()/part.p() << endmsg;
  }

  if (0 == part.charge()) {
    return Error("Neutral particle given as input to particle2State");
  }

  s.setState(part.referencePoint().X(),
             part.referencePoint().Y(),
             part.referencePoint().Z(),
             part.slopes().X(),
             part.slopes().Y(),
             part.charge()/part.p());

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Produced state at pos " << s.position()
            << " mom " << s.momentum() << " slopes "
            << s.slopes() << " p() " << s.p()<< endmsg ;

  Gaudi::Matrix5x5 cov5Dtmp ;

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Placing \n" << part.covMatrix() << "\n"<< endmsg ;

  // 2x2 for x and y
  cov5Dtmp.Place_at( part.covMatrix().Sub<Gaudi::SymMatrix2x2>( 0, 0 ), 0, 0 );

  // 2x3 for pos and mom
  cov5Dtmp.Place_at( part.covMatrix().Sub<Gaudi::Matrix3x2>( 3, 0 ), 2, 0 );

  // 2x3 for pos and mom
  cov5Dtmp.Place_at( part.covMatrix().Sub<Gaudi::Matrix2x3>( 0, 3 ), 0, 2 );

  // 3x3 for pos
  cov5Dtmp.Place_at( part.covMatrix().Sub<Gaudi::SymMatrix3x3>( 3, 3 ), 2, 2 );

  const Gaudi::SymMatrix5x5 cov5DtmpSym = Gaudi::Math::Symmetrize(cov5Dtmp);

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Getting \n" << cov5DtmpSym << "\n"<< endmsg ;
  // 5x5 Jacobian
  Gaudi::Matrix5x5 Jacob;
  stateJacobian( part.charge(), part.momentum().Vect(), Jacob );

  const Gaudi::SymMatrix5x5 cov = ROOT::Math::Similarity(Jacob, cov5DtmpSym);

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Covariance after similarity \n" << cov << "\n"<< endmsg ;

  s.setCovariance(cov);

  return StatusCode::SUCCESS;
}
//=============================================================================
/// Get jacobian
//=============================================================================
void Particle2State::stateJacobian( const int charge,
                                    const Gaudi::XYZVector& Momentum,
                                    Gaudi::Matrix5x5& Jacob ) const
{
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Making Jacobian with " << charge << " " << Momentum << endmsg ;

  // the variables
  const double Px = Momentum.X() ;
  const double Py = Momentum.Y() ;
  const double Pz = Momentum.Z() ;
  const double P  = Momentum.R() ;

  // const double Tx = Momentum.X()/Momentum.Z();   // Tx = Px/Pz
  // const double Ty = Momentum.Y()/Momentum.Z();   // Ty = Py/Pz
  // const double o  = part->charge()/part->p();                    // o = charge/sqrt(Px^2+Py^2+Pz^2)

  // unit matrix in the top left corner
  Jacob(0,0) = 1. ;                     //  dx/dy
  Jacob(1,1) = 1. ;                     //  dy/dy
  // non unit matrix in the bottom right corner
  Jacob(2,2) = 1/Pz ;                   //  dTx/dPx = 1/Pz
  Jacob(2,3) = 0. ;                     //  dTx/dPy = 0.
  Jacob(2,4) = -Px/(Pz*Pz) ;            //  dTx/dPz = -Px/Pz^2
  Jacob(3,2) = 0. ;                     //  dTy/dPx = 0.
  Jacob(3,3) = 1/Pz ;                   //  dTy/dPy = 1/Pz
  Jacob(3,4) = -Py/(Pz*Pz) ;            //  dTy/dPz = -Py/Pz^2
  Jacob(4,2) = -charge*Px/(P*P*P) ; //  do/dPx  = -charge*Px/P^3
  Jacob(4,3) = -charge*Py/(P*P*P) ; //  do/dPy  = -charge*Py/P^3
  Jacob(4,4) = -charge*Pz/(P*P*P) ; //  do/dPz  = -charge*Pz/P^3

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Returning Jacobian\n" << Jacob << endmsg ;
}

//=============================================================================
/// Test the tool
//=============================================================================
StatusCode Particle2State::test(const LHCb::Particle& p ) const 
{
  info() << "Testing with Particle \n" << p << "\n" << endmsg;
  LHCb::State s ;
  StatusCode sc = particle2State( p, s );
  if (!sc) return sc;
  info() << "Got State \n" << s << "\n" << endmsg;
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "This does not work: \n" << s.posMomCovariance() << "\n"<< endmsg ;
  LHCb::Particle newPart = LHCb::Particle(p);
  sc = state2Particle( s, newPart );
  if (!sc) return sc;
  info() << "And Particle \n" << newPart << "\n" << endmsg;
  return sc;
}

//=============================================================================
/// Test the tool
//=============================================================================
StatusCode Particle2State::test(const LHCb::State& s ) const 
{
  info() << "Testing with State \n" << s << "\n" << endmsg;
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "This does not work: \n" << s.posMomCovariance() << "\n"<< endmsg ;
  int pid = 211 ;
  if (s.qOverP()<0.) pid=-pid;
  LHCb::Particle p = LHCb::Particle(LHCb::ParticleID(pid)) ;
  StatusCode sc = state2Particle( s, p );
  if (!sc) return sc;
  info() << "Got Particle \n" << p << "\n" << endmsg;
  LHCb::State newState = LHCb::State(s);
  sc = particle2State( p, newState );
  if (!sc) return sc;
  info() << "And State \n" << newState << "\n" << endmsg;
  return sc;
}
