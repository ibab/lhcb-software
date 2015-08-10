#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiHistoTool.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram2D.h"

#include "AlignmentInterfaces/IAlignSolvTool.h"
#include "AlignmentInterfaces/IAlignUpdateTool.h"

#include "IGetElementsToBeAligned.h"
#include "IAlignConstraintTool.h"
#include "IAlignChisqConstraintTool.h"

#include <string>
#include <sstream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include "TH1D.h"
#include "TMath.h"

namespace Al
{

  class AlignUpdateTool : public GaudiHistoTool,
			  virtual public IAlignUpdateTool
  {
  public:
    typedef IGetElementsToBeAligned::Elements Elements ;
    typedef std::vector<double> AlignConstants;
    AlignUpdateTool(const std::string& type,
			const std::string& name,
			const IInterface* parent) ;
    ~AlignUpdateTool() ;
    StatusCode initialize() ;
    StatusCode finalize() ;
    StatusCode process( const Al::Equations& equations, size_t iter, size_t maxiter ) const ;
    StatusCode process( const Al::Equations& equations, ConvergenceStatus& convergencestatus) const ;
    StatusCode process( const Al::Equations& equations, ConvergenceStatus& convergencestatus, size_t iter, size_t maxiter ) const ;
    StatusCode queryInterface(const InterfaceID& riid, void** ppvi ) ;

  private:
    bool printDebug()   const {return msgLevel(MSG::DEBUG);};
    bool printVerbose() const {return msgLevel(MSG::VERBOSE);};
    void preCondition(const Elements& elements, const Al::Equations& equations,
		      AlVec& dChi2dAlpha, AlSymMat& d2Chi2dAlpha2,AlVec& scale) const ;
    void postCondition(AlVec& dChi2dAlpha, AlSymMat& d2Chi2dAlpha2, const AlVec& scale) const ;
    void getAlignmentConstants(const Elements& elements, AlignConstants& alignConstants) const ;
    void fillIterProfile( const HistoID& id,const std::string& title,size_t numiter,size_t iter,double val, double err=0) const ;
    StatusCode addDaughterDerivatives(Al::Equations& equations) const ;
    void dumpMostImportantDofs(const Elements& elements,const Al::Equations& equations,std::ostream& logmessage) const ;
    void dumpWorstSurveyOffenders(const Elements& elements,std::ostream& logmessage) const ;
    static double computeLookElsewhereChi2Cut(double chi2, double n) ;

  private:
    std::string                m_matrixSolverToolName;          ///< Name of linear algebra solver tool
    IAlignSolvTool*            m_matrixSolverTool;              ///< Pointer to linear algebra solver tool
    IGetElementsToBeAligned*   m_elementProvider ;
    ToolHandle<IAlignConstraintTool>      m_lagrangeconstrainttool ;
    ToolHandle<IAlignChisqConstraintTool> m_chisqconstrainttool ;
    size_t                     m_minNumberOfHits ;              ///< Minimum number of hits for an Alignable to be aligned
    bool                       m_usePreconditioning ;           ///< Precondition the system of equations before calling solver
    double                     m_maxDeltaChi2PDofForConvergence ;
    double                     m_maxModeDeltaChi2ForConvergence ;
    std::string                m_logFileName ;
    mutable std::ostringstream m_logMessage ;
    mutable unsigned int       m_iteration ;
  } ;
  
  DECLARE_TOOL_FACTORY( AlignUpdateTool )
  
  AlignUpdateTool::AlignUpdateTool(const std::string& type,
					   const std::string& name,
					   const IInterface* parent)
    : GaudiHistoTool(type,name,parent),
      m_lagrangeconstrainttool("Al::AlignConstraintTool",this),
      m_chisqconstrainttool("Al::AlignChisqConstraintTool",this),
      m_iteration(0)
  {
    // interfaces
    declareInterface<IAlignUpdateTool>(this); 
    declareProperty("MatrixSolverTool"            , m_matrixSolverToolName         = "SpmInvTool"            );
    declareProperty("MinNumberOfHits"             , m_minNumberOfHits              = 100u                    ); 
    declareProperty("UsePreconditioning"          , m_usePreconditioning           = true                    );
    declareProperty("LogFile"                     , m_logFileName                  = "alignlog.txt" ) ;
    declareProperty("SurveyConstraintTool", m_chisqconstrainttool ) ;
    declareProperty("LagrangeConstraintTool", m_lagrangeconstrainttool ) ;
    declareProperty("MaxDeltaChi2PDofForConvergence",m_maxDeltaChi2PDofForConvergence = 4) ;
    declareProperty("MaxModeDeltaChi2ForConvergence",m_maxModeDeltaChi2ForConvergence = 25) ;
  }

  AlignUpdateTool::~AlignUpdateTool()
  {
  }

  StatusCode AlignUpdateTool::initialize()
  {
    // check that constraints exist
    StatusCode sc = GaudiHistoTool::initialize() ;
    if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
    setHistoTopDir("") ;
    setHistoDir("Alignment") ;
    
    // Get tool to align detector, from the tool service
    m_elementProvider = tool<IGetElementsToBeAligned>("GetElementsToBeAligned");
    if (!m_elementProvider) return Error("==> Failed to retrieve detector selector tool!", StatusCode::FAILURE);
    
    // Get the constraint tool, also from the toolsvc
    sc = m_lagrangeconstrainttool.retrieve() ;
    if (!sc.isSuccess()) return Error("==> Failed to retrieve lagrange constraint tool!", StatusCode::FAILURE);

    // Get the constraint tool, also from the toolsvc
    sc = m_chisqconstrainttool.retrieve() ;
    if (!sc.isSuccess()) return Error("==> Failed to retrieve chisq constraint tool!", StatusCode::FAILURE);
    
    //Get matrix solver tool
    //m_matrixSolverTool = tool<IAlignSolvTool>(m_matrixSolverToolName, "MatrixSolver", this);
    m_matrixSolverTool = tool<IAlignSolvTool>(m_matrixSolverToolName);
    if (!m_matrixSolverTool) return Error("==> Failed to retrieve matrix solver tool", StatusCode::FAILURE);

    return sc ;
  }

