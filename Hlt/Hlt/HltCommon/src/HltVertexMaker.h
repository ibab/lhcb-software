// $Id: HltVertexMaker.h,v 1.8 2008-05-15 08:56:55 graven Exp $
#ifndef HLTVERTEXMAKER_H 
#define HLTVERTEXMAKER_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/IBiFunctionFactory.h"
#include "HltFunctions.h"

/** @class HltVertexMaker HltVertexMaker.h
 *  
 *
 *  @author Hugo Ruiz
 *  @date   2006-02-21
 */


class HltVertexMaker : public HltAlgorithm {
 public: 

  typedef IBiFunctionFactory<LHCb::Track,LHCb::Track> IBiTrackFactory;
  
  /// Standard constructor
  HltVertexMaker( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~HltVertexMaker( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
    
 private:

  void saveConfiguration();
  

 private:

  bool m_doMergeInputs;
  
  bool m_checkForOverlaps;
  bool m_twoContainers;

private:



  StringArrayProperty m_filterDescriptor;

  Hlt::Counter m_counterCombinations;

  std::vector<std::string> m_filterNames;
  std::vector<int> m_filterIDs;
  std::vector<Hlt::TrackBiFunction*> m_functions;
  std::vector<Hlt::Filter*> m_filters;
  std::vector<Hlt::Histo*> m_histos;
  std::vector<int> m_tcounters;

  std::vector<double> m_vals;

  std::vector<LHCb::Track*> m_input2;

  Hlt::VertexCreator _makeVertex;
};
#endif // HLTMAKEVERTICES_H 

