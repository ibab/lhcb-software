// $Id: AlignAlgorithm.cpp,v 1.9 2007-10-17 12:08:23 lnicolas Exp $
// Include files
// from std
#include <utility>
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
#include "DetDesc/ParamException.h"

// from Kernel
#include "Kernel/LHCbID.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/Node.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackProjectorSelector.h"
#include "TrackInterfaces/ITrackProjector.h"

// from AlignmentInterfaces
#include "AlignmentInterfaces/IAlignDetectorSelector.h"
#include "AlignmentInterfaces/IAlignSolvTool.h"

// from BOOST
#include "boost/tuple/tuple.hpp"
#include "boost/assign/list_of.hpp"


// AIDA
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram1D.h"

// local
#include "AlignAlgorithm.h"
#include "ResidualCovarianceTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlignAlgorithm
//
// 2007-03-05 : Jan Amoraal
//-----------------------------------------------------------------------------

namespace {

   class Equations {
   public:
       Equations(size_t nElem) : m_nElem(nElem), m_v(nElem) {}
       size_t nElem() const { return m_nElem; }
       Gaudi::Vector6&   V(int i) { return m_v[i]; }
       const Gaudi::Vector6&   V(int i) const { return m_v[i]; }
       Gaudi::Matrix6x6& M(int i, int j) { assert(i<=j); return m_m[std::make_pair(i,j)]; }
       //FIXME: return proxy that fixes i<j...
       const Gaudi::Matrix6x6& M(int i, int j) const { assert(i<=j); return const_cast<Equations*>(this)->m_m[std::make_pair(i,j)]; }
       void clear() { m_v.clear(); m_v.resize(m_nElem); m_m.clear(); }
   private:
       size_t m_nElem ;
       std::vector<Gaudi::Vector6>                   m_v;
       std::map<std::pair<int,int>,Gaudi::Matrix6x6> m_m;
   };

   Gaudi::Vector6 convert(const Gaudi::Matrix1x6& m) {
        Gaudi::Vector6 v;
        for (int i=0;i<6;++i) v(i)=m(0,i);
        return v;
   };


   class Data {
   public:
        Data(LHCb::LHCbID id,unsigned index, double r, const Gaudi::Matrix1x6& d) 
        : m_id(id),m_index(index),m_r(r),m_d(d) {}
        LHCb::LHCbID id() const { return m_id ; }
        unsigned index() const { return m_index; }
        double r() const { return m_r; }
        const Gaudi::Matrix1x6& d() const { return m_d; }
   private:
        LHCb::LHCbID m_id;
        unsigned m_index;
        double m_r;
        Gaudi::Matrix1x6 m_d;
   };


   //FIXME: the next two stand-alone functions should move into AlVec & AlSymMat...
   template <typename T>
   void ass(AlVec& lhs, unsigned row, const T& rhs) {
        for (unsigned i=0;i<T::kSize;++i) lhs[row+i] = rhs(i);
   }

   template <typename T>
   void ass(AlSymMat& lhs, unsigned col, unsigned row, const T& rhs) {
        for (unsigned i=0;i<T::kCols;++i) 
            for (unsigned j=0;j<T::kRows;++j) 
                lhs[col+i][row+j] = rhs(i,j);
   }
   
   std::string label(const DetectorElement& elem) {
        using std::vector;
        using std::string;
        using boost::assign::list_of;
        vector<string> prefixes = list_of( string("/dd/Structure/LHCb/") )
                                         ( string("AfterMagnetRegion/")  )
                                         ( string("BeforeMagnetRegion/") );
        string n = elem.name();
        for (vector<string>::const_iterator i= prefixes.begin();i!=prefixes.end();++i) {
           string::size_type m = n.find(*i);
           if (m!=string::npos) n.erase(m,m+i->size());
        }
        return n;
   }
};