  StatusCode AlignUpdateTool::finalize()
  { 
    m_lagrangeconstrainttool.release().ignore() ;
    m_chisqconstrainttool.release().ignore() ;
    return GaudiHistoTool::finalize() ;
  }

  void AlignUpdateTool::preCondition( const Elements& elements,
				      const Al::Equations& equations,
				      AlVec& halfDChi2DAlpha, 
				      AlSymMat& halfD2Chi2DAlpha2,
				      AlVec& scale) const
  {  
    // This is just not sufficiently fool proof!
    size_t size = halfDChi2DAlpha.size() ;
    scale.reSize(size) ;
    size_t ipar(0) ;
    // initialize all scales with 1
    for (ipar = 0u; ipar<size; ++ipar) scale[ipar] = 1 ;
    // now set the scales for the elements
    int iElem(0u) ;
    for (Elements::const_iterator it = elements.begin(); it != elements.end(); ++it, ++iElem) {
      int offset = (*it)->activeParOffset() ;
      if ( 0<=offset ) {
	size_t ndof = (*it)->dofMask().nActive() ;
	size_t N = std::max(equations.element(iElem).numHits(),size_t(1)) ;
	for (ipar = offset; ipar< offset+ndof; ++ipar) {
	  assert( ipar < size ) ;
	  if ( halfD2Chi2DAlpha2[ipar][ipar] > 0 ) scale[ipar] = std::sqrt( N / halfD2Chi2DAlpha2[ipar][ipar] ) ;
	}
      }
    }

    // Now determine the scales for the constraint. If the constraint
    // equation fixes an eigenmode with eigenvalue 'w', then the
    // two new eigenvalues become
    //  w_1 = w/2 - sqrt(norm^2 + w^2/4)
    //  w_2 = w/2 + sqrt(norm^2 + w^2/4)
    // where norm is the size of the derivative to the constraint. The
    // first eigenvalue is negative: that's a property of lagrange
    // constraints.
    //
    // Now, to make sure we can still identify which eigenvalue
    // corresponds to which constraints, we scale 'norm' such that it
    // is large and proportional to the index of the constraint:
    //   norm = 1e4 * (constraintindex + 1 )

    const double defaultnorm = 1e4 ;
    size_t numParameters(ipar) ;
    for(; ipar<size; ++ipar) {
      // first compute the length of the derivative vector (taking
      // into account the scales of the parameters)
      double norm2(0) ;
      for(size_t jpar=0; jpar<numParameters; ++jpar) {
	double derivative = halfD2Chi2DAlpha2[ipar][jpar] * scale[jpar] ;
	norm2 += derivative * derivative ;
      }
      // now set the scale
      if( norm2 > 0 ) 
	scale[ipar] = defaultnorm / std::sqrt(norm2) * (ipar - numParameters + 1) ;
    }

    // now apply the scales
    for (size_t i = 0u, iEnd = size; i < iEnd; ++i) {
      halfDChi2DAlpha[i] *= scale[i] ;
      for (size_t j = 0u; j <= i; ++j) halfD2Chi2DAlpha2[i][j] *= scale[i] * scale[j] ;
    }
  }
  
  void AlignUpdateTool::postCondition( AlVec& halfDChi2DAlpha, 
				       AlSymMat& halfD2Chi2DAlpha2,
				       const AlVec& scale ) const
  { 
    size_t size = halfDChi2DAlpha.size() ;
    for (size_t i = 0u, iEnd = size; i < iEnd; ++i) {
      halfDChi2DAlpha[i] *= scale[i] ;
      for (size_t j = 0u; j<=i; ++j) halfD2Chi2DAlpha2[i][j] *= scale[i] * scale[j] ;
    }
  }

  void AlignUpdateTool::fillIterProfile( const HistoID& id,
					 const std::string& title,
					 size_t numiter,
					 size_t iter,
					 double value,
					 double error) const
  {
    AIDA::IHistogram1D *histo = book1D(id,title,-0.5,numiter-0.5,numiter) ;
    TH1D* h1 = Gaudi::Utils::Aida2ROOT::aida2root(histo);
    h1->SetBinContent(iter+1, value); 
    h1->SetBinError(iter+1, error);
  }

