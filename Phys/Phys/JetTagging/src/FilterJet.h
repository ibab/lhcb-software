// $Id: FilterJet.h,v 1.3 2007/06/18 11:27:44 jpalac Exp $
// FilterJet created 25.02.06
// 
//

#ifndef FILTERJET_H
#define FILTERJET_H 1

// ============================================================================
// Include files
// ============================================================================
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// DaVinci Kernel
// ============================================================================
#include "Kernel/DaVinciAlgorithm.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/RecHeader.h"
#include "Kernel/IJetTagTool.h"



using namespace Gaudi::Units ;
using namespace LHCb ;
using namespace std ;



// ============================================================================
// Main Algorithm
// ============================================================================
class FilterJet :  public DaVinciAlgorithm
{
  friend class AlgFactory<FilterJet> ;
 public:
  /** Standard constructor
   *  @param name instance name
   *  @param pSvc pointer to Service Locator
   */
  FilterJet(const std::string& name,
		ISvcLocator* pSvcLocator) ;
  
  
  /// destructor
  virtual ~FilterJet( ){};
  virtual StatusCode initialize ();

  /** standard execution of the algorithm
   *  @return status code
   */
  virtual StatusCode execute   () ;
 private:
  // the default constructor is disabled
  FilterJet () ;
  // the copy constructor is disabled
  FilterJet ( const  FilterJet& )  ;
  // number of particle that have to verify the cut
  FilterJet operator=( const  FilterJet& )  ;

 protected:
  // ==========================================================================
  /// Write empty container if selection fails.
  void writeEmptyKeyedContainers  ( ) const;
  /// Write empty container if selection fails.
  void writeEmptySharedContainers ( ) const;
    // ==========================================================================
 private:
  // ==========================================================================
  /// save (clone if needed) selected particles in TES 
  /// Success if number of saved particles == number saved
  /// to TES.
  /// Overwritten from DaVinciAlgorithm. Is called automatically.
  virtual StatusCode _saveInTES () ;
  /// Write empty containers if selection fails.
  virtual void writeEmptyTESContainers() ;

private:
  // ============================================================================
  /// save (clone if needed) selected particles in TES 
  template <class PARTICLES>
  StatusCode _save () const ;

 private:

  // name to retrieve tool
  std::string m_tagToolName;
  IJetTagTool* m_tagTool;

  bool m_save_all;
  bool m_save_addinfo;
  int  m_start_addinfo;
  std::string  m_SubPID;
  double  m_cut;

  std::vector<LHCb::Particle*>  i_markedParticles;


};


#endif // FILTERJET_H
