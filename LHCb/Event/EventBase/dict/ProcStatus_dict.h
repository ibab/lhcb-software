
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

#ifndef Event_ProcStatus_H
#define Event_ProcStatus_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include "Kernel/SerializeStl.h"
#include <string>
#include <vector>
#include <ostream>


// Forward declarations

// Class ID definition
static const CLID& CLID_ProcStatus = 5001;

// Namespace for locations in TDS
namespace ProcStatusLocation {
  static const std::string& Default = "Rec/Status";
}


/** @class ProcStatus ProcStatus.h
 *
 * Record the status of the Brunel processing 
 *
 * @author Olivier Callot
 * created Wed Aug 17 09:59:32 2005
 * 
 */

class ProcStatus: public DataObject
{
public:

  friend class ProcStatus_dict;

  /// Default Constructor
  ProcStatus() : m_aborted(false),
                 m_algs(),
                 m_stat() {}

  /// Default Destructor
  virtual ~ProcStatus() {}

  // Retrieve pointer to class definition structure
  virtual const CLID& clID() const;
  static const CLID& classID();

  /// Operator overloading for stringoutput
  friend std::ostream& operator<< (std::ostream& str,
                                   const ProcStatus& obj);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

  /// Add an entry for an algorithm
  void addAlgorithmStatus(const std::string& name,
                          int status);

  /// Status of this algorithm
  int algorithmStatus(const std::string& name);

  /// returns the list of algorithms for which there is a status
  std::vector<std::string>& listOfAlgorithms();

  /// Retrieve const  Indicate if the processing should be aborted
  bool aborted() const;

  /// Update  Indicate if the processing should be aborted
  void setAborted(bool value);

  /// Retrieve const  List of algorithms for which there is a status
  const std::vector<std::string>& algs() const;

  /// Retrieve  List of algorithms for which there is a status
  std::vector<std::string>& algs();

  /// Update  List of algorithms for which there is a status
  void setAlgs(const std::vector<std::string>& value);

  /// Retrieve const  list of status. Just for proper Event Model handling
  const std::vector<int>& stat() const;

  /// Retrieve  list of status. Just for proper Event Model handling
  std::vector<int>& stat();

  /// Update  list of status. Just for proper Event Model handling
  void setStat(const std::vector<int>& value);

protected:

private:

  bool                     m_aborted; ///< Indicate if the processing should be aborted
  std::vector<std::string> m_algs;    ///< List of algorithms for which there is a status
  std::vector<int>         m_stat;    ///< list of status. Just for proper Event Model handling

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline const CLID& ProcStatus::clID() const
{
  return ProcStatus::classID();
}

inline const CLID& ProcStatus::classID()
{
  return CLID_ProcStatus;
}

inline std::ostream& operator<< (std::ostream& str,
                                 const ProcStatus& obj)
{
  return obj.fillStream(str);
}

inline std::ostream& ProcStatus::fillStream(std::ostream& s) const
{
  char l_aborted = (m_aborted) ? 'T' : 'F';
  s << "{ " << "aborted :	" << l_aborted << std::endl
            << "algs :	" << m_algs << std::endl
            << "stat :	" << m_stat << std::endl << " }";
  return s;
}


inline bool ProcStatus::aborted() const 
{
  return m_aborted;
}

inline void ProcStatus::setAborted(bool value) 
{
  m_aborted = value;
}

inline const std::vector<std::string>& ProcStatus::algs() const 
{
  return m_algs;
}

inline std::vector<std::string>& ProcStatus::algs() 
{
  return m_algs;
}

inline void ProcStatus::setAlgs(const std::vector<std::string>& value) 
{
  m_algs = value;
}

inline const std::vector<int>& ProcStatus::stat() const 
{
  return m_stat;
}

inline std::vector<int>& ProcStatus::stat() 
{
  return m_stat;
}

inline void ProcStatus::setStat(const std::vector<int>& value) 
{
  m_stat = value;
}

inline void ProcStatus::addAlgorithmStatus(const std::string& name,
                                           int status) 
{

  for ( unsigned int jj=0 ; m_algs.size() > jj ; jj++ ) {
    if ( m_algs[jj] == name ) {
      m_stat[jj] = status;
      return;
    }
  }
  m_algs.push_back( name   );
  m_stat.push_back( status );
        
}

inline int ProcStatus::algorithmStatus(const std::string& name) 
{

  for ( unsigned int jj=0 ; m_algs.size() > jj ; jj++ ) {
    if ( m_algs[jj] == name ) {
      return m_stat[jj];
    }
  }
  return 0;
        
}

inline std::vector<std::string>& ProcStatus::listOfAlgorithms() 
{
 return m_algs; 
}


#endif ///Event_ProcStatus_H
