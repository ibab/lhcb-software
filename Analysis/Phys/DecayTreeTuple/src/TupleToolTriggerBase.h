#ifndef TUPLETOOLTRIGGERBASE_H 
#define TUPLETOOLTRIGGERBASE_H 1

// Include files
// from Gaudi
#include "TupleToolBase.h"
#include "Kernel/IParticleTupleTool.h"            // Interface
//#include "Kernel/ITriggerTisTos.h" //
//#include "Kernel/IParticleDescendants.h"
/** @class TupleToolTriggerBase TupleToolTriggerBase.h
 *
 *  The baseclass for all tupletools interacting with the trigger
 *  This allows for code sharing between TupleToolTISTOS and TupleToolTrigger
 *
 *  For real data the triggers will change run by run. Eventually this means
 *  your stripped data may have a different set of trigger lines per event.
 *  To fins out all the trigger lines which ran, you can set CollateTriggerList
 *  to true, and then all triggers will be printed at the end of the event.
 * 
 *  Options:
 *  
 *  - FillL0:      Fill L0 decisions
 *  - VerboseL0:   Fill verbose L0 information
 *  - FillHlt1:    Fill Hlt1 decisions
 *  - VerboseHlt1: Fill verbose Hlt1 information
 *  - FillHlt2:    Fill Hlt1 decisions
 *  - VerboseHlt2: Fill verbose Hlt1 information
 *
 *  - Verbose:     Sets all the verbose flags to true
 *
 *  - UseAutomaticTriggerList: Get the list of triggers from the event and TCK
 *  
 *  - CollateTriggerList: Print an entire list of L0/HLT lines from the TCKs at the end
 *  
 *  - TriggerList: Supply a list of triggers you are interested in (all of them?)
 *                 Necessary if the triggers change in the middle of your job!!
 *
 *  @author R. Lambert
 *  @date   2010-01-25
 */

class TupleToolTriggerBase : public TupleToolBase, virtual public IParticleTupleTool {
public: 
  /// Standard constructor
  TupleToolTriggerBase( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~TupleToolTriggerBase( ); ///< Destructor

  StatusCode initialize() ;
  
  StatusCode finalize() ;

  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
			   , const std::string&
			   , Tuples::Tuple& );

  virtual StatusCode fill( Tuples::Tuple& );

protected:
  
  bool m_verboseL0;     ///< get details on L0
  bool m_verboseHlt1;   ///< get details on Hlt1
  bool m_verboseHlt2;   ///< get details on Hlt2
  
  bool m_useAutomaticTriggerList; ///<take the triggers from event 1
                                  ///< This is really bad if they change
  
  bool m_collateTriggerList;      ///<print a list of all found triggers
  
  
  
  // do not allow trigger names to mutate from event-to-event
  std::vector<std::string> m_l0;
  std::vector<std::string> m_hlt1;
  std::vector<std::string> m_hlt2;
  
  // cache the trigger to check if/when names mutate from event-to-event
  std::vector<std::string> m_hlt1_init;
  std::vector<std::string> m_hlt2_init;
  
  // collect all Trigger names for ease of the user, print in finalize
  std::vector<std::string> m_hlt1_all;
  std::vector<std::string> m_hlt2_all;
  
  std::vector<std::string> m_triggerList;//property: list of triggers to specifically look at
  
  StatusCode autoListMode();
  
  bool compileAutoList();
  bool compileMyList(const std::vector<std::string>& list);
  bool checkAutoList();
  bool appendToList();
  
  
private:
  
  virtual StatusCode fillBasic( const LHCb::Particle* 
                               , const LHCb::Particle* 
                               , const std::string& 
                               , Tuples::Tuple&  )
  {
    Error("Don't call the fill of the baseclass!!");
    return StatusCode::FAILURE;
  };
  
  virtual StatusCode fillBasic(Tuples::Tuple& )
  {
    Error("Don't call the fill of the baseclass!!");
    return StatusCode::FAILURE;
  };
  
  
  virtual StatusCode fillVerbose( const LHCb::Particle* 
                          , const LHCb::Particle* 
                          , const std::string& 
                          , Tuples::Tuple&  )
  {
    Error("Don't call the fill of the baseclass!!");
    return StatusCode::FAILURE;
  };
  
  virtual StatusCode fillVerbose( Tuples::Tuple&  )
  {
    Error("Don't call the fill of the baseclass!!");
    return StatusCode::FAILURE;
  };
  
  
};
#endif // TUPLETOOLTRIGGERBASE_H
