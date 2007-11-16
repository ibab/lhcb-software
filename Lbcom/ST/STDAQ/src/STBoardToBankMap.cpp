#include "STBoardToBankMap.h"
#include "Kernel/STTell1ID.h"

STBoardToBankMap::STBoardToBankMap(){

  // constructer
  m_bankMapping.clear();
}


STBoardToBankMap::~STBoardToBankMap(){
  // destructer
}

void STBoardToBankMap::addEntry(STTell1ID aBoard, unsigned int aBank){
  // add entry to map
 m_bankMapping[aBoard] =aBank;
}

STTell1ID STBoardToBankMap::findBoard(const unsigned int aBank){
  // board to bank
  std::map<STTell1ID,unsigned int>::iterator iterMap = m_bankMapping.begin();
  while ((iterMap !=  m_bankMapping.end())&&(iterMap->second != aBank)){
    ++iterMap;
  } // iterMap
  return (iterMap != m_bankMapping.end() ? iterMap->first : STTell1ID(STTell1ID::nullBoard));
}

unsigned int STBoardToBankMap::findBank(const STTell1ID aBoard){
  // bank to board
  return m_bankMapping[aBoard];
}
