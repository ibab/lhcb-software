// $Id: L0BufferCnvContainer.h,v 1.1 2007-08-27 09:32:23 jucogan Exp $
#ifndef L0MUONKERNEL_L0BUFFERCNVCONTAINER_H 
#define L0MUONKERNEL_L0BUFFERCNVCONTAINER_H 1

// Include files

/** @class L0BufferCnvContainer L0BufferCnvContainer.h L0MuonKernel/L0BufferCnvContainer.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2007-08-22
 */

#include "L0MuonKernel/L0BufferCnv.h"
#include "Kernel/MuonTileID.h"

namespace L0Muon {

  class L0BufferCnvContainer {

  public: 
    /// Standard constructor
    L0BufferCnvContainer( ); 
    L0BufferCnvContainer(std::vector<LHCb::MuonTileID> lpus ); 

    virtual ~L0BufferCnvContainer( ); ///< Destructor

    void open(std::string path);
    void close();  
    void write(int ievt);

  protected:

  private:
    std::vector<L0Muon::L0BufferCnv *> m_cnvs;

  };

}; // namespace L0Muon
#endif // L0MUONKERNEL_L0BUFFERCNVCONTAINER_H
