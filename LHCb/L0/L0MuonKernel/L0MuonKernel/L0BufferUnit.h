// $Id: L0BufferUnit.h,v 1.11 2006-02-10 12:31:20 jucogan Exp $
#ifndef L0MUONKERNEL_L0BUFFERUNIT_H
#define L0MUONKERNEL_L0BUFFERUNIT_H     1

/* @class L0BufferUnit L0BufferUnit.h  L0MuonKernel/L0BufferUnit.h

Abstract base class used to build and print out the L0Buffers.
This base class provide the functionnalities to:
 - opens a file at the beginning of the job (initialize)
 - dump the l0buffer into the file at each event (execute)
 - close the file at the end of the job (finalize)
It also provides an event counter:
 - set to 0  at the beginning of the job (initialize)
 - incremented at each event (postexecute)
It also ensures that the output register are reset at each event 
   
*/ 

// STL includes
#include <stdio.h>
#include <vector>
#include "L0MuonKernel/L0MUnit.h"

namespace L0Muon {

  class L0BufferUnit : public L0MUnit {

  public:

    /// Constructor
    L0BufferUnit();

    L0BufferUnit(LHCb::MuonTileID id);

    /// Destructor
    ~L0BufferUnit();

    /// Constructor
    L0BufferUnit(DOMNode* pNode);

    // Overloads from Unit
    void initialize(); 
    void execute();
    void postexecute();
    void finalize(); 

    /// Return the size of the L0Buffer
    virtual int bufferSize()=0;
    
     
  protected:

    // Event counter
    int m_evtCounter;

    // Output file
    FILE *m_outputFile;

  };

};  // namespace L0Muon

#endif      // L0MUONKERNEL_L0BUFFER_H  
