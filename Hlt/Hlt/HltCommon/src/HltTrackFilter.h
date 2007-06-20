// $Id: HltTrackFilter.h,v 1.1 2007-06-20 12:17:38 hernando Exp $
#ifndef HLTTRACKFILTER_H 
#define HLTTRACKFILTER_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/IHltFunctionFactory.h"

/** @class HltTrackFilter HltTrackFilter.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-03-22
 */
class HltTrackFilter : public HltAlgorithm {
public: 
  /// Standard constructor
  HltTrackFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltTrackFilter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  template <class CONT>
  void copy(const CONT& c1, CONT& c2)
  {std::copy(c1.begin(),c1.end(),std::back_inserter(c2));}  
  

protected:

  StringArrayProperty m_filterDescriptor;
  bool m_computeInfo;

  std::vector<LHCb::Track*> m_otracks;

  std::vector<std::string> m_filterNames;
  std::vector<std::string> m_filterModes;
  std::vector<int> m_filterIDs;
  std::vector<HltHisto> m_histos;
  std::vector<HltHisto> m_histos1;
  std::vector<Hlt::TrackFunction*> m_functions;
  std::vector<Hlt::TrackFilter*> m_filters;
  std::vector<int> m_tcounters;  
  

private:

};
#endif // HLTTRACKFILTER_H
