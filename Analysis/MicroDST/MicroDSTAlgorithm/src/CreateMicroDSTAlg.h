// $Id: CreateMicroDSTAlg.h,v 1.5 2007-04-19 14:50:09 ukerzel Exp $
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

/** @class CreateMicroDSTAlg CreateMicroDSTAlg.h
 *   
 *
 *  @author Ulrich Kerzel
 *  @date   2007-02-09
 */
class CreateMicroDSTAlg : public DVAlgorithm {

  // -------------------------------------------------------------------
  //                         PUBLIC
  // -------------------------------------------------------------------
public: 
  /// Standard constructor
  CreateMicroDSTAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CreateMicroDSTAlg( ); ///< Destructor

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
  typedef LinkerWithKey< LHCb::RecVertex ,LHCb::Particle > Part2Vertex;

  //
  // methods
  //
  StatusCode StorePV(std::string location);               ///< store primary vertices into microDST
  LHCb::Particle* StoreParticle(const LHCb::Particle * particle);
  StatusCode StoreLink2PV(const LHCb::Particle * particle);
  StatusCode StoreOdin();
  StatusCode StoreRecEventHeader();

  template<class T>
  T *getContainer(std::string containerID);

  inline std::string objectLocation(const DataObject *pObject) const 
  {
    return (!pObject ? "Null DataObject" :
            (pObject->registry() ? pObject->registry()->identifier() : "UnRegistered"));
  }

  //
  // data members
  //
  std::string              m_OutputPrefix;                    ///< prefix for the output paths, eg. microDST
  bool                     m_StoreCalo2DST;                   ///< store calorimeter info to microDST (true) or set SmartRef (false=default)
  std::map<std::string, ObjectContainerBase*> m_ContainerMap; ///< map of container - locations to store

};
#endif // CREATEMICRODSTALG_H
