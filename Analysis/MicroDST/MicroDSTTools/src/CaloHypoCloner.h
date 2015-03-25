
#ifndef MICRODST_CALOHYPOCLONER_H
#define MICRODST_CALOHYPOCLONER_H 1

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "ObjectClonerBase.h"

#include <MicroDST/ICloneCaloHypo.h>
#include <MicroDST/Functors.hpp>

#include "MicroDST/ICloneMCParticle.h"

// from LHCb
#include "Event/MCParticle.h"

// linker stuff
#include "Linker/LinkerTool.h"
#include "Linker/LinkerWithKey.h"

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

class CaloHypoCloner : public extends1<ObjectClonerBase,ICloneCaloHypo>,
                       virtual public IIncidentListener
{

public:

  /// Standard constructor
  CaloHypoCloner( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  virtual ~CaloHypoCloner( ); ///< Destructor

  StatusCode initialize();

  /** Implement the handle method for the Incident service.
   *  This is used to inform the tool of software incidents.
   *
   *  @param incident The incident identifier
   */
  void handle( const Incident& incident );
  
public:
  
  virtual LHCb::CaloHypo* operator() (const LHCb::CaloHypo* hypo);

private:

  typedef MicroDST::BasicItemCloner<LHCb::CaloHypo>    BasicCaloHypoCloner;
  typedef MicroDST::BasicItemCloner<LHCb::CaloDigit>   BasicCaloDigitCloner;
  typedef MicroDST::BasicItemCloner<LHCb::CaloCluster> BasicCaloClusterCloner;
  typedef std::vector<const LHCb::CaloHypo*>           HypoList;
  
private:

  /// Clone a Hypo
  LHCb::CaloHypo* clone(const LHCb::CaloHypo* hypo);

  /// Clone MC Links for the given Hypo and its clone
  void cloneMCLinks( const LHCb::CaloHypo* hypo,
                     const LHCb::CaloHypo* cloneHypo );

  /// Static list of cloned hypos
  HypoList & clonedHypoList() { static HypoList list; return list; }

private:

  ICloneCaloCluster* m_caloClusterCloner;
  std::string m_caloClusterClonerName;

  bool m_cloneDigits;    ///< Flag to turn on the cloning of associated Digits
  bool m_cloneHypos;     ///< Flag to turn on the cloning of associated Hypos
  bool m_cloneClusters;  ///< Flag to turn on the cloning of associated Clusters
  bool m_cloneMCLinks;   ///< Flag to turn on cloning of links to MCParticles

  /// Type of MCParticle cloner
  std::string m_mcpClonerName;
  /// MCParticle Cloner
  ICloneMCParticle * m_mcPcloner;

};

#endif // MICRODST_CALOHYPOCLONER_H
