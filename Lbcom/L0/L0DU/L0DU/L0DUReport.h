#ifndef L0DU_L0DUREPORT_H
#define L0DU_L0DUREPORT_H 1
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0DU/L0DU/L0DUReport.h,v 1.3 2001-08-31 11:05:44 ocallot Exp $

// Include files
#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"

/** @namespace L0Trig
 *
 */
namespace L0Trig 
{ 
  enum Trig {
    Decis = 0,
    Electron,
    Electron_down,
    Photon,
    Photon_down,
    Pi0Local,
    Pi0Local_down,
    Pi0Global,
    Pi0Global_down,
    Hadron,
    Hadron_down,
    Mu1,
    Mu1_down,
    SumMu,
    SumMu_down
  };
  enum MaxTrig {
    numberTrig = 32
  };
};

const CLID& CLID_L0DUReport = 6004;

/** @class L0DUReport L0DUReport L0DU/L0DUReport.h
 *
 *  This class represents the output of DecisionUnit
 *
 * @author Nicole Brun
 * @author Pascal Perret 
 * @date   15 June 2001
 */

class L0DUReport : public DataObject {

public:

/** Constructors has many arguments
 *
 * @param typeL0Trig : (integer) report of L0 DecisionUnit
 */

  L0DUReport( int typeL0Trig ) :
    m_typeL0Trig ( typeL0Trig ) { }

  virtual ~L0DUReport() { }

virtual const CLID& clID()    const  { return L0DUReport::classID();  }
static  const CLID& classID()        { return CLID_L0DUReport;  }

/// Returns the decision as boolean

  bool decision() const { return ( 0 != m_typeL0Trig ); }

/// Returns the type as integer

  int typeL0Trig() const { return m_typeL0Trig; }

/// Returns true if one trig is OK

  bool isTrig( int trig ) { return ( 0 != ( ( 1 << trig ) & m_typeL0Trig ) ); }

/// Returns the type as a string, for printing

  std::string typeL0TrigName() const;

// Serialize the object, for read/write

  virtual StreamBuffer& serialize( StreamBuffer& s ) const ;
  virtual StreamBuffer& serialize( StreamBuffer& s );

private:
  int m_typeL0Trig; ///< Type of L0 DecisionUnit 
};

// Returns the type as a string, for printing

inline std::string L0DUReport::typeL0TrigName() const {
  std::string mess = "";
  if ( 0 != ( ( 1 << L0Trig::Electron )  & m_typeL0Trig ) ) { 
    mess += "Electron ";
  }
  if ( 0 != ( ( 1 << L0Trig::Photon )    & m_typeL0Trig ) ) {
    mess += "Photon ";
  }
  if ( 0 != ( ( 1 <<  L0Trig::Hadron )   & m_typeL0Trig ) ) {
    mess += "Hadron ";
  }
  if ( 0 != ( ( 1 << L0Trig::Pi0Local )  & m_typeL0Trig ) ) {
    mess += "Pi0Local ";
  }
  if ( 0 != ( ( 1 << L0Trig::Pi0Global ) & m_typeL0Trig ) ) {
    mess += "Pi0Global ";
  }
  if ( 0 != ( ( 1 << L0Trig::Mu1 )       & m_typeL0Trig ) ) {
    mess += "Mu1 ";
  }
  if ( 0 != ( ( 1 << L0Trig::SumMu )     & m_typeL0Trig ) ) {
    mess += "SumMu"; 
  }
  if ( 0 != ( ( 1 << L0Trig::Electron_down )  & m_typeL0Trig ) ) { 
    mess += "Electron_down ";
  }
  if ( 0 != ( ( 1 << L0Trig::Photon_down )    & m_typeL0Trig ) ) {
    mess += "Photon_down ";
  }
  if ( 0 != ( ( 1 <<  L0Trig::Hadron_down )   & m_typeL0Trig ) ) {
    mess += "Hadron_down ";
  }
  if ( 0 != ( ( 1 << L0Trig::Pi0Local_down )  & m_typeL0Trig ) ) {
    mess += "Pi0Local_down ";
  }
  if ( 0 != ( ( 1 << L0Trig::Pi0Global_down ) & m_typeL0Trig ) ) {
    mess += "Pi0Global_down ";
  }
  if ( 0 != ( ( 1 << L0Trig::Mu1_down )       & m_typeL0Trig ) ) {
    mess += "Mu1_down ";
  }
  if ( 0 != ( ( 1 << L0Trig::SumMu_down )     & m_typeL0Trig ) ) {
    mess += "SumMu_down"; 
  }
  
  if ( "" == mess ) {
    mess = "(no trigger)";
  }
  
  return ( "LODUReport = " + mess ) ;
}

/// Serialize the object for reading

inline StreamBuffer& L0DUReport::serialize(StreamBuffer& s) {
  DataObject::serialize(s) >> m_typeL0Trig;
  return s;
}

/// Serialize the object for writing

inline StreamBuffer& L0DUReport::serialize(StreamBuffer& s) const {
  DataObject::serialize(s) << m_typeL0Trig;
  return s;
}

//template <class TYPE> class ObjectVector;
//typedef  ObjectVector<L0DUReport> L0DUReportVector;

#endif // L0DU_L0DUREPORT_H












