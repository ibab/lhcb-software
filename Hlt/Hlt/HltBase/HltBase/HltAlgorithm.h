// $Id: HltAlgorithm.h,v 1.30 2008-07-30 13:33:16 graven Exp $
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
  HltAlgorithm( const std::string& name, ISvcLocator* pSvcLocator, bool requireInputsToBeValid=true );

  // Standard destructor
  virtual ~HltAlgorithm( ); 

  // driver of the initialize()
  virtual StatusCode sysInitialize();

  // initialize algorithm
  virtual StatusCode initialize();

  // restart algorithm
  virtual StatusCode restart  ();

  // finalize algorithm
  virtual StatusCode finalize  ();

public:
  //@TODO: move the {retrieve,register}{,T}Selection into IHltDataSvc...
  // retrieve a selection
  Hlt::Selection& retrieveSelection(const stringKey& selname);

  // retrieve a selection with candidates of type T (e.g. Track)
  template <class T>
  Hlt::TSelection<T>& retrieveTSelection(const stringKey& selname) {
    Hlt::Selection& sel = retrieveSelection(selname);
    Hlt::TSelection<T> *tsel = sel.down_cast<T>();
    if (tsel==0) throw GaudiException("Failed to down_cast Selection",selname.str(),StatusCode::FAILURE);
    return *tsel;
  }

  // register an output selection of no candidates
  Hlt::Selection& registerSelection(const stringKey& selname) 
  {
    Hlt::Selection* tsel = new Hlt::Selection(selname);
    setOutputSelection(tsel);
    return *tsel;
  }

  // register an output selection with candidates of T type (e.g. Track)
  template <class T>
  Hlt::TSelection<T>& registerTSelection(const stringKey& selname)
  {
    Hlt::TSelection<T>* tsel = new Hlt::TSelection<T>(selname);
    setOutputSelection(tsel);
    return *tsel;
  }

  StatusCode registerTESInputSelection(Hlt::Selection* sel) {
    return dataSvc().addSelection(sel,this,true);
  }

protected:

  // initialize the counters
  virtual void initCounters();

  // save configurtion of algorithm
  virtual void saveConfiguration();

  // decision according with nCandidates and filter, or preset in the selection
  void setDecision() ;

private:

  // monitor inputs, fill histograms with the candidates of input selections
  void monitorInputs();

  // monitor output, fill histogram with candidates of output selection
  void monitorOutput();

  // force decision to the value of decision
  void setDecision(bool decision);
  //
  // check that all input selections are present & positive...
  bool verifyInput() ;

  // begin execution: monitor input selections
  StatusCode beginExecute();
  
  // end of the execution: monitor output selection and set decision
  StatusCode endExecute();
  
  // driver of the execute()
  StatusCode sysExecute();

private:
  // must inputs be valid?
  bool m_requireInputsToBeValid;

  // period to update filling of histogram
  int m_histogramUpdatePeriod;

  // fill the hlt configuration
  bool m_saveConf;

  // minimun number of candidates
  //   deciison true if output selection has more candidates than the minimun
  //TODO: since this is not applicable to all algorithms, remove from base...
  size_t m_minNCandidates;


private:
  // set this selection as output, to be monitor, and to decide if the 
  // event pass
  void setOutputSelection(Hlt::Selection* sel);

  // list of all the input selections
  std::vector<Hlt::Selection*> m_inputSelections;

  // (owner) pointer to the output selection
  Hlt::Selection* m_outputSelection;

protected:

  // counter with all the entries of the algorithm
  Hlt::Counter m_counterEntries;

private:
  // counter with all the events with fine inputs
  Hlt::Counter m_counterInputs;

  // counter with the accepted events (decision true)
  Hlt::Counter m_counterAccepted;

  // counter with all the event passed (accepted + forced)
  Hlt::Counter m_counterPassed;

  // coutner with the total number of candidates
  Hlt::Counter m_counterCandidates;

  // map of id of selection and histogram to monitor input candidate
  std::map<stringKey,Hlt::Histo*> m_inputHistos;
  
  // map of the output selection candidates
  Hlt::Histo* m_outputHisto;

};
#endif // HLTBASE_HLTALGORITHM_H
