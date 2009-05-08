#include <iomanip>
#include <stdio.h>
#include <sstream>

#include "GaudiKernel/PhysicalConstants.h"
#include "Event/Particle.h"

#include "DecayTreeFitter/VtxDoubleErr.h"
#include "DecayTreeFitter/VtxFitParams.h"

#include "Fitter.h"
#include "FitParams.h"
#include "DecayChain.h"
#include "ParticleBase.h"

namespace vtxtreefit
{

  extern int vtxverbose ;
  
  Fitter::Fitter(const LHCb::Particle& bc, double prec) 
    : m_bc(new LHCb::Particle(bc)),m_decaychain(0),m_fitparams(0),
      m_status(FitStatus::UnFitted),
      m_chiSquare(-1),m_niter(-1), m_prec(prec)
  {
    // build the tree
    if(vtxverbose>=2)
      std::cout << "VtkFitter::VtkFitter: building the tree" << std::endl ;
    m_decaychain = new DecayChain(bc,false) ;
    
    // allocate the fit parameters
    if(vtxverbose>=2)
      std::cout << "allocating fit parameters" << std::endl ;
    m_fitparams  = new FitParams(m_decaychain->dim()) ;
  }

  Fitter::~Fitter()
  {
    delete m_bc ;
    delete m_decaychain ;
    delete m_fitparams ;
  }
   
  void
  Fitter::fit()
  {
    const int nitermax=10 ;
    const int maxndiverging=3 ;
    const double dChisqConv = m_prec ; 
    //const double dChisqQuit = nDof() ; // if chi2 increases by more than this --> fit failed

    // initialize
    m_chiSquare = -1 ;
    m_errCode.reset() ;
    if( m_status== FitStatus::UnFitted )
      m_errCode = m_decaychain->init(*m_fitparams) ;
    
    if(m_errCode.failure()) {
      // the input tracks are too far apart
      m_status = FitStatus::BadInput ;
      
    } else {
      // reset the status flag
      m_status = FitStatus::UnFitted ;

      int ndiverging=0 ;
      bool finished = false ;
   
      for(m_niter=0; m_niter<nitermax && !finished; ++m_niter) {
	HepVector prevpar = m_fitparams->par() ;
	bool firstpass = m_niter==0 ;
	m_errCode = m_decaychain->filter(*m_fitparams,firstpass) ;
	double chisq = m_fitparams->chiSquare() ;
	double deltachisq = chisq - m_chiSquare ;
	// if chi2 increases by more than this --> fit failed
	const double dChisqQuit = std::max(double(2*nDof()),2*m_chiSquare) ;
	
	if(m_errCode.failure()) {
	  finished = true ;
	  m_status = FitStatus::Failed ;
	} else {
	  if( m_niter>0 ) {
	    if( fabs( deltachisq ) < dChisqConv ) {
	      m_chiSquare = chisq ;
	      m_status = FitStatus::Success ;
	      finished = true ; 
	    } else if( m_niter>1 && deltachisq > dChisqQuit ) {
	      m_fitparams->par() = prevpar ;
	      m_status  = FitStatus::Failed ;
	      m_errCode = ErrCode::fastdivergingfit ;
	      finished = true ;
	    } else if( deltachisq > 0 && ++ndiverging>=maxndiverging) {
	      m_fitparams->par() = prevpar ;
	      m_status = FitStatus::NonConverged ;
	      m_errCode = ErrCode::slowdivergingfit ;
	      finished = true ;
	    } else if( deltachisq > 0 ) {
	      // make a half step and reduce stepsize
	      m_fitparams->par()   += prevpar ;
	      m_fitparams->par()   *= 0.5 ;
	      //if( m_niter>10) m_fitparams->scale() *= 0.5 ;
	    } 
	  }
	  if ( deltachisq < 0 ) ndiverging=0 ; // start over with counting
	  if(!finished) m_chiSquare = chisq ;
	} 
    	
	if(vtxverbose>=1) {
	  std::cout << "step, chiSquare: "
		    << std::setw(3) << m_niter 
		    << std::setw(3) << m_status
		    << std::setw(3) << nDof()
		    << std::setprecision(6) 
		    << std::setw(12) << chisq
		    << std::setw(12) << deltachisq << std::endl ;
	}
	
	if(vtxverbose>=4) {
	  print() ;
	  std::cout << "press a key ...." << std::endl ;
	  getchar() ;
	}
      }
      
      if( m_niter == nitermax && m_status != FitStatus::Success )
	m_status = FitStatus::NonConverged ;

      //m_decaychain->mother()->forceP4Sum(*m_fitparams) ;

      if( !(m_fitparams->testCov() ) ) {
	std::cout << "vtxtreefitter::Fitter: Error matrix not positive definite. "
		  << "Changing status to failed." << std::endl ;
	m_status = FitStatus::Failed ;
	//print() ;
      }
    }
  }
  
