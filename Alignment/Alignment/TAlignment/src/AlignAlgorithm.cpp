// $Id: AlignAlgorithm.cpp,v 1.22 2008-02-04 16:09:03 wouter Exp $
// Include files
// from std
// #include <utility>
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDetDataSvc.h"

// from DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/GeometryInfoPlus.h"
#include "DetDesc/3DTransformationFunctions.h"
#include "DetDesc/GlobalToLocalDelta.h"
#include "DetDesc/Condition.h"
#include "DetDesc/AlignmentCondition.h"

// from Kernel
#include "Kernel/LHCbID.h"

// from BOOST
#include "boost/assign/std/vector.hpp"
#include "boost/assign/list_of.hpp"

// local
#include "AlignAlgorithm.h"
#include "AlParameters.h"
#include "ResidualCovarianceTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlignAlgorithm
//
// 2007-03-05 : Jan Amoraal
//-----------------------------------------------------------------------------

namespace {
  
 class Equations {
 public:
   Equations(size_t nElem)
     : m_nElem(nElem),
       m_v(nElem),
       m_numHits(nElem,0), m_weight(nElem,0), m_numTracks(0), m_totalChiSquare(0), m_totalNumDofs(0), m_numExternalHits(0)
   {}
   void clear() ;
   size_t nElem() const { return m_nElem; }
   Gaudi::Vector6&       V(int i) { return m_v[i]; }
   const Gaudi::Vector6& V(int i) const { return m_v[i]; }
   Gaudi::Matrix6x6& M(int i, int j) { assert(i<=j); return m_m[std::make_pair(i,j)]; }
   //FIXME: return proxy that fixes i<j...
   const Gaudi::Matrix6x6& M(int i, int j) const {
     assert(i<=j);
     return const_cast<Equations*>(this)->m_m[std::make_pair(i,j)];
   }
   size_t numHits(int i) const { return m_numHits[i] ; }
   double weight(int i) { return m_weight[i] ; }
  
   void addTrackSummary( double chisq, size_t ndof, size_t nexternal) {
     m_totalChiSquare +=chisq;
     m_totalNumDofs   +=ndof ;
     m_numExternalHits+=nexternal ;
     ++m_numTracks;
   }
   void addHitSummary(int index, double sigma) {
     ++m_numHits[index] ;
     m_weight[index] += 1/(sigma*sigma) ;
   }
   
   size_t numTracks() const { return m_numTracks ; }
   double totalChiSquare() const { return m_totalChiSquare ; }
   size_t totalNumDofs() const { return m_totalNumDofs ; }
   size_t numExternalHits() const { return m_numExternalHits ; }

  private:
   size_t                                         m_nElem ;
   std::vector<Gaudi::Vector6>                    m_v;
   std::map<std::pair<int,int>, Gaudi::Matrix6x6> m_m;
   std::vector<size_t>                            m_numHits ;
   std::vector<double>                            m_weight ;
   size_t                                         m_numTracks ;
   double                                         m_totalChiSquare ;
   size_t                                         m_totalNumDofs ;
   size_t                                         m_numExternalHits ;
 };
 
 void Equations::clear() 
 {
   m_v.clear(); m_v.resize(m_nElem); 
   m_m.clear(); 
   m_numHits.clear() ; m_numHits.resize(m_nElem,0) ;
   m_weight.clear() ; m_weight.resize(m_nElem,0) ;
   m_totalChiSquare = 0 ;
   m_numTracks = 0 ;
   m_totalNumDofs = 0 ;
   m_numExternalHits = 0 ;
 }

 Gaudi::Vector6 convert(const Gaudi::Matrix1x6& m) {
   Gaudi::Vector6 v;
   for (int i = 0; i < 6; ++i) v(i) = m(0,i);
   return v;
 }
 
 
 class Data {
 public:
   Data(const LHCb::Node& node, unsigned index, double r, const Gaudi::Matrix1x6& d)
     : m_id(&node),
       m_index(index),
       m_r(r),
       m_d(d) {}
   const LHCb::Node* id() const { return m_id ; }
   unsigned index() const { return m_index; }
   double r() const { return m_r; }
   const Gaudi::Matrix1x6& d() const { return m_d; }
 private:
   const LHCb::Node* m_id ;
   unsigned         m_index;
   double           m_r;
   Gaudi::Matrix1x6 m_d;
 };
 
