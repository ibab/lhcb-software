

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



#ifndef RichRecBase_RichLocalPID_H
#define RichRecBase_RichLocalPID_H 1

// Include files
#include <algorithm>
#include <vector>
#include "Kernel/CLHEPStreams.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"
#include "Event/RichPID.h"
#include "Kernel/SerializeStl.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/StreamBuffer.h"


// Forward declarations
class RichLocalPID;


// Class ID definition
  static const CLID& CLID_RichLocalPID = 12441;

// Namespace for locations in TDS
namespace RichLocalPIDLocation {
  static const std::string& Default = "Rec/Rich/LocalPIDs";
  static const std::string& Aerogel = "Rec/Rich/LocalAerogelPIDs";
  static const std::string& C4F10 = "Rec/Rich/LocalC4F10PIDs";
  static const std::string& CF4 = "Rec/Rich/LocalCF4PIDs";
}


/** @class RichLocalPID RichLocalPID.h 
 *
 *  Rich particle identification object for the local track based algorithm
 *
 *  @author Chris Jones    (Christopher.Rob.Jones@cern.ch)
 *  created Sun Mar  2 19:00:06 2003
 *
 */

class RichLocalPID: public RichPID
{

public: 

  friend class RichLocalPID_dict;

  /// Default constructor
  RichLocalPID()
    : m_gaussianSignal(Rich::NParticleTypes), m_gaussianBackgrd(Rich::NParticleTypes), m_signalCount(Rich::NParticleTypes), m_sideCount(Rich::NParticleTypes), m_signalProbability(Rich::NParticleTypes), m_ringProbability(Rich::NParticleTypes)
  {
this->setLocal(1);
  }

  /// Destructor 
  virtual ~RichLocalPID() {}

  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const; 
  static const CLID& classID(); 

  /// Retrieve origin of the PID result
  unsigned long localPIDType() const; 

  /// Update origin of the PID result
  void setLocalPIDType(unsigned long value);

  /// Update Information from aerogel was used to form this PID result
  void setUsedAerogel(unsigned long value);

  /// Retrieve Information from aerogel was used to form this PID result
  unsigned long usedAerogel() const;

  /// Update Information from C4F10 was used to form this PID result
  void setUsedC4F10(unsigned long value);

  /// Retrieve Information from C4F10 was used to form this PID result
  unsigned long usedC4F10() const;

  /// Update Information from CF4 was used to form this PID result
  void setUsedCF4(unsigned long value);

  /// Retrieve Information from CF4 was used to form this PID result
  unsigned long usedCF4() const;

  /// Update Information from all available radiators was used to form this PID result
  void setUsedAllRadiators(unsigned long value);

  /// Retrieve Information from all available radiators was used to form this PID result
  unsigned long usedAllRadiators() const;

  /// Retrieve Deduced signal using Gaussian weighted sum for each mass hypothesis
  const std::vector<float>& gaussianSignal() const; 

  /// Retrieve Deduced signal using Gaussian weighted sum for each mass hypothesis (non-const)
  std::vector<float>& gaussianSignal();

  /// Update Deduced signal using Gaussian weighted sum for each mass hypothesis
  void setGaussianSignal(const std::vector<float>& value);

  /// Retrieve Deduced background using Gaussian weighted sum for each mass hypothesis
  const std::vector<float>& gaussianBackgrd() const; 

  /// Retrieve Deduced background using Gaussian weighted sum for each mass hypothesis (non-const)
  std::vector<float>& gaussianBackgrd();

  /// Update Deduced background using Gaussian weighted sum for each mass hypothesis
  void setGaussianBackgrd(const std::vector<float>& value);

  /// Retrieve  Number of photon candidates in signal region for each mass hypothesis
  const std::vector<int>& signalCount() const; 

  /// Retrieve  Number of photon candidates in signal region for each mass hypothesis (non-const)
  std::vector<int>& signalCount();

  /// Update  Number of photon candidates in signal region for each mass hypothesis
  void setSignalCount(const std::vector<int>& value);

  /// Retrieve  Number of photon candidates in signal +side-band regions for each mass hypothesis
  const std::vector<int>& sideCount() const; 

  /// Retrieve  Number of photon candidates in signal +side-band regions for each mass hypothesis (non-const)
  std::vector<int>& sideCount();

  /// Update  Number of photon candidates in signal +side-band regions for each mass hypothesis
  void setSideCount(const std::vector<int>& value);

  /// Retrieve Probability values for each hypothesis based on observed signal
  const std::vector<float>& signalProbability() const; 

  /// Retrieve Probability values for each hypothesis based on observed signal (non-const)
  std::vector<float>& signalProbability();

  /// Update Probability values for each hypothesis based on observed signal
  void setSignalProbability(const std::vector<float>& value);

