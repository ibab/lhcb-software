// $Id: NoPIDsParticleMaker.h,v 1.1 2003-04-30 13:41:25 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef DAVINCITOOLS_NOPIDSPARTICLEMAKER_H 
#define DAVINCITOOLS_NOPIDSPARTICLEMAKER_H 1
// ============================================================================
// Include files
// DaVinciTools 
#include "DaVinciTools/IParticleMaker.h"
// Event 
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
// Kernel
#include "CaloKernel/CaloTool.h"
// forward declarations 
class IDataProviderSvc     ;
class IParticlePropertySvc ;
class  ParticleProperty    ;
// ============================================================================

/** @class NoPIDsParticleMaker NoPIDsParticleMaker.h
 *
 *  The simplest possible particle maker.
 *  It assigned PID hypothesis to charged protoparticle.
 *  Thus some imitation of "high-level-trigger-charged-particle" 
 *  is performed
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-17
 */
class NoPIDsParticleMaker :
  public virtual IParticleMaker , 
  public               CaloTool 
{
  /// friend class for instantiation 
  friend class ToolFactory<NoPIDsParticleMaker>;
public:
  
  /** Make the particles 
   *  @see IParticleMaker 
   *  @param particles  vector of particles  
   */
  virtual StatusCode makeParticles ( ParticleVector & particles ) ;
  
  /** standard initialization of tool 
   *  @see CaloTool
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode initialize    () ;   
  
  /** standard finalization  of tool 
   *  @see CaloTool
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode finalize      () ;

protected:
  
  /// accessor to event data sevice 
  inline IDataProviderSvc*     evtSvc () const { return m_evtSvc ; }
  
  /// accessor to Particle properties service 
  inline IParticlePropertySvc* ppSvc  () const { return m_ppSvc  ; }
  
  /// set particl eproperties for particle and for antiparticle  
  StatusCode setPPs( const std::string& pid ) ;
  
  
  /** Fill the particle from protoparticle using ID  
   *  @param proto    pointer to ProtoParticle
   *  @param property particle property information
   *  @param particle particle (output) 
   *  @return status code 
   */
  StatusCode fillParticle 
  ( const ProtoParticle*    proto    , 
    const ParticleProperty* property , 
    Particle*               particle ) const ;
  
protected :

  /** Standard constructor
   *  @param type   tool type
   *  @param name   tool name 
   *  @param parent tool parent
   */
  NoPIDsParticleMaker
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// virtual detructor
   virtual ~NoPIDsParticleMaker() ;

private:
  
  // default constructor  is disabled 
  NoPIDsParticleMaker();
  // copy    constructor  is disabled 
  NoPIDsParticleMaker           ( const NoPIDsParticleMaker& ) ;
  // assignement operator is disabled 
  NoPIDsParticleMaker& operator=( const NoPIDsParticleMaker& ) ;
 
private:
  
  // evend data service 
  IDataProviderSvc*     m_evtSvc  ;
  
  // particle property service 
  IParticlePropertySvc* m_ppSvc   ;  
  
  // ID of the particle 
  std::string             m_pid   ;
  // ID of the anti-particle 
  std::string             m_apid  ;
  
  // properties of particle
  const ParticleProperty* m_pp    ;
  
  // properties of anti-particle
  const ParticleProperty* m_app   ;
  
  typedef std::vector<std::string> Addresses ;
  Addresses               m_inputs ;

  // CL to be used 
  double                  m_CL     ;

  // statistics: 

  // number of calls 
  size_t                  m_calls  ;

  // total created particles 
  double                  m_sum    ;

  // total created particles (sum2)  
  double                  m_sum2   ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // NOPIDSPARTICLEMAKER_H
// ============================================================================
