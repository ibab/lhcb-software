// $Id: Decoder.h,v 1.1 2007-09-18 00:04:14 szumlat Exp $
#ifndef VELOTELL1ALGORITHM_DECODER_H 
#define VELOTELL1ALGORITHM_DECODER_H 1

// Include files
#include "VeloEvent/VeloFullBank.h"
#include "Kernel/VeloDecodeConf.h"
#include "Kernel/VeloDecodeCore.h"

/** @class Decoder Decoder.h Decoder.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-04-11
 */

class Decoder {

  enum initialShifts{
    NoShift=0,
    EmptyData=2,
    ADCShift=4
  };
  
public: 

  /// Standard constructor
  Decoder(const int decoderType);
  virtual ~Decoder( ); ///< Destructor
  dataVec& decode(VeloFullBank* inData);
  
protected:

  void decoderIni(const int decoderType);

private:

  int m_initialShift;  // if decoding adcs skip headers
  int m_dataLenght;    // lenght of the ALink
  int m_wordsToDecode; // words to decode
  dataVec m_decodedData;    

};
#endif // VELOTELL1ALG_DECODER_H
//
