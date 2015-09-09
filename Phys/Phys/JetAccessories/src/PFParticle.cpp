// Include files
#include "Kernel/PFParticle.h"


#include "GaudiKernel/GenericMatrixTypes.h"
#include "LHCbMath/MatrixManip.h"
#include "LHCbMath/MatrixTransforms.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PFParticle
//
// 2012-07-25 : Victor Coco
//-----------------------------------------------------------------------------


//=============================================================================
// Constructor for neutral hadron particles
//=============================================================================
/*
   LHCb::PFParticle::PFParticle( const std::vector<const LHCb::CaloCluster*>& clusters ,
   const LHCb::ProtoParticle * pp ) : Particle() {
   double x(0.),y(0.),z(0.),e(0.);
   for (std::vector< const LHCb::CaloCluster*>::const_iterator iclu = clusters.begin();
   clusters.end() != iclu ; ++iclu )
   {
   if ((*iclu)->seed().calo() == 2){
   x  =  (*iclu)->position().x();
   y  =  (*iclu)->position().y();
   z  =  (*iclu)->position().z();
   e +=  (*iclu)->e();
   }
   else if ((*iclu)->seed().calo() == 3){
//this->addInfo(PFParticle::NSatHCAL,(*iclu)->nSat());
//this->addHCALCluster((*iclu));
// Get the position
x  =  (*iclu)->position().x();
y  =  (*iclu)->position().y();
z  =  (*iclu)->position().z();
e +=  (*iclu)->e();
}
}
//TODO: recompute a 3D cluster?
const double px =  e*x/std::sqrt(x*x + y*y + z*z);
const double py =  e*y/std::sqrt(x*x + y*y + z*z);
const double pz =  e*z/std::sqrt(x*x + y*y + z*z);
// make a new Particle
this->setProto(pp);
// TODO: deal with the particleservices and recompute the momenta according to the mass
this->setParticleID( LHCb::ParticleID(22) );
this->setReferencePoint( Gaudi::XYZPoint(0.,0.,0.)  );
this->setMomentum( Gaudi::XYZTVector(px,py,pz,e) ) ;
}*/
//=============================================================================
// Constructor for neutral recovery particles
//=============================================================================
LHCb::PFParticle::PFParticle( const Gaudi::Vector6& barycenter ,
        const double oldEnergy ) : Particle()
{
    // Get the cluster position
    const double x = barycenter[0];
    const double y = barycenter[1];
    const double z = barycenter[2];
    const double remainingE = barycenter[5];
    // Get the momentum for 0 mass particle (fine for jet input)
    const double _tmp = std::sqrt(x*x + y*y + z*z);
    const double px =  remainingE*x/_tmp;
    const double py =  remainingE*y/_tmp;
    const double pz =  remainingE*z/_tmp;

    // Set the base particle information
    this->addInfo(PFParticle::Type,PFParticle::NeutralRecovery);
    this->addInfo(PFParticle::ClustType,-1.);
    this->addInfo(PFParticle::ClustE,oldEnergy);
    // Set PID
    // TODO: deal with the particleservices
    this->setParticleID( LHCb::ParticleID( 22 ) );
    this->setReferencePoint( Gaudi::XYZPoint(0.,0.,0.)  );
    // Set Momentum
    this->setMomentum( Gaudi::XYZTVector(px,py,pz,remainingE) ) ;
}

