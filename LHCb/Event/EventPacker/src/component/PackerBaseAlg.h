// $Id: PackerBaseAlg.h,v 1.3 2009-11-06 17:50:04 jonrob Exp $
#ifndef PACKERBASEALG_H
#define PACKERBASEALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

namespace DataPacking
{

  /** @class Pack PackerBaseAlg.h
   *
   *  Templated base algorithm for all packing algorithms
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-14
   */

  template< class PACKER >
  class Pack : public GaudiAlgorithm
  {

  public:

    /// Standard constructor
    Pack( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~Pack();     ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialize
    virtual StatusCode execute();    ///< Algorithm execution

  private:

    std::string m_inputName;  ///< Input location
    std::string m_outputName; ///< Output location
    unsigned short int m_version; ///< Packing version number
    bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing
    bool m_deleteInput;       ///< Flag to turn on the removal of the input data after packing
    bool m_enableCheck;       ///< Flag to turn on automatic unpacking and checking of the output post-packing
    bool m_clearRegistry;     ///< Flag to turn on the clearing of the registry if the input data is not kept

    /// Packer object
    const PACKER m_packer{ this };

  };

}

#endif // PACKERBASEALG_H
