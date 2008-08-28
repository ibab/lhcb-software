#include "IAlignConstraintTool.h"
#include "GaudiAlg/GaudiTool.h"
#include "SolvKernel/AlMat.h"
#include "GaudiKernel/ToolFactory.h"
#include "LHCbMath/LHCbMath.h"

/* helper class */

namespace Al
{
  class Constraints
  {
  public:
    enum EConstraints { Tx=0, Ty, Tz, Rx, Ry, Rz, Szx, Szy, Szz, SRz, Trx, Try, Trtx, Trty, Cur, NumConstraints } ;
    typedef AlDofMask<NumConstraints> DofMask ;
    Constraints(size_t dim, const std::vector<std::string>& activeconstraints) ;
    AlMat& derivatives() { return m_derivatives ; }
    const AlMat& derivatives() const { return m_derivatives ; }

    static std::vector<std::string> all()  { return m_names ; }
    static std::string name(size_t i)  { return m_names[i] ; }
    static size_t index(const std::string& aname) {
      std::vector<std::string>::const_iterator it = std::find( m_names.begin(), m_names.end(), aname) ;
      return size_t( it - m_names.begin() ) ;
      //return std::distance( names.begin(),it) ; // doesn't exist for random access iterator ?!
    }
    static bool check( const std::vector<std::string>& names ) ;

    bool isActive(size_t ipar) const {  return m_dofmask.isActive(ipar) ;}
    void setActive(size_t ipar, bool b=true) {  return m_dofmask.setActive(ipar,b) ;}
    size_t nActive() const { return m_dofmask.nActive() ;}
    int activeParIndex(size_t ipar) const { return m_dofmask.activeParIndex(ipar) ; }
    std::string activeParName(size_t iactive) const { return name(m_dofmask.parIndex(iactive)) ; }
  private:
    AlMat m_derivatives ;
    AlVec m_residuals ;
    AlDofMask<NumConstraints> m_dofmask ;
    static const std::vector<std::string> m_names ;
  } ;

  bool Constraints::check( const std::vector<std::string>& names ) 
  {
    bool success(true) ;
    for( std::vector<std::string>::const_iterator iname = names.begin();
	 iname != names.end() && success; ++iname) 
      if ( !(success = std::find( m_names.begin(), m_names.end(), *iname) != m_names.end()) ) 
	std::cout << "Warning: unknown constraint " << *iname << std::endl ;
    return success ;
  }
  
  Constraints::Constraints(size_t dim, const std::vector<std::string>& activeconstraints) 
    : m_derivatives(NumConstraints,dim), m_residuals(NumConstraints)
  {
    std::vector<bool> active(NumConstraints,false) ;
    if( check(activeconstraints) ) 
      for( std::vector<std::string>::const_iterator iname = activeconstraints.begin();
	   iname != activeconstraints.end(); ++iname) 
	active[index(*iname)] = true ;
    m_dofmask = AlDofMask<NumConstraints>(active) ;
  }
  
  const std::vector<std::string> Constraints::m_names =
     boost::assign::list_of("Tx")("Ty")("Tz")("Rx")("Ry")("Rz")("Szx")("Szy")("Szz")("SRz")
    ("Trx")("Try")("Trtx")("Trty")("Trcur");
  

  class AlignConstraintTool : public GaudiTool,  
			      virtual public IAlignConstraintTool
  {
  public:
    AlignConstraintTool(const std::string& type,
			const std::string& name,
			const IInterface* parent) ;
    ~AlignConstraintTool() ;
    StatusCode initialize() ;
    size_t addConstraints(const Elements& elements,
			  const Al::Equations& equations,
			  AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2) const ;
    
    void printConstraints( const AlVec& parameters, const AlSymMat& covariance,
			   std::ostream& logmessage) const ;
  private:
    void fillConstraintDerivatives(const Elements& elements,
				   const Al::Equations& equations ) const ;
    void addConstraintToNominal(const Elements& elements,
				AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2) const ;
  private:
    mutable Al::Constraints* m_constraints ;
    std::vector<std::string> m_constraintNames ;
    bool m_useWeightedAverage ;
    bool m_constrainToNominal ;
    double m_sigmaNominalTx ;
    double m_sigmaNominalTy ;
    double m_sigmaNominalTz ;
    double m_sigmaNominalRx ;
    double m_sigmaNominalRy ;
    double m_sigmaNominalRz ;
  } ;
  
  DECLARE_TOOL_FACTORY( AlignConstraintTool );
  