  void
  Fitter::fitOneStep()
  {   
    bool firstpass = m_status==FitStatus::UnFitted ;
    if( firstpass ) m_decaychain->init(*m_fitparams) ;
    m_decaychain->filter(*m_fitparams,firstpass) ;
    m_chiSquare = m_fitparams->chiSquare() ;
    if(vtxverbose>=1)
      std::cout << "In VtkFitter::fitOneStep(): " << m_status << " " << firstpass << " " << m_chiSquare << std::endl ;
    m_status = FitStatus::Success ;
 }

  void
  Fitter::print() const
  {
    m_decaychain->mother()->print(m_fitparams) ;
    std::cout << "chisq,ndof,ncontr,niter,status: " 
	 << chiSquare() << " "
	 << nDof() << " " << m_fitparams->nConstraints() << " "
	 << nIter() << " " << status() << " " << m_errCode << std::endl ;
  } 

  void
  Fitter::printConstraints(std::ostream& os) const
  {
    m_decaychain->printConstraints(os) ;
  }

  const HepSymMatrix& Fitter::cov() const { 
    return m_fitparams->cov() ;
  }

  const HepVector& Fitter::par() const { 
    return m_fitparams->par() ;
  }

  HepSymMatrix Fitter::cov(const std::vector<int>& indexVec) const {
    return m_fitparams->cov(indexVec) ;
  }

  HepVector Fitter::par(const std::vector<int>& indexVec) const {
    return m_fitparams->par(indexVec) ;
  }

  int
  Fitter::nDof() const {
    return m_fitparams->nDof() ;
  }

  int Fitter::posIndex(const LHCb::Particle& bc) const {
    return m_decaychain->posIndex(bc) ;
  }
  
  int Fitter::momIndex(const LHCb::Particle& bc) const {
    return m_decaychain->momIndex(bc) ;
  }
  
  int Fitter::tauIndex(const LHCb::Particle& bc) const {
    return m_decaychain->tauIndex(bc) ;
  }

  double Fitter::add(const LHCb::Particle& cand)
  {
    // first obtain a map
    //ParticleBase::indexmap indexmap ;
    //m_decaychain->mother()->addToMap( indexmap ) ;
    // add this track

    ParticleBase* bp = m_decaychain->mother()->addDaughter(cand) ;
    int offset = m_fitparams->dim() ;
    bp->updateIndex(offset) ;
    double deltachisq(999) ;
    if( bp ) {
      // reassign indices
      //int offset(0) ;
      //m_decaychain->mother()->updIndex(offset) ;
      // resize the fitparams
      m_fitparams->resize(offset) ;
      // initialize the added track, filter and return the delta chisq
      bp->initPar1(m_fitparams) ;
      bp->initPar2(m_fitparams) ;
      bp->initCov(m_fitparams) ;

      ParticleBase::constraintlist constraints ;
      bp->addToConstraintList(constraints,0) ;
      double chisq = m_fitparams->chiSquare() ;
      ErrCode status ;
      for( ParticleBase::constraintlist::const_iterator it = constraints.begin() ;
 	   it != constraints.end(); ++it)
	status |= it->filter(*m_fitparams) ;

      deltachisq = m_fitparams->chiSquare() - chisq ;
      m_chiSquare = m_fitparams->chiSquare() ;

      // we want this somewhere else, but too much work now
      decaychain()->initConstraintList() ;

      //    print() ;
    } else {
      std::cout << "cannot add track to this vertex ..." 
		<< m_decaychain->mother()->type() << std::endl ;
    }
    return deltachisq ;
  }

  double Fitter::remove(const LHCb::Particle& cand)
  {
    ParticleBase* pb = const_cast<ParticleBase*>(m_decaychain->locate(cand)) ;
    ErrCode status ;
    double dchisq(0) ;
    if(pb) {
      // filter with negative weight
      ParticleBase::constraintlist constraints ;
      pb->addToConstraintList(constraints,0) ;
      double chisq = m_fitparams->chiSquare() ;
      for( ParticleBase::constraintlist::iterator it = constraints.begin() ;
	   it != constraints.end(); ++it) {
	it->setWeight(-1) ;
	status |= it->filter(*m_fitparams) ;
      }
      dchisq = chisq - m_fitparams->chiSquare() ;
      // remove
      ParticleBase* themother = const_cast<ParticleBase*>(pb->mother()) ;
      if(themother) themother->removeDaughter(pb);
    }
    return dchisq ;
  }

