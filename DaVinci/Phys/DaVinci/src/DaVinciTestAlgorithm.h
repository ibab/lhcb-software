// $Id: DaVinciTestAlgorithm.h,v 1.2 2001-10-08 11:51:30 gcorti Exp $
#ifndef DAVINCITESTALGORITHM_H
#define DAVINCITESTALGORITHM_H 1

// Include files
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IAssociator;
class IParticlePropertySvc;

/** @class DaVinciTestAlgorithm DaVinciTestAlgorithm.h
 *  
 *  Example of use of various DaVinci features
 *
 *  @author Gloria Corti
 *  @date   19/06/2001
 */
class DaVinciTestAlgorithm : public Algorithm {

public:
  /// Standard constructor 
  DaVinciTestAlgorithm(const std::string& name, ISvcLocator* pSvcLocator); 

  virtual ~DaVinciTestAlgorithm( ); ///< Standard destructor
  
  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization

private:

  std::string m_selDec;    ///< Decay code of AXSEL for decay channel to get
                           ///< results for
  int         m_nEvents;   ///< N events processed
  int         m_nPhysSelMC;  ///< Debugging purposes, counters for MC PhysSel
  int         m_nPhysSelTKR; ///< Debugging purposes, counters for TKR PhysSel
  int         m_nPhysSelPQU; ///< Debugging purposes, counters for PQUAL PhysSel
  int         m_nPhysSelSEL; ///< Debugging purposes, counters for SEL PhysSel
  int         m_nPhysSelAGR; ///< Debugging purposes, counters for AGR PhysSel
  int         m_nPhysSelTAG; ///< Debugging purposes, counters for TAG PhysSel
 
  std::string m_pNameSearch;  ///< MCParticle name to search for
  long        m_pIDSearch;    ///< MCParticle ID to search for

  int         m_nL0Trigger;   ///< Counter for L0 triggered events  

  IAssociator* m_pAsct;           ///< Pointer to associator used  
  IParticlePropertySvc* m_ppSvc;  ///< Pointer to particle property service
  
};


# endif    // DAVINCITESTALGORITHM_H