  StatusCode AlignUpdateTool::addDaughterDerivatives(Al::Equations& equations) const
  {
    // Propagate information accumulated for daughters back to
    // parents.

    // Loop over the elements in reverse order. (Order is very
    // important: We do lowest in hierarchy first, such that it also
    // works for grandparents.)
    Elements  sortedelements = m_elementProvider->elements() ;
    for (Elements::reverse_iterator ielem = sortedelements.rbegin(); 
	 sortedelements.rend() != ielem ; ++ielem) 
      if( !(*ielem)->daughters().empty() ) {
	debug() << "Accumulating daughter data for element: "
	       << (*ielem)->name() << endreq ;

	// Important note: we accumulate the correlations first inside
	// the parent element. If that means that the correlation gets
	// in the wrong place (e.g. below the diagonal rather than
	// above), then we fix that only in the end. 

	ElementData& elementdata = const_cast<ElementData&>(equations.element((*ielem)->index())) ;
	if( elementdata.numHits() != 0 ) {
	  error() << "AlignUpdateTool::addDaughterDerivatives does not yet work if only subset of hits taken by daughters."
		  << endreq ;
	  return StatusCode::FAILURE ;
	}
	
	// the transpose is because we once made mistake to use a column vector for the derivative.
	Gaudi::Matrix6x6 parentJacobian = ROOT::Math::Transpose( (*ielem)->jacobian() ) ;
	// now accumulate information from all daughters. save the jacobians.
	std::vector< Gaudi::Matrix6x6 > daughterToMotherJacobians ;
	daughterToMotherJacobians.reserve( (*ielem)->daughters().size() ) ;
	std::vector<int> daughterIndices( equations.elements().size(), -1 ) ;
	size_t dauindex(0) ;
	for(AlignmentElement::DaughterContainer::const_iterator idau = (*ielem)->daughters().begin() ;
	    idau != (*ielem)->daughters().end(); ++idau, ++dauindex) {
	  // 1. create the jacobian
	  int ierr ;
	  Gaudi::Matrix6x6 totalJacobian = parentJacobian * 
	    ROOT::Math::Transpose( (*idau)->jacobian().Inverse(ierr)) ;
	  if( ierr ) {
	    error() << "Inversion error in AlignUpdateTool::addDaughterDerivatives. Failure." << endreq ;
	    return StatusCode::FAILURE ;
	  }
	  
	  // 2. add everything from the daughter, except for its
	  // correlations: those we'll do below all at once.
	  const ElementData& daudata = equations.element( (*idau)->index() ) ;
	  ElementData transformeddaudata = daudata ;
	  transformeddaudata.m_d2Chi2DAlphaDBeta.clear() ;
	  transformeddaudata.transform( totalJacobian ) ;
	  elementdata.add( transformeddaudata ) ;

	  // 3. add the correlation with the daughter
	  elementdata.m_d2Chi2DAlphaDBeta[ (*idau)->index() ].matrix() += totalJacobian * daudata.d2Chi2DAlpha2() ;

	  // save the jacobian for when we treat correlations
	  daughterToMotherJacobians.push_back( totalJacobian ) ;

	  // save index such that we can easily navigate to the daughters
	  daughterIndices[(*idau)->index()] = dauindex ;
	}

	// 4. Add all off-diagonal terms that were not added yet. This is quite tricky.
	size_t leftindex(0) ;
	for( Al::Equations::ElementContainer::const_iterator jdata = equations.elements().begin() ;
	     jdata != equations.elements().end(); ++jdata, ++leftindex)
	  if( leftindex != (*ielem)->index() ) { // skip the mother itself
	    for( Al::ElementData::OffDiagonalContainer::const_iterator jelem = jdata->d2Chi2DAlphaDBeta().begin() ;
		 jelem != jdata->d2Chi2DAlphaDBeta().end(); ++jelem) {
	      size_t rightindex = jelem->first ;
	      //if( rightindex == (*ielem)->index() ) {
	      //error() << "Very serious ordering problem: "
	      //	<< (*ielem)->index() << " " << leftindex << " " << rightindex << endreq ;
	      //assert(rightindex != (*ielem)->index()) ;
	      //}
	      int leftdaughter  = daughterIndices[leftindex] ;
	      int rightdaughter = daughterIndices[rightindex] ;
	      if( leftdaughter>=0 ) {
		elementdata.m_d2Chi2DAlphaDBeta[ rightindex ].matrix() +=
		  daughterToMotherJacobians[ leftdaughter ] * jelem->second.matrix() ;
	      }
	      if( rightdaughter>=0 ) {
		elementdata.m_d2Chi2DAlphaDBeta[ leftindex ].matrix() +=
		  daughterToMotherJacobians[ rightdaughter ] * ROOT::Math::Transpose( jelem->second.matrix() ) ;
	      }
	      // the elements where both are daughters need to be treated seperately
 	      if( leftdaughter>=0 && rightdaughter>=0) {
		// Add this term to the diagonal. Note that we
		// actually have this term twice. Its transpose is
		// hidden because we only keep lowerdiagonal. We
		// therefore symmetrize it as follows:
		Gaudi::Matrix6x6 tmpasym = 
		  daughterToMotherJacobians[ leftdaughter ] * jelem->second.matrix() *
		  ROOT::Math::Transpose( daughterToMotherJacobians[ rightdaughter ] ) ;
		Gaudi::SymMatrix6x6 tmpsym ;
		ROOT::Math::AssignSym::Evaluate(tmpsym, tmpasym + ROOT::Math::Transpose(tmpasym)) ;
		elementdata.m_d2Chi2DAlpha2 += tmpsym ;
	      }
	    }
	  }
	
	// now swap any element that we added with wrongly ordered index.
	Al::ElementData::OffDiagonalContainer keepelements ;	
	for( Al::ElementData::OffDiagonalContainer::const_iterator jelem = elementdata.d2Chi2DAlphaDBeta().begin() ;
	     jelem != elementdata.d2Chi2DAlphaDBeta().end(); ++jelem)
	  if( jelem->first < (*ielem)->index() ) {
	    //std::cout << "found wrongly ordered element: "
	    // << (*ielem)->index() << " " << jelem->first << std::endl ;
	    equations.element( jelem->first ).m_d2Chi2DAlphaDBeta[ (*ielem)->index() ].matrix() +=
	      ROOT::Math::Transpose(jelem->second.matrix()) ;
	  } else if( jelem->first == (*ielem)->index() ) {
	    error() << "VERY serious problem in addDaughterData. This still needs some development." 
		    << jelem->first << " " << (*ielem)->index() << endreq ;
	    // just symmetrize and add? (will only work if it is
	    // actually symmetric :-) First understand again how this
	    // could occur.
	  } else {
	    keepelements.insert( *jelem ) ;
	  }
	elementdata.m_d2Chi2DAlphaDBeta = keepelements ;
      }
    return StatusCode::SUCCESS ;
  }

