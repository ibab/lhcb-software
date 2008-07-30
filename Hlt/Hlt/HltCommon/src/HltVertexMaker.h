// $Id: HltVertexMaker.h,v 1.9 2008-07-30 13:37:33 graven Exp $
#ifndef HLTVERTEXMAKER_H 
#define HLTVERTEXMAKER_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/IBiFunctionFactory.h"
#include "HltFunctions.h"
#include <utility>

/** @class HltVertexMaker HltVertexMaker.h
 *  
 *
 *  @author Hugo Ruiz
 *  @date   2006-02-21
 */

//TODO: template on selectioncontainer...

template <typename SelectionContainer> 
class HltVertexMaker : public HltAlgorithm {
 public: 
   typedef SelectionContainer container_type;

  /// Standard constructor
  HltVertexMaker( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~HltVertexMaker( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
    
 protected:

  class combinatorics_engine {
  public: 
        typedef std::vector<LHCb::Track*>::iterator iterator;
        combinatorics_engine(iterator begin1,iterator end1,
                             iterator begin2,iterator end2,bool merge) ;
        combinatorics_engine(iterator begin1,iterator end1);
        std::pair<LHCb::Track*,LHCb::Track*> operator()() { return std::make_pair(*m_current.first,*m_current.second); }
        combinatorics_engine& operator++();
        bool end() const  { return atEnd1()||atEnd2(); }
  private:
        std::pair<iterator,iterator> m_range1,m_range2;
        std::pair<iterator,iterator> m_current;
        std::vector<LHCb::Track*> m_mergedList;
        bool m_merge;
        void inc1()    { if (m_current.first !=m_range1.second ) ++m_current.first; }
        void inc2()    { if (m_current.second!=m_range2.second ) ++m_current.second; }
        bool atEnd2() const { return m_current.second==m_range2.second; }
        bool atEnd1() const { return m_current.first==m_range1.second; }
  };

  virtual combinatorics_engine combine() = 0;
  SelectionContainer m_selections;
private:
  void saveConfiguration();
  bool m_checkForOverlaps;


  StringArrayProperty m_filterDescriptor;

  Hlt::Counter m_counterCombinations;

  std::vector<std::string> m_filterNames;
  std::vector<int> m_filterIDs;
  std::vector<Hlt::TrackBiFunction*> m_functions;
  std::vector<Hlt::Filter*> m_filters;
  std::vector<Hlt::Histo*> m_histos;
  std::vector<int> m_tcounters;

  std::vector<double> m_vals;
  Hlt::VertexCreator m_makeVertex;
};


class HltVertexMaker1 : public HltVertexMaker<Hlt::SelectionContainer2<LHCb::RecVertex,LHCb::Track> >{
    public:
        HltVertexMaker1( const std::string& name, ISvcLocator* pSvcLocator);
        combinatorics_engine combine(); 
};

class HltVertexMaker2 : public HltVertexMaker<Hlt::SelectionContainer3<LHCb::RecVertex,LHCb::Track,LHCb::Track> >{
    public:
        HltVertexMaker2( const std::string& name, ISvcLocator* pSvcLocator);
        combinatorics_engine combine();
    private:
        bool m_doMergeInputs;
};
#endif // HLTMAKEVERTICES_H 
