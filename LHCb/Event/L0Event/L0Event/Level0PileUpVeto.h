#ifndef LHCBEVENT_LEVEL0PILEUPVETO_H
#define LHCBEVENT_LEVEL0PILEUPVETO_H 1

// Include files
#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StreamBuffer.h"

#include "L0Event/CLID_Level0PileUpVeto.h"

//-----------------------------------------------------------------------------
//
// ClassName:   Level0PileUpVeto
//
// Description: This class represents the multiple vertices summary from the 
//              pileup veto
//
// Author:      Bruce Hay
//
//-----------------------------------------------------------------------------
class Level0PileUpVeto : public DataObject {

public:
  /// Constructor
  Level0PileUpVeto()                               {
  }
  /// Destructor
  virtual ~ Level0PileUpVeto()                     {
  }
  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const    { return Level0PileUpVeto::classID();  }
  static const CLID& classID()        { return CLID_Level0PileUpVeto;        }

  /// Pileup decision
  int decision() const                             { return m_decision; }
  /// Update the pileup decision
  void setDecision(int decision)                   { m_decision = decision; }
  
  /// Height of the first peak
  double heightPeak1() const                       { return m_hPeak1; }
  /// Update height of the first peak
  void setHeightPeak1(double hPeak1)               { m_hPeak1 = hPeak1; }
  
  /// Sum under the first peak
  double sumPeak1() const                          { return m_sPeak1; }
  /// Update sum under the first peak
  void setSumPeak1(double sPeak1)                  { m_sPeak1 = sPeak1; }

  /// Z position of the first peak
  double zPosPeak1() const                         { return m_zPeak1; }
  /// Update z position of the first peak
  void setZPosPeak1(double zPeak1)                 { m_zPeak1 = zPeak1; }

  /// Expected z resolution for first peak
  double zResPeak1() const                         { return m_rPeak1; }
  /// Update expected z resolution for first peak
  void setZResPeak1(double rPeak1)                 { m_rPeak1 = rPeak1; }

  /// Expected background level (in double event) at position of first peak
  double bkgPeak1() const                          { return m_bPeak1; }
  /// Update expected background level at position of first peak
  void setBkgPeak1(double bPeak1)                  { m_bPeak1 = bPeak1; }

  /// Height of the second peak
  double heightPeak2() const                       { return m_hPeak2; }
  /// Update height of the second peak
  void setHeightPeak2(double hPeak2)               { m_hPeak2 = hPeak2; }
  
  /// Sum under the second peak
  double sumPeak2() const                          { return m_sPeak2; }
  /// Update sum under the second peak
  void setSumPeak2(double sPeak2)                  { m_sPeak2 = sPeak2; }

  /// Z position of the second peak
  double zPosPeak2() const                         { return m_zPeak2; }
  /// Update z position of the second peak
  void setZPosPeak2(double zPeak2)                 { m_zPeak2 = zPeak2; }

  /// Expected z resolution for second peak
  double zResPeak2() const                         { return m_rPeak2; }
  /// Update expected z resolution for second peak
  void setZResPeak2(double rPeak2)                 { m_rPeak2 = rPeak2; }

  /// Expected background level (in double event) at position of second peak
  double bkgPeak2() const                          { return m_bPeak2; }
  /// Update expected background level position of second peak
  void setBkgPeak2(double bPeak2)                  { m_bPeak2 = bPeak2; }

  /// Total number of coincidences within histogram range
  double sTot() const                              { return m_sTot; }
  /// Update total number of coincidences within histogram range
  void setSTot(double sTot)                        { m_sTot = sTot; }

  /// Average background level after two found peaks are scratched out
  double bkgLevel() const                          { return m_bkgLevel; }
  /// Update average background level after two found peaks are scratched out
  void setBkgLevel(double bkgLevel)                { m_bkgLevel = bkgLevel; }

  /// Serialize the object for writing
  virtual StreamBuffer& serialize( StreamBuffer& s ) const ;
  /// Serialize the object for reading
  virtual StreamBuffer& serialize( StreamBuffer& s );
  
private:
  /// Pileup decision
  int      m_decision;
  // Height of the first peak
  double   m_hPeak1;
  /// Sum under the first peak
  double   m_sPeak1;
  /// Z position of the first peak
  double   m_zPeak1;
  /// Expected z resolution for first peak
  double   m_rPeak1;
  /// Expected background level (in double event) at position of first peak
  double   m_bPeak1;
  /// Height of the second peak
  double   m_hPeak2;
  /// Sum under the second peak
  double   m_sPeak2;
  /// Z position of the second peak
  double   m_zPeak2;
  /// Expected z resolution for second peak
  double   m_rPeak2;
  /// Expected background level (in double event) at position of second peak
  double   m_bPeak2;
  /// Total number of coincidences within histogram range
  double   m_sTot;
  /// Average background level after two found peaks are scratched out
  double  m_bkgLevel;
};

//
// Inline code must be outside the class definition
//

/// Serialize the object for reading
inline StreamBuffer& Level0PileUpVeto::serialize(StreamBuffer& s) {
  DataObject::serialize(s) >> m_decision >> m_hPeak1 >> m_sPeak1 >> m_zPeak1
			   >> m_rPeak1 >> m_bPeak1 >> m_hPeak2 >> m_sPeak2 
			   >> m_zPeak2 >> m_rPeak2 >> m_bPeak2 >> m_sTot
			   >> m_bkgLevel;
  return s;
}

/// Serialize the object for writing
inline StreamBuffer& Level0PileUpVeto::serialize(StreamBuffer& s) const {
  DataObject::serialize(s) << m_decision << m_hPeak1 << m_sPeak1 << m_zPeak1
			   << m_rPeak1 << m_bPeak1 << m_hPeak2 << m_sPeak2 
			   << m_zPeak2 << m_rPeak2 << m_bPeak2 << m_sTot
			   << m_bkgLevel;
  return s;
}

#endif // LHCBEVENT_LEVEL0PILEUPVETO_H
