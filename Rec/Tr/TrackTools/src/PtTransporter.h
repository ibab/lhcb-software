#ifndef PTTRANSPORTER_H 
#define PTTRANSPORTER_H 1

// Include files
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/IPtTransporter.h"            // Interface
#include "TrackInterfaces/IFunctionTool.h"             // Interface for Hlt


/** @class PtTransporter PtTransporter.h
 *
 * provide a fast way to calculate a pt estimate in the velo, given a
 * state after the magnet
 *
 * @author Manuel Tobias Schiller <schiller@phys.uni-heidelberg.de>
 * @date   2008-04-16
 */
class PtTransporter : public GaudiTool, 
                      virtual public IPtTransporter,
                      virtual public ITrackFunctionTool{
public: 
  /// Standard constructor
  PtTransporter( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~PtTransporter( ); ///< Destructor

  virtual double ptAtOrigin(double zref, double xref, double yref,
                            double tx, double ty, double p) const;
  virtual double ptAtOrigin(const LHCb::State& state) const;

  double function(const LHCb::Track& track);

protected:

private:
  double m_zMagnet;
};
#endif // PTTRANSPORTER_H
