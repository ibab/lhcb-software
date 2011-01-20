
#ifndef INCLUDE_BEAMGASTRIGVERTEXCUT_H
#define INCLUDE_BEAMGASTRIGVERTEXCUT_H 1

#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"

class BeamGasTrigVertexCut : public HltAlgorithm {

public: 
  /// Standard constructor
  BeamGasTrigVertexCut( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BeamGasTrigVertexCut(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

 private:
  double		 m_histoZMin;	  ///< Lower limit of the z_r0 histo
  double		 m_histoZMax;	  ///< Upper limit of the z_r0 histo
  int		     m_nBins;             ///< N of bins of the z_r0 histo  
  int            m_binWidth;          ///< bin-width of the z_r0 histo
  unsigned		 m_maxCut;            ///< value of the max-bin that triggers the event
  
  double		 m_zExclusionRangeLow;  ///< We don't plot z_r0 of tracks ...
  double		 m_zExclusionRangeUp;   ///< ... extrapolating to this (lumi) region
  unsigned       m_trigEventsZnegative;
  unsigned       m_trigEventsZpositive;
  
  Hlt::SelectionContainer2<LHCb::Track,LHCb::Track> m_trackSelection;
  std::string       m_outputSelectionName;  

};
#endif // INCLUDE_BEAMGASTRIGVERTEXCUT_H 
