// $Id: Decoder.cpp,v 1.1 2007-09-18 00:04:14 szumlat Exp $
// Include files

// local
#include "Decoder.h"
#include <iostream>

//-----------------------------------------------------------------------------
// Implementation file for class : Decoder
//
// 2006-04-21 : Tomasz Szumlak
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

using namespace VeloTELL1;

Decoder::Decoder(const int decoderType):
  m_decodedData ()
{
  decoderIni(decoderType);
}
//=============================================================================
// Destructor
//=============================================================================
Decoder::~Decoder() {}
//=============================================================================
dataVec& Decoder::decode(VeloFullBank* inData)
{
  dataVec::iterator secIt;
  unsigned int decodedWord=0;
  m_decodedData.clear();
  //                                             
  int NumberOfPPFPGA=VeloTELL1::NumberOfPPFPGA;
  int SectionsPerBlock=VeloTELL1::SectionsPerBlock;
  //                                          
  for(int aBlock=0; aBlock<NumberOfPPFPGA; aBlock++){
    for(int aSection=0; aSection<SectionsPerBlock; aSection++){
      dataVec section=inData->getSection(aBlock, aSection);
      for(int stream=0; stream<VeloTELL1::DataStreamPerSection; stream++){
        int ADCBitShift=stream*VeloTELL1::ADCShift;
        for(int aLink=0; aLink<VeloTELL1::ALinkPerDataStream; aLink++){
          int analogChanShift=aLink*m_dataLenght;
          int beginDecode=m_initialShift+analogChanShift;
          int endDecode=m_wordsToDecode+beginDecode;
          int count=0;
          if(aBlock==10&&aSection==0&&stream==0&&aLink==0){
            std::cout<< "chanShift: " << analogChanShift 
                     << ", beginDecode: " << beginDecode
                     << ", endDecode: " << endDecode 
                     << ", steram: " << stream <<std::endl;
          }
          //
          for(secIt=(section.begin()+beginDecode);
              secIt!=(section.begin()+endDecode); secIt++){
            decodedWord=(((*secIt)>>ADCBitShift)&VeloTELL1::ADCMask);
            if(!((aSection==EmptyData)&&(stream==EmptyData))){
              // skip the empty space in C section                    
              m_decodedData.push_back(decodedWord);
              if(aBlock==10&&aSection==0&&stream==0&&aLink==0){
                std::cout<< "counter: " << count << ", decoded ADC: "
                         << decodedWord <<std::endl;
              }
              count++;
            }
          } // loop over channels
        }   // loop over ALink
      }     // loop over coded int
    }       // Sections loop
  }         // FPGA loop
  //
  return ( m_decodedData );
}
//==============================================================================
// to make the decoder universal one the code below to execute inside
// Decoder constructor is provided
void Decoder::decoderIni(const int decoderType)
{
  if(decoderType==VeloFull){
    // during ADC decoding skip ADC headers
    m_initialShift=ADCShift;
    // number of channes in one ALink and ADC headers
    m_dataLenght=ALinkShift;
    m_wordsToDecode=CHANNELS;
  }else if(decoderType==VeloPedestal){
    // pedestals data has no ADC heders, no shift needed
    m_initialShift=NoShift;
    // one pedestal value for each strip
    m_dataLenght=CHANNELS;
    m_wordsToDecode=CHANNELS;
  }else if(decoderType==VeloHeader){
    m_initialShift=NoShift;
    m_dataLenght=ALinkShift;
    m_wordsToDecode=ADCHeaders;
  }
}
//