  StatusCode AlignUpdateTool::process( const Al::Equations& constequations,
				       size_t iteration,
				       size_t maxiteration) const
  {
    ConvergenceStatus status ;
    return process( constequations, status, iteration, maxiteration ) ;
  }

  
  StatusCode AlignUpdateTool::process( const Al::Equations& constequations,
				       ConvergenceStatus& convergencestatus ) const
  {
    return process( constequations, convergencestatus, m_iteration++, 20 ) ;
  }

  
  StatusCode AlignUpdateTool::process( const Al::Equations& constequations,
				       ConvergenceStatus& convergencestatus,
				       size_t iteration,
				       size_t maxiteration ) const
  {
    // Print a warning if the geometry used to compute the derivatives
    // is not the same as the actual one. It is no longer a reason to
    // abort since we allow using derivatives computed with a
    // different geometry.
    convergencestatus = Converged ;
    StatusCode sc = StatusCode::SUCCESS ;
    Al::Equations equations = constequations ;
    if( m_elementProvider->initTime() != equations.initTime() ) {
      warning() << "Time of geometry does not match time of equations: "
		<< m_elementProvider->initTime().ns() << " " << equations.initTime().ns()
		<< ". Will reinitalize alignment frame." << endmsg ;
      sc = m_elementProvider->initAlignmentFrame( equations.initTime() ) ;
      if( !sc.isSuccess() ) return sc ;
    }
    
    typedef Gaudi::Matrix1x6 Derivatives;
    if ( equations.nElem() == 0 ) {
      warning() << "==> No elements to align." << endmsg ;
      return sc ;
    }
    
    info() << "\n";
    debug() << "==> iteration " << iteration << " : Initial alignment conditions  : [";
    const Elements& elements = m_elementProvider->elements() ;
    info() << "==> Updating constants" << endmsg ;
    std::ostringstream logmessage ;
    logmessage << "********************* ALIGNMENT LOG ************************" << std::endl
	       << "Iteration: " << iteration << std::endl
	       << "Total number of events: " << equations.numEvents() << std::endl 
	       << "Time of first event [ns]: " << equations.firstTime().ns() 
	       << " --> " << equations.firstTime().format(true,"%F %r") << std::endl
	       << "Time of last event [ns] : " << equations.lastTime().ns() 
	       << " --> " << equations.lastTime().format(true,"%F %r") << std::endl
	       << "Time at initialize [ns] : " << equations.initTime().ns() 
	       << " --> " << equations.initTime().format(true,"%F %r") << std::endl
	       << "First/last run number: " << equations.firstRun() << "/" << equations.lastRun() << std::endl
	       << "Used " << equations.numVertices() << " vertices for alignment" << std::endl
	       << "Used " << equations.numDiMuons() << " particles for alignment" << std::endl
	       << "Used " << equations.numTracks() << " tracks for alignment" << std::endl
	       << "Total chisquare/dofs:    " << equations.totalChiSquare() << " / " << equations.totalNumDofs() << std::endl
	       << "Average track chisquare: " << equations.totalTrackChiSquare() / equations.numTracks() << std::endl
	       << "Track chisquare/dof:     " << equations.totalTrackChiSquare() / equations.totalTrackNumDofs() << std::endl
	       << "Vertex chisquare/dof:    " 
	       << (equations.totalVertexNumDofs()>0 ? equations.totalVertexChiSquare() / equations.totalVertexNumDofs() : 0.0 ) << std::endl
	       << "Used " << equations.numHits() << " hits for alignment" << std::endl
	       << "Total number of hits in external detectors: " << equations.numExternalHits() << std::endl;

    // if there are mother-daughter relations, the information from
    // the daugters must be transformed to the mothers. This is kind
    // of tricky. it should also be done only once!
    info() << "Adding daughter derivatives" << endreq ;
    sc = addDaughterDerivatives( equations ) ;
    if( !sc.isSuccess() ) return sc ;

    //logmessage << "Most important dofs, excluding survey: " << std::endl ;
    //dumpMostImportantDofs( elements,equations,logmessage ) ;

    if (printDebug()) { 
      size_t index(0) ;
      for( Al::Equations::ElementContainer::const_iterator ieq = equations.elements().begin() ;
	   ieq != equations.elements().end(); ++ieq,++index) {
	debug() << "\n==> V["<< index <<"] = "    << ieq->dChi2DAlpha() << endmsg ;
	debug() << "\n==> M["<< index << ',' << index <<"] = "    << ieq->d2Chi2DAlpha2() << endmsg ;
	for( Al::ElementData::OffDiagonalContainer::const_iterator jeq = ieq->d2Chi2DAlphaDBeta().begin() ;
	     jeq != ieq->d2Chi2DAlphaDBeta().end() ; ++jeq) 
	  debug() << "==> M["<<index<<","<<jeq->first<<"] = " << jeq->second.matrix() << endmsg;
      }
    }
    
    // add the survey constraints
    info() << "Adding survey constraints. " << endreq ;
    LHCb::ChiSquare surveychisq = m_chisqconstrainttool->addConstraints(elements,equations,logmessage) ;
    info() << "End of adding survey constraints. " << endreq ;

    // Create the dof mask and a map from AlignableElements to an
    // offset. The offsets are initialized with '-1', which signals 'not
    // enough hits'.
    size_t numParameters(0), numExcluded(0) ;
    for (Elements::const_iterator it = elements.begin(); it != elements.end(); ++it ) {
      if (equations.element((*it)->index()).numHits() >= m_minNumberOfHits &&
	  equations.element((*it)->index()).d2Chi2DAlpha2().Trace()>0 &&
	  (*it)->dofMask().nActive()>0) {
	(*it)->setActiveParOffset( numParameters ) ;
	numParameters += (*it)->dofMask().nActive() ;
      } else {
	(*it)->setActiveParOffset(-1) ;
	if( (*it)->dofMask().nActive()>0 ) ++numExcluded ;
      }
    }
    
    logmessage << "Most important dofs, including survey: " << std::endl ;
    dumpMostImportantDofs( elements,equations,logmessage ) ;

    if(numParameters>0) {
      
      // now fill the 'big' vector and matrix
      AlVec    halfDChi2dX(numParameters) ;
      AlSymMat halfD2Chi2dX2(numParameters);
      size_t index(0);
      int iactive, jactive ;
      for( Al::Equations::ElementContainer::const_iterator ieq = equations.elements().begin() ;
	   ieq != equations.elements().end(); ++ieq, ++index ) {
	const AlignmentElement& ielem = *(elements[index]) ;
	// 1st derivative and diagonal 2nd derivative. note that we correct here for the fraction of outliers
	double outliercorrection = 1./ieq->fracNonOutlier() ;
	for(unsigned ipar=0; ipar<Derivatives::kCols; ++ipar) 
	  if( 0<= (iactive = ielem.activeParIndex(ipar) ) ) {
	    halfDChi2dX(iactive) = ieq->dChi2DAlpha()(ipar) * outliercorrection ;
	    for(unsigned jpar=0; jpar<=ipar; ++jpar) 
	      if( 0<= ( jactive = ielem.activeParIndex(jpar) ) )
		halfD2Chi2dX2.fast(iactive,jactive) = ieq->d2Chi2DAlpha2()(ipar,jpar) ;
	  }

	// second derivative. fill only non-zero terms
	for( Al::ElementData::OffDiagonalContainer::const_iterator im = ieq->d2Chi2DAlphaDBeta().begin() ;
	     im != ieq->d2Chi2DAlphaDBeta().end(); ++im ) {
	  // guaranteed: index < jndex .. that's how we fill it. furthermore, row==i, col==j
	  if( !(index< im->first) ) {
	    error() << index << " " << im->first << endreq ;
	    assert(index < im->first) ;
	  }
	  const AlignmentElement& jelem = *(elements[im->first]) ;
	  for(unsigned ipar=0; ipar<Derivatives::kCols; ++ipar) 
	    if( 0<= ( iactive = ielem.activeParIndex(ipar) ) ) 
	      for(unsigned jpar=0; jpar<Derivatives::kCols; ++jpar) 
		if( 0<= ( jactive = jelem.activeParIndex(jpar) ) ) {
		  assert(jactive > iactive  ) ;
		  halfD2Chi2dX2.fast(jactive,iactive) = im->second.matrix()(ipar,jpar) ;
		}
	}
      }
      
      // add the lagrange constraints to the linear system
      size_t numConstraints = m_lagrangeconstrainttool->addConstraints(elements,equations,halfDChi2dX,halfD2Chi2dX2) ;
      size_t numDoFs = numParameters - numConstraints ;

      logmessage << "Number of alignables with insufficient statistics: " << numExcluded << std::endl
		 << "Number of lagrange constraints: "                    << numConstraints << std::endl
		 << "Number of chisq constraints:    "                    << surveychisq.nDoF() << std::endl
		 << "Number of active parameters:    "                    << numParameters << std::endl ;
      
      if ( halfDChi2dX.size() < 20 ) {
	info() << "AlVec Vector    = " << halfDChi2dX << endmsg;
	info() << "AlSymMat Matrix = " << halfD2Chi2dX2      << endmsg;
      } else {
	info() << "Matrices too big to dump to std" << endmsg ;
      }
      
      // Tool returns H^-1 and alignment constants. Need to copy the 2nd derivative because we still need it.
      AlVec    scale(halfD2Chi2dX2.size()) ;
      AlSymMat covmatrix = halfD2Chi2dX2 ;
      AlVec    solution  = halfDChi2dX ;
      if (m_usePreconditioning) {
	info() << "Applying pre-conditioning" << endreq ;
	preCondition(elements,equations,solution,covmatrix, scale) ;
      }
      info() << "Calling solver tool" << endreq ;
      IAlignSolvTool::SolutionInfo solinfo ;
      sc = m_matrixSolverTool->compute(covmatrix, solution,solinfo);
      if (sc.isSuccess()) {

	if (m_usePreconditioning) {
	  info() << "Applying post-conditioning" << endreq ;
	  postCondition(solution,covmatrix, scale) ;
	}
      
	info() << "computing delta-chi2" << endreq ;
	double deltaChi2 = solution * halfDChi2dX ; //somewhere we have been a bit sloppy with two minus signs!
	if( deltaChi2 < 0 ) {
	  error() << "Serious problem in update: delta-chi2 is negative: " << deltaChi2 << endreq ;
	  //<< "Will recopute solution without scaling to see what that does! " << endreq ;
	  //covmatrix = halfD2Chi2dX2 ;
	  //solution  = halfDChi2dX ;
	  //solved = m_matrixSolverTool->compute(covmatrix, solution);
	  //deltaChi2 = solution * halfDChi2dX ;
	}

	if (printDebug()) {
	  info() << "Solution = " << solution << endmsg ;
	  info() << "Covariance = " << covmatrix << endmsg ;
	}
	logmessage << "Alignment delta chisquare/dof: " 
		   << deltaChi2 << " / " << numDoFs << std::endl
		   << "Normalised alignment change chisquare: " << deltaChi2 / numDoFs << std::endl
		   << "Alignment delta chisquare/track dof: "
		   << deltaChi2 / equations.totalTrackNumDofs() << std::endl;
	logmessage << "Alternative total chisq: " << solinfo.totalChisq  << std::endl
		   << "Smallest eigenvalue: " << solinfo.minEigenValue << std::endl
		   << "Maximum chisq contribution of eigenmode: " << solinfo.maxChisqEigenMode << std::endl ;

	m_lagrangeconstrainttool->printConstraints(solution, covmatrix, logmessage) ;
	
	if (printDebug()) debug() << "==> Putting alignment constants" << endmsg;
	size_t iElem(0u) ;
	double totalLocalDeltaChi2(0) ; // another figure of merit of the size of the misalignment.
	std::ostringstream modmessage ;

	for (Elements::const_iterator it = elements.begin(); it != elements.end(); ++it, ++iElem) {
	  const Al::ElementData& elemdata = equations.element(iElem) ;
	  modmessage << "Alignable: " << (*it)->name() << std::endl
		     << "Global position: " << (*it)->centerOfGravity() 
		     << " Average position of hits: "
		     << elemdata.averageHitPosition() << std::endl
		     << "Number of tracks/hits/outliers seen: " 
		     << elemdata.numTracks() << " "
		     << elemdata.numHits() << " "
		     << elemdata.numOutliers() << std::endl ;
	  modmessage << "Local-to-global diagonal: " ;
	  for(int i=0; i<6; ++i)
	    modmessage << std::setprecision(3) << (*it)->jacobian()(i,i) << " " ;
	  modmessage << std::endl ;

	  //double d2 = equations.elements()[iElem].M()[iElem]
	  LHCb::ChiSquare surveychisqbefore = m_chisqconstrainttool->chiSquare( **it ) ;	    
	  
	  int offset = (*it)->activeParOffset() ;
	  if( offset < 0 && (*it)->dofMask().nActive()>0) {
            modmessage << "Not enough hits for alignment. Skipping update." << std::endl ;
          } else if ( offset>=0 ) {
	    AlParameters delta( solution, covmatrix, halfD2Chi2dX2, (*it)->dofMask(), offset ) ;
	    AlParameters reftotaldelta = (*it)->currentActiveTotalDelta() ;
	    AlParameters refdelta = (*it)->currentActiveDelta() ;
	    //modmessage << delta ;
	    for(unsigned int iactive = 0u; iactive < delta.dim(); ++iactive) 
	      modmessage << std::setiosflags(std::ios_base::left)
			 << std::setprecision(4)
			 << std::setw(3)  << delta.activeParName(iactive) << " :" 
			 << " curtot= " << std::setw(12) << reftotaldelta.parameters()[iactive]
			 << " cur= "    << std::setw(12) << refdelta.parameters()[iactive]
			 << " delta= "  << std::setw(12) << delta.parameters()[iactive] << " +/- "
			 << std::setw(12) << AlParameters::signedSqrt(delta.covariance()[iactive][iactive]) 
			 << " gcc= " << delta.globalCorrelationCoefficient(iactive) << std::endl ;
	    double contributionToCoordinateError = delta.measurementCoordinateSigma( elemdata.weightR() ) ;
	    double coordinateError = std::sqrt(elemdata.numHits()/elemdata.weightV()) ;
	    modmessage << "contribution to hit uncertainty (absolute/relative): "
		       << contributionToCoordinateError << " " << contributionToCoordinateError/coordinateError << std::endl ;
	    
	    // compute another figure of merit for the change in
	    // alignment parameters that does not rely on the
	    // correlations. this should also go into either
	    // AlParameters or AlEquation
	    //const Gaudi::SymMatrix6x6& thisdChi2dAlpha2 = equations.element(iElem).d2Chi2DAlpha2() ;
	    //Gaudi::Vector6 thisAlpha = delta.parameterVector6() ;
	    double thisLocalDeltaChi2 = ROOT::Math::Similarity(delta.transformParameters(),
							       equations.element(iElem).d2Chi2DAlpha2() ) ;
	    modmessage << "local delta chi2 / dof: " << thisLocalDeltaChi2 << " / " << delta.dim() << std::endl ;
	    totalLocalDeltaChi2 += thisLocalDeltaChi2 ;

	    // need const_cast because loki range givess access only to const values 
	    StatusCode sc = (const_cast<AlignmentElement*>(*it))->updateGeometry(delta) ;
	    if (!sc.isSuccess()) error() << "Failed to set alignment condition for " << (*it)->name() << endmsg ;


	    std::string name = (*it)->name(); 
	    std::string dirname =  "element" + boost::lexical_cast<std::string>( (*it)->index() ) + "/"; //name + "/" ;
	    fillIterProfile( dirname + boost::lexical_cast<std::string>(10000u),
			     "Delta Tx vs iteration for " + name, 
			     maxiteration, iteration,  delta.translation()[0], delta.errTranslation()[0]);
	    fillIterProfile( dirname + boost::lexical_cast<std::string>(20000u),
			     "Delta Ty vs iteration for " + name,
			     maxiteration, iteration,  delta.translation()[1], delta.errTranslation()[1]);
	    fillIterProfile( dirname + boost::lexical_cast<std::string>(30000u),
			     "Delta Tz vs iteration for " + name,
			     maxiteration, iteration,  delta.translation()[2], delta.errTranslation()[2]);
	    fillIterProfile( dirname + boost::lexical_cast<std::string>(40000u),
			     "Delta Rx vs iteration for " + name,
			     maxiteration, iteration, delta.rotation()[0]   , delta.errRotation()[0]);
	    fillIterProfile( dirname + boost::lexical_cast<std::string>(50000u),
			     "Delta Rx vs iteration for " + name,
			     maxiteration, iteration, delta.rotation()[1]   , delta.errRotation()[1]);
	    fillIterProfile( dirname + boost::lexical_cast<std::string>(60000u),
			     "Delta Rx vs iteration for " + name,
			     maxiteration, iteration, delta.rotation()[2]   , delta.errRotation()[2]);
	  }

	  // print this one after the update
	  LHCb::ChiSquare surveychisq = m_chisqconstrainttool->chiSquare( **it ) ;
	  modmessage << "survey chi2 / dof (before/after): " << surveychisqbefore.chi2() << " "
		     << surveychisq.chi2() << " / " << surveychisq.nDoF() << std::endl ;
	  
	  AlParameters aligndeltaafter = (*it)->currentDelta() ;
	  modmessage << "align pars: " << aligndeltaafter.transformParameters() << std::endl ;
	  const AlParameters* survey = m_chisqconstrainttool->surveyParameters( **it ) ;
	  if( survey != 0 ) {
	    modmessage << "survey pars:   " << survey->transformParameters() << std::endl ;
	    modmessage << "survey uncertainties: "<< survey->transformErrors() << std::endl ;
	  }
	}
	logmessage << "total local delta chi2 / dof: " << totalLocalDeltaChi2 << " / " << numParameters << std::endl ;


	// Did we converge?
	if( deltaChi2 / numDoFs <  m_maxDeltaChi2PDofForConvergence &&
	    solinfo.maxChisqEigenMode < computeLookElsewhereChi2Cut(m_maxModeDeltaChi2ForConvergence,numDoFs) ) 
	  convergencestatus = Converged ;
	else 
	  convergencestatus = NotConverged ;
	logmessage << "Convergence status: " << convergencestatus << std::endl ;
	
	// dump information on the worst survey constraints
	dumpWorstSurveyOffenders( elements, logmessage ) ;
	
	// all the messages from module updates will be at  the end
	logmessage << modmessage.str() ;

	// fill some histograms
	fillIterProfile(20, "Total number of used tracks for alignment vs iteration", maxiteration,iteration,equations.numTracks()) ;
	fillIterProfile(30, "Total sum of track chi2 vs iteration", maxiteration,iteration,equations.totalChiSquare()) ;
	fillIterProfile(31, "Average sum of track chi2 vs iteration", maxiteration,iteration,equations.totalChiSquare() / equations.numTracks()) ;
	fillIterProfile(32, "Normalised total sum of track chi2 vs iteration", maxiteration,iteration, equations.totalChiSquare() / equations.totalNumDofs()) ;
	fillIterProfile(40, "Delta Alignment chi2 vs iteration", maxiteration,iteration,deltaChi2) ;
	fillIterProfile(41, "Delta Alignment normalised chi2 vs iteration", maxiteration,iteration,deltaChi2 /numParameters) ;
	} else {
	error() << "Failed to solve system" << endmsg ;
      }
    } else {
      logmessage << "No alignable degrees of freedom ... skipping solver tools and update." << std::endl ;
    }
    
    logmessage << "********************* END OF ALIGNMENT LOG ************************" ;
    debug() << logmessage.str() << endmsg ;
    debug() << "\n";
    m_logMessage << logmessage.str() ;
    
    if( iteration == 0 ) {
      // fill a big 2D matrix with the number of hits
      size_t num = equations.elements().size() ;
      AIDA::IHistogram2D* h2 = book2D("HitMatrix","number of tracks per element",
				      -0.5,num-0.5,num,-0.5,num-0.5) ;
      size_t index(0) ;
      for( Al::Equations::ElementContainer::const_iterator ieq = equations.elements().begin() ;
	   ieq != equations.elements().end(); ++ieq,++index) {
	h2->fill(double(index),double(index), ieq->numTracks()) ;
	for( Al::ElementData::OffDiagonalContainer::const_iterator 
	       jeq = ieq->d2Chi2DAlphaDBeta().begin() ;
             jeq != ieq->d2Chi2DAlphaDBeta().end() ; ++jeq)
	  h2->fill(double(index),double(jeq->first),jeq->second.numTracks()) ;
      }
    }

    // FIXME: we don't want to do this on every update! Yet, we cannot wait for finalize in online.
    if(!m_logFileName.empty()) {
      std::ofstream logfile(m_logFileName.c_str()) ;
      logfile << m_logMessage.str() << std::endl ;
    }
    return sc ;
  }
    
