// $Id: MuonStationCabling.h,v 1.1 2004-02-03 11:40:27 asatta Exp $
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
  StatusCode update(std::string newL1Pointer);

 /// Class ID of this class
  inline static  const CLID& classID() {
    return CLID_MuonStationCabling;
  }

inline	long getNumberOfL1Board(){return m_numberOfL1Board;};
inline std::string getL1Name(unsigned int i){return m_listOfL1[i];};  
protected:

private:
  long m_numberOfL1Board;
  std::vector<std::string> m_listOfL1;
  
};
#endif // MUONDET_MUONSTATIONCABLING_H