  AlignConstraintTool::AlignConstraintTool(const std::string& type,
					   const std::string& name,
					   const IInterface* parent)
    : GaudiTool(type,name,parent),
      m_constraints(0)
  {
    // interfaces
    declareInterface<IAlignConstraintTool>(this);
    declareProperty("Constraints", m_constraintNames ) ;
    declareProperty("UseWeightedAverage", m_useWeightedAverage = false ) ;
    declareProperty("ConstrainToNominal", m_constrainToNominal = false ) ;
    declareProperty("SigmaNominalTx",m_sigmaNominalTx = 1*Gaudi::Units::mm) ;
    declareProperty("SigmaNominalTy",m_sigmaNominalTy = 1*Gaudi::Units::mm) ;
    declareProperty("SigmaNominalTz",m_sigmaNominalTz = 1*Gaudi::Units::mm) ;
    declareProperty("SigmaNominalRx",m_sigmaNominalRx = 0.01 ) ;
    declareProperty("SigmaNominalRy",m_sigmaNominalRy = 0.01 ) ;
    declareProperty("SigmaNominalRz",m_sigmaNominalRz = 0.01 ) ;
  }

  AlignConstraintTool::~AlignConstraintTool()
  {
    if( m_constraints ) delete m_constraints ;
  }

  StatusCode AlignConstraintTool::initialize()
  {
    // check that constraints exist
    StatusCode sc = GaudiTool::initialize() ;
    
    if( sc.isSuccess() ) {
      if( !Constraints::check(m_constraintNames) ) {
	error() << "Constraint list contains unknown constraints! "  << endreq ;
	sc = StatusCode::FAILURE ;
      } else {
	info() << "Constraining for " ;
	for( std::vector<std::string>::const_iterator it = m_constraintNames.begin() ;
	   it != m_constraintNames.end(); ++it )
	  info() << *it << " , " ;
	info() << endmsg ;
      }
    }
    return sc ;
  }
  
  void AlignConstraintTool::fillConstraintDerivatives(const Elements& elements,
						      const Al::Equations& equations) const
  {
    // This adds lagrange multipliers to constrain the average rotation
    // and translation. Ideally, we could calculate this in any
    // frame. In practise, the average depends on the reference frame in
    // which is is calculated. We will calculate a single 'pivot' point
    // to define the transform to the frame in which we apply the
    // constraint.
    
    // To make the bookkeeping easy, we add all possible constraints and
    // then 'disable' those we don't need with the 'dofmask'.
    // 
    // This is the numbering:
    // 0 -> Tx
    // 1 -> Ty
    // 2 -> Tz
    // 3 -> Rx
    // 4 -> Ry
    // 5 -> Rz
    // 6 -> Sx  (zx shearing)
    // 7 -> Sy  (zy shearing)
    // 8 -> Sz  (zz shearing == z-scale)
    // 9 -> SRz (twist around z-axis)
    // 10 -> Average track X
    // 11 -> Average track Y
    // 12 -> Average track Tx
    // 13 -> Average track Ty
    // 14 -> Curvature (Curvature constraint)
    
    // it should be possible to do this better
    double weight(0) ;
    size_t numhits(0) ;
    Gaudi::XYZVector pivot ;
    double zmin(9999999), zmax(-999999) ;
    size_t index(0u) ; 
    size_t numAlignPars(0) ;
    for (Elements::const_iterator it = elements.begin(); it !=elements.end() ; ++it, ++index) 
      if( it->activeParOffset() >= 0 ) { // only take selected elements
	double thisweight = equations.weight(index) ;
	weight += thisweight ;
	pivot += thisweight * Gaudi::XYZVector( it->centerOfGravity() ) ;
	zmin = std::min(it->centerOfGravity().z(),zmin) ;
	zmax = std::max(it->centerOfGravity().z(),zmax) ;
	numhits += equations.numHits(index) ;
	numAlignPars += it->dofMask().nActive() ;
      }
      
    if (weight>0) pivot *= 1/weight ;
    Gaudi::Transform3D canonicalframe( pivot ) ;
    Gaudi::Transform3D canonicalframeInv = canonicalframe.Inverse() ;
    info() << "Pivot, z-range for canonical constraints: " 
	   << pivot << ", [" << zmin << "," << zmax << "]" << endmsg ;

    if(m_constraints) delete m_constraints ;
    m_constraints = new Al::Constraints(numAlignPars,m_constraintNames) ;
 
    index = 0u ;
    for (Elements::const_iterator it = elements.begin(); it !=elements.end() ; ++it, ++index)
      if( it->activeParOffset() >= 0 ) { // only take selected elements
	// calculate the Jacobian for going from the 'alignment' frame to
	// the 'canonical' frame. This is the first place where we could
	// turn things around. It certainly works if the transforms are
	// just translations.
	Gaudi::Transform3D trans = canonicalframeInv * it->alignmentFrame() ;
	Gaudi::Matrix6x6 jacobian = AlParameters::jacobian( trans ) ;
	double thisweight = m_useWeightedAverage ? equations.weight(index)/weight : 1./double( equations.nElem() ) ;
	thisweight = 1.0/double( equations.nElem() ) ;
	double deltaZ = it->centerOfGravity().z() - zmin ;
	
	// loop over all parameters in this element. skip inactive parameters.
	for (size_t j=0 ; j<6; ++j) {
	  int jpar = it->activeParIndex(j) ;
	  if(jpar>=0) {
	    // Derivatives for global rotations and translations
	    for (size_t i = 0u; i < 6; ++i) 
	      // and here comes the 2nd place we could do things entirely
	      // wrong, but I think that this is right. if( it->activeParOffset() >= 0 ) { // only take selected elements
	      m_constraints->derivatives()(i,jpar) = thisweight * jacobian(i,j) ;
	    
	    // Derivatives for shearings
	    for (size_t i = 0u; i < 3u; ++i) 
	      m_constraints->derivatives()(i+6,jpar) = thisweight * deltaZ/(zmax-zmin) * jacobian(i,j) ;
	    
	    // Derivatives for twist around z-axis
	    m_constraints->derivatives()(Constraints::SRz,jpar) = thisweight * deltaZ/(zmax-zmin) * jacobian(5,j) ;
	    
	    // Curvature constraint. The constraint is on the average per track.
	    for(size_t trkpar=0; trkpar<5; ++trkpar) 
	      m_constraints->derivatives()(Constraints::Trx+trkpar,jpar) = equations.dStateDAlpha(index)(trkpar,j)/equations.numTracks() ;
	  }
	} 
      }
    
    // turn off constraints with only zero derivatives
    for(size_t ipar = 0; ipar<Constraints::NumConstraints; ++ipar ) 
      if( m_constraints->isActive(ipar) ) {
	bool hasNonZero(false) ;
	for (size_t jpar = 0; jpar <numAlignPars && !hasNonZero; ++jpar)
	  hasNonZero = std::abs(m_constraints->derivatives()(ipar,jpar)) > LHCb::Math::lowTolerance ;
	if( !hasNonZero ) {
	  warning() << "removing constraint \'" <<  m_constraints->name(ipar) << "\' because it has no non-zero derivatives." << endreq ;
	  m_constraints->setActive(ipar,false) ;
	}
      }
  }
  

