// $Id: HltAlgorithm.h,v 1.33 2008-12-19 17:29:52 graven Exp $
#ifndef HLTBASE_HLTALGORITHM_H 
#define HLTBASE_HLTALGORITHM_H 1

// Include files
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

  // restart algorithm
  virtual StatusCode restart  ();

  // finalize algorithm
  virtual StatusCode finalize  ();

  //@TODO: move the {retrieve,register}{,T}Selection into IHltDataSvc...
  // retrieve a selection
  Hlt::Selection& retrieveSelection(const stringKey& selname);

public:
  // retrieve a selection with candidates of type T (e.g. Track)
  template <class T>
  Hlt::TSelection<T>& retrieveTSelection(const stringKey& key) {
        Hlt::TSelection<T> *sel = (key.str().substr(0,4) == "TES:")  ?
                                  this->registerTESSelection<T>(key) :
                                  retrieveSelection(key).template down_cast<T>();
        if (sel==0) throw GaudiException("Failed to down_cast Selection",key.str(),StatusCode::FAILURE);
        return *sel;
  }

  // register an output selection of no candidates
  Hlt::Selection& registerSelection(const stringKey& key) {
        Hlt::Selection* tsel = new Hlt::Selection(key);
        setOutputSelection(tsel);
        return *tsel;
  }

  // register an output selection with candidates of T type (e.g. Track)
  template <typename T>
  Hlt::TSelection<T>& registerTSelection(const stringKey& key) {
        Hlt::TSelection<T>* tsel = new Hlt::TSelection<T>(key);
        setOutputSelection(tsel);
        return *tsel;
  }


protected:

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

  template <typename T> 
  Hlt::TSelection<T>* registerTESSelection(const stringKey& key) {
       // must ALWAYS add a callback to our stack for this 
       if (!dataSvc().hasSelection(key) 
           && dataSvc().addSelection( new Hlt::TSelection<T>(key), this, true).isFailure()) {
            throw GaudiException("Failed to register TES Selection",key.str(),StatusCode::FAILURE);
       }
       Hlt::TSelection<T>* selection = retrieveSelection(key).template down_cast<T>();
       if (selection==0) {
            throw GaudiException("Failed to retrieve TES-backed Selection",key.str(),StatusCode::FAILURE);
       }
       m_callbacks.push_back( new HltAlgorithm::TESSelectionCallBack<Hlt::TSelection<T> >( *selection,*this ) ); 
       return selection;
  }

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

protected:
  const Hlt::Counter counterEntries() const { return m_counterEntries; }
private:
  // set this selection as output, to be monitor, and to decide if the 
  // event pass
  void setOutputSelection(Hlt::Selection* sel);

  // list of all the input selections
  std::vector<Hlt::Selection*> m_inputSelections;

  // (owner) pointer to the output selection
  Hlt::Selection* m_outputSelection;

  // counter with all the entries of the algorithm
  Hlt::Counter m_counterEntries;

  // counter with all the events with fine inputs
  Hlt::Counter m_counterInputs;

  // counter with the accepted events (decision true)
  Hlt::Counter m_counterAccepted;

  // coutner with the total number of candidates
  Hlt::Counter m_counterCandidates;

  // map of id of selection and histogram to monitor input candidate
  std::map<stringKey,Hlt::Histo*> m_inputHistos;
  
  // map of the output selection candidates
  Hlt::Histo* m_outputHisto;


  struct CallBack {
    virtual ~CallBack() {}
    virtual void process() {}
  };

  template<typename T>
  class TESSelectionCallBack : public CallBack {
  public:
      TESSelectionCallBack(T &selection,HltAlgorithm &parent) : m_selection(selection),m_parent(parent) {
        assert(m_selection.id().str().substr(0,4)=="TES:");
      }
      void process() {
        typedef typename T::candidate_type::Container  container_type;
        container_type *obj = m_parent.get<container_type>( m_parent.evtSvc(), m_selection.id().str().substr(4) );
        m_selection.clean(); //TODO: check if/why this is needed??
        m_selection.insert(m_selection.end(),obj->begin(),obj->end());
        m_selection.setDecision( !m_selection.empty() ); // force it processed...
      }
  private:
      T&            m_selection;
      HltAlgorithm& m_parent;
  };

  std::vector<CallBack*> m_callbacks;

};




#endif // HLTBASE_HLTALGORITHM_H