using namespace LHCb;
using namespace boost;
using boost::tie;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( AlignAlgorithm );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlignAlgorithm::AlignAlgorithm( const std::string& name,
                                ISvcLocator* pSvcLocator)
                                : GaudiHistoAlg( name , pSvcLocator ),
                                m_iteration(0u),
                                m_elements(),
                                m_nTracks(0u),
                                m_alignDetector(0),
                                m_projSelector(0),
                                m_matrixSolverTool(0),
                                m_equations(0)
{
  declareProperty("NumberOfIterations"   , m_nIterations          = 10u                     );
  declareProperty("AlignDetectorSelector", m_alignDetectorName    = "AlignOTDetector"       );
  declareProperty("TracksLocation"       , m_tracksLocation       = TrackLocation::Default  );
  declareProperty("ProjectorSelector"    , m_projSelectorName     = "TrackProjectorSelector");
  declareProperty("MatrixSolverTool"     , m_matrixSolverToolName = "SpmInvTool"            ); 
  declareProperty("UseCorrelations"      , m_correlation = true            ); 
} 
//=============================================================================
// Destructor
//=============================================================================
AlignAlgorithm::~AlignAlgorithm() {} 

//=============================================================================
// Initialization
//=============================================================================
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
  m_alignDetector = tool<IAlignDetectorSelector>(m_alignDetectorName, "Detector", this);
  if (!m_alignDetector) return Error("==> Failed to retrieve detector selector tool!", StatusCode::FAILURE);
  
  /// Get detector elements
  m_elements = m_alignDetector->getDetectorElements();
  debug() << "Got " << m_elements.size() << " elements to align" << endmsg;
  for (Elements::const_iterator i=m_elements.begin(); i!=m_elements.end();++i) {
          info() << "==> " << (i-m_elements.begin()) << " : " << (*i)->name() << endmsg;
  }

  m_equations = new Equations(m_elements.size());

  // get constraints
  m_constraints = m_alignDetector->constraints();
  info() << "Number of constraints = " << m_constraints.size() << endmsg;
  unsigned nC = 0u;
  for (std::vector<std::vector<double> >::const_iterator i = m_constraints.begin(), iEnd = m_constraints.end(); i != iEnd; 
       ++i, ++nC) {
    info() << nC << ": f = [ ";
    for (std::vector<double>::const_iterator j = (*i).begin(), jEnd = (*i).end()-1; j != jEnd; ++j) {
    info()  << (*j) << " ";
    }
    info() << " ]" << endmsg;
    info() << "   f0 = " << (*i).back() << endmsg;
  }  

  /// Get pivot points
  sc = getPivotPoints(m_elements, m_pivotPoints);
  if (sc.isFailure()) return Error("==> Failed to get pivot points or not defined", StatusCode::FAILURE);
  
  /// Get projector selector tool
  m_projSelector = tool<ITrackProjectorSelector>(m_projSelectorName, "Projector", this);
  if (!m_projSelector) return Error("==> Failed to retrieve projector selector tool", StatusCode::FAILURE);
  
  ///Get matrix solver tool
  m_matrixSolverTool = tool<IAlignSolvTool>(m_matrixSolverToolName, "MatrixSolver", this);
  if (!m_matrixSolverTool) return Error("==> Failed to retrieve matrix solver tool", StatusCode::FAILURE);

  /// Monitoring
  /// Book residual histograms
  /// Residuals
  //unsigned int nE = 0u; 
  info() << "booking histograms assuming " << m_nIterations << " iterations " << endmsg;
  unsigned(nE) = 0u;
  for (ElemIter i = m_elements.begin(), end = m_elements.end(); 
       i != end; ++nE, ++i) {
    m_resHistos[nE] = book2D(1000u+nE, "residual vs iteration for " + label(**i),
                           -0.5, m_nIterations-0.5, m_nIterations, 
                           -1.00 , +1.00, 100); 
    m_pullHistos[nE] = book2D(2000u+nE, "pull vs iteration for " + label(**i),
                           -0.5, m_nIterations-0.5, m_nIterations, 
                           -5.00 , +5.00, 100); 
    m_nhitHistos[nE] = book1D(3000u+nE, "number of hits vs iteration for " + label(**i),
                           -0.5, m_nIterations-0.5, m_nIterations);

    m_autoCorrHistos[nE] = book2D(4000u+nE, "hit autocorrelation in " + label(**i) + "  vs iteration ",
                           -0.5, m_nIterations-0.5, m_nIterations,
                           -0.5,+0.5,250);
    unsigned(nj) = nE;
    for (ElemIter j=i;j!=end;++j,++nj) {
        m_corrHistos[std::make_pair(nE,nj)] = book2D(10000+nE*100+nj,"hit correlation of " + label(**i) + " with " + label(**j) + " vs iteration " + ( i==j?"(excluding autocorrelations!)":"" ) ,
                                            -0.5, m_nIterations-0.5, m_nIterations,
                                            -1.0,+1.0,250);
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode AlignAlgorithm::execute() {

  // Get tracks 
  Tracks* tracks = get<Tracks>(m_tracksLocation);
  if (printDebug()) debug() << "Number of tracks in container " + m_tracksLocation + ": " << tracks->size() << endmsg;
  m_nTracks += tracks->size();
  /// Loop over tracks
  typedef Tracks::const_iterator TrackIter; 
  for (TrackIter iT = tracks->begin(), iTend = tracks->end(); iT != iTend; ++iT) {
    std::vector<Data> data;
    // Get nodes. Need them for measurements, residuals and errors
    const Nodes& nodes = (*iT)->nodes();
    if (printDebug()) debug() << "==> Found " << nodes.size() << " nodes"<< endmsg;

    // Loop over nodes and get measurements, residuals and errors
    typedef Nodes::const_iterator NodeIter;
    for (NodeIter node = nodes.begin(), end = nodes.end(); node != end; ++node) {
      if (!(*node)->hasMeasurement()) {
        debug() << "==> Node has no measurement" << endmsg;
        continue;
      }
      /// Get measurement
      const Measurement& meas = (*node)->measurement();
      /// Check if LHCb is valid and get index associated to DetElem
      bool valid(false); unsigned int index(0);
      tie(valid,index) = m_alignDetector->index(meas.lhcbID());
      if (!valid) {
        debug() << "==> Measurement not on a to-be-aligned DetElem" << endmsg;
        continue;
      }
      if (printDebug()) {
          debug() << "==> " << meas.lhcbID() << " -> index = " << index << " -> " <<  m_elements[index]->name() << endmsg;
      }
      ITrackProjector* proj = m_projSelector->projector(meas);
      if (!proj) {
        debug() << "==> Could not get projector for selected measurement!" << endmsg;
        continue;
      }
      double res  = (*node)->residual();
      double err  = (*node)->errMeasure();

      m_nhitHistos[index]->fill(m_iteration); 
      m_resHistos[index]->fill(m_iteration, res); 
      m_pullHistos[index]->fill(m_iteration, res/err); 
      // Get projector for this measurement
      Derivatives der = proj->alignmentDerivatives(meas, m_pivotPoints[index]);
      // push back normalized residuals & derivatives;
      res/=err;
      der/=err;
      data.push_back( Data(meas.lhcbID(),index,res,der) );
    }

    ResidualCovarianceTool cov;
    cov.compute(**iT);


    for (std::vector<Data>::const_iterator id=data.begin();id!=data.end();++id) {
        m_equations->V(id->index())             -= convert(id->r()*id->d()) ; 
        m_equations->M(id->index(),id->index()) += (Transpose(id->d())*id->d());

        for (std::vector<Data>::const_iterator jd=id;jd!=data.end();++jd) {
           if ( m_correlation || jd==id) {
               // make sure we go for the upper triangle...
               std::vector<Data>::const_iterator i(id),j(jd);
               if (i->index()>j->index()) std::swap(i,j);
               double c = cov.HCH_norm(i->id(),j->id());
               m_equations->M(i->index(),j->index()) -= c * Transpose(i->d())*j->d();

               if ( !( i->id()==j->id())) {
                    m_corrHistos[std::make_pair(i->index(),j->index())]->fill(m_iteration,c/sqrt(cov.HCH_norm(i->id(),i->id())*cov.HCH_norm(j->id(),j->id())));
               } else {
                    m_autoCorrHistos[i->index()]->fill(m_iteration,c);
               }
           }
        }
    }

  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Update
//=============================================================================
void AlignAlgorithm::update() {
  
  info() <<   "==> Updating constants" 
         << "\n==> Used " << m_nTracks << " tracks for alignment" << endmsg;
  
  for (size_t i = 0; i < m_equations->nElem(); ++i) {
    for (size_t j = i; j < m_equations->nElem(); ++j) {
       info() << "==> M["<<i<<","<<j<<"] = "      << m_equations->M(i,j) << endmsg;
    }
    info() << "\n==> V["<<i<<"] = "    << m_equations->V(i) << endmsg;
  }  
    

  AlVec    derivatives(Derivatives::kCols*m_elements.size() + m_constraints.size());
  AlSymMat      matrix(Derivatives::kCols*m_elements.size() + m_constraints.size());

  /// Loop over map of index to 2nd derivative matrix and 1st derivative vector 
  for (unsigned(i) = 0u, iEnd = m_equations->nElem(); i < iEnd ; ++i) {                                           
    ass(derivatives,i*Derivatives::kCols,m_equations->V(i));
    /// (assume upper triangular input!)
    for (unsigned(j) = i ; j < iEnd ; ++j) ass(matrix,i*Derivatives::kCols,j*Derivatives::kCols,m_equations->M(i,j));
  }

  /// Add constraints
  if (!m_constraints.empty()) {
    for (unsigned(i) = Derivatives::kCols*m_elements.size(), iEnd = Derivatives::kCols*m_elements.size() + m_constraints.size(), nC=0u;  
         i != iEnd; ++i, ++nC) {
      const std::vector<double>& constraint = m_constraints.at(nC);
      derivatives[i] = constraint.back();
      unsigned cEntry = 0u;
      for (std::vector<double>::const_iterator j = constraint.begin(), jEnd = constraint.end()-1; j != jEnd; ++j, ++cEntry) {
        matrix[i][cEntry] = (*j);
      }
    }
  }

  info() << "AlVec Vector = " << derivatives << endmsg;
  info() << "AlSymMat Matrix = " << matrix << endmsg;
  
  /// Tool returns H^-1 and alignment constants 
  bool solved = m_matrixSolverTool->compute(matrix, derivatives);  
  if (solved) {
    info() << "Solution = " << derivatives << endmsg;
    /// Update alignment iff we've solved Ax=b 
    debug() << "==> Putting alignment constants" << endmsg;
    StatusCode sc = putAlignmentConstants(m_elements, derivatives);
    if (sc.isFailure()) error() << "==> Failed to put alignment constants" << endmsg;
  } else {
    error() << "Failed to solve system" << endmsg;
  }
}

void AlignAlgorithm::reset() {
  debug() << "increasing iteration counter and resetting accumulated data..." << endmsg;
  /// increment iteration counter
  ++m_iteration;
  /// reset track counters
  m_nTracks = 0u;
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

StatusCode AlignAlgorithm::getPivotPoints(const Elements& elements, PivotPoints& pivotPoints) const
{
  const AlignmentCondition *alignCond;
  pivotPoints.reserve(elements.size());
  for (ElemIter iE = elements.begin(), iEend = elements.end(); iE != iEend; ++iE) {
    if (printDebug()) debug() << (*iE)->name() << endmsg;
    /// Get alignment condition
    alignCond = (*iE)->geometry()->alignmentCondition();
    if (!alignCond) return Error("==> Failed to get alignment condition!", StatusCode::FAILURE);
    /// Get pivot point
    /// Set default point
    Gaudi::XYZPoint pivotPoint = (*iE)->geometry()->toGlobal(Gaudi::XYZPoint(0.0, 0.0, 0.0));
    try { /// If pivot is defined for this det elem get it
      std::vector<double> pivot = alignCond->paramVect<double>("pivotXYZ");
      pivotPoint = Gaudi::XYZPoint(pivot[0], pivot[1], pivot[2]);
      info() << "==> Pivot point explicity defined for " << (*iE)->name() 
             << ". Using " << pivotPoint << " as pivot" << endmsg; 
    } catch (ParamException& p) { /// Print info and use default
      info() << "==> Pivot point not explicity defined for " << (*iE)->name() 
             << ". Using global centre of element, " << pivotPoint << ", as pivot point" << endmsg; 
    }
    /// Insert pivot point
    pivotPoints.push_back(pivotPoint);

  }
  return StatusCode::SUCCESS;
}

StatusCode AlignAlgorithm::getAlignmentConstants(const Elements& elements, AlignConstants& alignConstants) const
{
  const AlignmentCondition *alignCond;
  alignConstants.reserve(6*elements.size()); /// 9 constants * num elements
  for (ElemIter iE = elements.begin(), iEend = elements.end(); iE != iEend; ++iE) {
    if (printDebug()) debug() << (*iE)->name() << endmsg;
    /// Get alignment condition
    alignCond = (*iE)->geometry()->alignmentCondition();
    if (!alignCond) return Error("==> Failed to get alignment conditions", StatusCode::FAILURE);
    /// Get translations and rotations
    std::vector<double> translations = alignCond->paramVect<double>("dPosXYZ");
    std::vector<double> rotations    = alignCond->paramVect<double>("dRotXYZ");
    /// Insert intitial constants (per element)
    alignConstants.insert(alignConstants.end(), translations.begin(), translations.end());
    alignConstants.insert(alignConstants.end(), rotations.begin(), rotations.end());
  }
  return StatusCode::SUCCESS;
}

StatusCode AlignAlgorithm::putAlignmentConstants(const Elements& elements, const AlVec& alignConstants) const
{
  if (printDebug()) debug() << "Size of alignment constants vector is " << alignConstants.size() << endmsg;
  unsigned(nE) = 0u;
  for (ElemIter iE = elements.begin(), iEend = elements.end(); iE != iEend; ++nE, ++iE) {
    if (printDebug()) { debug() << (*iE)->name() 
                                << "\n current local delta : \n" << (*iE)->geometry()->ownToOffNominalMatrix()
                                << " current local centre (0,0,0) in global coordinates: " 
                                << (*iE)->geometry()->toGlobal(Gaudi::XYZPoint(0.0, 0.0, 0.0)) << endmsg;
    }
    /// 3 translations
    std::vector<double> translations;
    for (unsigned(i) = 0u; i < 3u; ++i) translations.push_back(alignConstants[nE*Derivatives::kCols + i]);
    /// 3 rotations
    std::vector<double> rotations;
    for (unsigned(i) = 3u; i < 6u; ++i) rotations.push_back(alignConstants[nE*Derivatives::kCols + i]);
    /// Pivot point
    Gaudi::XYZPoint p = m_pivotPoints[nE];
    std::vector<double> pivot = boost::assign::list_of(p.x())(p.y())(p.z());
    /// Get transformation matrix to go from old delta to new delta
    Gaudi::Transform3D globalDeltaMatrix = DetDesc::localToGlobalTransformation(translations, rotations, pivot);
    /// Transform global deltas to new local deltas (takes into account current local delta!!)
    Gaudi::Transform3D localDeltaMatrix  = DetDesc::localDeltaMatrix((*iE), globalDeltaMatrix);
    if (printDebug()) {
       debug() << " align transform in global frame: \n" << globalDeltaMatrix
               << "\n updated delta: \n" << localDeltaMatrix << endmsg;
    }
    /// Update position of detector elements
    StatusCode sc = (*iE)->geometry()->ownToOffNominalMatrix(localDeltaMatrix); 
    if (sc.isFailure()) return Error("Failed to set alignment conditions", StatusCode::FAILURE);
  }
  return StatusCode::SUCCESS;
}
