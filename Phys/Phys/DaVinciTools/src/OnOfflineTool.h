// $Id$
// ============================================================================
#ifndef ONOFFLINETOOL_H 
#define ONOFFLINETOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IOnOffline.h"            // Interface
// ============================================================================
/** @class OnOfflineTool OnOfflineTool.h
 *
 *  Tool that sets online or offline environment
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-06-08
 */
class OnOfflineTool : public GaudiTool, 
                      virtual public IOnOffline 
{

public: 

  /// Standard constructor
  OnOfflineTool( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);
  
  virtual ~OnOfflineTool( ); ///< Destructor
  
  StatusCode initialize();

public:
  
  virtual bool online() const {return m_online;} ;
  
  virtual const std::string& distanceCalculatorType () const  ;
  
  virtual const std::string& vertexFitterType       () const  ;
  
  virtual const std::string& particleCombinerType   () const  ;
  
  virtual const std::string& relatedPVFinderType    () const  ;

  virtual const std::string& primaryVertexLocation  () const  ;
  
  virtual const std::string& trunkOnTES             () const  ;
  
private:

  // ==========================================================================
  /// On or Offline?
  bool        m_online               ;          //               On or Offline?
  /// PV location 
  std::string m_onlinePVLocation     ;          //                  PV location
  /// PV location 
  std::string m_offlinePVLocation    ;          //                  PV location
  /// Offline distance  tool
  std::string m_offlineDistTool      ;          //        offline distance tool
  /// Online distance  tool
  std::string m_onlineDistTool       ;          //         online distance tool
  /// Offline vertex fitter     
  std::string m_offlineVertexFitter  ;          //        offline vertex fitter
  /// Online vertex fitter            
  std::string m_onlineVertexFitter   ;          //         online vertex fitter
  /// Offline particle combiner     
  std::string m_offlineCombiner      ;          //             offline combiner
  /// Online particle combiner             
  std::string m_onlineCombiner       ;          //              online combiner 
  /// Offline PV relator name 
  std::string m_offlinePVRelatorName ;          //      offline PV Relator Name
  /// Onlne PV relator name 
  std::string m_onlinePVRelatorName  ;          //       online PV Relator Name
  /// Offline root of TES location. Default "Phys".
  std::string m_offlineTESTrunk      ;          // offline root of TES location
  /// Online root of TES location. Default "Hlt".
  std::string m_onlineTESTrunk       ;          //  online root of TES location
  // ==========================================================================  
};
// ============================================================================
// The END 
// ============================================================================
#endif // ONOFFLINETOOL_H
// ============================================================================

