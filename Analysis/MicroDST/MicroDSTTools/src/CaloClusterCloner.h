// $Id: CaloClusterCloner.h,v 1.1 2010-08-13 14:33:57 jpalac Exp $
#ifndef MICRODST_CALOCLUSTERCLONER_H
#define MICRODST_CALOCLUSTERCLONER_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneCaloCluster.h>            // Interface
#include <MicroDST/Functors.hpp>

namespace LHCb
{
  class CaloDigit;
  class CaloCluster;
}

/** @class CaloClusterCloner CaloClusterCloner.h src/CaloClusterCloner.h
 *
 *  Clone an LHCb::CaloCluster. Deep-clones CaloClusterEntries (CaloDigits).
 *
 *  @author Juan PALACIOS
 *  @date   2010-08-13
 */

class CaloClusterCloner : public extends1<MicroDSTTool, ICloneCaloCluster>
{

public:

  /// Standard constructor
  CaloClusterCloner( const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

  virtual ~CaloClusterCloner( ); ///< Destructor

  virtual LHCb::CaloCluster* operator() (const LHCb::CaloCluster* hypo);

private:

  LHCb::CaloCluster* clone(const LHCb::CaloCluster* hypo);

private:

  typedef MicroDST::BasicItemCloner<LHCb::CaloCluster> BasicCaloClusterCloner;
  typedef MicroDST::BasicItemCloner<LHCb::CaloDigit> BasicCaloDigitCloner;

};

#endif // MICRODST_CALOCLUSTERCLONER_H
