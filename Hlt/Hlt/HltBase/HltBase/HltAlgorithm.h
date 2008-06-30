// $Id: HltAlgorithm.h,v 1.28 2008-06-30 08:58:15 graven Exp $
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

  // driver of the initialize()
  virtual StatusCode sysInitialize();

  // initialize algorithm
  virtual StatusCode initialize();

  // finalize algorithm
  virtual StatusCode finalize  ();

protected:

  // initialize the counters
  virtual void initCounters();

  // save configurtion of algorithm
  virtual void saveConfiguration();

private:  

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
    
public:
  //@TODO: move the {retrieve,register}{,T}Selection into IHltDataSvc...
  // retrieve a selection
  Hlt::Selection& retrieveSelection(const stringKey& selname) {
    Assert(!selname.empty()," retrieveSelection() no selection name");
    debug() << " retrieveSelection " << selname << endreq;
    if (!validHltSelectionName(selname)) {
      error() << " No valid selection name " << selname << endreq;
      Assert(0," retrieveSelection, no valid name!");
    }
    Hlt::Selection& sel = dataSvc().selection(selname,this);
    setInputSelection(sel);
    debug() << " retrieved selection " << sel.id() << endreq;    
    return sel;
  }

  // retrieve a selection with candidates of type T (e.g. Track)
  template <class T>
  Hlt::TSelection<T>& retrieveTSelection(const stringKey& selname) {
      // dynamic_cast with reference will throw an execption when it fails,
      // which is preferred over a SEGV...
    return dynamic_cast<Hlt::TSelection<T>&>(retrieveSelection(selname));
  }


  // register a selection of no candidates
  Hlt::Selection& registerSelection() {
    debug() << " registerSelection " << m_outputSelectionName << endreq;
    Hlt::Selection* sel = new Hlt::Selection(m_outputSelectionName);
    StatusCode sc = dataSvc().addSelection(sel,this,useTES());
    if ( sc.isFailure()) {
       throw GaudiException("Failed to add Selection",m_outputSelectionName.str(),StatusCode::FAILURE);
    }
    setOutputSelection(sel);
    debug() << " registered selection " << m_outputSelectionName << endreq;
    return *sel;
  }
  
  // register a selection with candidates of T type (i.e Track)
  template <class T>
  Hlt::TSelection<T>& registerTSelection()
  {
    debug() << " registerTSelection " << m_outputSelectionName << endreq;
    Hlt::TSelection<T>* tsel = new Hlt::TSelection<T>(m_outputSelectionName);
    StatusCode sc = dataSvc().addSelection(tsel,this,useTES());
    if (sc.isFailure()) {
       throw GaudiException("Failed to add Selection",m_outputSelectionName.str(),StatusCode::FAILURE);
    }
    setOutputSelection(tsel);
    debug() << " registered selection " << m_outputSelectionName
                 << " type " << tsel->classID() << endreq;
    return *tsel;
  }

    
private:

  // period to update filling of histogram
  int m_histogramUpdatePeriod;

  // fill the hlt configuration
  bool m_saveConf;

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
 
private:
  // name of the output selection
 stringKey m_outputSelectionName;

protected:
  stringKey outputSelectionName()  const { return m_outputSelectionName; }

protected:
  // list of all the input selections
  std::vector<Hlt::Selection*> m_inputSelections;

  // (owner) pointer to the output selection
  Hlt::Selection* m_outputSelection;

protected:

  // bool to check if the inputs are fine in execute()
  bool m_considerInputs;

private:
  // set this selection as input, to be check and monitor every event
  void setInputSelection(Hlt::Selection& sel);
  
  // set this selection as output, to be monitor, and to decide if the 
  // event pass
  void setOutputSelection(Hlt::Selection* sel);

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
  std::map<stringKey,Hlt::Histo*> m_inputHistos;
  
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