  void AlignUpdateTool::getAlignmentConstants(const Elements& elements, AlignConstants& alignConstants) const {
    alignConstants.reserve(6*elements.size()); // 6 constants * num elements
    for (Elements::const_iterator i = elements.begin(); i != elements.end(); ++i) {
      if (printDebug()) debug() << "Getting alignment constants for " << (**i) << endmsg;
      /// Get translations and rotations
	const std::vector<double>& translations = (*i)->deltaTranslations();
	const std::vector<double>& rotations    = (*i)->deltaRotations();
	/// Insert intitial constants (per element)
	  alignConstants.insert(alignConstants.end(), translations.begin(), translations.end());
	  alignConstants.insert(alignConstants.end(), rotations.begin()   , rotations.end()   );
    }
  }

  StatusCode AlignUpdateTool::queryInterface(const InterfaceID& id, void** ppI) {
    // check the placeholder:
    if (0 == ppI) return StatusCode::FAILURE;
    // check ID of the interface
    if( IAlignUpdateTool::interfaceID() == id ) {
      *ppI = static_cast<IAlignUpdateTool*>(this);
      addRef();
    } else {
      return AlgTool::queryInterface(id, ppI);
    }
    return StatusCode::SUCCESS;
  }

  namespace {
    struct DofChisq
    {
      const AlignmentElement* element ;
      int dof ;
      double chi2 ;
      double par ;
      double sur ;
      double err ;
      double surerr ;
    } ;
    
