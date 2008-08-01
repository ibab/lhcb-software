#include "IAlignUpdateTool.h"
#include "GaudiAlg/GaudiHistoTool.h"
#include "SolvKernel/AlMat.h"
#include "GaudiKernel/ToolFactory.h"
#include "LHCbMath/LHCbMath.h"
#include "IGetElementsToBeAligned.h"
#include "AlignmentInterfaces/IAlignSolvTool.h"
#include "IAlignConstraintTool.h"
#include "AIDA/IProfile1D.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "boost/lexical_cast.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include "TH1D.h"

namespace Al
{

  class AlignUpdateTool : public GaudiHistoTool,
			  virtual public IAlignUpdateTool
  {
  public:
    typedef IGetElementsToBeAligned::ElementRange Elements ;
    typedef std::vector<double> AlignConstants;
    AlignUpdateTool(const std::string& type,
			const std::string& name,
			const IInterface* parent) ;
    ~AlignUpdateTool() ;
    StatusCode initialize() ;
    StatusCode finalize() ;
    StatusCode process( const Al::Equations& equations, size_t iter, size_t maxiter ) const ;
  private:
    bool printDebug()   const {return msgLevel(MSG::DEBUG);};
    bool printVerbose() const {return msgLevel(MSG::VERBOSE);};
    void preCondition(const Elements& elements, const Al::Equations& equations,
		      AlVec& dChi2dAlpha, AlSymMat& d2Chi2dAlpha2,AlVec& scale) const ;
    void postCondition(AlVec& dChi2dAlpha, AlSymMat& d2Chi2dAlpha2, const AlVec& scale) const ;
    void getAlignmentConstants(const Elements& rangeElements, AlignConstants& alignConstants) const ;
    void fillIterProfile( const HistoID& id,const std::string& title,size_t numiter,size_t iter,double val, double err=0) const ;
  private:
    std::string                m_matrixSolverToolName;          ///< Name of linear algebra solver tool
    IAlignSolvTool*            m_matrixSolverTool;              ///< Pointer to linear algebra solver tool
    IGetElementsToBeAligned*   m_elementProvider ;
    IAlignConstraintTool*      m_constrainttool ;
    size_t                     m_nIterations;                   ///< Number of iterations
    size_t                     m_minNumberOfHits ;              ///< Minimum number of hits for an Alignable to be aligned
    bool                       m_usePreconditioning ;           ///< Precondition the system of equations before calling solver
    std::string                m_logFileName ;
    mutable std::ostringstream m_logMessage ;
    AIDA::IProfile1D*          m_totNusedTracksvsIterHisto;
    AIDA::IProfile1D*          m_totChi2vsIterHisto;
    AIDA::IProfile1D*          m_norTotChi2vsIterHisto;
    AIDA::IProfile1D*          m_avgChi2vsIterHisto;
    AIDA::IProfile1D*          m_dAlignChi2vsIterHisto;
    AIDA::IProfile1D*          m_nordAlignChi2vsIterHisto;
  } ;
  
  DECLARE_TOOL_FACTORY( AlignUpdateTool );
  
  AlignUpdateTool::AlignUpdateTool(const std::string& type,
					   const std::string& name,
					   const IInterface* parent)
    : GaudiHistoTool(type,name,parent)
  {
    // interfaces
    declareInterface<IAlignUpdateTool>(this); 
    declareProperty("NumberOfIterations"          , m_nIterations                  = 10u                     );
    declareProperty("MatrixSolverTool"            , m_matrixSolverToolName         = "SpmInvTool"            );
    declareProperty("MinNumberOfHits"             , m_minNumberOfHits              = 100u                    ); 
    declareProperty("UsePreconditioning"          , m_usePreconditioning           = false                   );
    declareProperty("LogFile"                     , m_logFileName                  = "alignlog.txt" ) ;
  }

  AlignUpdateTool::~AlignUpdateTool()
  {
  }

  StatusCode AlignUpdateTool::initialize()
  {
    // check that constraints exist
    StatusCode sc = GaudiHistoTool::initialize() ;
    if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
    //setHistoTopDir("/Alignment") ;

    // Get tool to align detector, from the tool service
    m_elementProvider = tool<IGetElementsToBeAligned>("GetElementsToBeAligned");
    if (!m_elementProvider) return Error("==> Failed to retrieve detector selector tool!", StatusCode::FAILURE);
    
    // Get the constraint tool, also from the toolsvc
    m_constrainttool = tool<IAlignConstraintTool>("AlignConstraintTool") ;
    if (!m_constrainttool) return Error("==> Failed to retrieve constraint tool!", StatusCode::FAILURE);
    
    //Get matrix solver tool
    m_matrixSolverTool = tool<IAlignSolvTool>(m_matrixSolverToolName, "MatrixSolver", this);
    if (!m_matrixSolverTool) return Error("==> Failed to retrieve matrix solver tool", StatusCode::FAILURE);

    return sc ;
  }

