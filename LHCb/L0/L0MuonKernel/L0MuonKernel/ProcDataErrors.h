// $Id: ProcDataErrors.h,v 1.3 2008-11-07 16:23:39 jucogan Exp $
#ifndef PROCDATAERRORS_H 
#define PROCDATAERRORS_H 1

#include "L0MuonKernel/ErrorHandler.h"

namespace L0Muon {

  /** @class ProcDataErrors ProcDataErrors.h L0MuonKernel/ProcDataErrors.h

      Class holding the error related to the L0MuonProcData bank.

      It deals with the error encountered in one processing board, i.e. :
      - PU input link errors : 8 optical links, 6 serial links, 8 parallel links for each PU; 1 bit per link 
      (see the processing board specifications; EDMS 339143)
      - errors encountered during the decoding, i.e. wrong data size.

      The ErrorHandler instances of this class are filled in ProcDataCnv::decodeBank.

      @author Julien Cogan
      @date 2008 June the 5th
  */
  class ProcDataErrors
  {
  
  public:
  
    /// Constructor
    ProcDataErrors();
    
    /// Destructor
    virtual ~ProcDataErrors();  
    
    //std::string counters(std::string tab="");

    /// Header string to be used for error output
    static std::string header(std::string tab=""){
      std::string h=tab;
      h+=" * Errors detected in the L0MuonProcData bank";
      h+="\n"+tab;
      return h;
    } 

    /// Return the sum of all error counters.
    const int sumCounters() const ;
    
    /** Returns true if the specified PU is in error.
        
        The error condition is raised if :
        - a decoding error was found in the processing board frame (see decodingError() const)
        - a hardware error was found in this PU (see hardwareError(int ipu) const)

        @param ipu : index of the pu in the board
    */
    const bool inError(int ipu) const;
    
    /** Returns the decoding error code for the quarter attached to this error container.
        
        Error code > 0 if a bit was set in a field which is supposed to be empty.

        For further details on the frame structure, the L0Muon TELL1 specifications (EDMS 818559).
    */
    const int decodingError() const;

    /** Returns the hardware error code for the given PU.

        - bits  0-7  : parallel link errors (see #par_link)
        - bits  8-13 : serial link errors (see #ser_link)
        - bits 16-23 : optical link errors (see #opt_link)

        For further details, please refer to the processing board specifications (EDMS 339143).
        
        @param ib : index of the PU in the board
    */
    const int hardwareError(int ipu) const;
    
    ErrorHandler present;     ///< Incremented each time the decoding of L0MuonProcData is called. 
    ErrorHandler decoding;    ///< Incremented when a decoding error is found in L0MuonCtrlCand.
    ErrorHandler opt_link[4]; ///< Array of optical link errors. 
                              ///< Array element : 8 bits; 1 bit per ol; from lsb to msb : M1a-M1b-M2a-M2b-M3a-M3b-M4-M5).
                              ///< Array index is the PU number in the board. 
    ErrorHandler par_link[4]; ///< Array of parallel link errors. 
                              ///< Array element : 8 bits; 1 bit per link.
                              ///< Array index is the PU number in the board.
    ErrorHandler ser_link[4]; ///< Array of serial link errors. 
                              ///< Array element : 6 bits; 1 bit per link.
                              ///< Array index is the PU number in the board.
    
  };

  std::ostream &operator<<(std::ostream &os, const L0Muon::ProcDataErrors &pberr);

} // namespace L0Muon



#endif // PROCDATAERRORS_H
