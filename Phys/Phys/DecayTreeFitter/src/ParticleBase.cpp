#include <iomanip>
#include <float.h>

#include "Event/Particle.h"
#include "LoKi/ParticleProperties.h"

#include "ParticleBase.h"
#include "InternalParticle.h"
#include "RecoComposite.h"
#include "RecoResonance.h"
#include "RecoTrack.h"
#include "RecoPhoton.h"
#include "Resonance.h"
#include "MissingParticle.h"
#include "FitParams.h"
#include "Configuration.h"

#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "GaudiKernel/Service.h"

namespace DecayTreeFitter
{

  //template<class T>
  //inline T sqr(T x) { return x*x ; }

  int vtxverbose=0 ;

  ParticleBase::ParticleBase(const LHCb::Particle& particle, const ParticleBase* mother)
    : m_particle(&particle),m_mother(mother),
      m_prop(LoKi::Particles::ppFromPID(particle.particleID())),
      m_index(0),m_pdtMass(0),m_pdtWidth(0),m_pdtCLifeTime(0),m_charge(0),
      m_name("Unknown"), m_hasMassConstraint(false)
  {
    if( m_prop ) {
      m_pdtMass      = m_prop->mass() ;
      m_pdtWidth     = m_prop->width() ;
      m_pdtCLifeTime = m_prop->ctau() ;
      double fltcharge = m_prop->charge() ;
      m_charge = fltcharge < 0 ? int(fltcharge-0.5) : int(fltcharge+0.5) ;
      m_name = m_prop->name() ;
    } else {
      m_charge = particle.charge()>0 ? 1 : (particle.charge()<0 ? -1 : 0) ;
    }
  }


  ParticleBase::ParticleBase(const std::string& name)
    : m_particle(0),m_mother(0),
      m_prop(0),
      m_index(0),m_pdtMass(0),m_pdtWidth(0),m_pdtCLifeTime(0),m_charge(0),
      m_name(name), m_hasMassConstraint(false)
  {
  }

  ParticleBase::~ParticleBase()
  {
    for(daucontainer::iterator it = m_daughters.begin() ;
        it != m_daughters.end() ; ++it)
      delete *it ;
    m_daughters.clear() ;
  }

  void ParticleBase::updateIndex(int& offset)
  {
    // first the daughters
    for(const_iterator it = begin(); it!= end(); ++it) (*it)->updateIndex(offset) ;
    // now the real work
    m_index = offset ;
    offset += dim() ;
  }

  ParticleBase* ParticleBase::addDaughter(const LHCb::Particle& cand, const Configuration& config)
  {
    m_daughters.push_back( ParticleBase::createParticle(cand,this,config) ) ;
    return m_daughters.back() ;
  }

  void ParticleBase::removeDaughter(const ParticleBase* pb)
  {
    daucontainer::iterator it = std::find(m_daughters.begin(),m_daughters.end(),pb) ;
    if(it != m_daughters.end() ) {
      ParticleBase* _dp = *it ;
      m_daughters.erase(it) ;
      delete _dp ;
    } else {
      std::cout << "ERROR: cannot remove particle, because not found ..." << std::endl ;
    }
  }

