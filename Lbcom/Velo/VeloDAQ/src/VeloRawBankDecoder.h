// $Id: VeloRawBankDecoder.h,v 1.3 2006-02-23 15:41:50 krinnert Exp $
#ifndef VELORAWBANKDECODER_H 
#define VELORAWBANKDECODER_H 1

#include "SiDAQ/SiRawBankDecoder.h"
#include "VeloClusterWord.h"


/** @class VeloRawBankDecoder VeloRawBankDecoder.h
 *  
 *  Decoder for raw Velo banks
 *
 *  This is a simple type wrapper for a template
 *  instantiation of a SiRawBankDecoder with
 *  VeloCLusterWord.
 *
 *  @see VeloClusterWord
 *  @see SiRawBankDecoder
 * 
 *  @author Kurt Rinnert
 *  @date   2006-02-08
 */
typedef SiRawBankDecoder<VeloClusterWord> VeloRawBankDecoder;

#endif // VELORAWBANKDECODER_H
