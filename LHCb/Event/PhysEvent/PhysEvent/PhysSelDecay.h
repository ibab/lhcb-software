

//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************



#ifndef PhysEvent_PhysSelDecay_H
#define PhysEvent_PhysSelDecay_H 1

// Include files
#include "LHCbEvent/CLHEPStreams.h"


/** @class PhysSelDecay PhysSelDecay.h 
 *
 *  Results of standards selection for a specific decay channel
 *
 *  @author Gloria Corti
 *  created Mon Feb  4 20:21:56 2002
 *
 */

class PhysSelDecay
{

public: 

  /// Default Constructor 
  PhysSelDecay() 
    : m_decayID(0),
    m_results(0) {}

  /// Destructor 
  virtual ~PhysSelDecay() {}

  /// Retrieve all results
  void results(bool& mc, bool& trk, bool& pqual, bool& sel, bool& agr, bool& tag) const;

  /// Retrieve all results
  void setResults(bool mc, bool trk, bool pqual, bool sel, bool agr, bool tag);

  /// Return if decay is present in MC truth
  bool decayIsInMCTree() const;

  /// Return if decay has end particles reconstructed
  bool decayHasTrkRecon() const;

  /// Return if decay has end particles of physics quality
  bool decayHasTrkPQual() const;

  /// Return if event satisfy the decay selection
  bool decayIsSelected() const;

  /// Return if event satisfy the decay selection and one combination is the MC truth
  bool decaySelectedIsMC() const;

  /// Return if one selected combination is also flavour tagged
  bool decayIsFlavourTagged() const;

  /// Set if decay channel is present in MC truth
  void setDecayIsInMCTree();

  /// Set if decay channel has all end particles reconstructed
  void setDecayHasTrkRecon();

  /// Set if decay channel has all end particles reconstructed and of physics quality
  void setDecayHasTrkPQual();

  /// Set if event satisfy the decay selection
  void setDecayIsSelected();

  /// Set if event satisfy the decay selection and one combination is the MC truth
  void setDecaySelectedIsMC();

  /// Set if one selected combination is also flavour tagged
  void setDecayIsFlavourTagged();

  /// Retrieve decayCode as used in Bookkeeping
  long decayID() const; 

  /// Update decayCode as used in Bookkeeping
  void setDecayID(long value);

  /// Retrieve bitset with results of selection analysis
  long results() const; 

  /// Update bitset with results of selection analysis
  void setResults(long value);

  /// Operator overloading for serializing (writing)
  friend StreamBuffer& operator<< (StreamBuffer& s, const PhysSelDecay& obj)
  {
    return obj.serialize(s);
  }

  /// Operator overloading for serializing (reading)
  friend StreamBuffer& operator>> (StreamBuffer& s, PhysSelDecay& obj)
  {
    return obj.serialize(s);
  }

  /// Operator overloading for stringoutput
  friend std::ostream& operator<< (std::ostream& s, const PhysSelDecay& obj)
  {
    return obj.fillStream(s);
  }

  /// Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  virtual StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

  enum selFlags {MCFlag=0, TRKFlag, PQUALFlag, SELFlag, AGRFlag, TAGFlag};   ///<Bit position for results

protected: 


private: 

  long m_decayID; ///<      decayCode as used in Bookkeeping
  long m_results; ///<      bitset with results of selection analysis

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------


inline bool PhysSelDecay::decayIsInMCTree() const
{
   return ( 0 != ( m_results & (1<<MCFlag) ) );
}

inline bool PhysSelDecay::decayHasTrkRecon() const
{
   return ( 0 != ( m_results & (1<<TRKFlag) ) );
}

inline bool PhysSelDecay::decayHasTrkPQual() const
{
   return ( 0 != ( m_results & (1<<PQUALFlag) ) );
}

inline bool PhysSelDecay::decayIsSelected() const
{
   return ( 0 != ( m_results & (1<<SELFlag) ) );
}

inline bool PhysSelDecay::decaySelectedIsMC() const
{
   return ( 0 != ( m_results & (1<<AGRFlag) ) );
}

inline bool PhysSelDecay::decayIsFlavourTagged() const
{
   return ( 0 != (m_results & (1<<TAGFlag) ) );
}

inline void PhysSelDecay::setDecayIsInMCTree()
{
   m_results |= 1<<MCFlag;
}

inline void PhysSelDecay::setDecayHasTrkRecon()
{
   m_results |= 1<<TRKFlag;
}

inline void PhysSelDecay::setDecayHasTrkPQual()
{
   m_results |= 1<<PQUALFlag;
}

inline void PhysSelDecay::setDecayIsSelected()
{
   m_results |= 1<<SELFlag;
}

inline void PhysSelDecay::setDecaySelectedIsMC()
{
   m_results |= 1<<AGRFlag;
}

inline void PhysSelDecay::setDecayIsFlavourTagged()
{
   m_results |= 1<<TAGFlag;
}

inline long PhysSelDecay::decayID() const 
{
  return m_decayID;
}

inline void PhysSelDecay::setDecayID(long value)
{
  m_decayID = value; 
}

inline long PhysSelDecay::results() const 
{
  return m_results;
}

inline void PhysSelDecay::setResults(long value)
{
  m_results = value; 
}

inline StreamBuffer& PhysSelDecay::serialize(StreamBuffer& s) const 
{
  s << m_decayID
    << m_results;
  return s;
}

inline StreamBuffer& PhysSelDecay::serialize(StreamBuffer& s)
{
  s >> m_decayID
    >> m_results;
  return s;
}

inline std::ostream& PhysSelDecay::fillStream(std::ostream& s) const
{
  s << "{ "
    << " decayID:\t" << m_decayID << std::endl
    << "   results:\t" << m_results << " } ";
  return s;
}


#endif   ///PhysEvent_PhysSelDecay_H
