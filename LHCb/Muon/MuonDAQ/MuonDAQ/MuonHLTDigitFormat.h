// $Id: MuonHLTDigitFormat.h,v 1.1 2004-08-31 10:06:10 asatta Exp $
#ifndef MUONHLTDIGITFORMAT_H 
#define MUONHLTDIGITFORMAT_H 1

// Include files
#include "MuonHLTBase.h"

/** @class MuonHLTDigitFormat MuonHLTDigitFormat.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2004-01-23
 */
class MuonHLTDigitFormat {
public: 
  /// Standard constructor
  MuonHLTDigitFormat( ); 
  MuonHLTDigitFormat(unsigned int num );
  virtual ~MuonHLTDigitFormat( ); ///< Destructor
  inline void setData(unsigned int num){m_data=num;};
  void setAddress(unsigned int num);
  void setTime(unsigned int num);
  unsigned int getTime();
  unsigned int getAddress();
  unsigned int getWord();
  
protected:

private:
  unsigned int m_data;
  

};






#endif // MUONHLTDIGITFORMAT_H