  void Fitter::setMassConstraint( const LHCb::Particle& bc, bool add)
  {
    m_decaychain->setMassConstraint(bc,add) ;
  }

  void Fitter::updateIndex()
  {
    int offset=0 ;
    m_decaychain->mother()->updateIndex(offset) ;
    m_fitparams->resize(offset) ;
  }

  double Fitter::globalChiSquare() const 
  {
    return m_decaychain->chiSquare(m_fitparams) ;
  }

  VtxFitParams 
  Fitter::fitParams(const ParticleBase& pb) const
  {
    int posindex = pb.posIndex() ;
    // hack: for tracks and photons, use the production vertex
    if(posindex<0 && pb.mother()) posindex = pb.mother()->posIndex() ;
    int momindex = pb.momIndex() ;

    Gaudi::XYZPoint pos(m_fitparams->par()(posindex+1),
			m_fitparams->par()(posindex+2),
			m_fitparams->par()(posindex+3)) ;
    Gaudi::LorentzVector p ;
    p.SetPx( m_fitparams->par()(momindex+1) ) ;
    p.SetPy( m_fitparams->par()(momindex+2) ) ;
    p.SetPz( m_fitparams->par()(momindex+3) ) ;
    Gaudi::SymMatrix7x7 cov7 ;
    if( pb.hasEnergy() ) {
      // if particle has energy, get full p4 from fitparams
      p.SetE( m_fitparams->par()(momindex+4) ) ;
      int parmap[7] ;
      for(int i=0; i<3; ++i) parmap[i]   = posindex + i ;
      for(int i=0; i<4; ++i) parmap[i+3] = momindex + i ;    
      for(int row=0; row<7; ++row)
	for(int col=0; col<=row; ++col)
	  cov7(row,col) = m_fitparams->cov()(parmap[row]+1,parmap[col]+1) ;
    } else {
      // if not, use the pdttable mass

      Gaudi::SymMatrix6x6 cov6 ;
      int parmap[6] ;
      for(int i=0; i<3; ++i) parmap[i]   = posindex + i ;
      for(int i=0; i<3; ++i) parmap[i+3] = momindex + i ;
      for(int row=0; row<6; ++row)
	for(int col=0; col<=row; ++col)
	  cov6(row,col) = m_fitparams->cov()(parmap[row]+1,parmap[col]+1) ;
   
      // now fill the jacobian
      double mass = pb.pdtMass() ;
      double energy2 = mass*mass ;
      for(int row=0; row<3; ++row) {
	double px = m_fitparams->par()(momindex+row+1) ;
	energy2 += px*px ;
      }
      double energy = std::sqrt(energy2) ;
      
      ROOT::Math::SMatrix<double,7,6> jacobian ;
      for(int col=0; col<6; ++col)
	jacobian(col,col) = 1;
      for(int col=3; col<6; ++col)
	jacobian(6,col) = m_fitparams->par()(parmap[col]+1)/energy ;

      p.SetE(energy) ;
      cov7 = ROOT::Math::Similarity(jacobian,cov6) ;
    }
    VtxFitParams vtxfitparams(pb.charge(),pos,p,cov7) ;
    vtxfitparams.setDecayLength(decayLength(pb)) ;
    return vtxfitparams ;
  }
  
  VtxFitParams 
  Fitter::fitParams(const LHCb::Particle& cand) const 
  {
    const ParticleBase* pb = m_decaychain->locate(cand) ;
    if(pb==0) {
      std::cout << "cann't find candidate in tree: " << cand
		<< " head of tree = " << m_bc
		<< std::endl ;
      return VtxFitParams() ;
    }
    return fitParams(*pb) ;
  }

  std::string
  Fitter::name(const LHCb::Particle& cand) const 
  {
    const ParticleBase* pb = m_decaychain->locate(cand) ;
    return pb ? pb->name() : "Not found" ;
  }

  LHCb::Particle
  Fitter::getFitted() const
  {
    LHCb::Particle thecand = *bc() ;
    updateCand(thecand) ;
    return thecand ;
  }

  LHCb::Particle
  Fitter::getFitted(const LHCb::Particle& cand) const
  {
    LHCb::Particle thecand = cand ;
    updateCand(thecand) ;
    return thecand ;
  }

