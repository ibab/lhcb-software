#ifdef BUILD_WRITER

#ifndef REPROCESSINGWRITER_H
#define REPROCESSINGWRITER_H

#include "MDF/RawEventHelpers.h"
#include "Writer/MDFWriterNet.h"

/*
 * LHCb namespace
 */
namespace LHCb {

  /** @class ReprocessingWriter
   *
   * @author:  M.Frank
   * @version: 1.0
   */
  class ReprocessingWriter : public MDFWriterNet {
  protected:
    /** Obtains the run number from the MDF header.
     * @param data  The data from which MDF information may be retrieved
     * @param len   The length of the data.
     * @return The run number.
     */
    virtual unsigned int getRunNumber(const void *data, size_t len);

    /** Generates a new file name from the MDF information.
     * @param runNumber  The current run number, to be included in the file name.
     */
    virtual std::string getNewFileName(unsigned int runNumber);

  public:
    /// Standard Algorithm constructor (delegates to MDFWriter constructor).
    ReprocessingWriter(const std::string& nam, ISvcLocator* pSvc)
      : MDFWriterNet(nam, pSvc) {}
    /// Destructor.
    virtual ~ReprocessingWriter() {}
  };
}
#endif //REPROCESSINGWRITER_H


//#include "Writer/ReprocessingWriter.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "MDF/MDFHeader.h"
#include "Event/RawBank.h"


#include <stdexcept>

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,ReprocessingWriter);

using namespace std;
using namespace LHCb;


/// Generates a new file name from the MDF information.
std::string ReprocessingWriter::getNewFileName(unsigned int /* runNumber */ )  {
  throw std::runtime_error("The communication to the reprocessing database failed. This file cannot be written.");
  return std::string();
}
#define DAQ_FILEID_BANK 255

/// Returns the run number from an MDF header.
inline unsigned int ReprocessingWriter::getRunNumber(const void *data, size_t len)    {
  MDFHeader* h = (MDFHeader*)data;
  const char* start = ((char*)h)+h->sizeOf(h->headerVersion());
  const char* end   = start+h->size();
  while(start<end)    {
    RawBank* b = (RawBank*)start;
    if ( b->type() == RawBank::DAQ && b->version() == DAQ_FILEID_BANK ) {
      return *b->begin<int>();
      // time_t now = time(0);
      // return int(now/180);
    }
    start += b->totalSize();
  }
  throw std::runtime_error("No FileID bank present. Such events are not foreseen!!!!");
}
#endif /* _WIN32 */