  size_t AlignConstraintTool::addConstraints(const Elements& elements,
					     const Al::Equations& equations,
					     AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2) const
  {
    // fill the constraint derivatives
    fillConstraintDerivatives( elements, equations ) ;
    assert( m_constraints->derivatives().ncol() == halfDChi2DAlpha.size() ) ;

    // now add them, if any constraints are active
    if( m_constraints->nActive() > 0 ) {

      // create new matrices
      size_t size = halfDChi2DAlpha.size() ;
      size_t dim = size + m_constraints->nActive() ;
      AlVec halfDChi2DAlphaNew( dim ) ;
      AlSymMat halfD2Chi2DAlpha2New( dim ) ;
      
      // copy the old matrices in there
      for(size_t irow=0; irow< halfDChi2DAlpha.size(); ++irow) {
	halfDChi2DAlphaNew(irow) = halfDChi2DAlpha(irow) ;
	for(size_t icol=0; icol<=irow; ++icol) 
	  halfD2Chi2DAlpha2New.fast(irow,icol) = halfD2Chi2DAlpha2.fast(irow,icol) ;
      }
      
      // now add the constraints, only to the 2nd derivative (residuals are zero for now)
      for(size_t irow=0; irow<Al::Constraints::NumConstraints; ++irow) {
	int iactive = m_constraints->activeParIndex(irow) ;
	if( 0 <= iactive ) {
	  for(size_t jrow=0; jrow<size; ++jrow)
	    halfD2Chi2DAlpha2New.fast( iactive + size, jrow ) = m_constraints->derivatives()(irow,jrow) ;
	}
      }
      
      // copy the result back
      halfDChi2DAlpha = halfDChi2DAlphaNew ;
      halfD2Chi2DAlpha2 = halfD2Chi2DAlpha2New ;
    }

    // eventually, add also the chisquare constraints
    if( m_constrainToNominal ) 
      addConstraintToNominal(elements,halfDChi2DAlpha,halfD2Chi2DAlpha2) ;

    return m_constraints->nActive() ;
  }
  