//=============================================================================
// Constructor for charged particles
//=============================================================================
LHCb::PFParticle::PFParticle( const LHCb::ProtoParticle * pp ,
        int trackTag ,
        std::map< std::string , std::pair< const IProtoParticleFilter* , const LHCb::ParticleProperty * > > & protoMap ,  const LHCb::ParticleProperty* pprop) : Particle()
{
  // Start filling particle with ProtoParticle
  this->setProto(pp);

  // determine the PID
  if( !pprop ) {
    if( trackTag == Keep ) {
      // TODO: move that to a "best PID function?"
      bool pid_found(false);
      //Create a map of PID and ProbNN pairs
      typedef std::map< double , std::string > MapProbNNPID;

      MapProbNNPID  probNNpid ;
      probNNpid[ pp->info( LHCb::ProtoParticle::ProbNNpi   ,-0.5) ] =  "pi+" ;
      probNNpid[ pp->info( LHCb::ProtoParticle::ProbNNp    ,-0.5) ] =  "p+"  ;
      probNNpid[ pp->info( LHCb::ProtoParticle::ProbNNk    ,-0.5) ] =  "K+"  ;
      probNNpid[ pp->info( LHCb::ProtoParticle::ProbNNe    ,-0.5) ] =  "e+"  ;
      probNNpid[ pp->info( LHCb::ProtoParticle::ProbNNmu   ,-0.5) ] =  "mu+" ;

      // the map is by construction sorted by key value (lower first)
      MapProbNNPID::iterator iNN = probNNpid.end();
      iNN--;

      //verbose()<<"Ordered hypothesis: type "<<(*iNN).first<<" "<<(*iNN).second<<endreq;
      // Reverse loop, check if filter is satisfied
      if (protoMap[(*iNN).second].first->isSatisfied( pp ))pid_found = true ;

      while( !pid_found && iNN != probNNpid.begin() ){
	iNN--;
	//verbose()<<"Ordered hypothesis: type "<<(*iNN).first <<" with proba "<<(*iNN).second <<endreq;
	if (protoMap[(*iNN).second].first->isSatisfied( pp ))pid_found = true ;
      }
      if( pid_found ){
	pprop = protoMap[(*iNN).second].second ;
	//verbose()<<"PID applied: "<<(*iNN).second <<endreq;
      }
      // if no PID information, apply pion hypothesis
      if( !pid_found ){
	pprop = protoMap["pi+"].second ;
	//verbose()<<"PID applied: pi+ (by default)" <<endreq;
      }
    } else  {
      pprop =  protoMap["pi+"].second ;
    }
  }

  // now set it, but take care of the charge
  const int pID = pprop->particleID().pid() * (int)(pp->charge());
  this->setParticleID( LHCb::ParticleID( pID ) );
  this->setMeasuredMass(pprop->mass());
  this->setMeasuredMassErr(0.);

  // set the PF type
  if( trackTag == TurnTo0Momentum) {
    this -> setPFType   (  Charged0Momentum  );
  } else if ( trackTag == PFParticle::KeepInfMom ) {
    //TODO what is the pid here???
    // set the minus sigma momenta
    this -> setPFType( PFParticle::ChargedInfMomentum );
  } else {
    if ( this->particleID().abspid() == 11  ) this -> setPFType   ( PFParticle::Electron );
    else if ( this->particleID().abspid() == 13 ) this -> setPFType   (  PFParticle::Muon  );
    else  this -> setPFType (  PFParticle::ChargedHadron  );
  }

  // get track state
  LHCb::State state = pp->track()->firstState() ;
  this->setReferencePoint( state.position() ) ;

  // fix the momentum for velo-only
  if( trackTag == TurnTo0Momentum) {
    const double fixedmomentum = 1.0*Gaudi::Units::MeV ;
    state.setQOverP( pp->charge() / fixedmomentum ) ;
  }

  // Update the energy from the mass and momenta a la state2Particle
  // TODO: just use state2Particle!
  const Gaudi::XYZVector& mom = state.momentum();
  const double mass = this->measuredMass();
  const double p = mom.R() ;
  const double e = std::sqrt( p*p + mass*mass );
  this->setMomentum( Gaudi::XYZTVector(mom.X(),mom.Y(),mom.Z(),e) ) ;

  // get the jacobian for dp4/d(tx,ty,qop)
  Gaudi::Matrix4x3 dP4dMom;
  Gaudi::Math::JacobdP4dMom(state.stateVector().Sub<Gaudi::Vector3>(2),mass,dP4dMom);

  // now fill the total jacobia, but only nonzero elements
  ROOT::Math::SMatrix<double,7,5> Jacob ;
  Jacob(0,0) = Jacob(1,1) = 1 ;
  Jacob.Place_at(dP4dMom,3,2) ;

  const Gaudi::SymMatrix7x7 cov = ROOT::Math::Similarity<double,7,5>( Jacob, state.covariance() );

  // error on position
  Gaudi::SymMatrix3x3& errPos = *(const_cast<Gaudi::SymMatrix3x3*>(&(this->posCovMatrix())));
  errPos.Place_at(cov.Sub<Gaudi::SymMatrix3x3>( 0, 0 ), 0, 0 );
  //m_posCovMatrix.Place_at(cov.Sub<Gaudi::SymMatrix3x3>( 0, 0 ), 0, 0 );
  // error on momentum
  Gaudi::SymMatrix4x4& errMom = *(const_cast<Gaudi::SymMatrix4x4*>(&(this->momCovMatrix())));
  errMom.Place_at(cov.Sub<Gaudi::SymMatrix4x4>( 3, 3 ), 0, 0 );
  // m_momCovMatrix.Place_at(cov.Sub<Gaudi::SymMatrix4x4>( 3, 3 ), 0, 0 );
  // correlation
  Gaudi::Matrix4x3& errPosMom = *(const_cast<Gaudi::Matrix4x3*>(&(this->posMomCovMatrix())));
  errPosMom.Place_at(cov.Sub<Gaudi::Matrix4x3>( 3, 0 ), 0, 0 );

  //const double e = std::sqrt( this->p()*this->p() + this->measuredMass()*this->measuredMass() );
  //this->setMomentum( Gaudi::LorentzVector(this->momentum().X(),this->momentum().Y(),this->momentum().Z(),e) ) ;
  // TODO: Append the clusters and saturation values --> this might be done at the Neutral reco level??? because here we do not have the saturation info anymore
}
