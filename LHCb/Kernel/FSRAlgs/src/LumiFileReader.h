// $Id: LumiFileReader.h,v 1.2 2009-10-09 15:51:08 panmanj Exp $
#ifndef LUMIFILEREADER_H 
#define LUMIFILEREADER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/RawEvent.h"
#include "Event/ODIN.h"
#include "Event/HltLumiSummary.h"
#include "Event/FileId.h"

// forward declarations
namespace LHCb {
  class RawBank ;
}


/** @class LumiFileReader LumiFileReader.h
 *   
 *
 *  @author Jaap Panman
 *  @date   2009-10-06
 */
class LumiFileReader : public GaudiAlgorithm {
public: 
  /// Standard constructor
  LumiFileReader( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LumiFileReader( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  // data
  std::string m_rawEventLocation;           ///< Location where we get the RawEvent
  std::string m_OutputFileName;

  // Statistics  
  double m_totDataSize;
  int m_nbEvents;
  LHCb::RawBank::BankType   m_bankType;  
  LHCb::HltLumiSummary*     m_HltLumiSummary;
  LHCb::ODIN*               m_odin;

  LHCb::FileId m_fileId;

};
#endif // FILEREADER_H