  void AlignConstraintTool::printConstraints(const AlVec& parameters, const AlSymMat& covariance,
					     std::ostream& logmessage) const
  {
    // first extract the part concerning the lagrange multipliers
    size_t numConstraints = m_constraints->nActive() ;
    size_t numPars = parameters.size() - numConstraints ;
    
    AlVec lambda(numConstraints);
    AlSymMat covlambda(numConstraints) ;
    for (size_t i = 0u; i < numConstraints; ++i) {
      lambda[i] = parameters[numPars + i] ;
      for (size_t j = 0u; j <= i; ++j)
	covlambda[i][j] = covariance[numPars + i][numPars + j] ;
    }
    // For a lagrage constraint defined as delta-chisqyare = 2 * lambda * constraint, the solution for lambda is
    //  lambda     =   W * constraint
    //  cov-lambda =   W
    // where W is minus the inverse of the covariance of the constraint. From this we extract
    //   constraint       = W^{-1} * lambda
    //   constraint error = sqrt( W^{-1} )
    //   chisquare = - lambda * constraint
    // Note that this all needs to work for vectors.
    
    AlSymMat V = -1 * covlambda ; // Al$%$%^&$%&&*&^
    int ierr ;
    V.invert(ierr) ;
    AlVec x = (V * lambda) ; // This might need a minus sign ...
    double chisquare = (lambda * V * lambda) ;
    logmessage << "Canonical constraint values: " << std::endl ;
    for (size_t iactive = 0u; iactive < numConstraints; ++iactive)
      logmessage << std::setw(5) << m_constraints->activeParName(iactive)
		 << std::setw(12) << x(iactive)
		 << " +/- " << AlParameters::signedSqrt(V.fast(iactive,iactive)) << std::endl ;
    logmessage << "Canonical constraint chisquare: " << chisquare << std::endl ;
    
    // Now the inactove constraints. Shrink the parameter and covariance matrix to the parameters only part
    AlVec subparameters(numPars) ;
    AlSymMat subcovariance(numPars) ;
    for(size_t irow=0; irow<numPars; ++irow) {
      subparameters[irow] = parameters[irow] ;
      for(size_t icol=0; icol<=irow; ++icol) 
	subcovariance.fast(irow,icol) = covariance.fast(irow,icol) ;
    }
    // Calculate the delta
    AlVec constraintdelta  = m_constraints->derivatives() * subparameters ;
    AlMat constraintcov = m_constraints->derivatives() * subcovariance * m_constraints->derivatives().T() ;
    
    logmessage << "Values of constraint equations after solution (X=active,O=inactive): "
	       << std::endl ;
    for(size_t i = 0; i<Al::Constraints::NumConstraints; ++i) {
      logmessage << std::setw(4) << m_constraints->name(i) << " "
		 << std::setw(2) << (m_constraints->isActive(i)?'X':'O') << " "
		 << std::setw(12) << constraintdelta(i) 
		 << " +/- " << AlParameters::signedSqrt( constraintcov(i,i) ) << std::endl ;
      
    }
  }

  void AlignConstraintTool::addConstraintToNominal(const Elements& elements,
						   AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2) const
  {
    info() << "Adding constraint to nominal geometry" << endreq ;
    // this adds chisquare constraints for the survey. since we don't
    // really have the survey we constrain to the 'nomina; position.
    double sigma[6] ;
    sigma[0] = m_sigmaNominalTx ;
    sigma[1] = m_sigmaNominalTy ;
    sigma[2] = m_sigmaNominalTz ;
    sigma[3] = m_sigmaNominalRx ;
    sigma[4] = m_sigmaNominalRy ;
    sigma[5] = m_sigmaNominalRz ;

    for(Elements::const_iterator ielem = elements.begin() ;
	ielem != elements.end(); ++ielem) {
      // get the current difference with nominal
      AlParameters currentdelta = (*ielem).currentActiveDelta() ;
      // now assign errors. for the moment we choose errors that are
      // constant: 1mm for all positions and 1mrad for all rotations.
      // update the first and second derivative
      for(size_t iactive = 0; iactive<(*ielem).dofMask().nActive(); ++iactive) {
	double thissigma = sigma[(*ielem).dofMask().parIndex(iactive)] ;
	double weight = 1/(thissigma*thissigma) ;
	// this is tricky: need to get the sign right!
	double residual = -currentdelta.parameters()(iactive) ;
	size_t ipar = iactive + (*ielem).activeParOffset() ;
	halfDChi2DAlpha(ipar) += weight * residual; 
	halfD2Chi2DAlpha2.fast(ipar,ipar) += weight ;
      }
    }
  }
}
