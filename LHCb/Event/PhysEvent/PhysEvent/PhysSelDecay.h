// $Id: PhysSelDecay.h,v 1.1.1.1 2001-07-09 09:23:58 gcorti Exp $
#ifndef PHYSEVENT_PHYSSELDECAY_H
#define PHYSEVENT_PHYSSELDECAY_H 1
 
// Include files
#ifdef WIN32
  // Disable warning C4800: implicit convertion from integer to bool
  #pragma warning ( disable : 4800 )
#endif
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StreamBuffer.h"
#include "LHCbEvent/CLHEPStreams.h"
#include "LHCbEvent/Definitions.h"
#include <iostream>
#include <vector>

/** @class PhysSelDecay PhysSelDecay.h PhysEvent/PhysSelDecay.h
 *
 *  This class contains the results of the selection for a specific
 *  decay channel set as standard algorithm
 *
 *  @author Gloria Corti
 *  @date   30/05/2001
 */

class PhysSelDecay {

  enum selFlags { MCFlag=0, TKRFlag=1, PQUALFlag=2,
                  SELFlag=3, AGRFlag=4, TAGFlag= 5 };

public:
  /// Default Constructor
  PhysSelDecay() : m_decayID(0), m_results(0) { 
  }  

  /// Destructor
  ~PhysSelDecay() { 
  }

  /// Retrieve decayID
  long decayID() const {
    return m_decayID; 
  }

  /// Set decayID
  void setDecayID( long value ) {
    m_decayID = value; 
  }

  /// Retrieve all results
  long results ( ) const { 
    return m_results; 
  }
  void results ( bool& lMCFlag, bool& lTKRFlag,  
                 bool& lPQUALFlag, bool& lSELFlag, 
                 bool& lAGRFlag, bool& lTAGFlag );

  /// Set all results
  void setResults( long value ) {
    m_results = value; 
  }
  void setResults( bool lMCFlag, bool lTKRFlag,  
                   bool lPQUALFlag, bool lSELFlag, 
                   bool lAGRFlag, bool lTAGFlag );

  /// Retrieve if the decay channel is present in the Monte Carlo truth
  bool decayIsInMCTree() {
    return ( m_results & (1<<MCFlag) );
  }
  /// Set to true idf the decay channel is present in the MC truth
  void setDecayIsInMCTree() {
    m_results |= 1<<MCFlag;
  }

  /// Retrieve if the decay channel has all end particles reconstructed
  bool decayHasTrkRecon() {
    return ( m_results & (1<<TKRFlag) );
  }
  /// Set if the decay channel has all end particles reconstructed
  void setDecayHasTrkRecon() {
    m_results |= 1<<TKRFlag;
  }

  /// Retrieve if the decay channel has all end particles reconstructed 
  /// and the charged ones are of physics quality
  bool decayHasTrkPQual() {
    return ( m_results & (1<<PQUALFlag) );
  }
  /// Set if the decay channel has all end particles reconstructed and 
  /// the charged ones are of physics quality
  void setDecayHasTrkPQual() {
    m_results |= 1<<PQUALFlag;
  }
        
  /// Retrieve if the event satisfy the selection for the decay channel 
  bool decayIsSelected() {
    return ( m_results & (1<<SELFlag) );
  }
  /// Set if the event satisfy the selection for the decay channel
  void setDecayIsSelected() {
    m_results |= 1<<SELFlag;
  }
    
  /// Retrieve if the event satisfy the selection for the decay channel
  /// and at least one combination is the MC truth
  bool decaySelectedIsMC() {
    return ( m_results & (1<<AGRFlag) );
  }
  /// Set if the event satisfy the selection for the decay channel
  /// and at least one combination is the MC truth
  void setDecaySelectedIsMC() {
    m_results |= 1<<AGRFlag;
  }

  /// Retrieve if at least one selected combination for the decay channel
  /// is also flavour tagged 
  bool decayIsFlavourTagged() {
    return ( m_results & (1<<TAGFlag) );
  }
  /// Set if at least one selected combination for the decay channel
  /// is also flavour tagged
  void setDecayIsFlavourTagged() {
    m_results |= 1<<TAGFlag;
  }

  /// Serialize the object for writing
  friend StreamBuffer& operator<< ( StreamBuffer& s, const PhysSelDecay& obj ) {
  return s << obj.m_decayID
           << obj.m_results;
  }
  /// Serialize the object for reading
  friend StreamBuffer& operator>> ( StreamBuffer& s, PhysSelDecay& obj ) {
    return s >> obj.m_decayID
             >> obj.m_results;
  }

  /// Output operator (ASCII)
  friend std::ostream& operator<< ( std::ostream& s, const PhysSelDecay& obj ) {
    return obj.fillStream(s);
  }

  /// Fill the output stream (ASCII)
  std::ostream& fillStream( std::ostream& s ) const;

private: 

  unsigned long m_decayID;    ///< decayCode as used in book-keeping
  unsigned long m_results;    ///< bitset with results of selection analysis

};

//
// Inline code must be outside the class definition
//

/// Fill the output stream (ASCII)
inline std::ostream& PhysSelDecay::fillStream( std::ostream& s ) const {
  return s
    << "class PhysSelDecay :\n"
    << "\n    Decay ID code = " << m_decayID
    << "\n    Decay Results = " << m_results;
}


#endif    // PHYSEVENT_PHYSSELDECAY_H