  /// Retrieve Probability values for each hypothesis based on a measure of the ring quality
  const std::vector<float>& ringProbability() const; 

  /// Retrieve Probability values for each hypothesis based on a measure of the ring quality (non-const)
  std::vector<float>& ringProbability();

  /// Update Probability values for each hypothesis based on a measure of the ring quality
  void setRingProbability(const std::vector<float>& value);

  /// Retrieve related local PIDs (const)
  const SmartRefVector<RichLocalPID>& relatedPIDs() const;

  /// Retrieve related local PIDs (non-const)
  SmartRefVector<RichLocalPID>& relatedPIDs();

  /// Update related local PIDs
  void setRelatedPIDs(const SmartRefVector<RichLocalPID>& value);

  /// Add related local PIDs
  void addToRelatedPIDs(const SmartRef<RichLocalPID>& value); 

  /// Remove related local PIDs
  void removeFromRelatedPIDs(const SmartRef<RichLocalPID>& value); 

  /// Clear related local PIDs
  void clearRelatedPIDs();

  /// Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  virtual StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

protected: 

private: 

  enum localPIDTypeBits {usedAerogelBits = 0, usedC4F10Bits = 1, usedCF4Bits = 2, usedAllRadiatorsBits = 3}; ///< Offsets of bitfield localPIDType
  enum localPIDTypeMasks {usedAerogelMask = 0x00000001, usedC4F10Mask = 0x00000002, usedCF4Mask = 0x00000004, usedAllRadiatorsMask = 0x00000008}; ///< Bitmasks for bitfield localPIDType

