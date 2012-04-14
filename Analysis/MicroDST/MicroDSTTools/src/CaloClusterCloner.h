// $Id: CaloClusterCloner.h,v 1.1 2010-08-13 14:33:57 jpalac Exp $
#ifndef MICRODST_CALOCLUSTERCLONER_H
#define MICRODST_CALOCLUSTERCLONER_H 1

#include "ObjectClonerBase.h"

#include <MicroDST/ICloneCaloCluster.h>            // Interface
#include <MicroDST/Functors.hpp>

// from LHCb
#include "Event/CaloCluster.h"
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"

/** @class CaloClusterCloner CaloClusterCloner.h src/CaloClusterCloner.h
 *
 *  Clone an LHCb::CaloCluster. Deep-clones CaloClusterEntries (CaloDigits).
 *
 *  @author Juan PALACIOS
 *  @date   2010-08-13
 */

class CaloClusterCloner : public extends1<ObjectClonerBase,ICloneCaloCluster>
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
  typedef MicroDST::BasicItemCloner<LHCb::CaloDigit>   BasicCaloDigitCloner;

  bool m_cloneEntries;

};

#endif // MICRODST_CALOCLUSTERCLONER_H
