// $Id: MCReconstructed.h,v 1.1 2009-10-08 16:14:20 pkoppenb Exp $
#ifndef MCRECONSTRUCTED_H 
#define MCRECONSTRUCTED_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Interface
#include "MCInterfaces/IMCReconstructed.h"

// from DaVinci
#include "Kernel/Particle2MCLinker.h"

// from
//#include "Relations/IRelationWeighted.h"


/** @class MCReconstructed MCReconstructed.h
 *   
 *
 *  @author Christian Jacoby
 *  @date   2004-03-08
 *
 *  The @c MCReconstructed tool determines if a @c MCParticle has been
 *  reconstructed by reverse association using @c ProtoParticle2MCLinker.
 *  That is, if the tested @c MCParticle has an associated @c ProtoParticle,
 *  then @c MCReconstructed considers it to have been reconstructed.
 *  The @c IMCReconstructed::RecCategory is determined by examining the charge
 *  of the @c MCParticle and the track type of the associated @c ProtoParticle.
 */
class MCReconstructed : public GaudiTool, virtual public IMCReconstructed {
public: 
  /// Standard constructor
  MCReconstructed( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~MCReconstructed( ); ///< Destructor

  /// Initialize and finalize
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual IMCReconstructed::RecCategory reconstructed( const LHCb::MCParticle * );

protected:

private:

  ///< Acceptance parameters (neutrals)
  //double m_zECAL, m_xECALInn, m_yECALInn, m_xECALOut, m_yECALOut;
  // Threshold for Et gammas reconstructibility
  //double m_lowEt;             

  ProtoParticle2MCLinker *m_pCPPAsct; ///< Ref. to CPP2MC Associator used
  ProtoParticle2MCLinker *m_pNPPAsct; ///< Ref. to NPP2MC Associator used

  /// Reconstruted as merged pi0
  //bool recoAsMergedPi0( const LHCb::MCParticle* mcpart );
  
  //typedef IRelationWeighted<CaloCluster,LHCb::MCParticle,float> MCCaloAsct;
  //typedef const MCCaloAsct::InverseType MCCaloTable  ;
  //typedef const MCCaloTable::Range     MCCaloRange    ;
  //MCCaloAsct* m_asctCaloClus;

};
#endif // MCRECONSTRUCTED_H
