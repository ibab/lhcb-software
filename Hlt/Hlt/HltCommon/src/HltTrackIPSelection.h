// $Id: HltTrackIPSelection.h,v 1.1.1.1 2006-01-19 10:01:59 hernando Exp $
#ifndef HLTTRACKIPSELECTION_H 
#define HLTTRACKIPSELECTION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/Track.h"
#include "Event/TrgVertex.h"

#include "PatTools/PatDataStore.h"

/** @class HltTrackIPSelection HltTrackIPSelection.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2005-11-17
 */

class HltTrackIPSelection : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltTrackIPSelection( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltTrackIPSelection( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  StatusCode loadEvent();

  void computeTrackSmallestIP();

  virtual void selectVertex();
  
  void flagSelectedTracks();

protected:

  std::string m_inputTracksName;
  std::string m_inputVerticesName;

  double m_ipmin;
  double m_ipmax;
  double m_ipabs;

  bool m_selectVertex;
  bool m_doFlag;

protected:

  PatTrackContainer* m_itracks;

  PatVertexContainer* m_vertices;
  
protected:

  std::vector<double> m_sip;
  std::vector<unsigned int> m_kver;
  std::vector<bool> m_ipsel;

  std::vector<int> m_canver;
  unsigned int m_vertexKey;

private:

};
#endif // TRGTRACKIPSELECTION_H
