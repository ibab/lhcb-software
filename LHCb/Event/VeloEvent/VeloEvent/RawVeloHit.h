// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Event/VeloEvent/VeloEvent/RawVeloHit.h,v 1.1.1.1 2000-11-23 13:39:24 ranjard Exp $
#ifndef LHCBEVENT_RAWVELOHIT_H
#define LHCBEVENT_RAWVELOHIT_H 1

// Include files
#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/ContainedObject.h"

// Externals 
extern const CLID& CLID_RawVeloHit;

//--------------------------------------------------------------------------
//
// ClassName:   RawVeloHit 
//  
// Description: Vertex detector digits
//
// Author:      B.Hay
// Changes:     P.Binko 19/10/1999 : Formating of ASCII output
//
//--------------------------------------------------------------------------
class RawVeloHit : virtual public ContainedObject {

public:
	// Default constructor
	RawVeloHit()                    { 
    /// Is the hit a R-hit or Phi-hit
    m_isRt        = false;
    /// Sector number
    m_sector      = -1;
    /// Detector type
    m_type        = -1;
    /// Strip number
    m_stripNumber = -1;
    /// StripCharge
    m_charge      = 0.0;
    /// ADC value
    m_adcCount    = -1;
    /// Wafer number
    m_waferNumber = -1;
  }
	// Virtual destructor
	virtual ~RawVeloHit()              { 
  }
	/// Retrieve pointer to class defininition structure
	virtual const CLID& clID() const    { 
    return RawVeloHit::classID(); 
  }
	static const CLID& classID()        { 
    return CLID_RawVeloHit; 
  }
  /// Is the hit a R-hit
  bool isR ()  const  {
    return m_isRt;
  }
  /// Is the hit a Phi-hit
  bool isPhi () const  {
    return !m_isRt;
  }
  /// Set hit to R-type
  void setR (bool val)    {
    m_isRt = val;
  }
  /// Sector number
  long sector()  const  {
    return m_sector;
  }
  /// Set Sector number
  void setSector(long val)   {
    m_sector = val;
  }
  /// Detector type
  long type()   const   {
    return m_type;
  }
  /// Detector type
  void setType(long typ)   {
    m_type = typ;
  }
  /// Strip number
  long stripNumber()    const   {
    return m_stripNumber;
  }
  /// Strip number
  void setStripNumber(long num)   {
    m_stripNumber = num;
  }
  /// StripCharge
  float charge()   const   {
    return m_charge;
  }
  /// StripCharge
  void setCharge(float val)    {
    m_charge = val;
  }
  /// ADC value
  long adcCount()   const   {
    return m_adcCount;
  }
  /// ADC value
  void setAdcCount(long val)   {
    m_adcCount = val;
  }
  /// Access wafer number 
  long waferNumber()    const    {
    return m_waferNumber;
  }
  /// Set wafer number 
  void setWaferNumber(long wafer)     {
    m_waferNumber = wafer;
  }
  /// Serialize the object for writing
  virtual StreamBuffer& serialize( StreamBuffer& s ) const    {
    unsigned char rphi = m_isRt ? 1 : 0;
    ContainedObject::serialize(s) << rphi << m_sector << m_type << m_charge << m_adcCount;
    return s;
  }
  /// Serialize the object for reading
  virtual StreamBuffer& serialize( StreamBuffer& s )          {
    unsigned char rphi;
    ContainedObject::serialize(s) >> rphi >> m_sector >> m_type >> m_charge >> m_adcCount;
    m_isRt = rphi==1;
    return s;
  }
private:
  /// Is the hit a R-hit or Phi-hit
  bool  m_isRt;
  /// Sector number
  long  m_sector;
  /// Detector type
  long  m_type;
  /// Strip number
  long  m_stripNumber;
  /// StripCharge
  float m_charge;
  /// ADC value
  long  m_adcCount;
  /// Wafer number
  long m_waferNumber;
};
#endif  // LHCBEVENT_RAWVELOHIT_H
