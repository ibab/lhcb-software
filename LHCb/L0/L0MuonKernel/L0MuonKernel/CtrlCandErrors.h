// $Id: CtrlCandErrors.h,v 1.4 2008-12-09 18:47:28 cattanem Exp $
#ifndef CTRLCANDERRORS_H 
#define CTRLCANDERRORS_H 1

#include "L0MuonKernel/ErrorHandler.h"

namespace L0Muon {

  /** @class CtrlCandErrors CtrlCandErrors.h L0MuonKernel/CtrlCandErrors.h

      Class holding the error related to the L0MuonCtrlCand bank.

      It deals with the errors encountered in one quarter, i.e. :
      - errors reported by the CU and SU FPGAs (see the controller board specifications; EDMS 544412)
        - input link errors (12 links for each FPGA, 2 error bits per link)
        - status bits reported by the controller and processing boards
      - errors reported by the TELL1 board (see the L0Muon TELL1 board specifications ; EDMS 818559)
        - errJ : L0_B_Id mismatch between the 2 controller board input channels of the TELL1
        - errK : l0EventNumber mismatch between the 2 controller board input channels of the TELL1
      - errors encountered during the decoding, i.e.:
        - mismatch between a L0_B_Id or a BCID in the data and the reference L0_B_Id specified for the decoding
        - mismatch between a l0EventNumber in the data and the reference l0EventNumber specified for the decoding
        - bit set in a field which is supposed to be empty

      The ErrorHandler instances owned by this class are filled in CtrlCandCnv::decodeBank. 

      @author Julien Cogan
      @date 2008 June the 5th
  */
  class CtrlCandErrors
  {
  
  public:
  

    /// Constructor
    CtrlCandErrors();

    /// Destructor
    virtual ~CtrlCandErrors();  

    //std::string counters(std::string tab="");
    
    /// Header string to be used for error output
    static std::string header(std::string tab=""){
      std::string h=tab;
      h+=" * Errors detected in the L0MuonCtrlCand Bank";
      h+="\n"+tab;
      h+=" Pos   L0EvtNum  L0_B_Id     ";
      h+="-- BCIDs -  ";
      h+="  - Status -";
      h+="\n"+tab;
      h+=" 0  1   0    1    0    1   ";
      h+="BCSU 0.....4  ";
      h+="BCSU 0.....4";
      return h;
    } 

    /// Return the sum of all error counters
    int sumCounters() const ;

    /** Returns true if an error was detected in the L0MuonCtrlCand bank part
        corresponding to the quarter attached to this error container.

        Error flag is raised if one of the following error case was found :
        - a decoding error (see decodingError() const) 
        - a hardware error (see hardwareError() const)
        - a bcid error (see bcidError() const)
        - a status error (see statusError() const)
    */
    bool inError() const;

    /** Returns true if an error was detected in the L0MuonCtrlCand bank part
        corresponding to the given board of the quarter attached to this error container.

        Error flag is raised if one of the following error case was found :
        - a decoding error (see decodingError() const) 
        - a hardware error (see hardwareError(int ib) const)
        - a bcid error (see bcidError(int ib) const)
        - a status error (see statusError(int ib) const)

        @param ib : board index
    */
    bool inError(int ib) const ;

    /** Returns the decoding error code for the quarter attached to this error container.
        
        Error code > 0 if a bit was set in a field which is supposed to be empty.

        For further details on the frame structure, the L0Muon TELL1 specifications (EDMS 818559).
    */
    int decodingError() const;

    /** Returns the hardware error code for the quarter attached to this error container.

        - bits 0-3: bcid of CU if not compatible with reference L0_B_Id
        - bits 4-7: bcid of SU if not compatible with reference L0_B_Id
        - bit   8 : J error
        - bit  12 : K error

        For further details, please refer to the controller board specifications (EDMS 544412).
    */
    int hardwareError() const;

    /** Returns the hardware error code for the given board of the quarter attached to this error container.
        
        - bits 0-1 : CU link error for board with index ib 
        - bits 4-5 : SU link error for board with index ib 
        - bit    8 : if a mismatch was detected on the bcids sent to CU and SU for board with index ib

        For further details, please refer to the controller board specifications (EDMS 544412).

        @param ib : board index

    */
    int hardwareError(int ib) const;

    /** Error on the bcid of the best candidate.
        Return -1 if no mismatch, return the faulty bcid otherwise.
 
    */
    int bcidError() const ;

    /** Error on the bcid of the given by the BCSU of the board with index ib.
        Return -1 if no mismatch, return the faulty bcid otherwise.
        
        @param ib : board index
    */
    int bcidError(int ib) const;

    /** Return the error bits of the status of the controller board of the quarter attached to this error container.
    */
    int statusError()const ;

    /** Return the error bits of the status of the BCSU of the board with index ib 
        of the quarter attached to this error container.

        @param ib : board index
    */
    int statusError(int ib) const;
    
    ErrorHandler present;       ///< Incremented each time the decoding of L0MuonCtrlCand is called. 
    ErrorHandler decoding;      ///< Incremented when a decoding error is found in L0MuonCtrlCand. 
    ErrorHandler l0EventNumber; ///< Incremented when the l0EventNumber in L0MuonCtrlCand does not match reference.
    ErrorHandler l0_B_Id;       ///< Incremented when the l0_B_Id in L0MuonCtrlCand does not match reference
    ErrorHandler ctrl_bcid[2];  ///< Indexed to CU(=0)/SU(=1); 
                                ///< incremented when the bcid of CU/SU does not match reference l0_B_Id.
    ErrorHandler best_bcid;     ///< Incremented when the bcid of selected pair does not match reference l0_B_Id .
    ErrorHandler bcsu_bcid[12]; ///< Indexed to board processing index; 
                                ///< incremented when the bcid of the board does not match reference l0_B_Id.
    ErrorHandler bcsu_bcid_mismatch[12]; ///< Indexed to board processing index; 
                                         ///< incremented when a mismatch was seen between the bcid of the board
                                         ///< sent to CU and SU.
    ErrorHandler best_status;     ///< Incremented with CU status   .
    ErrorHandler bcsu_status[12]; ///< Incremented with BCSU status.
    ErrorHandler errJ;       ///< Incremented with J errors (see TELL1 specifications ; EDMS 818559).
    ErrorHandler errK;       ///< Incremented with K errors (see TELL1 specifications ; EDMS 818559). 
    ErrorHandler link[12][2];///< Indexed to board processing index (first index) and CU(=0)/SU(=1) (second index); 
                             ///< incremented with link error.
    
  };

  std::ostream &operator<<(std::ostream &os, const L0Muon::CtrlCandErrors &pberr);

} // namespace L0Muon



#endif // CTRLCANDERRORS_H
