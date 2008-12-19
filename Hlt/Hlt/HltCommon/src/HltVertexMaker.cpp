// $Id: HltVertexMaker.cpp,v 1.26 2008-12-19 17:32:08 graven Exp $
// Include files 


// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "HltVertexMaker.h"
#include "HltBase/EParser.h"
#include "HltBase/ESequences.h"

#include "boost/lambda/lambda.hpp"
#include "boost/lambda/construct.hpp"

//-----------------------------------------------------------------------------
// Implementation file for class : HltVertexMaker
//
// 2006-02-21 : Hugo Ruiz
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltVertexMaker1 );
DECLARE_ALGORITHM_FACTORY( HltVertexMaker2 );

using namespace LHCb;
using namespace boost::lambda;
// #namespace bl = boost::lambda;

template <typename Selections>
HltVertexMaker<Selections>::combinatorics_engine::combinatorics_engine(HltVertexMaker::combinatorics_engine::iterator begin1,
                                                           HltVertexMaker::combinatorics_engine::iterator end1,
                                                           HltVertexMaker::combinatorics_engine::iterator begin2,
                                                           HltVertexMaker::combinatorics_engine::iterator end2,bool merge) 
    : m_range1(begin1,end1)
    , m_merge(merge)
{
    if (merge) { 
        m_mergedList.insert(m_mergedList.end(),begin1,end1);
        m_mergedList.insert(m_mergedList.end(),begin2,end2);
        m_range2 = std::make_pair(m_mergedList.begin(),m_mergedList.end());
    } else {
        m_range2 = std::make_pair(begin2,end2);
    }
    m_current = std::make_pair( m_range1.first , m_range2.first );
    if (merge) inc2();
}

template <typename Selections>
HltVertexMaker<Selections>::combinatorics_engine::combinatorics_engine(iterator begin1,iterator end1)
            : m_range1(begin1,end1)
            , m_range2(begin1,end1)
            , m_current(begin1,begin1) 
            , m_merge(true)
{ 
    inc2(); 
}

