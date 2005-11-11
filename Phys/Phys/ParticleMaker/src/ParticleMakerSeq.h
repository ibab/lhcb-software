// $Id: ParticleMakerSeq.h,v 1.4 2005-11-11 16:26:40 pkoppenb Exp $
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
// Revision 1.3  2004/08/23 13:55:49  pkoppenb
// GaudiAlg
//
// Revision 1.2  2004/07/28 14:15:24  pkoppenb
// untag
//
// Revision 1.1  2004/06/18 12:11:36  pkoppenb
// ParticleMakerSeq
// 
// ============================================================================
#ifndef DAVINCITOOLS_PARTICLEMAKERSEQ_H 
#define DAVINCITOOLS_PARTICLEMAKERSEQ_H 1
// ============================================================================
// Include files
// ============================================================================
// from STL
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// from GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVincitTools 
// ============================================================================
#include "Kernel/IParticleMaker.h"
// ============================================================================


/** @class ParticleMakerSeq ParticleMakerSeq.h
 *
 *  Simple particle maker which does delegates all its job to few concrete
 *  Particle makers 
 *  
 *  In principel this maker does nor provide any gain in the code ,
 *  but it results in a simplicication of job configuration
 *  Preload algorithm become not nesessary anymore
 *
 *  In combines few "public" makers and few "private" makers
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-04-29
 */
class ParticleMakerSeq : 
  public         GaudiTool      ,
  public virtual IParticleMaker 
{
  /// friend factory for instantiation
  friend class ToolFactory<ParticleMakerSeq>;
  //
public:
  
  /** make the particles
   *  @see IParticleMaker
   *  @param particles (output) vector of Particles
   *  @return status code 
   */
  virtual StatusCode makeParticles( ParticleVector & particles ) ;
  
  /// standard initializetion of the tool
  virtual StatusCode initialize () ;

  /// standard finalization of the tool
  virtual StatusCode finalize   () ;

protected:

  /** Standard constructor
   *  @see GaudiTool
   *  @see AlgTool
   *  @see ToolFactory
   *  @see IToolFactory
   *  @param type tool type (?)
   *  @param name tool name 
   *  @param parent tool's parent
   */
  ParticleMakerSeq 
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor 
  virtual ~ParticleMakerSeq ();  

private:

  // default constructor is private
  ParticleMakerSeq() ;
  // copy    constructor is private
  ParticleMakerSeq( const ParticleMakerSeq& ) ;
  // assignement operator is private is private
  ParticleMakerSeq& operator=( const ParticleMakerSeq& ) ;
  
private:

  typedef std::vector<std::string>     Names   ;
  typedef std::vector<IParticleMaker*> Makers  ;
  typedef unsigned long                Counter ;
  
  Names   m_private ;
  Names   m_public  ;
  Makers  m_makers  ;
  Counter m_counter ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // PARTICLEMAKERSEQ_H
// ============================================================================
