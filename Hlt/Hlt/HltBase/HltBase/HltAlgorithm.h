// $Id: HltAlgorithm.h,v 1.24 2008-05-07 11:36:40 graven Exp $
#ifndef HLTBASE_HLTALGORITHM_H 
#define HLTBASE_HLTALGORITHM_H 1

// Include files
#include "Event/HltSummary.h"
#include "Event/RecVertex.h"
#include "HltBase/HltTypes.h"
#include "HltBase/HltBaseAlg.h"

/** @class HltAlgorithm 
 *  
 *  Base class for algorithms of HLT (alleys)
 *
 *  functionality:
 *        - retrieve hlt input selections and register an output selection
 *        - monitor input and output selection candidates
 *        - algorithm decision if true if output candidates >= minimum (1)
 *
 *  options:
 *      InputSelection, InputSelection2, InputSelections, OutputSelection
 *      (if no OutputSelection takes name of the algorithm)
 *
 *  @author Hugo Ruiz Perez
 *  @author Jose Angel Hernando Morata
 *  @date   2006-06-15
 */
class HltAlgorithm : public HltBaseAlg {
public:

  // Standard constructor
  HltAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  // Standard destructor
  virtual ~HltAlgorithm( ); 

  // initialize algorithm
  virtual StatusCode initialize();

  // finalize algorithm
  virtual StatusCode finalize  ();

protected:

  // initialize the counters
  virtual void initCounters();

  // save configurtion of algorithm
  virtual void saveConfiguration();

protected:  

  // begin execution: monitor input selections
  StatusCode beginExecute();
  
  // end of the execution: monitor output selection and set decision
  StatusCode endExecute();
  
  // driver of the execute()
  StatusCode sysExecute();

protected:

  // check if ther input selections are positive
  bool considerInputs() ;

  // monitor inputs, fill histograms with the candidates of input selections
  void monitorInputs();

  // monitor output, fill histogram with candidates of output selection
  void monitorOutput();
  
  // force decision to the value of decision
  void setDecision(bool decision);
  
  // decision according with nCandidates and filter
  void setDecision() ;
    
protected:

  // period to always force decision true
  int m_passPeriod;

  // period to update filling of histogram
  int m_histogramUpdatePeriod;

  // fill the hlt configuration
  bool m_saveConf;

  // internal bool to indicate if this event must be filter
  bool m_filter;
  
  // internal bool to indicate if this event should be monitor
  bool m_monitor;

protected:
  
  // minimun number of candidates
  //   deciison true if output selection has more candidates than the minimun
  size_t m_minNCandidates;

protected:

  // name of the input selection (option)
  std::string m_inputSelectionName;

  // name of the 2nd input selection (option)
  std::string m_inputSelection2Name;

  // names of the extra input selections (option)
  StringArrayProperty m_extraInputSelectionsNames;
  
  // list of all the input selections names
  std::vector<std::string> m_inputSelectionsNames;

  // list of all the input selections
  std::vector<Hlt::Selection*> m_inputSelections;

  // name of the output selection
  std::string m_outputSelectionName;

  // (owner) pointer to the output selection
  Hlt::Selection* m_outputSelection;

protected:

  // bool to check if the inputs are fine in execute()
  bool m_considerInputs;

  // set this selection as input, to be check and monitor every event
  void setInputSelection(Hlt::Selection& sel, 
                         const std::string& selname);
  
  // set this selection as output, to be monitor, and to decide if the 
  // event pass
  void setOutputSelection(Hlt::Selection* sel,
                          const std::string& selname);

protected:


  // counter with all the entries of the algorithm
  Hlt::Counter m_counterEntries;

  // counter with all the events with fine inputs
  Hlt::Counter m_counterInputs;

  // counter with the accepted events (decision true)
  Hlt::Counter m_counterAccepted;

  // counter with all the event passed (accepted + forced)
  Hlt::Counter m_counterPassed;

  // coutner with the total number of candidates
  Hlt::Counter m_counterCandidates;

protected:

  // map of id of selection and histogram to monitor input candidate
  std::map<int,Hlt::Histo*> m_inputHistos;
  
  // map of the output selection candidates
  Hlt::Histo* m_outputHisto;

protected:

  Hlt::TrackSelection* m_inputTracks;
  Hlt::TrackSelection* m_inputTracks2;
  Hlt::VertexSelection* m_inputVertices;
  Hlt::VertexSelection* m_primaryVertices;
  

  Hlt::TrackSelection* m_outputTracks;
  Hlt::VertexSelection* m_outputVertices;

};
#endif // HLTBASE_HLTALGORITHM_H
