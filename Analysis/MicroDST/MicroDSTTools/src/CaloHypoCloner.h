// $Id: CaloHypoCloner.h,v 1.2 2010-08-13 14:38:09 jpalac Exp $
#ifndef MICRODST_CALOHYPOCLONER_H
#define MICRODST_CALOHYPOCLONER_H 1

#include "ObjectClonerBase.h"

#include <MicroDST/ICloneCaloHypo.h>
#include <MicroDST/Functors.hpp>

namespace LHCb
{
  class CaloDigit;
  class CaloCluster;
}

class ICloneCaloCluster;

/** @class CaloHypoCloner CaloHypoCloner.h src/CaloHypoCloner.h
 *
 *  Clone an LHCb::CaloHypo. Recursively clones its member CaloHypos.
 *  Clones member CaloDigits, CaloClusters.
 *
 *  @author Juan PALACIOS
 *  @date   2010-08-13
 */

class CaloHypoCloner : public extends1<ObjectClonerBase,ICloneCaloHypo>
{

public:

  /// Standard constructor
  CaloHypoCloner( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  virtual ~CaloHypoCloner( ); ///< Destructor

  StatusCode initialize();

  virtual LHCb::CaloHypo* operator() (const LHCb::CaloHypo* hypo);

private:

  LHCb::CaloHypo* clone(const LHCb::CaloHypo* hypo);

private:

  typedef MicroDST::BasicItemCloner<LHCb::CaloHypo> BasicCaloHypoCloner;
  typedef MicroDST::BasicItemCloner<LHCb::CaloDigit> BasicCaloDigitCloner;
  typedef MicroDST::BasicItemCloner<LHCb::CaloCluster> BasicCaloClusterCloner;

  ICloneCaloCluster* m_caloClusterCloner;
  std::string m_caloClusterClonerName;

  bool m_cloneDigits;    ///< Flag to turn on the cloning of associated Digits
  bool m_cloneHypos;     ///< Flag to turn on the cloning of associated Hypos
  bool m_cloneClusters;  ///< Flag to turn on the cloning of associated Clusters

};

#endif // MICRODST_CALOHYPOCLONER_H
