// $Id: NoPIDsParticleMaker.h,v 1.4 2005-11-11 16:26:40 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2005/02/09 18:01:34  pkoppenb
// CLHEP
//
// Revision 1.2  2005/01/06 10:46:54  pkoppenb
// Moved interfaces
//
// Revision 1.1.1.1  2004/08/24 06:47:48  pkoppenb
// ParticleMaker extracted from DaVinciTools
//
// Revision 1.6  2004/07/28 14:15:24  pkoppenb
// untag
//
// Revision 1.5  2004/07/08 10:14:26  pkoppenb
// HLT
//
// Revision 1.4  2004/05/11 16:01:25  pkoppenb
// DVAlgorithm.cpp
//
// Revision 1.3  2004/04/22 02:55:13  ibelyaev
//  fix a problem with release of some tools/services
//
// Revision 1.2  2004/03/11 13:02:14  pkoppenb
// Split DaVinciTools into DaVinciTools and DaVinciKernel
//
// Revision 1.1  2003/04/30 13:41:25  gcorti
// new particlemaker for HLT studies
//
// ============================================================================
#ifndef DAVINCITOOLS_NOPIDSPARTICLEMAKER_H 
#define DAVINCITOOLS_NOPIDSPARTICLEMAKER_H 1
// ============================================================================
// Include files
// DaVinciTools 
#include "Kernel/IParticleMaker.h"
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
  
  // Job options to keep long tracks
  bool m_longTracks;

  // Job options to keep upstream tracks
  bool m_downstreamTracks;
  
  // Job options to keep VTT tracks
  bool m_vttTracks;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // NOPIDSPARTICLEMAKER_H
// ============================================================================