  StatusCode AlignUpdateTool::finalize()
  { 
    if(!m_logFileName.empty()) {
      std::ofstream logfile(m_logFileName.c_str()) ;
      logfile << m_logMessage.str() << std::endl ;
    }
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
    for (size_t i = 0u, iEnd = size; i< iEnd; ++i) scale[i] = 1 ;
    int iElem(0u) ;
    for (Elements::const_iterator it = elements.begin(), itEnd = elements.end(); it != itEnd; ++it, ++iElem) {
      int offset = it->activeParOffset() ;
      if ( 0<=offset ) {
	size_t ndof = (*it).dofMask().nActive() ;
	size_t N = equations.numHits(iElem) ;
	for (size_t i = offset; i< offset+ndof; ++i) {
	  assert( i < size ) ;
	  if ( halfD2Chi2DAlpha2[i][i] > 0 ) scale[i] = std::sqrt( N / halfD2Chi2DAlpha2[i][i] ) ;
	}
      }
    }
    
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

  StatusCode AlignUpdateTool::process( const Al::Equations& equations,
				       size_t iteration,
				       size_t maxiteration) const
  {  
    typedef Gaudi::Matrix1x6 Derivatives;
    StatusCode sc = StatusCode::SUCCESS ;
    if ( equations.nElem() == 0 ) {
      warning() << "==> No elements to align." << endmsg ;
      return sc ;
    }
    
    info() << "\n";
    info() << "==> iteration " << iteration << " : Initial alignment conditions  : [";
    Elements elements = m_elementProvider->rangeElements() ;
    std::vector<double> deltas;
    deltas.reserve(elements.size()*6u);  
    getAlignmentConstants(elements, deltas);
    for (std::vector<double>::const_iterator i = deltas.begin(), iEnd = deltas.end(); i != iEnd; ++i) {
      info() << (*i) << (i != iEnd-1u ? ", " : "]");
    }
    info() << endmsg;
    
    info() << "==> Updating constants" << endmsg ;
    std::ostringstream logmessage ;
    logmessage << "********************* ALIGNMENT LOG ************************" << std::endl
	       << "Iteration: " << iteration << std::endl
	       << "Total number of events: " << equations.numEvents() << std::endl 
      //<< "Total number of tracks: " << m_nTracks << std::endl
      //<< "Number of covariance calculation failures: " << m_covFailure << std::endl
	       << "Used " << equations.numVertices() << " vertices for alignment" << std::endl
	       << "Used " << equations.numTracks() << " tracks for alignment" << std::endl
	       << "Total chisquare/dofs: " << equations.totalChiSquare() << " / " << equations.totalNumDofs() << std::endl
	       << "Average track chisquare: " << equations.totalTrackChiSquare() / equations.numTracks() << std::endl
	       << "Track chisquare/dof: " << equations.totalTrackChiSquare() / equations.totalTrackNumDofs() << std::endl
	       << "Vertex chisquare/dof: " 
	       << (equations.totalVertexNumDofs()>0 ? equations.totalVertexChiSquare() / equations.totalVertexNumDofs() : 0.0 ) << std::endl
	       << "Used " << equations.numHits() << " hits for alignment" << std::endl
	       << "Total number of hits in external detectors: " << equations.numExternalHits() << std::endl;
    
    if (printDebug()) {
      for (size_t i = 0; i < equations.nElem(); ++i) {
	for (size_t j = i; j < equations.nElem(); ++j) {
	  debug() << "==> M["<<i<<","<<j<<"] = "      << equations.M(i,j) << endmsg;
	}
	debug() << "\n==> V["<<i<<"] = "    << equations.V(i) << endmsg;
      }
    }
    
    // Create the dof mask and a map from AlignableElements to an
    // offset. The offsets are initialized with '-1', which signals 'not
    // enough hits'.
    size_t numParameters(0), numExcluded(0) ;
    for (Elements::const_iterator it = elements.begin(); it != elements.end(); ++it ) {
      if (equations.numHits(it->index()) >= m_minNumberOfHits) {
	it->setActiveParOffset( numParameters ) ;
	numParameters += it->dofMask().nActive() ;
      } else {
	it->setActiveParOffset(-1) ;
	++numExcluded ;
      }
    }

    if(numParameters>0) {
      
      // now fill the 'big' vector and matrix
      AlVec    halfDChi2dX(numParameters) ;
      AlSymMat halfD2Chi2dX2(numParameters);
      size_t index(0);
      int iactive, jactive ;
      for( Al::Equations::ElementContainer::const_iterator ieq = equations.elements().begin() ;
	   ieq != equations.elements().end(); ++ieq, ++index ) {
	const AlignmentElement& ielem = elements[index] ;
	// first derivative. note that we correct here for the fraction of outliers
	double outliercorrection = 1./ieq->fracNonOutlier() ;
	for(unsigned ipar=0; ipar<Derivatives::kCols; ++ipar) 
	  if( 0<= (iactive = ielem.activeParIndex(ipar) ) )
	    halfDChi2dX(iactive) = ieq->V()(ipar) * outliercorrection ;
	
	// second derivative. fill only non-zero terms
	for( Al::ElementData::OffdiagonalContainer::const_iterator im = ieq->M().begin() ;
	     im != ieq->M().end(); ++im ) {
	  size_t jndex = im->first ;
	  // guaranteed: index <= jndex .. that's how we fill it. furthermore, row==i, col==j
	  if(jndex==index) { // diagonal element
	    for(unsigned ipar=0; ipar<Derivatives::kCols; ++ipar) 
	      if( 0<= ( iactive = ielem.activeParIndex(ipar) ) ) 
		for(unsigned jpar=0; jpar<=ipar; ++jpar) 
		  if( 0<= ( jactive = ielem.activeParIndex(jpar) ) )
		    halfD2Chi2dX2.fast(iactive,jactive) = im->second(ipar,jpar) ;
	  } else {
	    const AlignmentElement& jelem = elements[jndex] ;
	    for(unsigned ipar=0; ipar<Derivatives::kCols; ++ipar) 
	      if( 0<= ( iactive = ielem.activeParIndex(ipar) ) ) 
		for(unsigned jpar=0; jpar<Derivatives::kCols; ++jpar) 
		  if( 0<= ( jactive = jelem.activeParIndex(jpar) ) ) {
		    assert(jactive > iactive  ) ;
		    halfD2Chi2dX2.fast(jactive,iactive) = im->second(ipar,jpar) ;
		  }
	  }
	}
      }
      
      // add the constraints
      size_t numConstraints = m_constrainttool->addConstraints(elements,equations,halfDChi2dX,halfD2Chi2dX2) ;
      
      logmessage << "Number of alignables with insufficient statistics: " << numExcluded << std::endl
		 << "Number of constraints: "                             << numConstraints << std::endl
		 << "Number of active parameters: "                       << numParameters << std::endl ;
      
      int numDoFs = halfDChi2dX.size() ;
      if (numDoFs < 50 ) {
	info() << "AlVec Vector    = " << halfDChi2dX << endmsg;
	info() << "AlSymMat Matrix = " << halfD2Chi2dX2      << endmsg;
      } else {
	info() << "Matrices too big to dump to std" << endmsg ;
      }
      
      // Tool returns H^-1 and alignment constants. Need to copy the 2nd derivative because we still need it.
      AlVec    scale(halfD2Chi2dX2.size()) ;
      AlSymMat covmatrix = halfD2Chi2dX2 ;
      AlVec    solution  = halfDChi2dX ;
      if (m_usePreconditioning) preCondition(elements,equations,solution,covmatrix, scale) ;
      bool solved = m_matrixSolverTool->compute(covmatrix, solution);
      if (m_usePreconditioning) postCondition(solution,covmatrix, scale) ;
      
      if (solved) {
	double deltaChi2 = solution * halfDChi2dX ; //somewhere we have been a bit sloppy with two minus signs!
	if (printDebug()) {
	  info() << "Solution = " << solution << endmsg ;
	  info() << "Covariance = " << covmatrix << endmsg ;
	}
	logmessage << "Alignment change chisquare/dof: " 
		   << deltaChi2 << " / " << numParameters << std::endl
		   << "Normalised alignment change chisquare: " << deltaChi2 / numParameters << std::endl;
	
	//m_dAlignChi2vsIterHisto->fill(iteration, deltaChi2) ;
	//m_nordAlignChi2vsIterHisto->fill(iteration, deltaChi2 /numParameters);
	
	m_constrainttool->printConstraints(solution, covmatrix, logmessage) ;
	
	if (printDebug()) debug() << "==> Putting alignment constants" << endmsg;
	size_t iElem(0u) ;
	double totalLocalDeltaChi2(0) ; // another figure of merit of the size of the misalignment.
	for (Elements::iterator it = elements.begin(); it != elements.end(); ++it, ++iElem) {
	  logmessage << "Alignable: " << it->name() << std::endl
		     << "Number of hits/outliers seen: " << equations.numHits(iElem) << " "
		     << equations.numOutliers(iElem) << std::endl ;
	  int offset = it->activeParOffset() ;
	  if( offset < 0 ) {
	    logmessage << "Not enough hits for alignment. Skipping update." << std::endl ;
	  } else {
	    AlParameters delta( solution, covmatrix, halfD2Chi2dX2, it->dofMask(), offset ) ;
	    AlParameters refdelta = it->currentActiveDelta() ;
	    //logmessage << delta ;
	    for(unsigned int iactive = 0u; iactive < delta.dim(); ++iactive) 
	      logmessage << std::setw(6)  << delta.activeParName(iactive) << ":" 
			 << " cur= " << std::setw(12) << refdelta.parameters()[iactive]
			 << " delta= " << std::setw(12) << delta.parameters()[iactive] << " +/- "
			 << std::setw(12) << AlParameters::signedSqrt(delta.covariance()[iactive][iactive]) 
			 << " gcc= " << delta.globalCorrelationCoefficient(iactive) << std::endl ;
	    double contributionToCoordinateError = delta.measurementCoordinateSigma( equations.weightR(iElem) ) ;
	    double coordinateError = std::sqrt(equations.numHits(iElem)/equations.weight(iElem)) ;
	    logmessage << "contribution to hit error (absolute/relative): "
		       << contributionToCoordinateError << " " << contributionToCoordinateError/coordinateError << std::endl ;
	    
	    // compute another figure of merit for the change in
	    // alignment parameters that does not rely on the
	    // correlations. this should also go into either
	    // AlParameters or AlEquation
	    const Gaudi::Matrix6x6& thisdChi2dAlpha2 = equations.elements()[iElem].M().find(iElem)->second ;
	    Gaudi::Vector6 thisAlpha = delta.parameterVector6() ;
	    double thisLocalDeltaChi2 = ROOT::Math::Dot(thisAlpha,thisdChi2dAlpha2 * thisAlpha) ;
	    logmessage << "local delta chi2 / dof: " << thisLocalDeltaChi2 << " / " << delta.dim() << std::endl ;
	    totalLocalDeltaChi2 += thisLocalDeltaChi2 ;
	    //double d2 = equations.elements()[iElem].M()[iElem]
	    
	    
	    // need const_cast because loki range givess access only to const values 
	    StatusCode sc = (const_cast<AlignmentElement&>(*it)).updateGeometry(delta) ;
	    if (!sc.isSuccess()) error() << "Failed to set alignment condition for " << it->name() << endmsg ;
	   
	    std::string name = it->name(); 
	    std::string dirname =  "element" + boost::lexical_cast<std::string>( it->index() ) + "/"; //name + "/" ;
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
	}
	logmessage << "total local delta chi2 / dof: " << totalLocalDeltaChi2 << " / " << numParameters << std::endl ;

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
    info() << logmessage.str() << endmsg ;
    info() << "\n";
    info() << "==> iteration " << iteration << " : Updated alignment conditions  : [";
    deltas.clear();
    getAlignmentConstants(elements, deltas);
    for (std::vector<double>::const_iterator i = deltas.begin(), iEnd = deltas.end(); i != iEnd; ++i) {
      info() << (*i) << (i != iEnd-1u ? ", " : "]");
    }
    info() << endmsg;
    m_logMessage << logmessage.str() ;
    return sc ;
  }
    
  void AlignUpdateTool::getAlignmentConstants(const Elements& rangeElements, AlignConstants& alignConstants) const {
    alignConstants.reserve(6*rangeElements.size()); // 6 constants * num elements
    for (Elements::const_iterator i = rangeElements.begin(); i != rangeElements.end(); ++i) {
      if (printDebug()) debug() << "Getting alignment constants for " << (*i) << endmsg;
      /// Get translations and rotations
	const std::vector<double>& translations = i->deltaTranslations();
	const std::vector<double>& rotations    = i->deltaRotations();
	/// Insert intitial constants (per element)
	  alignConstants.insert(alignConstants.end(), translations.begin(), translations.end());
	  alignConstants.insert(alignConstants.end(), rotations.begin()   , rotations.end()   );
    }
  }
}
