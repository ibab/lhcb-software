// $Id: FileIdBankWriter.h,v 1.1 2009/10/09 15:52:34 panmanj Exp $
#ifndef FILEIDBANKWRITER_H 
#define FILEIDBANKWRITER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/RawEvent.h"
#include "Event/FileId.h"

/** @class FileIdBankWriter FileIdBankWriter.h
 *   
 *
 *  @author Jaap Panman
 *  @date   2009-10-01
 */
class FileIdBankWriter : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FileIdBankWriter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FileIdBankWriter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  std::string m_rawEventLocation;     // Location where we get the RawEvent
  std::string m_current_fname;        // current file ID string 
  int         m_count_files;          // number of files read

  std::vector<unsigned int> m_bank;
  LHCb::RawBank::BankType m_bankType;  

  LHCb::FileId m_fileId;

private:

};
#endif // FILEIDBANKWRITER_H
