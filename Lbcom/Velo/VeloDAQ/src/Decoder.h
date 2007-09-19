// $Id: Decoder.h,v 1.3 2007-09-19 13:43:38 cattanem Exp $
#ifndef VELOTELL1ALGORITHM_DECODER_H 
#define VELOTELL1ALGORITHM_DECODER_H 1

// Include files
#include "VeloEvent/VeloFullBank.h"
#include "Tell1Kernel/VeloDecodeConf.h"
#include "Tell1Kernel/VeloDecodeCore.h"

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
  VeloTELL1::dataVec& decode(VeloFullBank* inData);
  
protected:

  void decoderIni(const int decoderType);

private:

  int m_initialShift;  // if decoding adcs skip headers
  int m_dataLenght;    // lenght of the ALink
  int m_wordsToDecode; // words to decode
  VeloTELL1::dataVec m_decodedData;    

};
#endif // VELOTELL1ALG_DECODER_H
//
