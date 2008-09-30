// $Id: OnOfflineTool.h,v 1.7 2008-09-30 15:11:56 jpalac Exp $
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
  virtual const bool online() const {return m_online;} ;

  /// Return DistanceCalculator
  virtual const std::string& distanceCalculatorType() const  ;

  /// Return Unconstrained Vertex Fitter
  virtual const std::string& vertexFitterType() const  ;

  /// Return name of IRelatedPVFinder implementation
  virtual const std::string& relatedPVFinderType() const;

  /// Get PV location in TES
  virtual const std::string& primaryVertexLocation() const  ;

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

};
#endif // ONOFFLINETOOL_H
