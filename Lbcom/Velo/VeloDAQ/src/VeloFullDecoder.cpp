// $Id: VeloFullDecoder.cpp,v 1.3 2008-03-18 08:40:15 krinnert Exp $
// Include files

// local
#include "VeloFullDecoder.h"
#include <iostream>

//-----------------------------------------------------------------------------
// Implementation file for class : VeloFullDecoder
//
// 2006-04-21 : Tomasz Szumlak
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

using namespace VeloTELL1;

VeloFullDecoder::VeloFullDecoder(const int decoderType)
{
  decoderIni(decoderType);
}
//=============================================================================
// Destructor
//=============================================================================
VeloFullDecoder::~VeloFullDecoder() {}
//=============================================================================
void VeloFullDecoder::decode(VeloFullBank* inData, VeloTELL1::sdataVec& decodedData)
{
  dataVec::iterator secIt;
  unsigned int decodedWord=0;
  decodedData.clear();
    //                                             
  int NumberOfPPFPGA=VeloTELL1::NumberOfPPFPGA;
  int SectionsPerBlock=VeloTELL1::SectionsPerBlock;
  //                                          
  for(int aBlock=0; aBlock<NumberOfPPFPGA; aBlock++){
    for(int aSection=0; aSection<SectionsPerBlock; aSection++){
      dataVec section=inData->getSection(aBlock, aSection);
      for(int stream=0; stream<VeloTELL1::DataStreamPerSection; stream++){
        // skip the empty space in C section                    
        if(!((aSection==EmptyData)&&(stream==EmptyData))){
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
                secIt!=(section.begin()+endDecode); ++secIt){
              decodedWord=(((*secIt)>>ADCBitShift)&VeloTELL1::ADCMask);
              // from now on we must be prepared for negative values, hence
              // the cast to int.
              decodedData.push_back(static_cast<int>(decodedWord));
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
}

//==============================================================================
// to make the decoder universal one the code below to execute inside
// VeloFullDecoder constructor is provided
void VeloFullDecoder::decoderIni(const int decoderType)
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
