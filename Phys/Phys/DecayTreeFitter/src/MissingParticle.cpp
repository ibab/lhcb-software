// #include "DecayTreeFitter/VtkFitParams.h"
// #include "DecayTreeFitter/VtkMissingParticle.h"
// #include "Event/Particle.h"

// namespace vtxtreefit
// {

//   extern int vtxverbose ;

//   MissingParticle::MissingParticle(const LHCb::Particle* bc, const ParticleBase* mother)
//     : ParticleBase(bc,mother),m_constrainMass(false)
//   {
//     // this will be one of the very few particles for which we adjust
//     // the dimension if there is a constraint
//     // copy constraints
//     // m_constrainMass = bc && bc->constraint(BtaConstraint::Mass) ;
//   }

//   MissingParticle::~MissingParticle() {}

//   ErrCode MissingParticle::initPar1(FitParams* fitpar)
//   {
//     // take them from the bc
//     Gaudi::LorentzVector p4 = bc() ? bc()->momentum() : Gaudi::LorentzVector(0,0,1,1) ;
//     int momindex = momIndex();
//     fitpar->par()(momindex+1) = p4.x() ;
//     fitpar->par()(momindex+2) = p4.y() ;
//     fitpar->par()(momindex+3) = p4.z() ;
//     if(hasEnergy()) fitpar->par()(momindex+4) = p4.t() ;
//     return ErrCode() ;
//   }
  
//   std::string MissingParticle::parname(int index) const
//   {
//     return ParticleBase::parname(index+4) ;
//   }
// }