 //FIXME: the next two stand-alone functions should move into AlVec & AlSymMat...
 template <typename T>
 void ass(AlVec& lhs, unsigned row, const T& rhs) {
   for (unsigned i = 0; i < T::kSize; ++i) lhs[row+i] = rhs(i);
 }
 
 template <typename T>
 void ass(AlSymMat& lhs, unsigned col, unsigned row, const T& rhs) {
   for (unsigned i = 0; i < T::kCols; ++i)
     for (unsigned j = 0; j < T::kRows; ++j)
       lhs[col+i][row+j] = rhs(i,j);
 }

};

using namespace LHCb;
using namespace boost;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( AlignAlgorithm );

AlignAlgorithm::AlignAlgorithm( const std::string& name,
				ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_iteration(0u),
    m_nIterations(0u),
    m_rangeElements(),
    m_initAlignConstants(),
    m_align(0),
    m_projSelector(0),
    m_matrixSolverTool(0),
    m_equations(0),
    m_constraints()
{
  declareProperty("NumberOfIterations"   , m_nIterations          = 10u                     );
  declareProperty("TracksLocation"       , m_tracksLocation       = TrackLocation::Default  );
  declareProperty("ProjectorSelector"    , m_projSelectorName     = "TrackProjectorSelector");
  declareProperty("MatrixSolverTool"     , m_matrixSolverToolName = "SpmInvTool"            );
  declareProperty("UseCorrelations"      , m_correlation          = true                    );
  declareProperty("UpdateInFinalize"     , m_updateInFinalize     = false                   );
  declareProperty("CanonicalConstraintStrategy", m_canonicalConstraintStrategy = CanonicalConstraintAuto ) ; 
  declareProperty("ConstrainZShearings"  , m_constrainZShearings = false ) ;
  declareProperty("MinNumberOfHits"      , m_minNumberOfHits = 1 ) ; 
  declareProperty("FillCorrelationsHistos" , m_fillCorrelationHistos = false ) ;
}

AlignAlgorithm::~AlignAlgorithm() {}


StatusCode AlignAlgorithm::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  /// Set histogram path
  if ("" == histoTopDir()) setHistoTopDir("Alignment/");

  /// Get pointer to incident service and add listener
  IIncidentSvc* incSvc = svc<IIncidentSvc>("IncidentSvc");
  if (!incSvc) return Error("==> Failed to retrieve IncidentSvc!", StatusCode::FAILURE);
  incSvc->addListener(this, "UpdateConstants");

  /// Get tool to align detector
  m_align = tool<IGetElementsToBeAligned>("GetElementsToBeAligned", this);
  if (!m_align) return Error("==> Failed to retrieve detector selector tool!", StatusCode::FAILURE);

  /// Get range  detector elements
  m_rangeElements = m_align->rangeElements();
  
  if (printDebug()) {
    debug() << "==> Got " << m_rangeElements.size() << " elements to align!" << endmsg;
    for(ElementRange::const_iterator i = m_rangeElements.begin(); i!= m_rangeElements.end(); ++i) {
      const AlDofMask& ownDoFMask = i->dofMask();
      debug() <<  "        " << (*i) << endmsg;
      const std::vector<std::string> dofs = boost::assign::list_of("Tx")("Ty")("Tz")("Rx")("Ry")("Rz");
      debug() << "DOFs: ";
      for (AlDofMask::const_iterator j = ownDoFMask.begin(), jEnd = ownDoFMask.end(); j != jEnd; ++j) {
        if ((*j)) info() << dofs.at(std::distance(ownDoFMask.begin(), j));
      }
      debug() << endmsg;
    }
  }
  
  m_equations = new Equations(m_rangeElements.size());
  m_equations->clear() ;

  // get constraints
  m_constraints = m_align->constraints();
  if (printDebug()) {
    debug() << "Number of constraints = " << m_constraints.size() << endmsg;
    unsigned nC = 0u;
    for (std::vector<std::vector<double> >::const_iterator i = m_constraints.begin(), iEnd = m_constraints.end(); i != iEnd;
         ++i, ++nC) {
      debug() << nC << ": f = [ ";
      for (std::vector<double>::const_iterator j = i->begin(), jEnd = i->end()-1; j != jEnd; ++j) {
        info()  << (*j) << " ";
      }
      debug() << " ]" << endmsg;
      debug() << "   f0 = " << i->back() << endmsg;
    }
  }

  /// Get projector selector tool
  m_projSelector = tool<ITrackProjectorSelector>(m_projSelectorName, "Projector", this);
  if (!m_projSelector) return Error("==> Failed to retrieve projector selector tool", StatusCode::FAILURE);

  ///Get matrix solver tool
  m_matrixSolverTool = tool<IAlignSolvTool>(m_matrixSolverToolName, "MatrixSolver", this);
  if (!m_matrixSolverTool) return Error("==> Failed to retrieve matrix solver tool", StatusCode::FAILURE);

  /// Monitoring
  /// Book residual histograms
  /// Residuals
  /// @todo: this should go into a monitoring tool
  info() << "booking histograms assuming " << m_nIterations << " iterations " << endmsg;
  m_totChi2vsIterHisto    = bookProfile1D(10, "Total chi2/dof vs iteration", -0.5, m_nIterations-0.5, m_nIterations);
  m_dChi2AlignvsIterHisto = bookProfile1D(20, "Chi2/dof of alignment change vs iteration", -0.5, m_nIterations-0.5, m_nIterations);
  for(ElementRange::const_iterator i = m_rangeElements.begin(); i!= m_rangeElements.end(); ++i) {
    const unsigned    indexI = i->index();
    const std::string name   = i->name();
    if (printDebug()) debug() << "Booking histograms for element " << name << " with index " << indexI <<endmsg;
    m_resHistos[indexI]     = book2D(1000u+indexI, "residual vs iteration for " + name,
                                     -0.5, m_nIterations-0.5, m_nIterations,
                                     -1.00 , +1.00, 100);
    m_pullHistos[indexI]     = book2D(2000u+indexI, "pull vs iteration for " + name,
                                      -0.5, m_nIterations-0.5, m_nIterations,
                                      -5.00 , +5.00, 100);
    m_nHitsHistos[indexI]     = book1D(3000u+indexI, "number of hits vs iteration for " + name,
                                       -0.5, m_nIterations-0.5, m_nIterations);
    if( m_fillCorrelationHistos ) {
      m_autoCorrHistos[indexI] = book2D(4000u+indexI, "hit autocorrelation in " + name + "  vs iteration ",
					-0.5, m_nIterations-0.5, m_nIterations,
					-0.5,+0.5,250);
      unsigned(indexJ) = indexI; 
      for(ElementRange::const_iterator j = i; j != m_rangeElements.end(); ++j, ++indexJ) {
	m_corrHistos[std::make_pair(indexI, indexJ)] = book2D((10000 + indexI) * (100 + indexJ),
							      "hit correlation of " + name + " with " + j->name()
							      + " vs iteration "
							      + ( i == j ? "(excluding autocorrelations!)" : "" ),
							      -0.5, m_nIterations-0.5, m_nIterations,
							      -1.0, +1.0, 250);
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode AlignAlgorithm::finalize() {
  if (m_updateInFinalize) update() ;
  return GaudiHistoAlg::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode AlignAlgorithm::execute() {

  // Get tracks
  Tracks* tracks = get<Tracks>(m_tracksLocation);
  if (printDebug()) debug() << "Number of tracks in container " + m_tracksLocation + ": " << tracks->size() << endmsg;
  /// Loop over tracks
  typedef Tracks::const_iterator TrackIter;
  for (TrackIter iTrack = tracks->begin(), iTrackEnd = tracks->end(); iTrack != iTrackEnd; ++iTrack) {
    //
    std::vector<Data> data;
    // Get nodes. Need them for measurements, residuals and errors
    const Nodes& nodes = (*iTrack)->nodes();
    if (printDebug()) debug() << "==> Found " << nodes.size() << " nodes"<< endmsg;

    // Loop over nodes and get measurements, residuals and errors
    typedef Nodes::const_iterator NodeIter;
    size_t numexternalhits(0) ;
    for (NodeIter node = nodes.begin(), end = nodes.end(); node != end; ++node) 
      if ((*node)->hasMeasurement()) {
	// Get measurement
	const Measurement& meas = (*node)->measurement();
	// Get element that belongs to this measurment
	const AlignmentElement* elem = m_align->findElement(meas.lhcbID());
	if (!elem) {
	  if (printVerbose()) verbose() << "==> Measurement not on a to-be-aligned DetElem " 
					<< meas.lhcbID() << endmsg;
	  ++numexternalhits ;
	  continue;
	}
    	
	const unsigned index = elem->index();
	if (printVerbose()) verbose() << "==> measure = " << meas.measure() << " id = " << meas.lhcbID() << " -> index = " 
				      << index << " -> " <<  elem->name() << endmsg;

	// Project measurement
	ITrackProjector* proj = m_projSelector->projector(meas);
	if (!proj) {
	  error() << "==> Could not get projector for selected measurement!" << endmsg;
	  continue;
	}
	double res  = (*node)->residual();
	double err  = (*node)->errMeasure();
	
	m_nHitsHistos[index]->fill(m_iteration);
	m_resHistos[index]->fill(m_iteration, res);
	m_pullHistos[index]->fill(m_iteration, res/err);
	// Get alignment derivatives
	LHCb::StateVector state((*node)->state().stateVector(),(*node)->state().z());
	Derivatives der = proj->alignmentDerivatives(state, meas, elem->pivotXYZPoint());
	// push back normalized residuals & derivatives;
	res /= err;
	der /= err;
	data.push_back(Data(**node, index, res, der));
	m_equations->addHitSummary( index, err ) ;
      }
    
    if (!data.empty()) {
      ResidualCovarianceTool cov;
      cov.compute(*(*iTrack));

      for (std::vector<Data>::const_iterator id = data.begin(), idEnd = data.end(); id != idEnd; ++id) {
	m_equations->V(id->index())               -= convert(id->r()*id->d()) ;
	m_equations->M(id->index(), id->index())  += (Transpose(id->d())*id->d());
	
	for (std::vector<Data>::const_iterator jd = data.begin(); jd != idEnd; ++jd) 
	  if( id == jd || ( m_correlation && id->index() <= jd->index() )) {
	    double c = cov.HCH_norm(*id->id(),*jd->id());
	    m_equations->M(id->index(), jd->index()) -= c * (Transpose(id->d())*jd->d());
	    
	    if( m_fillCorrelationHistos ) {
	      if (!( id->id() == jd->id())) {
		m_corrHistos[std::make_pair(id->index(), jd->index())]->
		  fill(m_iteration, c/std::sqrt(cov.HCH_norm(*id->id(), *id->id())*cov.HCH_norm(*jd->id(), *jd->id())));
	      } else {
		m_autoCorrHistos[id->index()]->fill(m_iteration, c);
	      }
	    }
	  }
      }
      // keep some information about the tracks that we have seen
      m_equations->addTrackSummary( (*iTrack)->chi2(), (*iTrack)->nDoF(), numexternalhits ) ;
    }
  }

  return StatusCode::SUCCESS;
}

inline std::ostream& operator<<(std::ostream& os, std::vector<bool>& mask) {
  for(size_t i=0; i<mask.size(); ++i)
    os << " " << mask[i] ;
  return os ;
}
inline std::ostream& operator<<(std::ostream& os, std::vector<double>& mask) {
  for(size_t i=0; i<mask.size(); ++i)
    os << " " << mask[i] ;
  return os ;
}

size_t AlignAlgorithm::addCanonicalConstraints(AlVec& dChi2dAlpha, AlSymMat& d2Chi2dAlpha2) const
{
  // This add lagrange multipliers to constrain the average rotation
  // and translation. Ideally, we could calculate this in ant
  // frame. In practise, the average depends on the reference frame in
  // which is is calculated. We will calculate a single 'pivot' point
  // to define the transform to the frame in which we apply the
  // constraint.
  //
  // In the neare future we also need to constrain the z-scale and the
  // z-shearing.

  double weight(0) ;
  Gaudi::XYZVector pivot ;
  size_t iElem(0) ;
  for( ElementRange::const_iterator it = m_rangeElements.begin(); it != m_rangeElements.end(); ++it, ++iElem) {
    double thisweight = m_equations->weight(iElem) ;
    weight += thisweight ;
    pivot += thisweight * Gaudi::XYZVector( it->pivotXYZPoint() ) ;
  }
  if(weight>0) pivot *= 1/weight ;
  Gaudi::Transform3D canonicalframe( pivot ) ;
  Gaudi::Transform3D canonicalframeInv = canonicalframe.Inverse() ;

  // add extra rows/columns
  size_t size = dChi2dAlpha.size() ;
  size_t numConstraints = 6 ;
  if(m_constrainZShearings) numConstraints += 3 ;
  info() << "Adding canonical constraints."
	 << " Pivot = " << pivot 
	 << " #constraints = " << numConstraints << endmsg ;
  
  dChi2dAlpha.reSize(size + numConstraints ) ;
  d2Chi2dAlpha2.reSize(size + numConstraints ) ;
  // Set all new elements to 0
  for(size_t i=size; i<size+numConstraints; ++i) {
    dChi2dAlpha[i] = 0 ;
    for(size_t j=0; j<=i; ++j)
      d2Chi2dAlpha2[i][j] = 0 ;
  }
  
  // Now fill the only patr that is nonzero: d2Chi2/dAlphadLambda. 
  iElem = 0 ;
  for( ElementRange::const_iterator it = m_rangeElements.begin(); it != m_rangeElements.end(); ++it, ++iElem) {
    // calculate the Jacobian for going from the 'alignment' frame to
    // the 'canonical' frame. This is the first place where we could
    // turn things around. It certainly works if the transforms are
    // just translations.
    Gaudi::Transform3D trans = canonicalframeInv * it->alignmentFrame() ;
    Gaudi::Matrix6x6 jacobian = AlParameters::jacobian( trans ) ;
    for(size_t i=0; i<6; ++i)
      for(size_t j=0; j<Derivatives::kCols; ++j)
	// and here comes the 2nd place we could do things entirely
	// wrong, but I think that this is right.
	d2Chi2dAlpha2[size+i][j+iElem*Derivatives::kCols] = jacobian(i,j) ;
    if(m_constrainZShearings) {
      double deltaZ = it->pivotXYZPoint().z() - pivot.z() ;
      // the 3 constraints are in this order: zx-shearing, zy-shearing and z-scale ('zz-shearing')
      for(size_t i=0; i<3; ++i)
	for(size_t j=0; j<Derivatives::kCols; ++j) 
	  d2Chi2dAlpha2[size+i+6][j+iElem*Derivatives::kCols] = deltaZ * jacobian(i,j) ;
    }
  }
  return numConstraints ;
}


//=============================================================================
//  Update
//=============================================================================
void AlignAlgorithm::update() {

  if( m_equations->nElem()==0 ) {
    warning() << "==> No elements to align." << endmsg ;
    return ;
  }
  
  info() << "==> Updating constants" << endmsg ;
  std::ostringstream logmessage ;
  logmessage << "********************* ALIGNMENT LOG ************************" << std::endl
	     << "Iteration: " << m_iteration << std::endl
	     << "Used " << m_equations->numTracks() << " tracks for alignment" << std::endl
	     << "Total chisquare/dofs = " << m_equations->totalChiSquare() << " / " << m_equations->totalNumDofs() << std::endl
	     << "Total number of hits in external detectors = " << m_equations->numExternalHits() << std::endl;

  m_totChi2vsIterHisto->fill(m_iteration, m_equations->totalChiSquare()/m_equations->totalNumDofs());
  
  if(printDebug()) {
    for (size_t i = 0; i < m_equations->nElem(); ++i) {
      for (size_t j = i; j < m_equations->nElem(); ++j) {
	debug() << "==> M["<<i<<","<<j<<"] = "      << m_equations->M(i,j) << endmsg;
      }
      debug() << "\n==> V["<<i<<"] = "    << m_equations->V(i) << endmsg;
    }
  }
    
  /// Take the gaudi vector and matix and fill AlVec and AlSym for ALL elements and ALL parameters
  size_t numParameters = Derivatives::kCols * m_equations->nElem() ;
  AlVec    tmpDerivatives(numParameters);
  AlSymMat tmpMatrix(numParameters);

  /// Loop over map of index to 2nd derivative matrix and 1st derivative vector
  for (unsigned i = 0u, iEnd = m_equations->nElem(); i < iEnd ; ++i) {
    ass(tmpDerivatives, i*Derivatives::kCols, m_equations->V(i));
    /// (assume upper triangular input!)
    for (unsigned j = i ; j < iEnd; ++j ) ass(tmpMatrix, i*Derivatives::kCols, j*Derivatives::kCols, m_equations->M(i,j));
  }

  // Add the canonical constraints, if needed
  size_t numConstraints(0) ;
  if( m_canonicalConstraintStrategy==CanonicalConstraintOn ||
      (m_canonicalConstraintStrategy==CanonicalConstraintAuto && m_equations->numExternalHits()==0) ) {
    numConstraints = addCanonicalConstraints(tmpDerivatives,tmpMatrix) ;
  }
  
  // Now reduce the large system to the active parameters

  // Create the dof mask and a map from AlignableElements to an
  // offset. The offsets are initialized with '-1', which signals 'not
  // enough hits'.
  size_t dim(0) ;
  size_t iElem(0) ;
  std::vector<int>  offsets(m_rangeElements.size(),-1) ;
  std::vector<bool> dofmask(tmpDerivatives.size(),true) ;
  for( ElementRange::const_iterator it = m_rangeElements.begin(); it != m_rangeElements.end(); ++it, ++iElem) 
    if( m_equations->numHits(iElem)>= m_minNumberOfHits) {
      offsets[iElem] = dim ;
      for(size_t ipar=0; ipar<Derivatives::kCols; ++ipar) dofmask[iElem*Derivatives::kCols+ipar] = it->dofMask()[ipar] ;
      dim += it->dofMask().nActive() ;
    } else {
      for(size_t ipar=0; ipar<Derivatives::kCols; ++ipar) dofmask[iElem*Derivatives::kCols+ipar] = false ;
    }

  // only add the contraints if they have non-zero derivatives. need
  // to replace the following with something more sensible:
  const double threshold = FLT_MIN ;
  size_t numRemovedConstraints(0) ;
  for(size_t i=0; i<numConstraints; ++i) {
    bool active=false ;
    for(size_t j=0; j<numParameters && !active; ++j)
      active = dofmask[j] && fabs(tmpMatrix[numParameters+i][j])>threshold ;
    dofmask[numParameters+i] = active ;
    numRemovedConstraints += active ? 0 : 1 ;
  }
  numConstraints -= numRemovedConstraints ;

  size_t nDOFs = std::count(dofmask.begin(), dofmask.end(), true) ;
  AlVec    derivatives(nDOFs);
  AlSymMat matrix(nDOFs);

  // Remove all parameters that are masked out.
  unsigned insert_i = 0u;
  unsigned insert_j = 0u;
  typedef std::vector<bool>::const_iterator boolIter; 
  for (std::vector<bool>::const_iterator i = dofmask.begin(), iEnd = dofmask.end(); i != iEnd; ++i) {
    if (*i) {
      const unsigned index_i = std::distance(boolIter(dofmask.begin()), i);
      derivatives[insert_i] = tmpDerivatives[index_i];
      for (std::vector<bool>::const_iterator j = i, jEnd = dofmask.end(); j != jEnd; ++j) {
        if (*j) {
          if (printDebug()) {
            debug() << "insert_i = " << insert_i << " insert_j = " << insert_j 
                    << " matrix[" << index_i << "][" << std::distance(boolIter(dofmask.begin()), j) << "]" 
                    << tmpMatrix[index_i][std::distance(boolIter(dofmask.begin()), j)] << endmsg;
          }
          matrix[insert_i][insert_j++] = tmpMatrix[index_i][std::distance(boolIter(dofmask.begin()), j)];
        }
      }
      insert_j = ++insert_i;
    }
  }

  if (printDebug()) {
    debug() << "Tmp AlVec Vector    = " << tmpDerivatives << endmsg;
    debug() << "Tmp AlSymMat Matrix = " << tmpMatrix      << endmsg;
  }

  logmessage << "Number of alignables with insufficient statistics: " << std::count(offsets.begin(),offsets.end(),-1) << std::endl
	     << "Number of constraints: " << numConstraints << std::endl
	     << "Number of active parameters: " << nDOFs-numConstraints << std::endl ;
  
  if(nDOFs < 50 ) {
    info() << "AlVec Vector    = " << derivatives << endmsg;
    info() << "AlSymMat Matrix = " << matrix      << endmsg;
  } else {
    info() << "Matrices too big to dump to std" << endmsg ;
  }

  // Tool returns H^-1 and alignment constants. Need to copy the 2nd derivative because we still need it.
  AlSymMat dChi2dX = matrix ;
  bool solved = m_matrixSolverTool->compute(matrix, derivatives);

  if (solved) {
    StatusCode sc = StatusCode::SUCCESS;
    double deltaChi2 = derivatives * dChi2dX * derivatives ;
    if(printDebug()) {
      info() << "Solution = " << derivatives << endmsg ;
      info() << "Covariance = " << matrix << endmsg ;
    }
    logmessage << "Chisquare/dof of the alignment change: " 
	       << deltaChi2 << " / " << nDOFs-numConstraints << std::endl ;
    m_dChi2AlignvsIterHisto->fill(m_iteration,deltaChi2) ;

    if (printDebug()) debug() << "==> Putting alignment constants" << endmsg;
    size_t iElem(0) ;
    for( ElementRange::const_iterator it = m_rangeElements.begin(); it != m_rangeElements.end(); ++it, ++iElem) 
      if( offsets[iElem]>=0 ) {
	AlParameters delta( derivatives, matrix, it->dofMask(), offsets[iElem] ) ;
	logmessage << "Alignable: " << it->name() << std::endl
		   << "Number of hits seen: " << m_equations->numHits(iElem) << std::endl
		   << delta ;
	StatusCode sc = it->updateGeometry(delta) ;
	if(!sc.isSuccess()) error() << "Failed to set alignment condition for " << it->name() << endmsg ; 
      }
  } else {
    error() << "Failed to solve system" << endmsg ;
  }
  logmessage << "********************* END OF ALIGNMENT LOG ************************" ;
  info() << logmessage.str() << endmsg ;
}

void AlignAlgorithm::reset() {
  if (printDebug()) debug() << "increasing iteration counter and resetting accumulated data..." << endmsg;
  /// increment iteration counter
  ++m_iteration;
  // clear derivatives and H maps
  m_equations->clear();
}

StatusCode AlignAlgorithm::queryInterface(const InterfaceID& id, void** ppI) {
  // check the placeholder:
  if (0 == ppI) return StatusCode::FAILURE;
  // check ID of the interface
  if (IIncidentListener::interfaceID() != id) return Algorithm::queryInterface(id, ppI);
  *ppI = static_cast<IIncidentListener*>(this);
  addRef();
  return StatusCode::SUCCESS;
}

void AlignAlgorithm::handle(const Incident& incident) {
  if ("UpdateConstants" == incident.type()) {
    update();
    reset();
  }
}

void AlignAlgorithm::getAlignmentConstants(const ElementRange& rangeElements, AlignConstants& alignConstants) const {
  alignConstants.reserve(6*rangeElements.size()); /// 6 constants * num elements
  for (ElementRange::const_iterator i = rangeElements.begin(); i != rangeElements.end(); ++i) {
    if (printDebug()) debug() << "Getting alignment constants for " << (*i) << endmsg;
    /// Get translations and rotations
    const std::vector<double>& translations = i->deltaTranslations();
    const std::vector<double>& rotations    = i->deltaRotations();
    /// Insert intitial constants (per element)
    alignConstants.insert(alignConstants.end(), translations.begin(), translations.end());
    alignConstants.insert(alignConstants.end(), rotations.begin()   , rotations.end()   );
  }
}
