// $Id: OnOfflineTool.h,v 1.3 2005-10-17 12:28:42 pkoppenb Exp $
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

  /// Return DispCalculator
  std::string dispCalculator() const  ;

  /// Return Unconstrained Vertex Fitter
  std::string vertexFitter() const  ;

  /// Get PV location in TES
  std::string getPVLocation(void) const  ;

  /// Set PV location in TES
  StatusCode setPVLocation(const std::string&)  ;

protected :

private:
  std::string m_onlinePVLocation ; ///< PV location
  std::string m_offlinePVLocation ; ///< PV location
  bool m_online ; ///< On or Offline?
  std::string m_offlineGeomTool ; ///< offline geom tool
  std::string m_onlineGeomTool ; ///< online geom tool
  std::string m_offlineVertexFitter ; ///< offline vertex fitter
  std::string m_onlineVertexFitter ; ///< online vertex fitter
};
#endif // ONOFFLINETOOL_H
