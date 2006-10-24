// $Id: HltMonitorAlgorithm.h,v 1.1 2006-10-24 09:31:20 hernando Exp $
#ifndef HLTBASE_HLTMONITORALGORITHM_H 
#define HLTBASE_HLTMONITORALGORITHM_H 1

// Include files

#include "GaudiAlg/GaudiHistoAlg.h"
#include "Event/HltSummary.h"
#include "Event/HltNames.h"

/** @class HltMonitorAlgorithm 
 *  
 *  Base class for HltSummaryAlg monitor
 *
 *  @author Jose A. Hernando 
 *  @date   2006-10-19
 */
class HltMonitorAlgorithm : public GaudiHistoAlg {
public:

  /// Standard constructor
  HltMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltMonitorAlgorithm( ); ///< Destructor

  virtual StatusCode initialize();    ///< SummaryMonitorAlg initialization
  virtual StatusCode execute   ();
  virtual StatusCode finalize  ();    ///< SummaryMonitorAlg finalization

protected:

  // typedef to make code compatible
  typedef AIDA::IHistogram1D* HltHisto;
  
  // book and histogram that can be rebooked using the HistoDescription option
  void initializeHisto( HltHisto& histo, const std::string& name,
                        float min, float max, int nBins );
  
  // fill histo (for compatibility)
  inline void fillHisto( HltHisto& histo, float x, float weight ) 
  {fill(histo,x,1.);}

protected:

  // begin the execution
  // It retrieves the summary and summary box (if in options)
  // set in m_tracks and m_vertices the tracks and vertices from the box
  bool beginExecute();

  // fill the histogram with the info stored in the objects of the container
  template <class CONTAINER >
  void monitor(const CONTAINER& con, int key, HltHisto& histo){
    for (typename CONTAINER::const_iterator it = con.begin(); 
         it != con.end(); ++it) {
      double d = (*it)->info(key,-1.);
      verbose() << " monitor " << key << " " << d << endreq;
      fillHisto( histo, d, 1.);
    }
  }

  // fill histogram with the info from the keys
  // (histos should have the same name)
  template <class CONTAINER>
  void monitor(const CONTAINER& con, 
               const std::vector<std::string>& keyNames) {
    for (std::vector<std::string>::const_iterator it = keyNames.begin();
         it != keyNames.end(); ++it) {
      const std::string& title = (*it);
      int key = HltNames::particleInfoID(title);
      if (key <0) continue;
      for (typename CONTAINER::const_iterator it2 = con.begin();
           it2 != con.end(); it2++) {
        if ((*it2)->hasInfo(key))
          fill( histo1D(title), (*it2)->info(key,0), 1.);
      }
    } 
  }
  
protected:

  // fill the vector with the objects from the summary
  template <class T>
  void getFromSummary(std::vector<T*>& tobjs, int idbox) {
    if (!m_summary->hasSelectionSummary(idbox)) return;
    const LHCb::HltSelectionSummary& box = m_summary->selectionSummary(idbox);
    const std::vector<ContainedObject*>& dobjs = box.data();
    for (std::vector<ContainedObject*>::const_iterator it = dobjs.begin();
         it != dobjs.end(); ++it) {
      ContainedObject* obj = (ContainedObject*) (*it);
      if ((obj)->clID() == T::classID()) 
      {T* t = dynamic_cast<T*>(obj); tobjs.push_back(t);}
    }
  }
  
  // returns a vector with the objects from the summary
  template <class T>
  std::vector<T*> getFromSummary(int idbox ) 
  {std::vector<T*> tobjs; getFromSummary(tobjs,idbox); return tobjs;}

protected:

  std::vector<LHCb::Track*> m_tracks;

  std::vector<LHCb::RecVertex*> m_vertices;  

protected:
  
  // name of the location of the summary
  std::string m_summaryName;
  
  // pointer to the summary
  LHCb::HltSummary* m_summary;

  // name of the algorithm name to report
  std::string m_selectionSummaryName;

  // ID of the summary Box
  int m_selectionSummaryID;

  // pointer to the algorithm report
  LHCb::HltSelectionSummary* m_selectionSummary;

protected:

  // Property to rebook histogram from options
  StringArrayProperty m_histoDescriptor;

};
#endif // HLTBASE_HLTALGORITHM_H
