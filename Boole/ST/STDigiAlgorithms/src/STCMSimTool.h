// $Id: STCMSimTool.h,v 1.1 2010-04-07 09:27:37 mneedham Exp $
#ifndef STPEDESTALSIMTOOL_H
#define STPEDESTALSIMTOOL_H 1


// local interface
#include "ISTCMSimTool.h"

#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"

#include "GaudiKernel/IIncidentListener.h"

#include "Kernel/STToolBase.h"


/** @class STCMSimTool STCMSimTool.h
 *
 *  Tool for for simulating pedestal
 *
 *  @author M.Needham
 *  @date   14/3/2010
 */

class STCMSimTool : public ST::ToolBase, 
                   virtual public ISTCMSimTool,
                   virtual public IIncidentListener {

public: 

  STCMSimTool( const std::string& type, const std::string& name,
                       const IInterface* parent );

  /// destructer
  virtual ~STCMSimTool();

  /// initialize
  StatusCode initialize();

  /// return the simulated cm noise in this sector for this event
  virtual double noise(const LHCb::STChannelID& chan) const;

  /** Implement the handle method for the Incident service.
  *  This is used to nform the tool of software incidents.
  *
  *  @param incident The incident identifier
  */
  void handle( const Incident& incident );

private:

  void initEvent() const;

  mutable bool m_isInitialized;  
  SmartIF<IRndmGen> m_gaussDist;
  bool m_forceOptions;
  double m_cmNoise; 

  typedef std::map<LHCb::STChannelID,double> NoiseMap;
  mutable NoiseMap m_cmValues; 

};

#endif // STCHARGESHARINGTOOL_H