  unsigned long m_localPIDType; ///< origin of the PID result
  std::vector<float> m_gaussianSignal; ///< Deduced signal using Gaussian weighted sum for each mass hypothesis
  std::vector<float> m_gaussianBackgrd; ///< Deduced background using Gaussian weighted sum for each mass hypothesis
  std::vector<int> m_signalCount; ///<  Number of photon candidates in signal region for each mass hypothesis
  std::vector<int> m_sideCount; ///<  Number of photon candidates in signal +side-band regions for each mass hypothesis
  std::vector<float> m_signalProbability; ///< Probability values for each hypothesis based on observed signal
  std::vector<float> m_ringProbability; ///< Probability values for each hypothesis based on a measure of the ring quality
  SmartRefVector<RichLocalPID> m_relatedPIDs; ///< related local PIDs

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------


// Including forward declarations
#include "Event/RichLocalPID.h"


inline const CLID& RichLocalPID::clID() const 
{
  return RichLocalPID::classID();
}

inline const CLID& RichLocalPID::classID()
{
  return CLID_RichLocalPID;
}

inline unsigned long RichLocalPID::localPIDType() const 
{
  return m_localPIDType;
}

inline void RichLocalPID::setLocalPIDType(unsigned long value)
{
  m_localPIDType = value; 
}

inline void RichLocalPID::setUsedAerogel(unsigned long value)
{
  m_localPIDType &= ~usedAerogelMask;
  m_localPIDType |= ((value << usedAerogelBits) & usedAerogelMask);
}

inline unsigned long RichLocalPID::usedAerogel() const
{
  return ((m_localPIDType & usedAerogelMask) >> usedAerogelBits);
}

inline void RichLocalPID::setUsedC4F10(unsigned long value)
{
  m_localPIDType &= ~usedC4F10Mask;
  m_localPIDType |= ((value << usedC4F10Bits) & usedC4F10Mask);
}

inline unsigned long RichLocalPID::usedC4F10() const
{
  return ((m_localPIDType & usedC4F10Mask) >> usedC4F10Bits);
}

inline void RichLocalPID::setUsedCF4(unsigned long value)
{
  m_localPIDType &= ~usedCF4Mask;
  m_localPIDType |= ((value << usedCF4Bits) & usedCF4Mask);
}

inline unsigned long RichLocalPID::usedCF4() const
{
  return ((m_localPIDType & usedCF4Mask) >> usedCF4Bits);
}

inline void RichLocalPID::setUsedAllRadiators(unsigned long value)
{
  m_localPIDType &= ~usedAllRadiatorsMask;
  m_localPIDType |= ((value << usedAllRadiatorsBits) & usedAllRadiatorsMask);
}

inline unsigned long RichLocalPID::usedAllRadiators() const
{
  return ((m_localPIDType & usedAllRadiatorsMask) >> usedAllRadiatorsBits);
}

inline const std::vector<float>& RichLocalPID::gaussianSignal() const 
{
  return m_gaussianSignal;
}

inline std::vector<float>& RichLocalPID::gaussianSignal()
{
  return m_gaussianSignal;
}

inline void RichLocalPID::setGaussianSignal(const std::vector<float>& value)
{
  m_gaussianSignal = value; 
}

inline const std::vector<float>& RichLocalPID::gaussianBackgrd() const 
{
  return m_gaussianBackgrd;
}

inline std::vector<float>& RichLocalPID::gaussianBackgrd()
{
  return m_gaussianBackgrd;
}

inline void RichLocalPID::setGaussianBackgrd(const std::vector<float>& value)
{
  m_gaussianBackgrd = value; 
}

inline const std::vector<int>& RichLocalPID::signalCount() const 
{
  return m_signalCount;
}

inline std::vector<int>& RichLocalPID::signalCount()
{
  return m_signalCount;
}

inline void RichLocalPID::setSignalCount(const std::vector<int>& value)
{
  m_signalCount = value; 
}

inline const std::vector<int>& RichLocalPID::sideCount() const 
{
  return m_sideCount;
}

inline std::vector<int>& RichLocalPID::sideCount()
{
  return m_sideCount;
}

inline void RichLocalPID::setSideCount(const std::vector<int>& value)
{
  m_sideCount = value; 
}

inline const std::vector<float>& RichLocalPID::signalProbability() const 
{
  return m_signalProbability;
}

inline std::vector<float>& RichLocalPID::signalProbability()
{
  return m_signalProbability;
}

inline void RichLocalPID::setSignalProbability(const std::vector<float>& value)
{
  m_signalProbability = value; 
}

inline const std::vector<float>& RichLocalPID::ringProbability() const 
{
  return m_ringProbability;
}

inline std::vector<float>& RichLocalPID::ringProbability()
{
  return m_ringProbability;
}

inline void RichLocalPID::setRingProbability(const std::vector<float>& value)
{
  m_ringProbability = value; 
}

inline const SmartRefVector<RichLocalPID>& RichLocalPID::relatedPIDs() const
{
   return m_relatedPIDs;
}

inline SmartRefVector<RichLocalPID>& RichLocalPID::relatedPIDs()
{
   return m_relatedPIDs;
}

inline void RichLocalPID::setRelatedPIDs(const SmartRefVector<RichLocalPID>& value)
{
   m_relatedPIDs = value;
}

inline void RichLocalPID::addToRelatedPIDs(const SmartRef<RichLocalPID>& value)
{
   m_relatedPIDs.push_back(value);
}

inline void RichLocalPID::removeFromRelatedPIDs(const SmartRef<RichLocalPID>& value)
{
  SmartRefVector<RichLocalPID>::iterator iter =
    std::remove(m_relatedPIDs.begin(), m_relatedPIDs.end(), value);
  m_relatedPIDs.erase(iter, m_relatedPIDs.end());
}

inline void RichLocalPID::clearRelatedPIDs()
{
   m_relatedPIDs.clear();
}

inline StreamBuffer& RichLocalPID::serialize(StreamBuffer& s) const 
{
  RichPID::serialize(s);
  s << m_localPIDType
    << m_gaussianSignal
    << m_gaussianBackgrd
    << m_signalCount
    << m_sideCount
    << m_signalProbability
    << m_ringProbability
    << m_relatedPIDs(this);
  return s;
}

inline StreamBuffer& RichLocalPID::serialize(StreamBuffer& s)
{
  RichPID::serialize(s);
  s >> m_localPIDType
    >> m_gaussianSignal
    >> m_gaussianBackgrd
    >> m_signalCount
    >> m_sideCount
    >> m_signalProbability
    >> m_ringProbability
    >> m_relatedPIDs(this);
  return s;
}

inline std::ostream& RichLocalPID::fillStream(std::ostream& s) const
{
  s << "{ "
    << " localPIDType:\t" << m_localPIDType << std::endl
    << "   usedAerogel:\t" << usedAerogel() << std::endl
    << "   usedC4F10:\t" << usedC4F10() << std::endl
    << "   usedCF4:\t" << usedCF4() << std::endl
    << "   usedAllRadiators:\t" << usedAllRadiators() << std::endl
    << "   gaussianSignal:\t" << m_gaussianSignal << std::endl
    << "   gaussianBackgrd:\t" << m_gaussianBackgrd << std::endl
    << "   signalCount:\t" << m_signalCount << std::endl
    << "   sideCount:\t" << m_sideCount << std::endl
    << "   signalProbability:\t" << m_signalProbability << std::endl
    << "   ringProbability:\t" << m_ringProbability << " } ";
  return s;
}

//Defintion of keyed container for RichLocalPID
typedef KeyedContainer<RichLocalPID, Containers::HashMap> RichLocalPIDs;
// typedef for std::vector of RichLocalPID
typedef std::vector<RichLocalPID*> RichLocalPIDVector;

#endif   ///RichRecBase_RichLocalPID_H
