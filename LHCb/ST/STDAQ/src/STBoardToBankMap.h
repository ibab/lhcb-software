#ifndef  _STBoardToBankMap_H
#define _STBoardToBankMap_H 1

/** @class STBoardToBankMap STBoardToBankMap.h
 *
 *  Helper class for mapping boards to banks 
 *  basically hides a a map - used in 2 places....
 *
 *  @author M Needham
 *  @date   2004-08-01
 */

#include <map>

#include "Kernel/STTell1ID.h"

class STBoardToBankMap final {

public:

 STBoardToBankMap() = default;

 void addEntry(STTell1ID aBoard, unsigned int aBank);
 STTell1ID findBoard(const unsigned int aBank) const;

 // bank to board
 unsigned int findBank(const STTell1ID aBoard) const
 { return m_bankMapping.at(aBoard); }

 void clear() { m_bankMapping.clear(); }

private:

 std::map<STTell1ID,unsigned int> m_bankMapping;

};

#endif // _STBoardToBankMap_H