  LHCb::Particle* 
  Fitter::fittedCand(const LHCb::Particle& /*cand*/, 
		     LHCb::Particle* /*headOfTree*/) const 
  {
    std::cout << "Fitter::fittedCand: not yet implemented" << std::endl ;
    return 0 ;
    // assigns fitted parameters to candidate in tree
    //LHCb::Particle* acand = const_cast<LHCb::Particle*>(headOfTree->cloneInTree(cand)) ;
    //updateCand(*acand) ;
    //return acand ;
  }

  LHCb::Particle
  Fitter::getFittedTree() const
  {
    LHCb::Particle cand = *bc() ;
    updateTree( cand ) ;
    return cand ;
  }

  /*
  std::string myvtxprint(const LHCb::Particle & cand,
			 const ComIOManip & format) {
    std::ostringstream stream;
    const PdtEntry * pdtEntry = cand.pdtEntry();
    if (0 != pdtEntry){
      stream << pdtEntry->name();
      if(!cand.isComposite()) 
	stream << "(" << cand.uid() << ")" ;
      stream << std::ends;
    }
    HepString result(stream.str());
    //delete [] stream.str();           // must take ownership here
    return result;
  }
  */

  void
  Fitter::updateCand(LHCb::Particle& /*cand*/) const
  {
    std::cout << "Fitter::updateCand: not yet implemented" << std::endl ;
    /*

    // assigns fitted parameters to a candidate
    const ParticleBase* pb = m_decaychain->locate(&cand) ;
    if(pb) {
      assert( pb->bc()->pdtEntry() == cand.pdtEntry() ) ;
      VtxFitParams vtxpar = vtxFitParams(pb) ;
      VtxVertex* vtx(0) ;
      int posindex = pb->posIndex() ;
      if( posindex>=0 ) {
	if( pb ==m_decaychain->cand() ) {
	  vtx = new VtxVertex(chiSquare(),nDof(),vtxpar.pos(),vtxpar.posCov(),vtxpar.xp4Cov()) ;
	  vtx->setStatus(FitStatus::VtxStatus(status())) ;
	  vtx->setType(FitStatus::Geometric) ;
	} else {
	  // all updated daughters are reset to unfitted, but leave the chisquare
	  double chisq = cand.decayVtx() ? cand.decayVtx()->chiSquared() : 0 ;
	  int ndof     = cand.decayVtx() ? cand.decayVtx()->nDof() : 0 ;
	  vtx = new VtxVertex(chisq,ndof,vtxpar.pos(),vtxpar.posCov(),vtxpar.xp4Cov()) ;
	  vtx->setStatus(FitStatus::UnFitted) ;
	}
      }
      cand.setTrajectory(vtxpar,vtx) ;
    } else {
      // this error message does not make sense, because it is also
      // triggered for daughters that were simply not refitted. we
      // have to do something about that.
//       VtxPrintTree printer(&myvtxprint) ;
//       ErrMsg(error) << "cann't find candidate " << std::endl
// 		    << printer.print(cand)
// 		    << "in tree " << std::endl
// 		    << printer.print(*_bc)
// 		    << endmsg;
    }
    */
  }

  void
  Fitter::updateTree(LHCb::Particle& /*cand*/) const
  {
    std::cout << "Fitter::updateTree: not yet implemented." << std::endl ;
    /*
    // assigns fitted parameters to all candidates in a decay tree
    updateCand(cand) ;
    HepAListIterator<LHCb::Particle> iter=cand.daughterIterator();
    LHCb::Particle* daughter ;
    while( (daughter=iter()) )  updateTree(*daughter) ;
    */
  }

  VtxDoubleErr
  Fitter::lifeTime(const LHCb::Particle& cand) const
  {
    // returns the lifetime in the rest frame of the candidate
    VtxDoubleErr rc(0,0) ;
    const ParticleBase* pb = m_decaychain->locate(cand) ;
    if(pb && pb->tauIndex()>=0 && pb->mother()) {
      int tauindex = pb->tauIndex() ;
      double tau    = m_fitparams->par()(tauindex+1) ;
      double taucov = m_fitparams->cov()(tauindex+1,tauindex+1) ;
      double mass   = pb->pdtMass() ; 
      double convfac = mass/Gaudi::Units::c_light ;
      rc = VtxDoubleErr(convfac*tau,convfac*convfac*taucov) ;
    }
    return rc ;
  }

  VtxDoubleErr
  Fitter::decayLength(const ParticleBase& pb) const 
  {
    // returns the decaylength in the lab frame
    return decayLength(pb,*m_fitparams) ;
  }

