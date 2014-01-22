// $Id: HltAlgorithm.h,v 1.51 2010-08-17 08:49:18 graven Exp $
#ifndef HLTBASE_HLTALGORITHM_H 
#define HLTBASE_HLTALGORITHM_H 1

// Include files
#include <memory>
#include "HltBase/IHltUnit.h"
#include "HltBase/HltBaseAlg.h"
#include "HltBase/HltSelection.h"
#include "boost/utility.hpp"
#include "boost/type_traits/integral_constant.hpp"
#include <boost/type_traits/remove_const.hpp>

#include "boost/iterator/transform_iterator.hpp"
#include "boost/mpl/if.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/casts.hpp"

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
class HltAlgorithm : public HltBaseAlg,
                     virtual public Hlt::IUnit {
public:

  // Standard constructor
  HltAlgorithm( std::string name, ISvcLocator* pSvcLocator, bool requireInputsToBeValid=true );

  // Standard destructor
  virtual ~HltAlgorithm( ); 

  // driver of the initialize()
  virtual StatusCode sysInitialize();
  virtual StatusCode sysFinalize();

  // restart algorithm
  virtual StatusCode restart  ();

  StatusCode queryInterface(const InterfaceID& iid, void** ppvi ) ;

  // retrieve a selection
  const Hlt::Selection* selection(const Gaudi::StringKey& key) const;
  const Hlt::Selection* selection(const Gaudi::StringKey& key, const Hlt::IUnit::Client&) const;
  const Hlt::Selection* declareInput(const Gaudi::StringKey&, const Hlt::IUnit::Client&) const;
  StatusCode registerTESInput(const Gaudi::StringKey&, const Hlt::IUnit::Client&) const;
  StatusCode registerTESInput(const Gaudi::StringKey&) const;
  const DataObject* tes(const Hlt::IUnit::Client&, const Gaudi::StringKey&) const;
  Hlt::Selection* retrieve(const Hlt::IUnit::Client&, const Gaudi::StringKey&) const;


  // retrieve a selection with candidates of type T (e.g. Track)
  template <class T>
  const Hlt::TSelection<T>& retrieveTSelection(const Gaudi::StringKey& key) {
        const Hlt::TSelection<T> *sel = (key.str().substr(0,4) == "TES:")  ?
                                  this->registerTESSelection<T>(Gaudi::StringKey(key.str().substr(4))) :
                                  selection(key)->template down_cast<T>();
        if (!sel) throw GaudiException("Failed to down_cast Selection",key.str(),StatusCode::FAILURE);
        return *sel;
  }


  // register an output selection of no candidates
  Hlt::Selection& registerSelection(const Gaudi::StringKey& key) {
        Hlt::Selection* tsel = new Hlt::Selection(key);
        registerOutput(tsel).ignore();
        return *tsel;
  }

  // register an output selection with candidates of T type (e.g. Track)
  template <typename T>
  Hlt::TSelection<T>& registerTSelection(const Gaudi::StringKey& key) {
        Hlt::TSelection<T>* tsel = new Hlt::TSelection<T>(key);
        registerOutput(tsel).ignore();
        return *tsel;
  }

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
       auto sc = registerTESInput(key);
       if (sc.isFailure()) {
            throw GaudiException("Failed to register TES Selection",key.str(),StatusCode::FAILURE);
       }
       typedef HltAlgorithm::TESSelectionCallBack<Hlt::TSelection<T> > cb_t;
       std::unique_ptr<cb_t> cb{  new cb_t{ key, *this } };
       auto ret = cb->selection();
       m_callbacks.emplace_back( std::move(cb) );
       return ret;
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
  StatusCode registerOutput(Hlt::Selection* sel, const Hlt::IUnit::Client& client) const;
  StatusCode registerOutput(Hlt::Selection* sel) const;

  // (owner!) pointer to the output selection
  mutable Hlt::Selection* m_outputSelection;

  /// container of keys 
  typedef GaudiUtils::VectorMap<Key,      Hlt::Selection*> OMap ;
  typedef GaudiUtils::VectorMap<Key,const Hlt::Selection*> IMap ;
  typedef std::vector<Key>                                 LVct ;
  /// keys for all "input"  selections 
  mutable IMap      m_in  ;               // keys for all "input"  selections 
  /// keys for all "output" selections 
  mutable OMap      m_out ;               // keys for all "output" selections 
  /// keys for all TES input locations 
  mutable LVct      m_tes ;               // keys for all TES input locations 

  // map of id of selection and histogram to monitor input candidate
  mutable std::map<Gaudi::StringKey,AIDA::IHistogram1D*> m_inputHistos;
  
  // map of the output selection candidates
  mutable AIDA::IHistogram1D* m_outputHisto;


  struct CallBack {
    virtual ~CallBack() {}
    virtual StatusCode execute()  = 0;
  };

  template<typename T>
  class TESSelectionCallBack : boost::noncopyable, public CallBack {
      std::unique_ptr<T>  m_selection;
      GaudiAlgorithm& m_parent;
  public:
      TESSelectionCallBack(Gaudi::StringKey&& key,GaudiAlgorithm &parent) 
       : m_selection{new T{std::move(key)}}
       , m_parent{parent} 
      { }
      TESSelectionCallBack(const Gaudi::StringKey& key,GaudiAlgorithm &parent) 
       : m_selection{new T{key}}
       , m_parent(parent) 
      { }

      ~TESSelectionCallBack() { }
      const T *selection() const { return m_selection.get(); }

      StatusCode execute() {
        m_selection->clean(); //TODO: check if/why this is needed??
        m_parent.get<typename T::candidate_type::Range>( m_parent.evtSvc()
                                                       , m_selection->id().str() )
            >> *m_selection;
        m_selection->setDecision( !m_selection->empty() ); // force it processed...
        return StatusCode::SUCCESS;
      }
  };

  std::vector<std::unique_ptr<CallBack>> m_callbacks;

};

#endif // HLTBASE_HLTALGORITHM_H
