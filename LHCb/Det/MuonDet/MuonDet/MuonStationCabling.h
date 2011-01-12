// $Id: MuonStationCabling.h,v 1.2 2007-02-27 11:13:57 asatta Exp $
#ifndef MUONDET_MUONSTATIONCABLING_H 
#define MUONDET_MUONSTATIONCABLING_H 1

#include <vector>
#include "GaudiKernel/DataObject.h"
#include "DetDesc/Condition.h"

// Include files
#include "MuonDet/CLID_MuonStationCabling.h"
/** @class MuonStationCabling MuonStationCabling.h MuonDet/MuonStationCabling.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2004-01-07
 */
class MuonStationCabling: public Condition {
public: 
  /// Standard constructor
  MuonStationCabling( ); 

  virtual ~MuonStationCabling( ); ///< Destructor

#ifdef __INTEL_COMPILER        // Disable ICC warning
  #pragma warning(disable:654) // virtual function is hidden, override intended?
  #pragma warning(push)
#endif
  virtual void update ( Condition& obj );
  virtual void update ( ValidDataObject& obj );
#ifdef __INTEL_COMPILER // Re-enable ICC warning
  #pragma warning(pop)
#endif

 /// Class ID of this class
  inline static  const CLID& classID() {
    return CLID_MuonStationCabling;
  }

inline	long getNumberOfL1Board(){return m_numberOfL1Board;};
inline std::string getL1Name(unsigned int i){return m_listOfL1[i];};  
inline std::vector<std::string> getAllL1Names(){return m_listOfL1;};  
 StatusCode update(long l1numb);
  StatusCode addL1Name(std::string name);

 
protected:

private:
  long m_numberOfL1Board;
  std::vector<std::string> m_listOfL1;
  
};
#endif // MUONDET_MUONSTATIONCABLING_H