  VtxDoubleErr
  Fitter::decayLength(const ParticleBase& pb,
		      const FitParams& fitparams)
  {
    // returns the decaylength in the lab frame
    VtxDoubleErr rc(0,0) ;
    if(pb.tauIndex()>=0 && pb.mother()) {
      // one can calculate the error in many ways. I managed to make
      // them all agree, with a few outliers. this one seems to be
      // most conservative/stable/simple one.
 
      // len = tau |mom|
      int tauindex = pb.tauIndex() ;
      int momindex = pb.momIndex() ;
      double tau    = fitparams.par()(tauindex+1) ;
      double mom2(0) ;
      for(int row=1; row<=3; ++row) {
	double px = fitparams.par()(momindex+row) ;
	mom2 += px*px ;
      }
      double mom = sqrt(mom2) ;
      double len = mom*tau ;

      std::vector<int> indexvec ;
      indexvec.push_back(tauindex) ;
      indexvec.push_back(momindex+0) ;
      indexvec.push_back(momindex+1) ;
      indexvec.push_back(momindex+2) ;
      
      HepVector jacobian(4) ;
      jacobian(1) = mom ;
      jacobian(2) = tau*fitparams.par()(momindex+1)/mom ;
      jacobian(3) = tau*fitparams.par()(momindex+2)/mom ;
      jacobian(4) = tau*fitparams.par()(momindex+3)/mom ;

      rc = VtxDoubleErr(len,fitparams.cov(indexvec).similarity(jacobian)) ;
    }
    return rc ;
  }

  VtxDoubleErr
  Fitter::decayLength(const LHCb::Particle& cand) const
  {
    VtxDoubleErr rc(0,0) ;
    const ParticleBase* pb = m_decaychain->locate(cand) ;
    if(pb && pb->tauIndex()>=0 && pb->mother()) rc = decayLength(*pb) ;
    return rc ;
  }  

  VtxDoubleErr
  Fitter::decayLengthSum(const ParticleBase& pbA, const ParticleBase& pbB) const 
  {
    // returns the decaylengthsum in the lab frame
    VtxDoubleErr rc(0,0) ;
    if(pbA.tauIndex()>=0 && pbA.mother() &&
       pbB.tauIndex()>=0 && pbB.mother() ) {

      // len = tau |mom|
      int tauindexA = pbA.tauIndex() ;
      int momindexA = pbA.momIndex() ;
      double tauA    = m_fitparams->par()(tauindexA+1) ;
      double mom2A(0) ;
      for(int row=1; row<=3; ++row) {
	double px = m_fitparams->par()(momindexA+row) ;
	mom2A += px*px ;
      }
      double momA = sqrt(mom2A) ;
      double lenA = momA*tauA ;

      int tauindexB = pbB.tauIndex() ;
      int momindexB = pbB.momIndex() ;
      double tauB    = m_fitparams->par()(tauindexB+1) ;
      double mom2B(0) ;
      for(int row=1; row<=3; ++row) {
	double px = m_fitparams->par()(momindexB+row) ;
	mom2B += px*px ;
      }
      double momB = sqrt(mom2B) ;
      double lenB = momB*tauB ;

      std::vector<int> indexvec ;
      indexvec.push_back(tauindexA) ;
      for(int i=0; i<3; ++i) indexvec.push_back(momindexA+i) ;
      indexvec.push_back(tauindexB) ;
      for(int i=0; i<3; ++i) indexvec.push_back(momindexB+i) ;
      
      HepVector jacobian(8) ;
      jacobian(1) = momA ;
      for(int irow=1; irow<=3; ++irow) 
	jacobian(irow+1) = tauA*m_fitparams->par()(momindexA+irow)/momA ;
      jacobian(5) = momB ;
      for(int irow=1; irow<=3; ++irow) 
	jacobian(irow+5) = tauB*m_fitparams->par()(momindexB+irow)/momB ;
      
      rc = VtxDoubleErr(lenA+lenB,cov(indexvec).similarity(jacobian)) ;
    }
    return rc ;
  }
  
  VtxDoubleErr
  Fitter::decayLengthSum(const LHCb::Particle& candA, const LHCb::Particle& candB) const
  {
    VtxDoubleErr rc(0,0) ;
    const ParticleBase* pbA = m_decaychain->locate(candA) ;
    const ParticleBase* pbB = m_decaychain->locate(candB) ;
    if(pbA && pbB)  rc = decayLengthSum(*pbA,*pbB) ;
    return rc ;
  }  

}
  