  ParticleBase*
  ParticleBase::createParticle(const LHCb::Particle& particle,
                               const ParticleBase* mother,
                               const Configuration& config)
  {
    // This routine interpretes a beta candidate as one of the
    // 'Particles' used by the fitter.

    const LHCb::ParticleProperty* prop =
      LoKi::Particles::ppFromPID(particle.particleID()) ;

    if(vtxverbose>=2)
      std::cout << "ParticleBase::createParticle: " << config.forceFitAll() << std::endl ;
    ParticleBase* rc=0 ;
    //bool bsconstraint = false ;

    // We refit invalid fits, kinematic fits and composites with beamspot
    // constraint if not at head of tree.
    bool validfit = !config.forceFitAll() && particle.endVertex() != 0 ;
    bool iscomposite = particle.daughters().size()>0 ;
    bool isresonance = iscomposite && prop && isAResonance(*prop) ;

    if(!mother) { // 'head of tree' particles
      //if ( bsconstraint )
      //rc = new InteractionPoint(particle,forceFitAll) ;
      //else
      if( iscomposite )
        rc = new InternalParticle(particle,0,config) ; // still need proper head-of-tree class
      else {
        std::cout << "VtkParticleBase::createParticle: You are fitting a decay tree that exists of "
                  << "a single, non-composite particle and which does not have a beamconstraint."
                  << "I do not understand what you want me to do with this." << std::endl ;
        rc = new InternalParticle(particle,0,config) ; // still need proper head-of-tree class
      }
    } else if( !iscomposite ) { // external particles
      const LHCb::ProtoParticle* proto = particle.proto() ;
      bool hastrack = proto && proto->track() ;
      bool hascalo  = proto && !(proto->calo().empty()) ;

      if( hastrack )
        rc = new RecoTrack(particle,mother,config) ;  // reconstructed track
      else if( hascalo )
        rc = new RecoPhoton(particle,mother) ; // reconstructed photon
      else if( validfit ) {  // fitted composites w/o daughters?
        if( isresonance )
          rc = new RecoResonance(particle,mother) ;
        else
          rc = new RecoComposite(particle,mother) ;
      } else // missing particle! 
        rc = new MissingParticle(particle,mother) ;
    } else { // 'internal' particles
      if( validfit /*|| isconversion*/ ) {  // fitted composites
        if( isresonance )
          rc = new RecoResonance(particle,mother) ;
        else
          rc = new RecoComposite(particle,mother) ;
      } else {         // unfited composites
        if( isresonance )
          rc = new Resonance(particle,mother,config) ;
        else
          rc = new InternalParticle(particle,mother,config) ;
      }
    }

    if(vtxverbose>=2)
      std::cout << "ParticleBase::createParticle returns " << rc->type()
                << " " << rc->index() << std::endl ;
    return rc ;
  }

  bool
  ParticleBase::isAResonance(const LHCb::ParticleProperty& prop) {
    const int pidgamma  = 22 ;
    const int pideplus  = 11 ;
    const int pideminus = -11 ;
    bool rc = false ;
    switch(prop.particleID().pid()) {
    case pidgamma: // conversions are not treated as a resonance
      rc = false;
      break ;
    case pideplus: // bremstrahlung is treated as a resonance
    case pideminus:
      rc = true ;
      break ;
    default: // this should take care of the pi0
      rc = prop.ctau() < 0.001*Gaudi::Units::mm ;
    }
    return rc ;
  }

  void ParticleBase::collectVertexDaughters( daucontainer& particles, int posindex )
  {
    // collect all particles emitted from vertex with position posindex
    if( mother() && mother()->posIndex() == posindex )
      particles.push_back( this ) ;
    for( daucontainer::const_iterator idau = daughters().begin() ;
         idau != daughters().end() ; ++idau )
      collectVertexDaughters(particles,posindex ) ;
  }

  ErrCode
  ParticleBase::initCov(FitParams* fitparams) const
  {
    ErrCode status ;

    if(vtxverbose>=2) {
      std::cout << "ParticleBase::initCov for " << name() << std::endl ;
    }

    // position
    int posindex = posIndex() ;
    if( posindex>=0 ) {
      const double sigx   = 10 * Gaudi::Units::cm ;
      const double sigy   = 10 * Gaudi::Units::cm ;
      const double sigz   = 50 * Gaudi::Units::cm ;
      fitparams->cov().fast(posindex+1,posindex+1) = sigx*sigx ;
      fitparams->cov().fast(posindex+2,posindex+2) = sigy*sigy ;
      fitparams->cov().fast(posindex+3,posindex+3) = sigz*sigz ;
    }

    // momentum
    int momindex = momIndex() ;
    if(momindex>=0) {
      // TODO: calo at high energy?
      const double sigmom = 10 * Gaudi::Units::GeV ; // GeV
      int maxrow = hasEnergy() ? 4 : 3 ;
      for(int row=momindex+1; row<=momindex+maxrow; ++row)
        fitparams->cov() .fast (row,row) = sigmom*sigmom ;
    }

    // lifetime
    int lenindex = lenIndex() ;
    if(lenindex>=0) {
      const double sigz   = 50 * Gaudi::Units::cm ;
      fitparams->cov().fast (lenindex+1,lenindex+1) = sigz*sigz ;
    }

    for(daucontainer::const_iterator it = m_daughters.begin() ;
        it != m_daughters.end() ; ++it)
      status |= (*it)->initCov(fitparams) ;
    return status ;
  }


