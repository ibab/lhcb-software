// $Id: VeloFullBank.h,v 1.5 2008-12-09 16:26:25 cattanem Exp $
#ifndef VELOFULLBANK_H 
#define VELOFULLBANK_H 1

// Include files
#include <vector>
#include <string>
#include <ostream>
#include "GaudiKernel/KeyedObject.h"
#include "GaudiKernel/KeyedContainer.h"
#include "Tell1Kernel/VeloDecodeConf.h"
#include "Tell1Kernel/VeloDecodeCore.h"

/** @class VeloFullBank VeloFullBank.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-04-06
 */

namespace VeloFullBankLocation {
  static const std::string& Default = "Raw/Velo/ADCBank";
  static const std::string& Pedestals = "Raw/Velo/PedBank";
}

class VeloFullBank: public KeyedObject<int>{

public: 
  /// Standard constructor
  typedef std::vector<unsigned int> section;
  //
  VeloFullBank(const int numberOfTELL1, unsigned int* inBank, const int type):
    KeyedObject<int>(numberOfTELL1)
    {
      initialize(type);
      m_bank=inBank;
      writeSections();
      writeEventInfos(type);
    }
  virtual ~VeloFullBank( ) { }; ///< Destructor
  VeloTELL1::dataVec& getSection(int blockNnb, int secNbn);
  VeloTELL1::allEvt& getEvtInfo();
  //

protected:  

  void initialize(const int type);
  void writeSections();
  void writeEventInfos(const int type);
  int NumberOfWordsInSection() const;
  int NumberOfWordsInBlock() const;

private:

  struct PPFPGABlock
  {
    std::vector<section> sections;
  };
  unsigned int* m_bank;
  std::vector<PPFPGABlock> m_PPFPGABlocks;
  int m_NumberOfWordsInBlock;
  int m_NumberOfWordsInSection;
  VeloTELL1::allEvt m_infos;
  
};
//=============================================================================
/*inline const CLID& VeloFullBank::clID() const
{
  return VeloFullBank::classID();
}
//=============================================================================
inline const CLID& VeloFullBank::classID()
{
  return CLID_VeloFullBank;
}
*/
//=============================================================================
inline void VeloFullBank::initialize(const int type)
{
  if(type==VeloTELL1::VeloFull){
    m_NumberOfWordsInBlock=(VeloTELL1::PPFPGADataBlock);
    m_NumberOfWordsInSection=(VeloTELL1::PPFPGADataSection); 
  }
  else if(type==VeloTELL1::VeloPedestal){
    m_NumberOfWordsInBlock=VeloTELL1::PPFPGAPedBlock;
    m_NumberOfWordsInSection=VeloTELL1::PPFPGAPedBlock; 
  }else{
    std::cout<< " Wrong initialization! " <<std::endl;
  }
}
//=============================================================================
inline void VeloFullBank::writeSections()
{
  // set length of the block
  const int wordsInBlock=NumberOfWordsInBlock();
  // skip the last eight words which contain event info
  const int dataWords=NumberOfWordsInSection();
  //                                                
  for(int i=0; i<VeloTELL1::NumberOfPPFPGA; i++){
    // we can divide data stream form the bank on four logical
    // units each from which corrsponds to data that will be sent
    // to one PPFPGA processor
    PPFPGABlock aBlock;
    // set the pointer at the beginning of the PPFPGA block
    unsigned int* dataPtr=m_bank+i*wordsInBlock;
    for(int k=0; k<VeloTELL1::SectionsPerBlock; k++){
      // create section object, there are three sections 
      // per one data block
      section sec;
      unsigned int* secPtr=dataPtr+k;
      int ii=0;
      while(ii<dataWords){
        sec.push_back((*(secPtr+ii)));
        ii+=3; 
      }
      aBlock.sections.push_back(sec); 
    }
    m_PPFPGABlocks.push_back(aBlock); 
  } 
}
//============================================================================
inline void VeloFullBank::writeEventInfos(const int type)
{
  if(type==VeloTELL1::VeloFull){
    int DataBlock=VeloTELL1::PPFPGADataBlock;
    int DataSection=VeloTELL1::PPFPGADataSection;
    //
    for(int PPFPGA=0; PPFPGA<VeloTELL1::NumberOfPPFPGA; PPFPGA++){
      unsigned int* dataPtr=m_bank+(PPFPGA*DataBlock)+DataSection;
      VeloTELL1::evtInfo anInfo;
      for(int k=0; k<VeloTELL1::EventInfoLength; k++){
        unsigned int* infoPtr=dataPtr+k;
        anInfo.push_back(*(infoPtr));
      }
      m_infos.push_back(anInfo);
    }
  }
}
//=============================================================================
inline int VeloFullBank::NumberOfWordsInBlock() const
{
  return ( m_NumberOfWordsInBlock ); 
}
//=============================================================================
inline int VeloFullBank::NumberOfWordsInSection() const
{
  return ( m_NumberOfWordsInSection ); 
}
//=============================================================================
inline VeloTELL1::allEvt& VeloFullBank::getEvtInfo()
{
  return ( m_infos );
}
//=============================================================================
inline VeloTELL1::dataVec& VeloFullBank::getSection(
                                            int blockNbn, int secNbn)
{
  return ( m_PPFPGABlocks[blockNbn].sections[secNbn] );
}
typedef KeyedContainer<VeloFullBank, Containers::HashMap> VeloFullBanks;
#endif // VELOFULLBANK_H