template <typename Selections>
typename HltVertexMaker<Selections>::combinatorics_engine&
HltVertexMaker<Selections>::combinatorics_engine::operator++() {
    inc2(); 
    if (atEnd2()) {
        inc1();
        if (m_merge && m_range2.first!=m_range2.second) { 
            ++m_range2.first;
        }
        m_current.second=m_range2.first;
    }
    return *this;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
template <typename SelectionContainer>
HltVertexMaker<SelectionContainer>::HltVertexMaker( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selections(*this)
  , m_counterCombinations("N combinations")
{
  declareProperty("CheckForOverlaps", m_checkForOverlaps = false );
  declareProperty("FilterDescriptor", m_filterDescriptor);
  m_selections.declareProperties();
}

//=============================================================================
// Destructor
//=============================================================================
template <typename Selections>
HltVertexMaker<Selections>::~HltVertexMaker() {
  std::for_each(m_functions.begin(),m_functions.end(), delete_ptr());
  std::for_each(m_filters.begin(),m_filters.end(), delete_ptr());
};

//=============================================================================
// Initialization
//=============================================================================

template <typename Selections>
StatusCode HltVertexMaker<Selections>::initialize() {
  StatusCode sc = HltAlgorithm::initialize();

  m_selections.retrieveSelections();

  typedef IBiFunctionFactory<LHCb::Track,LHCb::Track> IBiTrackFactory;
  IBiTrackFactory* factory = 
    tool<IBiTrackFactory>("HltTrackBiFunctionFactory",this);

  const std::vector<std::string>& hdes = m_filterDescriptor.value();
  for (std::vector<std::string>::const_iterator it = hdes.begin();
       it != hdes.end(); it++){
    std::string filtername = *it;
    std::string funname = "";
    std::string mode = "";
    double x0 = -1.e6;
    double xf =  1.e6;
    bool ok = EParser::parseFilter(filtername,funname,mode,x0,xf);
    Assert(ok," initialize() not able to parse filtername "+filtername).ignore();
    
    m_filterNames.push_back(funname);

    int id = hltInfoID(funname);
    Hlt::TrackBiFunction* fun = factory->function(funname);
    Assert( 0 !=  fun,  " initialize() function no created"+funname).ignore();
    m_functions.push_back(fun);
    m_filterIDs.push_back(id);


    Hlt::Filter* fil = (  (mode == "<") ? (Hlt::Filter*)new zen::less<double>(x0) :
                          (mode == ">") ? (Hlt::Filter*)new zen::greater<double>(x0) : 
                                          (Hlt::Filter*)new zen::in_range<double>(x0,xf) );

    Assert( 0 !=  fil,  " initialize() filter no created"+filtername).ignore();
    m_filters.push_back(fil);
    
    m_tcounters.push_back(0);   

    if (produceHistos()) {
      m_histos.push_back( initializeHisto(funname,0.,100.,100) );
    }

    if (m_debug) debug() << " filter " << filtername << " " << id << " "
                         << mode << x0 << "," << xf << endreq;
  }

  m_selections.registerSelection();
  
  saveConfiguration();

  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
template <typename Selections>
StatusCode HltVertexMaker<Selections>::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  if ( m_debug ) debug() << "HltVertexMaker: Execute" << endmsg;

  RecVertices* overtices = new RecVertices();
  put(overtices,"Hlt/Vertex/"+m_selections.output()->id().str());

  for (combinatorics_engine combinations = combine();!combinations.end();++combinations) {
      const LHCb::Track* track1 = combinations().first;
      const LHCb::Track* track2 = combinations().second;
      assert(track1!=0);
      assert(track2!=0);
      verbose() << " track 1 " << track1->key() << " " << track1->slopes() << endreq;
      verbose() << " track 2 " << track2->key() << " " << track2->slopes() << endreq;
      
      // can not be the same track
      if (track1 == track2) continue;

      m_counterCombinations.increase();

      // Check for segment overlaps
      bool rejected = ( m_checkForOverlaps && HltUtils::matchIDs(*track1,*track2) );
      
      verbose() << " rejected due to overlaps?" << rejected<< endreq;
      if (rejected) continue;

      m_vals.clear();
      for (size_t i = 0; i < m_functions.size(); ++i) {
        verbose() << " filter " << m_filterNames[i] << endreq;
        double val = (*m_functions[i])(*track1,*track2);
        rejected = !(*m_filters[i])(val);
        verbose() << " value " << val << " rejected? " << rejected << endreq;
        fillHisto(*m_histos[i],val,1.);
        if (rejected) break;
        m_tcounters[i] += 1;
        m_vals.push_back(val);
      }
      
      if (rejected) continue;

      verbose()<<" pair found [0] "<<track1->key()<<" "<<track1->slopes() << endreq;
      verbose()<<" pair found [1] "<<track2->key()<<" "<<track2->slopes() << endreq;
      // Write vertex
      LHCb::RecVertex* sv = new RecVertex();
      m_makeVertex(*track1,*track2,*sv);
      m_selections.output()->push_back(sv);
      overtices->insert(sv);
      for (size_t i = 0; i < m_filterIDs.size(); ++i) {
        verbose()<< " info " << m_filterIDs[i]<<" = "<<m_vals[i] << endreq;
        sv->addInfo(m_filterIDs[i],m_vals[i]);
      }
      if (m_debug) printInfo(" make vertex ",*sv);
  } 
  return sc;
}

template <typename Selections>
void HltVertexMaker<Selections>::saveConfiguration() {
  HltAlgorithm::saveConfiguration();

  const std::vector<std::string>& values = m_filterDescriptor.value();
  confregister("Filters",values);
  info() << " HLT filters " << values << endreq;
}


//=============================================================================
//  Finalize
//=============================================================================
template <typename Selections>
StatusCode HltVertexMaker<Selections>::finalize() {

  
  info() << " N Combinations " << m_counterCombinations << endreq;
  for (size_t i = 0; i < m_filterNames.size(); ++i) {
    std::string title =  " accepted combinations " 
      + m_filterNames[i] + " / total ";
    infoSubsetEvents(m_tcounters[i],m_counterCombinations,title);
  }

  return HltAlgorithm::finalize(); 
  
}

//=============================================================================
////////////////////////// TrackContainer -> VertexContainer

HltVertexMaker1::HltVertexMaker1( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : HltVertexMaker<Hlt::SelectionContainer2<LHCb::RecVertex,LHCb::Track> >( name , pSvcLocator )
{ 
}

HltVertexMaker<Hlt::SelectionContainer2<LHCb::RecVertex,LHCb::Track> >::combinatorics_engine
HltVertexMaker1::combine() {
    typedef HltVertexMaker<container_type>::combinatorics_engine engine_type;
    return engine_type(m_selections.input<1>()->begin(),
                       m_selections.input<1>()->end());
}
////////////////////////// TrackContainer + TrackContainer -> VertexContainer

HltVertexMaker2::HltVertexMaker2( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : HltVertexMaker<Hlt::SelectionContainer3<LHCb::RecVertex,LHCb::Track,LHCb::Track> >( name , pSvcLocator )
{
    declareProperty("DoMergeInputs", m_doMergeInputs = true);
}

HltVertexMaker<Hlt::SelectionContainer3<LHCb::RecVertex,LHCb::Track,LHCb::Track> >::combinatorics_engine
HltVertexMaker2::combine() {
    typedef HltVertexMaker<container_type>::combinatorics_engine engine_type;
    return engine_type(m_selections.input<1>()->begin(), m_selections.input<1>()->end(),
                       m_selections.input<2>()->begin(), m_selections.input<2>()->end(),
                       m_doMergeInputs);
}
