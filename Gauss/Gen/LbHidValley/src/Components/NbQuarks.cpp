// $Id: NbQuarks.cpp,v 1.2 2007-03-07 19:14:00 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2006/10/28 14:06:41  ibelyaev
// New package: "Hidden Valley" production 
// 
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/SystemOfUnits.h" 
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/GenAlgs.h"
#include "LoKi/GenParticleCuts.h"
// ============================================================================
// Generators 
// ============================================================================
#include "MCInterfaces/IGenCutTool.h"
// ============================================================================
/** @class NbQuarks NbQuarks.cpp
 *  Simple cut tool, based on the counting of b-quarks in the acceptance 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-10-26
 */
class NbQuarks 
  : public           GaudiTool 
  , virtual public IGenCutTool 
{
  // friend factory for instantiations 
  friend class ToolFactory<NbQuarks> ;
public:
  virtual StatusCode initialize () 
  {
    StatusCode sc = GaudiTool::initialize() ;
    if ( sc.isFailure() ) { return sc ; }
    LoKi::ILoKiSvc* loki = svc<LoKi::ILoKiSvc>("LoKiSvc",true) ;
    if ( 0 == loki ) { return StatusCode::FAILURE ; }
    return StatusCode::SUCCESS ;
  } ;
  /** Applies the cut on the signal interaction.
   *  @param[in,out] theParticleVector  List of signal particles. The 
   *                                    generator level cut is applied to
   *                                    all these particles and particles
   *                                    which do not pass the cut are removed
   *                                    from theParticleVector.
   *  @param[in]     theGenEvent        Generated interaction. The generator
   *                                    level cut can use the particles in 
   *                                    this event to take the decision.
   *  @param[in]     theCollision       Hard process information of the
   *                                    interaction which can be used by
   *                                    the cut to take the decision.
   *  @param[in]     theDecayTool       tool to generate the signal decay
   *                                    before applying the cut.
   *  @param[in]     cpMixture          indicate the generation of a CP mixture
   *  @param[in]     theSignalAtRest    signal at reset (for forced 
   *                                    fragmentation).
   *  @return        true  if the event passes the generator level cut.
   */
  virtual bool applyCut
  ( ParticleVector&           theParticleVector , 
    const HepMC::GenEvent*    theGenEvent       ,
    const LHCb::GenCollision* theCollision      ) const ;
protected:
  /** standard constructor
   *  @param type tool type(?)
   *  @param name tool name 
   *  @param parent tool parent 
   */
  NbQuarks
  ( const std::string& type , 
    const std::string& name , 
    const IInterface*  parent ) 
    : GaudiTool  ( type , name , parent ) 
    , m_nb       ( 3 )
    , m_thetaMax ( 400 * Gaudi::Units::mrad )
  {
    declareInterface<IGenCutTool> ( this ) ;
    declareProperty ( "Nb"       , m_nb        ) ;
    declareProperty ( "ThetaMax" , m_thetaMax  ) ;    
  } ;
  /// virtual and protected constructor 
  virtual ~NbQuarks() {}
private:
  // number of b-quarks in acceptance 
  unsigned int m_nb ; ///< number of b-quarks in acceptance 
  // "acceptance": max theta 
  double m_thetaMax ; ///< "acceptance": max theta
};

// ============================================================================
/// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY ( NbQuarks )

// ============================================================================
/** Applies the cut on the signal interaction.
 *  @param[in,out] theParticleVector  List of signal particles. The 
 *                                    generator level cut is applied to
 *                                    all these particles and particles
 *                                    which do not pass the cut are removed
 *                                    from theParticleVector.
 *  @param[in]     theGenEvent        Generated interaction. The generator
 *                                    level cut can use the particles in 
 *                                    this event to take the decision.
 *  @param[in]     theCollision       Hard process information of the
 *                                    interaction which can be used by
 *                                    the cut to take the decision.
 *  @param[in]     theDecayTool       tool to generate the signal decay
 *                                    before applying the cut.
 *  @param[in]     cpMixture          indicate the generation of a CP mixture
 *  @param[in]     theSignalAtRest    signal at reset (for forced 
 *                                    fragmentation).
 *  @return        true  if the event passes the generator level cut.
 */
// ============================================================================
bool NbQuarks::applyCut
( ParticleVector&           /* theParticleVector */ , 
  const HepMC::GenEvent*       theGenEvent          ,
  const LHCb::GenCollision* /* theCollision      */ ) const 
{
  using namespace LoKi::Cuts ;
  
  if ( msgLevel ( MSG::DEBUG ) || statPrint() ) 
  {
    counter ("#b") += LoKi::GenAlgs::count_if 
      ( theGenEvent , "b" == GABSID && 2 == GSTATUS ) ;
  }
  
  const size_t nBq = 
    LoKi::GenAlgs::count_if ( theGenEvent , 
                              "b" == GABSID  && 
                              2   == GSTATUS &&
                              GTHETA < m_thetaMax ) ;
  
  if ( msgLevel ( MSG::DEBUG ) || statPrint() ) { counter ("#bacc") += nBq ; }
  
  return m_nb <= nBq ;
}


// ============================================================================
// The END 
// ============================================================================
