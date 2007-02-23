// $Id: CreateMicroDSTMCAlg.h,v 1.4 2007-02-23 09:22:59 ukerzel Exp $
#ifndef CREATEMICRODSTALG_H 
#define CREATEMICRODSTALG_H 1

// Include files
// from Gaudi
#include "Kernel/DVAlgorithm.h"

// Linker
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkerTool.h"
#include "Linker/LinkedTo.h"

#include <GaudiKernel/KeyedContainer.h>
#include <GaudiKernel/IRegistry.h>

#include "DaVinciAssociators/Particle2MCLinker.h"

/** @class CreateMicroDSTMCAlg CreateMicroDSTMCAlg.h
 *   
 *
 *  @author Ulrich Kerzel
 *  @date   2007-02-09
 */
class CreateMicroDSTMCAlg : public DVAlgorithm {

  // -------------------------------------------------------------------
  //                         PUBLIC
  // -------------------------------------------------------------------
public: 
  /// Standard constructor
  CreateMicroDSTMCAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CreateMicroDSTMCAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  // -------------------------------------------------------------------
  //                         PROTECTED
  // -------------------------------------------------------------------
protected:


  // -------------------------------------------------------------------
  //                         PRIVATE
  // -------------------------------------------------------------------
private:

  //
  // typedef
  //

  //
  // methods
  //
  StatusCode StoreMCParticle(const LHCb::Particle   *particle);
  LHCb::MCParticle*StoreMCParticle(const LHCb::MCParticle *mcParticle,
                                   bool                    storeOriginVertex = false);
  LHCb::MCVertex*  StoreMCVertex  (const LHCb::MCVertex   *mcVertex);
  std::vector<const LHCb::MCParticle*> GetMCParticle (const LHCb::Particle* particle,
                                                      Particle2MCLinker* particleLinker);

  template<class T>
  T *getContainer(std::string containerID);

  template<class S, class T>                           
  StatusCode StoreLink(const S* s, const T* t);                  /** store link between cloned reconstructed
                                                    *  object (Source s) to cloned corresponding
                                                    *  MC object (Target t)
                                                    */

  inline std::string objectLocation(const DataObject *pObject) const 
  {
    return (!pObject ? "Null DataObject" :
            (pObject->registry() ? pObject->registry()->identifier() : "UnRegistered"));
  }

  //
  // data members
  //
  std::string  m_OutputPrefix;         ///< prefix for the output paths, eg. microDST
  bool         m_CloneEndVertices;     /**  store end-vertices of cloned MC Particle into 
                                        *   microDST (true) or set SmartRef (false) to save space
                                        *   default: false, use SmartRef
                                        *   N.B. setting this option to 'true' increases the size of
                                        *        the microDST significantly
                                        */
  bool         m_CloneVertexProducts;  /** store products of cloned MC vertex into
                                        *   microDST (true) or set SmartRef (false) to save space
                                        *   default: false, use SmartRef
                                        */

  std::string        m_nameMCAsct;                ///< Name of MC associator
  Particle2MCLinker* m_part2MCLinkerComposite ;   ///< associator using composite method
  Particle2MCLinker* m_part2MCLinkerLinks     ;   ///< associator       links

  // counters
  int m_nBasicParticles;                          ///< these counters are updated when trying to do the association part -> MC part
  int m_nCompositeParticles;
  int m_nBasicParticlesAssociated;
  int m_nCompositeParticlesAssociated;
};
#endif // CREATEMICRODSTALG_H