  std::string ParticleBase::parname(int thisindex) const
  {
    std::string rc = name() ;
    switch(thisindex) {
    case 0: rc += " x  " ; break ;
    case 1: rc += " y  " ; break ;
    case 2: rc += " z  " ; break ;
    case 3: rc += " len" ; break ;
    case 4: rc += " px " ; break ;
    case 5: rc += " py " ; break ;
    case 6: rc += " pz " ; break ;
    case 7: rc += " E  " ; break ;
    default: ;
    }
    return rc ;
  }

  void
  ParticleBase::print(const FitParams* fitpar) const
  {
    std::cout << std::setw(5) << "[" << type() << "]" << std::setw(15) << std::flush << name().c_str()
              << " val" << std::setw(15) << "err" << std::endl ;
    std::cout << std::setprecision(5) ;
    for(int i=0; i<dim(); ++i) {
      int theindex = index()+i ;
      std::cout << std::setw(2) << theindex << " "
                << std::setw(20) << parname(i).c_str()
                << std::setw(15) << fitpar->par()(theindex+1)
                << std::setw(15) << sqrt(fitpar->cov()(theindex+1,theindex+1))
                << std::setw(15) << fitpar->cov()(theindex+1,theindex+1) <<std::endl ;
    }
    if( hasEnergy() ) {
      int momindex = momIndex() ;
      double E  = fitpar->par()(momindex+4) ;
      double px = fitpar->par()(momindex+1) ;
      double py = fitpar->par()(momindex+2) ;
      double pz = fitpar->par()(momindex+3) ;
      double mass2 = E*E-px*px-py*py-pz*pz ;
      double mass = mass2>0 ? sqrt(mass2) : -sqrt(-mass2) ;

      HepSymMatrix cov = fitpar->cov().sub(momindex+1,momindex+4) ;
      HepVector G(4,0) ;
      G(1) = -px/mass ;
      G(2) = -py/mass ;
      G(3) = -pz/mass ;
      G(4) =   E/mass ;
      double massvar = cov.similarity(G) ;
      std::cout << std::setw(2) << std::setw(20) << "mass: "
                << std::setw(15) << mass
                << std::setw(15) << sqrt(massvar) << std::endl ;
    }

    for(daucontainer::const_iterator it = m_daughters.begin() ;
        it != m_daughters.end() ; ++it)
      (*it)->print(fitpar) ;

  }

  const
  ParticleBase* ParticleBase::locate(const LHCb::Particle& abc) const
  {
    // does there exist an 'iscloneof' in lhcb?
    const ParticleBase* rc = m_particle == &abc ? this : 0 ;
    for(daucontainer::const_iterator it = m_daughters.begin() ;
        !rc && it != m_daughters.end(); ++it)
      rc = (*it)->locate(abc) ;
    return rc ;
  }

  void ParticleBase::locate( const LHCb::ParticleID& pid,
                             ParticleContainer& result )
  {
    /// @attention Comparison by ABSPID!
    if ( m_particle &&
         m_particle->particleID().abspid() ==pid.abspid() )
    {  result.push_back(this) ; } ;
    //
    for( daucontainer::iterator it = m_daughters.begin() ;
         it != m_daughters.end(); ++it)
    { (*it)-> locate( pid, result ) ; }
    //
  }

  void ParticleBase::retrieveIndexMap(indexmap& anindexmap) const
  {

    anindexmap.push_back(std::pair<const ParticleBase*,int>(this,index())) ;
    for(daucontainer::const_iterator it = m_daughters.begin() ;
        it != m_daughters.end() ; ++it)
      (*it)->retrieveIndexMap(anindexmap) ;
  }


