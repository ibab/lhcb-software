
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

#ifndef Event_AlgUsedTime_H
#define Event_AlgUsedTime_H 1

// Include files
#include "GaudiKernel/KeyedObject.h"
#include "GaudiKernel/KeyedContainer.h"
#include "Kernel/SerializeStl.h"
#include <string>
#include <ostream>


// Forward declarations

// Class ID definition
static const CLID& CLID_AlgUsedTime = 5002;

// Namespace for locations in TDS
namespace AlgUsedTimeLocation {
  static const std::string& Default = "Trig/AlgUsedTimes";
}


/** @class AlgUsedTime AlgUsedTime.h
 *
 * store name of algorithm instance and CPU time it used 
 *
 * @author Massimiliano Ferro-Luzzi
 * created Wed Aug 17 09:58:34 2005
 * 
 */

class AlgUsedTime: public KeyedObject<int>
{
public:

  friend class AlgUsedTime_dict;

  /// Default Constructor
  AlgUsedTime() : m_algName(),
                  m_userTime(0),
                  m_elapTime(0) {}

  /// Default Destructor
  virtual ~AlgUsedTime() {}

  // Retrieve pointer to class definition structure
  virtual const CLID& clID() const;
  static const CLID& classID();

  /// Operator overloading for stringoutput
  friend std::ostream& operator<< (std::ostream& str,
                                   const AlgUsedTime& obj);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

  /// Retrieve const  name of timed algorithm instance
  const std::string& algName() const;

  /// Retrieve  name of timed algorithm instance
  std::string& algName();

  /// Update  name of timed algorithm instance
  void setAlgName(const std::string& value);

  /// Retrieve const  USER time consumed by algorithm instance
  float userTime() const;

  /// Update  USER time consumed by algorithm instance
  void setUserTime(float value);

  /// Retrieve const  ELAPSED time consumed by algorithm instance
  float elapTime() const;

  /// Update  ELAPSED time consumed by algorithm instance
  void setElapTime(float value);

protected:

private:

  std::string m_algName;  ///< name of timed algorithm instance
  float       m_userTime; ///< USER time consumed by algorithm instance
  float       m_elapTime; ///< ELAPSED time consumed by algorithm instance

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline const CLID& AlgUsedTime::clID() const
{
  return AlgUsedTime::classID();
}

inline const CLID& AlgUsedTime::classID()
{
  return CLID_AlgUsedTime;
}

inline std::ostream& operator<< (std::ostream& str,
                                 const AlgUsedTime& obj)
{
  return obj.fillStream(str);
}

inline std::ostream& AlgUsedTime::fillStream(std::ostream& s) const
{
  s << "{ " << "algName :	" << m_algName << std::endl
            << "userTime :	" << m_userTime << std::endl
            << "elapTime :	" << m_elapTime << std::endl << " }";
  return s;
}


inline const std::string& AlgUsedTime::algName() const 
{
  return m_algName;
}

inline std::string& AlgUsedTime::algName() 
{
  return m_algName;
}

inline void AlgUsedTime::setAlgName(const std::string& value) 
{
  m_algName = value;
}

inline float AlgUsedTime::userTime() const 
{
  return m_userTime;
}

inline void AlgUsedTime::setUserTime(float value) 
{
  m_userTime = value;
}

inline float AlgUsedTime::elapTime() const 
{
  return m_elapTime;
}

inline void AlgUsedTime::setElapTime(float value) 
{
  m_elapTime = value;
}

// Definition of Keyed Container for AlgUsedTime
typedef KeyedContainer<AlgUsedTime, Containers::HashMap> AlgUsedTimes;



#endif ///Event_AlgUsedTime_H
