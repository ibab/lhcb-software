// $Id: STFullEventDump.h,v 1.1.1.1 2009-03-02 19:13:44 mtobin Exp $
#ifndef STFULLEVENTDUMP_H 
#define STFULLEVENTDUMP_H 1
//===========================================
// Include files
// from Gaudi
#include "Kernel/STAlgBase.h"

/** @class STFullEventDump STFullEventDump.h
 *   
 *  Algorithm to dump the content of the NZS STTELL1Data objects. Job options:
 *  - \b InputLocation: Location of NZS data e.g. TTFull or ITFull
 *  - \b TELL1List: List of sourceIDs of Tell1's that you want to dump (Default
 *                  no Tell1's are dumped).
 *  - \b LinkList: List of links that you want to dump (Default all 23 links are
 *                  dumped).
 *
 *  @author Mathias Knecht, M Needham and Jeroen van Tilburg
 *  @date   2007-09-11 
 */
class STFullEventDump: public ST::AlgBase {

public: 

  /// Standard constructor
  STFullEventDump( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  std::string m_inputLocation;
  std::vector< unsigned int > m_TELL1List;
  std::vector< unsigned int > m_linkList;
   
};
#endif // STFULLEVENTDUMP_H
