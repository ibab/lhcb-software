// $Id: P2MCPFromProtoP.h,v 1.2 2009-10-08 16:14:20 pkoppenb Exp $
#ifndef P2MCPFROMPROTOP_H
#define P2MCPFROMPROTOP_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticle2MCWeightedAssociator.h"            // Interface
#include "Kernel/Particle2MCAssociatorBase.h"
// from LoKi
#include "LoKi/PhysMCTypes.h"

/** @class P2MCPFromProtoP P2MCPFromProtoP.h
 *
 *  ProtoParticle to MC association tool
 *
 *  @author Juan PALACIOS
 *  @date   2009-03-31
 */
class P2MCPFromProtoP : public Particle2MCAssociatorBase
{

public:

  /// Standard constructor
  P2MCPFromProtoP( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  virtual ~P2MCPFromProtoP( ); ///< Destructor

  virtual StatusCode initialize() ;

private :

  typedef std::vector<std::string> Addresses ;
  typedef std::vector<const LoKi::Types::TablePP2MC*> Tables;

  virtual Particle2MCParticle::ToVector
  relatedMCPsImpl(const LHCb::Particle* particle,
                  const LHCb::MCParticle::ConstVector& mcParticles) const ;

  void loadTables() const;

  inline const Tables& i_tables() const
  {
    return m_tables;
  }

private:

  Addresses m_PP2MC; ///< standard addresses (without /Event)

  mutable Tables m_tables;

};
#endif // P2MCPFROMPROTOP_H
