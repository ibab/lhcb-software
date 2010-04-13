#ifndef ONLINEKERNEL_ERRORFILE_H
#define ONLINEKERNEL_ERRORFILE_H

// C++ header files
#include <string>

namespace LHCb {

  class ErrorFile {
    /// File name
    std::string m_name;
    /// Memory block containing data iof the error file
    char*       m_memory;
    /// Length of memory block in bytes
    size_t      m_length;
    /// Error counters: Number of bad, unrecognized blocks in the data file
    int         m_badBlock;
    /// Error counters: Number of MDF headers with bad version number
    int         m_badVsn;
    /// Error counters: Number of bad MDF headers
    int         m_badHdr;
    /// Data  counters: Total number of events
    int         m_numEvt;
    /// Data  counters: Number of banks found
    int         m_numBank;
    /// Printput flag
    int         m_print;
  public:
    /// Printout enums. Value passed to the constructor may be OR of these
    enum { PRINT_ERRORS = 1, PRINT_EVTS = 2, PRINT_MDF_HEADERS = 4, PRINT_BANKS = 8 };
    /// Standard constructor
    ErrorFile(int prt, const std::string& nam);
    /// Default destructor
    virtual ~ErrorFile();
    /// Access to event number 
    int numEvents() const { return m_numEvt; }
    /// Access to number of banks
    int numBanks() const { return m_numBank; }
    /// Load the file into memory
    int read();
    /// Analyze data content from memory buffer
    int analyze();
    /// Summarize analysis
    int summarize();
  };
}
#endif // ONLINEKERNEL_ERRORFILE_H
