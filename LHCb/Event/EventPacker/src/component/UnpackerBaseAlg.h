// $Id: UnpackerBaseAlg.h,v 1.3 2009-11-06 17:50:04 jonrob Exp $
#ifndef UNPACKERBASEALG_H
#define UNPACKERBASEALG_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

namespace DataPacking
{

  /** @class Unpack UnpackerBaseAlg.h
   *
   *  Templated base algorithm for all unpacking algorithms
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-14
   */

  template< class PACKER >
  class Unpack : public GaudiAlgorithm
  {

  public:

    /// Standard constructor
    Unpack( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~Unpack();     ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialize
    virtual StatusCode execute();    ///< Algorithm execution

  private:

    std::string m_inputName;  ///< Input location
    std::string m_outputName; ///< Output location
    bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing

    /// Packer object
    const PACKER m_packer{ this };  

  };

}

#endif // UNPACKERBASEALG_H
