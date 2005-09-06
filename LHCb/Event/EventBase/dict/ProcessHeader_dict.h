
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

#ifndef Event_ProcessHeader_H
#define Event_ProcessHeader_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include "Kernel/SerializeStl.h"
#include <vector>
#include <string>
#include <ostream>


// Forward declarations

// Class ID definition
static const CLID& CLID_ProcessHeader = 199;

// Namespace for locations in TDS
namespace ProcessHeaderLocation {
  static const std::string& Default = "Gen/Header";
}


/** @class ProcessHeader ProcessHeader.h
 *
 * Common base class for all process headers: GenHeader, MCHeader... @todo ASK
 * MARCO FOR ID 
 *
 * @author P. Koppenburg
 * created Wed Aug 17 09:58:35 2005
 * 
 */

class ProcessHeader: public DataObject
{
public:

  friend class ProcessHeader_dict;

  /// Default Constructor
  ProcessHeader() : m_randomSeeds(),
                    m_applicationName("Unknown"),
                    m_applicationVersion("Unknown") {}

  /// Default Destructor
  virtual ~ProcessHeader() {}

  // Retrieve pointer to class definition structure
  virtual const CLID& clID() const;
  static const CLID& classID();

  /// Operator overloading for stringoutput
  friend std::ostream& operator<< (std::ostream& str,
                                   const ProcessHeader& obj);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

  /// Retrieve const  random number seeds for this step (int or long?)
  const std::vector < int >& randomSeeds() const;

  /// Retrieve  random number seeds for this step (int or long?)
  std::vector < int >& randomSeeds();

  /// Update  random number seeds for this step (int or long?)
  void setRandomSeeds(const std::vector < int >& value);

  /// Retrieve const  Application or Program Name (e.g. Gauss, Boole or Pythia)
  const std::string& applicationName() const;

  /// Retrieve  Application or Program Name (e.g. Gauss, Boole or Pythia)
  std::string& applicationName();

  /// Update  Application or Program Name (e.g. Gauss, Boole or Pythia)
  void setApplicationName(const std::string& value);

  /// Retrieve const  Application or Program version
  const std::string& applicationVersion() const;

  /// Retrieve  Application or Program version
  std::string& applicationVersion();

  /// Update  Application or Program version
  void setApplicationVersion(const std::string& value);

protected:

private:

  std::vector < int > m_randomSeeds;        ///< random number seeds for this step (int or long?)
  std::string         m_applicationName;    ///< Application or Program Name (e.g. Gauss, Boole or Pythia)
  std::string         m_applicationVersion; ///< Application or Program version

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline const CLID& ProcessHeader::clID() const
{
  return ProcessHeader::classID();
}

inline const CLID& ProcessHeader::classID()
{
  return CLID_ProcessHeader;
}

inline std::ostream& operator<< (std::ostream& str,
                                 const ProcessHeader& obj)
{
  return obj.fillStream(str);
}

inline std::ostream& ProcessHeader::fillStream(std::ostream& s) const
{
  s << "{ " << "randomSeeds :	" << m_randomSeeds << std::endl
            << "applicationName :	" << m_applicationName << std::endl
            << "applicationVersion :	" << m_applicationVersion << std::endl << " }";
  return s;
}


inline const std::vector < int >& ProcessHeader::randomSeeds() const 
{
  return m_randomSeeds;
}

inline std::vector < int >& ProcessHeader::randomSeeds() 
{
  return m_randomSeeds;
}

inline void ProcessHeader::setRandomSeeds(const std::vector < int >& value) 
{
  m_randomSeeds = value;
}

inline const std::string& ProcessHeader::applicationName() const 
{
  return m_applicationName;
}

inline std::string& ProcessHeader::applicationName() 
{
  return m_applicationName;
}

inline void ProcessHeader::setApplicationName(const std::string& value) 
{
  m_applicationName = value;
}

inline const std::string& ProcessHeader::applicationVersion() const 
{
  return m_applicationVersion;
}

inline std::string& ProcessHeader::applicationVersion() 
{
  return m_applicationVersion;
}

inline void ProcessHeader::setApplicationVersion(const std::string& value) 
{
  m_applicationVersion = value;
}


#endif ///Event_ProcessHeader_H
