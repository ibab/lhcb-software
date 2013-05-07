/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/
#ifndef ONLINE_FINITESTATEMACHINE_STATE_H
#define ONLINE_FINITESTATEMACHINE_STATE_H

// Framework include files
#include "FiniteStateMachine/Rule.h"

// C/C++ include files
#include <set>
#include <vector>

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine {

  // Forward declarations
  class Transition;
  class When;
  class Path;

  /**@class StateActions  State  State.h Ctrl/State.h
   *
   *  Definition of the state callback structure attached to a 
   *  single FSM machine.
   *  While executing a transition 2 callbacks attached to a 
   *  STATE may be called:
   *   o The state leave action (before the transition starts).
   *   o The state enter action (after the transition is completed).
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class StateActions    {
  public:
    /// Default Constructor
    StateActions();

    /** Class Constructor
     *  @arg  in  [Callback,read-only]  Reference to state incoming callback structure
     *  @arg  out [Callback,read-only]  Reference to state incoming callback structure
     */
    StateActions(const Callback& in, const Callback& out);

    /// Class Destructor
    virtual ~StateActions();

    /** Update callback structures for stat enter actions. 
     *  If a callback is already present, replace the old structure
     *  with the current staructure and delete the callback object.
     *
     *  @arg  cb  [Callback,read-only]  Reference to state incoming callback structure
     */
    StateActions& setInAction(const Callback& cb);
    /** Update callback structures for stat leave actions. 
     *  If a callback is already present, replace the old structure
     *  with the current staructure and delete the callback object.
     *
     *  @arg  cb  [Callback,read-only]  Reference to state incoming callback structure
     */
    StateActions& setOutAction(const Callback& cb);
    /// Return pointer to FSM state leave callback structure
    const Callback& out()  const  { return m_out; }
    /// Return pointer to FSM state enter callback structure
    const Callback& in()   const  { return m_in;  }
  protected:
    /// Pointer to FSM state leave callback structure
    Callback m_out;
    /// Pointer to FSM state enter callback structure
    Callback m_in;
  };

  /**@class State  State.h Ctrl/State.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class State : public TypedObject    {
  public:
    /// Transition container definition
    typedef std::set<const Transition*> Transitions;
    typedef std::vector<When*>           Whens;

  protected:
    /// Set of enter transitions to the current state
    Transitions  m_incoming;
    /// Set of leave transitions from the current state
    Transitions  m_outgoing;
    /// Container of when clauses defining state changes
    Whens        m_when;
    /// Flag for a transient state
    int          m_transient;

  public:
    /** Class Constructor
     *
     * @arg nam [string, read-only] State name
     * @arg typ [string, read-only] FSM type
     */
    State(const Type *typ, const std::string& nam);
    /// Standatrd destructor
    virtual ~State();    
    /** Return Pointer to FSM transition object leading to the requested target state
     *  @arg    state   Reference to FSM target state object
     *  @return reference to Transition object or NULL if not existant
     */
    const Transition* findTrans(const State* state)  const;
    /** Return Pointer to FSM transition object leading to the requested target state
     *  @arg    state   Name of the FSM target state object
     *  @return reference to Transition object or NULL if not existant
     */
    const Transition* findTrans (const std::string& state)  const;
    /** Return Pointer to FSM transition object leading to the requested target state
     *  @arg    transition  Name of the FSM transition object
     *  @return reference to Transition object or NULL if not existant
     */
    const Transition* findTransByName (const std::string&  transition)  const;
    /// Set the state to be transient 
    void setTransient ()                { m_transient = 1;         }
    /// Return flag if the state is transient
    int isTransient () const            { return m_transient != 0; }

    /// Access the when clauses
    const Whens& when() const           { return m_when;           }
    /// Add when clause to state object
    const When* when(const std::pair<When::Multiplicity,When::States>& p1, 
		     const State* target, 
		     Rule::Direction direction = Rule::SLAVE2MASTER) const;

    /// Return reference to set of leave transitions (CONST)
    const Transitions& outgoing() const { return m_outgoing;       }
    /// Return reference to set of enter transitions (CONST)
    const Transitions& incoming() const { return m_incoming;       }
    /// Return reference to set of leave transitions
    Transitions& outgoing()             { return m_outgoing;       }
    /// Return reference to set of enter transitions
    Transitions& incoming()             { return m_incoming;       }

  };   //  End class State

  typedef FsmHandle<State> StateHandle;
}      //  End namespace 
#endif //  ONLINE_FINITESTATEMACHINE_STATE_H

