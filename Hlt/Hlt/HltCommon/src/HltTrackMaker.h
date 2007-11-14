// $Id: HltTrackMaker.h,v 1.2 2007-11-14 13:57:04 hernando Exp $
#ifndef HLTTRACKMAKER_H 
#define HLTTRACKMAKER_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"
#include "HltBase/ITrackMatchUpgrade.h"

/** @class HltTrackMaker HltTrackMaker.h
 *  
 *
 *  @author Hugo Ruiz
 *  @date   2006-02-21
 */


class HltTrackMaker : public HltAlgorithm {
 public: 

  /// Standard constructor
  HltTrackMaker( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~HltTrackMaker( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
    
 protected:

  void saveConfiguration();

 private:

  std::string m_trackMakerName;


protected:

  StringArrayProperty m_filterDescriptor;

  std::vector<std::string> m_filterNames;
  std::vector<int> m_filterIDs;
  std::vector<Hlt::TrackBiFunction*> m_functions;
  std::vector<Hlt::Filter*> m_filters;
  std::vector<HltHisto> m_histos;
  std::vector<int> m_tcounters;

  std::vector<double> m_vals;

  ITrackMatchUpgrade* m_trackMaker;

};
#endif // HLTMAKEVERTICES_H 

