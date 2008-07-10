// $Id: OnOfflineTool.h,v 1.6 2008-07-10 15:44:38 pkoppenb Exp $
#ifndef ONOFFLINETOOL_H 
#define ONOFFLINETOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IOnOffline.h"            // Interface
/** @class OnOfflineTool OnOfflineTool.h
 *
 *  Tool that sets online or offline environment
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-06-08
 */
class IRelatedPVFinder ;
class OnOfflineTool : public GaudiTool, virtual public IOnOffline {
public: 
  /// Standard constructor
  OnOfflineTool( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~OnOfflineTool( ); ///< Destructor

  StatusCode initialize();

  /// On or Offline ?
  bool online(void) const {return m_online;} ;

  /// Return DistanceCalculator
  std::string distanceCalculator() const  ;

  /// Return Unconstrained Vertex Fitter
  std::string vertexFitter() const  ;

  /// Get PV location in TES
  std::string getPVLocation(void) const  ;

  /// Set PV location in TES
  StatusCode setPVLocation(const std::string&)  ;

  /// Get PV relator
  IRelatedPVFinder* pvRelator(void) const { return m_pvRelator ; } ;

protected :

private:
  std::string m_onlinePVLocation ; ///< PV location
  std::string m_offlinePVLocation ; ///< PV location
  bool m_online ; ///< On or Offline?
  std::string m_offlineDistTool ; ///< offline distance tool
  std::string m_onlineDistTool ; ///< online distance tool
  std::string m_offlineVertexFitter ; ///< offline vertex fitter
  std::string m_onlineVertexFitter ; ///< online vertex fitter
  std::string m_offlinePVRelatorName ; ///< offline PV Relator Name
  std::string m_onlinePVRelatorName ; ///< online PV Relator Name
  IRelatedPVFinder* m_pvRelator ; ///< PV relator
};
#endif // ONOFFLINETOOL_H
