// $Id: MCDecayFinderAlgorithm.h,v 1.1.1.1 2001-07-09 09:28:42 gcorti Exp $
#ifndef DAVINCITOOLS_MCDECAYFINDERALGORITHM_H
#define DAVINCITOOLS_MCDECAYFINDERALGORITHM_H 1


// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

// Forward declarations
class IParticlePropertySvc;

/** class MCDecayFinderAlgorithm MCDecayFinderAlgorithm.h DaVinciTools/MCDecayFinderAlgorithm.h
 *               Retrieves MCParticles objects,
 *               and looks for one step decay specified in the job
 *               options.
 *               Queries the ParticlePropertySvc
 *
 *  @author Gloria Corti  
 *  @date   13/03/2001
 */

class MCDecayFinderAlgorithm : public Algorithm {

public:
  /// Constructor 
  MCDecayFinderAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);

  /// Standard Destructor
  virtual ~MCDecayFinderAlgorithm();

  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  std::string               m_nameParent;   ///< name of parent Decay particle
  std::vector<std::string>  m_nameProducts; ///< name of Daughters particle

  NTuple::Item<float>  m_eventN;            ///< ntuple event number
  NTuple::Item<long>   m_parentID;          ///< ntuple pID of the parent
  NTuple::Item<float>  m_parentPx, m_parentPy, m_parentPz; ///< Parent momentum
  NTuple::Item<float>  m_originX, m_originY, m_originZ;    ///< origin vertex of parent
  NTuple::Item<float>  m_decayX, m_decayY, m_decayZ;       ///< decay vertex of parent
  NTuple::Item<long>   m_nDecProd;                         ///< Number of decay products
  NTuple::Array<long>  m_decayPID;                         ///< pID of each decat product
  NTuple::Array<float> m_decayPx, m_decayPy, m_decayPz;    ///< Momentum of each decay product

  IParticlePropertySvc* m_ppSvc;  ///< pointer to ParticlePropertySvc

  long                  m_nEvents;      ///< N events processed
  long                  m_nEvSuccess;   ///< N events with at least one decay

};


# endif    // DAVINCITOOLS_MCDECAYFINDERALGORITHM_H
