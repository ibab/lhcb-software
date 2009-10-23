// $Id: PackerBaseAlg.h,v 1.2 2009-10-23 10:46:04 jonrob Exp $
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
    unsigned int m_version;   ///< Version schema number

  };

}

#endif // PACKERBASEALG_H
