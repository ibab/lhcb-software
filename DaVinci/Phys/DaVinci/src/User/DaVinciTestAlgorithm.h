// $Id: DaVinciTestAlgorithm.h,v 1.1 2002-02-08 19:08:10 gcorti Exp $
#ifndef DAVINCITESTALGORITHM_H
#define DAVINCITESTALGORITHM_H 1

// Include files
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IAssociator;
class IPhysSelTool;
class IMCUtilityTool;
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

  std::string m_pNameSearch;  ///< MCParticle name to search for
  long        m_pIDSearch;    ///< MCParticle ID to search for

  std::vector<std::string>  m_daugName; ///< Name of daughters to be found
  std::vector<long>         m_daugID;   ///< GeantID of daughters to be found  

  int         m_nEvents;     ///< N events processed
  int         m_nPhysSelMC;  ///< Debugging purposes, counters for MC PhysSel
  int         m_nPhysSelTKR; ///< Debugging purposes, counters for TKR PhysSel
  int         m_nPhysSelPQU; ///< Debugging purposes, counters for PQUAL PhysSel
  int         m_nPhysSelSEL; ///< Debugging purposes, counters for SEL PhysSel
  int         m_nPhysSelAGR; ///< Debugging purposes, counters for AGR PhysSel
  int         m_nPhysSelTAG; ///< Debugging purposes, counters for TAG PhysSel
  int         m_nL0Trigger;  ///< Counter for L0 triggered events  

  long        m_bIDLower;    ///< GeantID Lower B meson identifyer
  long        m_bIDUpper;    ///< GeantID Upper B meson identifyer
  
  std::string m_nameUtilTool;  ///< Name of Utility tool for MC queries

  IAssociator*     m_pAsct;    ///< Reference to associator used  
  IPhysSelTool*    m_pSelTool;  ///< Reference to tool for Phys Selection details
  IMCUtilityTool*  m_pUtilTool; ///< Reference to MC utility tool
  
  IParticlePropertySvc* m_ppSvc;   ///< Reference to particle property service

};


# endif    // DAVINCITESTALGORITHM_H