    inline bool operator<( const DofChisq& lhs, const DofChisq& rhs)
    {
      return lhs.chi2 > rhs.chi2 ;
    }
  }
  
  void AlignUpdateTool::dumpMostImportantDofs(const Elements& elements,
					      const Al::Equations& equations,
					      std::ostream& logmessage) const
  {
    std::vector<DofChisq> dofchi2s ;
    dofchi2s.reserve(6*elements.size()) ;
    DofChisq dofchi2 ;
    size_t index(0);
    for( Al::Equations::ElementContainer::const_iterator ieq = equations.elements().begin() ;
	 ieq != equations.elements().end(); ++ieq, ++index ) 
      if( ieq->numHits() > 0 ) {
	dofchi2.element = elements[index] ;
	for(dofchi2.dof = 0; dofchi2.dof<6; ++dofchi2.dof) {
	  double dChi2DAlpha = ieq->dChi2DAlpha()(dofchi2.dof) ;
	  double d2Chi2DAlpha2 = ieq->d2Chi2DAlpha2()(dofchi2.dof,dofchi2.dof) ;
	  // I could be a factor 2 wrong here
	  dofchi2.chi2 = dChi2DAlpha * 1/d2Chi2DAlpha2 * dChi2DAlpha ;
	  dofchi2.par  = - 1/d2Chi2DAlpha2 * dChi2DAlpha ;
	  dofchi2.err  = 1/std::sqrt(d2Chi2DAlpha2) ;
	  dofchi2s.push_back( dofchi2 ) ;
	}
      }
    std::sort( dofchi2s.begin(), dofchi2s.end() ) ;
    
    for(size_t i=0; i<25 && i<dofchi2s.size() ; ++i) 
      logmessage << "  " << i << " " << dofchi2s[i].element->name() 
		 << " dof=" << dofchi2s[i].dof 
		 << " chi2= " << dofchi2s[i].chi2 
		 << " delta= " << dofchi2s[i].par << " +/- " << dofchi2s[i].err << std::endl ;
  }
  
