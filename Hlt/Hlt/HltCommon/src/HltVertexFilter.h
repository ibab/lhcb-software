// $Id: HltVertexFilter.h,v 1.1 2007-06-20 12:17:38 hernando Exp $
#ifndef HLTVERTEXFILTER_H 
#define HLTVERTEXFILTER_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/IHltFunctionFactory.h"

/** @class HltVertexFilter HltVertexFilter.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-03-22
 */
class HltVertexFilter : public HltAlgorithm {
public: 
  /// Standard constructor
  HltVertexFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltVertexFilter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  template <class CONT>
  void copy(const CONT& c1, CONT& c2)
  {std::copy(c1.begin(),c1.end(),std::back_inserter(c2));}  
  

protected:

  StringArrayProperty m_filterDescriptor;
  bool m_computeInfo;

  std::vector<LHCb::RecVertex*> m_otracks;

  std::vector<std::string> m_filterNames;
  std::vector<std::string> m_filterModes;
  std::vector<int> m_filterIDs;
  std::vector<HltHisto> m_histos;
  std::vector<HltHisto> m_histos1;
  std::vector<Hlt::VertexFunction*> m_functions;
  std::vector<Hlt::VertexFilter*> m_filters;
  std::vector<int> m_tcounters;  
  

private:

};
#endif // HLTVERTEXFILTER_H