  ErrCode ParticleBase::projectGeoConstraint(const FitParams& fitparams,
                                             Projection& p) const
  {
    // implements the constraint

    //  vec{x}_decay = vec{x}_production + decaylength * vec{p} / p
    int posindexmother = mother()->posIndex() ;
    int posindex = posIndex();
    int lenindex = lenIndex() ;
    int momindex = momIndex() ;
    assert(posindexmother>=0 && posindex>=0 && lenindex>=0 && momindex>=0) ;

    // decay length
    double len =  fitparams.par()(lenindex+1) ;

    // size of momentum
    double px = fitparams.par()(momindex+1) ;
    double py = fitparams.par()(momindex+2) ;
    double pz = fitparams.par()(momindex+3) ;
    double p2 = px*px+py*py+pz*pz ;
    double mom  = std::sqrt(p2) ;

    // lineair approximation is fine for now
    for(int row=1; row<=3; ++row) {
      double posxmother = fitparams.par()(posindexmother+row) ;
      double posx       = fitparams.par()(posindex+row) ;
      double momx       = fitparams.par()(momindex+row) ;
      p.r(row) = posxmother - posx + len*momx/mom ;
      p.H(row,posindexmother+row) = 1 ;
      p.H(row,posindex+row)       = -1 ;
      p.H(row,lenindex+1)         = momx/mom ;
    }
    // still need these as well
    p.H(1,momindex+1)  = len/mom*( 1 - px*px/p2 ) ;
    p.H(1,momindex+2)  = len/mom*( 0 - px*py/p2 ) ;
    p.H(1,momindex+3)  = len/mom*( 0 - px*pz/p2 ) ;
    p.H(2,momindex+1)  = len/mom*( 0 - py*px/p2 ) ;
    p.H(2,momindex+2)  = len/mom*( 1 - py*py/p2 ) ;
    p.H(2,momindex+3)  = len/mom*( 0 - py*pz/p2 ) ;
    p.H(3,momindex+1)  = len/mom*( 0 - pz*px/p2 ) ;
    p.H(3,momindex+2)  = len/mom*( 0 - pz*py/p2 ) ;
    p.H(3,momindex+3)  = len/mom*( 1 - pz*pz/p2 ) ;

    //     if( false && charge()!=0 ) {
    //       double lambda = bFieldOverC() * charge() ;
    //       double px0 = fitparams.par()(momindex+1) ;
    //       double py0 = fitparams.par()(momindex+2) ;
    //       double pt0 = sqrt(px0*px0+py0*py0) ;
    //       const double posprecision = 1e-4 ; // 1mu
    //       if( fabs(pt0*lambda*tau*tau) > posprecision ) {
    //  // use the helix, but as if it were a 'correction'
    //  double sinlt = sin(lambda*tau) ;
    //  double coslt = cos(lambda*tau) ;
    //  double px = px0*coslt - py0*sinlt ;
    //  double py = py0*coslt + px0*sinlt ;

    //  p.r(1) += -tau*px0 + (py-py0)/lambda ;
    //  p.r(2) += -tau*py0 - (px-px0)/lambda ;

    //  p.H(1,lenindex+1) += -px0 + px ;
    //  p.H(1,momindex+1) += -tau + sinlt/lambda ;
    //  p.H(1,momindex+2) +=        (coslt-1)/lambda ;
    //  p.H(2,lenindex+1) += -py0 + py ;
    //  p.H(2,momindex+1) +=      - (coslt-1)/lambda ;
    //  p.H(2,momindex+2) += -tau + sinlt/lambda ;

    //  if(vtxverbose>=2)
    //    std::cout << "Using helix for position of particle: " << name().c_str() << " "
    //         << lambda << " " << lambda*tau
    //         << "  delta-x,y: " << -tau*px0 + (py-py0)/lambda << "  "
    //         << -tau*py0 - (px-px0)/lambda << std::endl ;
    //       }
    //  }
    p.setParticle( *mother() ) ;
    return ErrCode::success ;
  }