    void AlignUpdateTool::dumpWorstSurveyOffenders(const Elements& elements,
						   std::ostream& logmessage) const
    {
    std::vector<DofChisq> dofchi2s ;
    dofchi2s.reserve(6*elements.size()) ;
    DofChisq dofchi2 ;
    BOOST_FOREACH( const AlignmentElement* element, elements ) {
      const AlParameters * psurvey = m_chisqconstrainttool->surveyParameters(*element) ;
      if( psurvey ) {
	AlParameters::TransformParameters surveypars = psurvey->transformParameters() ;
	AlParameters::TransformCovariance surveycov = psurvey->transformCovariance() ;

	AlParameters currentdelta = element->currentDelta() ;
	AlParameters::TransformParameters alignpars  = currentdelta.transformParameters() ;
	AlParameters::TransformCovariance aligncov  = currentdelta.transformCovariance() ;

	// this should work for both active and inactive parameters
	AlParameters::TransformParameters residual  = alignpars - surveypars ;
	AlParameters::TransformCovariance rescov    = surveycov - aligncov ;
	dofchi2.element = element ;
	for(unsigned int dof =0; dof<6; ++dof ) {
	  dofchi2.sur  = surveypars(dof) ;
	  dofchi2.par  = alignpars(dof) ;
	  dofchi2.err  = std::sqrt( aligncov(dof,dof) ) ;
	  dofchi2.surerr  = std::sqrt( surveycov(dof,dof) ) ;
	  dofchi2.dof  = dof ;
	  dofchi2.chi2 = residual(dof) * residual(dof) / rescov(dof,dof) ;
	  dofchi2s.push_back( dofchi2 ) ;
	}
      }
    }
    std::sort( dofchi2s.begin(), dofchi2s.end() ) ;
    
    logmessage << "Survey constraints with largest chisquare contribution: " << std::endl ;
    for(size_t i=0; i<10 && i<dofchi2s.size() ; ++i) 
      logmessage << "  " << i << " " 
		 << std::setw(30) << std::setiosflags(std::ios_base::left) << dofchi2s[i].element->name() 
		 << " dof=" << dofchi2s[i].dof 
		 << " active=" << dofchi2s[i].element->dofMask().isActive( dofchi2s[i].dof ) << " "
		 << " chi2=" << std::setprecision(3) << dofchi2s[i].chi2 << " "
		 << " survey=" << std::setprecision(3) << dofchi2s[i].sur 
		 << " delta=" << std::setprecision(3) << dofchi2s[i].par << " "
		 << " surveyerr=" << std::setprecision(3) << dofchi2s[i].surerr 	
		 << " deltaerr=" << std::setprecision(3) << dofchi2s[i].err << std::endl ;
    }

  double AlignUpdateTool::computeLookElsewhereChi2Cut(double chi2, double n)
  {
    // one convergence criterion is the maximum chi2 of any of the
    // alignment 'modes' after diagonalization. of course, due to
    // statistics the maximum that we find depends on the number of
    // modes. the following corrects the chi2 cut for this
    // 'look-elsewhere effect'.
    double chi2prime = chi2 ;
    if(n>0) {
      double p = TMath::Erfc(std::sqrt(chi2/2)) ;
      double pprime = 1-std::pow((1-p),1./n) ;
      double x = TMath::ErfcInverse(pprime) ;
      chi2prime = 2*x*x ;
    }
    return chi2prime ;
  }

}
