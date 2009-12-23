// $Id: HltAlgorithm.h,v 1.43 2009-12-23 17:59:47 graven Exp $
#ifndef HLTBASE_HLTALGORITHM_H 
#define HLTBASE_HLTALGORITHM_H 1

// Include files
#include "HltBase/HltBaseAlg.h"
#include "HltBase/HltSelection.h"
#include <boost/type_traits/remove_const.hpp>

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

  //@TODO: move the {retrieve,register}{,T}Selection into IHltDataSvc...
  // retrieve a selection
  const Hlt::Selection& retrieveSelection(const Gaudi::StringKey& selname);

  // retrieve a selection with candidates of type T (e.g. Track)
  template <class T>
  const Hlt::TSelection<T>& retrieveTSelection(const Gaudi::StringKey& key) {
        const Hlt::TSelection<T> *sel = (key.str().substr(0,4) == "TES:")  ?
                                  this->registerTESSelection<T>(key) :
                                  retrieveSelection(key).template down_cast<T>();
        if (sel==0) throw GaudiException("Failed to down_cast Selection",key.str(),StatusCode::FAILURE);
        return *sel;
  }

  // register an output selection of no candidates
  Hlt::Selection& registerSelection(const Gaudi::StringKey& key) {
        Hlt::Selection* tsel = new Hlt::Selection(key);
        setOutputSelection(tsel);
        return *tsel;
  }

  // register an output selection with candidates of T type (e.g. Track)
  template <typename T>
  Hlt::TSelection<T>& registerTSelection(const Gaudi::StringKey& key) {
        Hlt::TSelection<T>* tsel = new Hlt::TSelection<T>(key);
        setOutputSelection(tsel);
        return *tsel;
  }

  Hlt::Selection& outputSelection() const {
    Assert(m_outputSelection != 0, "No defined output selection!!");
    return *m_outputSelection;
  }
  
  size_t outputSelectionSize() const 
  {return m_outputSelection != 0?  m_outputSelection->size():0;}
  

private:

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
  const Hlt::TSelection<T>* registerTESSelection(const Gaudi::StringKey& key) {
       // must ALWAYS add a callback to our stack for this 
       if (!dataSvc().hasSelection(key) 
           && dataSvc().addSelection( new Hlt::TSelection<T>(key), this, true).isFailure()) {
            throw GaudiException("Failed to register TES Selection",key.str(),StatusCode::FAILURE);
       }
       const Hlt::TSelection<T>* selection = retrieveSelection(key).template down_cast<T>();
       if (selection==0) {
            throw GaudiException("Failed to retrieve TES-backed Selection",key.str(),StatusCode::FAILURE);
       }
       m_callbacks.push_back( new HltAlgorithm::TESSelectionCallBack<const Hlt::TSelection<T> >( *selection,*this ) ); 
       return selection;
  }

  // must inputs be valid?
  bool m_requireInputsToBeValid;

  // period to update filling of histogram
  int m_histogramUpdatePeriod;

  // minimun number of candidates
  //   deciison true if output selection has more candidates than the minimun
  //TODO: since this is not applicable to all algorithms, remove from base...
  size_t m_minNCandidates;

  // set this selection as output, to be monitor, and to decide if the 
  // event pass
  void setOutputSelection(Hlt::Selection* sel);

  // list of all the input selections
  std::vector<const Hlt::Selection*> m_inputSelections;

  // (owner) pointer to the output selection
  Hlt::Selection* m_outputSelection;

  // map of id of selection and histogram to monitor input candidate
  std::map<Gaudi::StringKey,AIDA::IHistogram1D*> m_inputHistos;
  
  // map of the output selection candidates
  AIDA::IHistogram1D* m_outputHisto;


  struct CallBack {
    virtual ~CallBack() {}
    virtual StatusCode execute()  = 0;
  };

  template<typename T>
  class TESSelectionCallBack : public CallBack {
  public:
      TESSelectionCallBack(T &selection,HltAlgorithm &parent) : 
        m_selection(const_cast<typename boost::remove_const<T>::type&>(selection)),m_parent(parent) { //antonio
        assert(m_selection.id().str().substr(0,4)=="TES:");
      }
      StatusCode execute() {
        typedef typename T::candidate_type::Container  container_type;
        // TODO: does not work, as fullTESLocation is private...
        //container_type *obj = SmartDataPtr<container_type>( m_parent.evtSvc(), m_parent.fullTESLocation( m_selection.id().str().substr(4), true ) );
        // if (obj==0) { }
        container_type *obj = m_parent.get<container_type>( m_parent.evtSvc(), m_selection.id().str().substr(4) );
        m_selection.clean(); //TODO: check if/why this is needed??
        m_selection.insert(m_selection.end(),obj->begin(),obj->end());
        m_selection.setDecision( !m_selection.empty() ); // force it processed...
        return StatusCode::SUCCESS;
      }
  private:
      typename boost::remove_const<T>::type&            m_selection;
      HltAlgorithm& m_parent;
  };

  std::vector<CallBack*> m_callbacks;

};
#endif // HLTBASE_HLTALGORITHM_H
