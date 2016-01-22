// $Id: CheckerBaseAlg.h,v 1.1 2009-10-21 16:41:13 jonrob Exp $
#ifndef CHECKERBASEALG_H
#define CHECKERBASEALG_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

namespace DataPacking
{

  /** @class Check CheckerBaseAlg.h
   *
   *  Templated base algorithm for checking data packing
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-14
   */

  template< class PACKER >
  class Check : public GaudiAlgorithm
  {

  public:

    /// Standard constructor
    Check( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~Check();     ///< Destructor

    virtual StatusCode execute(); ///< Algorithm execution

  private:

    std::string m_dataA;  ///< Data location
    std::string m_dataB;  ///< Data location
    const PACKER m_packer{this};  ///< Packer

  };

}

#endif // CHECKERBASEALG_H
