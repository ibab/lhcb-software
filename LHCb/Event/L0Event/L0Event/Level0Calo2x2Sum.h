#ifndef LHCBEVENT_LEVEL0CALO2X2SUM_H
#define LHCBEVENT_LEVEL0CALO2X2SUM_H 1

// Include files
#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRefVector.h"
#include "CLHEP/Geometry/Point3D.h"

// Forward declarations
class MCCaloDeposit;

// Externals 
extern const CLID& CLID_Level0Calo2x2Sum;

//------------------------------------------------------------------------------
//
// ClassName:   Level0Calo2x2Sum
//
// Description: This class represents a 2x2 energy sum in the calorimeter L0
//              trigger
//
// Author:      Bruce Hay
//
//------------------------------------------------------------------------------
class Level0Calo2x2Sum : public ContainedObject {

public:
  /// Constructor
  Level0Calo2x2Sum()                               { 
  }
  /// Destructor
  virtual ~Level0Calo2x2Sum()                      {
  }
  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const                 {
    return Level0Calo2x2Sum::classID();
  }
  static const CLID& classID()                     {
    return CLID_Level0Calo2x2Sum;
  }

  /// Type of trigger: 1=electron, 2=gamma, 3=hadron
  int type() const                                 { return m_type; }
  /// Update trigger type: 1=electron, 2=gamma, 3=hadron
  void setType(int type)                           { m_type = type; }

  /// Transverse energy
  double et() const                                { return m_et; }
  /// Update transverse energy
  void setEt(double et)                            { m_et = et; }

  /// Position
  const HepPoint3D& position() const               { return m_position; }
  /// Update position
  void setPosition(HepPoint3D& position)           { m_position = position; }
  void setPosition(double x, double y, double z)   { 
    m_position.setX(x);
    m_position.setY(y);
    m_position.setZ(z);
  }

  /// Tolerance on x/y position = 1/2 cell size
  double posTol() const                            { return m_posTol; }
  // Update tolerance on x/y position = 1/2 cell size
  void setPosTol(double posTol)                    { m_posTol = posTol; }

  /// Preshower energy
  double ePrs() const                              { return m_ePrs; }
  /// Update preshower energy
  void setEPrs(double ePrs)                        { m_ePrs = ePrs; }

  /// ECAL contribution to energy
  double eEcal() const                             { return m_eEcal; }
  /// Update ECAL contribution to energy
  void setEEcal(double eEcal)                      { m_eEcal = eEcal; }

  /// References to ECAL cells used
  const SmartRefVector<MCCaloDeposit>& ecalCells() const;
  SmartRefVector<MCCaloDeposit>& ecalCells();
  /// References to HCAL cells used
  const SmartRefVector<MCCaloDeposit>& hcalCells() const;
  SmartRefVector<MCCaloDeposit>& hcalCells();

  /// Serialize the object for writing
  virtual StreamBuffer& serialize( StreamBuffer& s ) const ;
  /// Serialize the object for reading
  virtual StreamBuffer& serialize( StreamBuffer& s );

private:
  /// Type of trigger: 1=electron, 2=gamma, 3=hadron
  int         m_type;
  /// Transverse energy
  double      m_et;
  /// Position
  HepPoint3D  m_position;
  /// Tolerance on x/y position = 1/2 cell size
  double      m_posTol;
  /// Preshower energy
  double      m_ePrs;
  /// ECAL contribution to energy
  double      m_eEcal;
  /// References to ECAL cells used
  SmartRefVector<MCCaloDeposit> m_ecal;
  /// References to HCAL cells used
  SmartRefVector<MCCaloDeposit> m_hcal;  
};

#include "CaloEvent/MCCaloDeposit.h"
#include "LHCbEvent/CLHEPStreams.h"
//
// Inline code must be outside the class definition
//

/// References to ECAL cells used
inline const SmartRefVector<MCCaloDeposit>& Level0Calo2x2Sum::ecalCells() const {
  return m_ecal;
}
inline SmartRefVector<MCCaloDeposit>& Level0Calo2x2Sum::ecalCells() {
  return m_ecal;
}


/// References to HCAL cells used
inline const SmartRefVector<MCCaloDeposit>& Level0Calo2x2Sum::hcalCells() const {
  return m_hcal;
}
inline SmartRefVector<MCCaloDeposit>& Level0Calo2x2Sum::hcalCells() {
  return m_hcal;
}

/// Serialize the object for reading
inline StreamBuffer& Level0Calo2x2Sum::serialize(StreamBuffer& s) {
  ContainedObject::serialize(s) >> m_type >> m_et >> m_position >> m_posTol >> m_ePrs
				>> m_eEcal >> m_ecal(this) >> m_hcal(this);
  return s;
}

/// Serialize the object for writing
inline StreamBuffer& Level0Calo2x2Sum::serialize(StreamBuffer& s) const {
  ContainedObject::serialize(s) << m_type << m_et << m_position << m_posTol << m_ePrs
				<< m_eEcal << m_ecal(this) << m_hcal(this);
  return s;
}

#endif // LHCBEVENT_LEVEL0CALO2X2SUM_H
