// $Id: MuonHLTData.h,v 1.1 2004-08-31 10:06:10 asatta Exp $
#ifndef MUONHLTDATA_H 
#define MUONHLTDATA_H 1

// Include files
#include "Event/DAQTypes.h"
#include "Event/MuonDigit.h"
#include "Event/RawEvent.h"
#include "Event/DAQTypes.h"
#include "Event/RawBuffer.h"
#include "MuonHLTBase.h"

/** @class MuonHLTData MuonHLTData.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2004-01-23
 */
class MuonHLTData {
public: 
  /// Standard constructor
  MuonHLTData( ); 
  MuonHLTData(raw_int num );

  virtual ~MuonHLTData( ); ///< Destructor
	void setData(raw_int num);	
  void setFirstHalf(unsigned int num);
  void setSecondHalf(unsigned int num);
  //  void setAddress(unsigned int num, unsigned int& value);
  // void setTime(unsigned int num, unsigned int& value);  
  unsigned int getFirstHalf();
  unsigned int getSecondHalf();
  unsigned int getWord();
  
protected:

private:
  
  raw_int m_data;
};
inline void MuonHLTData::setData(raw_int num){m_data=num;};
#endif // MUONHLTDATA_H