  ErrCode ParticleBase::projectMassConstraint(const FitParams& fitparams,
                                              Projection& p) const
  {
    double mass  = pdtMass() ;
    double mass2 = mass*mass ;
    int momindex = momIndex() ;

    // initialize the value
    double px = fitparams.par()(momindex+1) ;
    double py = fitparams.par()(momindex+2) ;
    double pz = fitparams.par()(momindex+3) ;
    double E  = fitparams.par()(momindex+4) ;
    p.r(1) = E*E-px*px-py*py-pz*pz-mass2 ;

    // calculate the projection matrix
    p.H(1,momindex+1) = -2.0*px ;
    p.H(1,momindex+2) = -2.0*py ;
    p.H(1,momindex+3) = -2.0*pz ;
    p.H(1,momindex+4) =  2.0*E ;

    // set the variance in the residual
    double width = pdtWidth() ;
    p.Vfast(1,1) = 4*mass*mass*width*width ;

    return ErrCode::success ;
  }

  ErrCode
  ParticleBase::projectConstraint(Constraint::Type atype, const FitParams&, Projection&) const
  {
    std::cout << "no method to project this constaint: "
              << name() << " " << type() << " " << atype << std::endl ;
    return ErrCode::badsetup ;
  }

  //   double ParticleBase::bFieldOverC()
  //   {
  //     static const BField* bfield =  gblEnv->getTrk()->magneticField();
  //     static const double Bz = BField::cmTeslaToGeVc*bfield->bFieldNominal() ;
  //     return Bz ;
  //   }

  ErrCode
  ParticleBase::initTau(FitParams* fitparams) const
  {
    int lenindex = lenIndex() ;
    if(lenindex>=0 && hasPosition() ) {
      const ParticleBase* amother = mother() ;
      int momposindex = amother ? amother->posIndex() : -1 ;
      int posindex = posIndex() ;
      int momindex = momIndex() ;
      assert(momposindex>=0) ; // check code logic: no mother -> no tau
      //assert(fitparams->par(momposindex+1)!=0 ||fitparams->par(momposindex+2)!=0
      //      ||fitparams->par(momposindex+3)!=0) ; // mother must be initialized

      HepVector dX(3),mom(3) ;
      double mom2(0) ;
      for(int irow=1; irow<=3; ++irow) {
        dX(irow)  = fitparams->par(posindex+irow) - fitparams->par(momposindex+irow) ;
        double px = fitparams->par(momindex+irow) ;
        mom(irow) = px ;
        mom2 += px*px ;
      }
      double tau = dot(dX,mom)/mom2 ;
      // we don't like 0 and we don't like very negative values
      if( tau==0 ) tau=pdtTau() ;
      //tau = tau==0 ? pdtTau() : std::max(tau,-pdtTau()) ;
      fitparams->par(lenindex+1) = tau ;
    }
    return ErrCode::success ;
  }

  double ParticleBase::chiSquare(const FitParams* fitparams) const
  {
    double rc = 0;
    for(daucontainer::const_iterator it = m_daughters.begin() ;
        it != m_daughters.end(); ++it)
      rc += (*it)->chiSquare(fitparams) ;
    return rc ;
  }

  int ParticleBase::nFinalChargedCandidates() const {
    int rc=0;
    for(daucontainer::const_iterator it = m_daughters.begin() ;
        it != m_daughters.end() ; ++it)
      rc +=(*it)->nFinalChargedCandidates() ;
    return rc ;
  }

  ChiSquare ParticleBase::chiSquare( const FitParams& params ) const
  {
    ChiSquare chi2 ;
    // add contribution from daughters
    for(daucontainer::const_iterator it = m_daughters.begin() ;
        it != m_daughters.end() ; ++it) {
      chi2 += (*it)->chiSquare(params) ;
    }
    // add own chisquare, adjust for number of parameters
    chi2 += params.chiSquare( *this ) ;
    chi2 += ChiSquare( 0, -dim() ) ;
    return chi2 ;
  }
}
