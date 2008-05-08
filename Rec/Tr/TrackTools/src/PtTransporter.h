// $Id: PtTransporter.h,v 1.1 2008-05-08 13:09:45 albrecht Exp $
#ifndef PTTRANSPORTER_H 
#define PTTRANSPORTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/IPtTransporter.h"            // Interface



/** @class PtTransporter PtTransporter.h
 *
 * provide a fast way to calculate a pt estimate in the velo, given a
 * state after the magnet
 *
 * @author Manuel Tobias Schiller <schiller@phys.uni-heidelberg.de>
 * @date   2008-04-16
 */
class PtTransporter : public GaudiTool, virtual public IPtTransporter {
public: 
  /// Standard constructor
  PtTransporter( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~PtTransporter( ); ///< Destructor

  virtual StatusCode initialize(); ///< Tool initialization
  virtual StatusCode   finalize(); ///< Tool finalize

  virtual double ptAtOrigin(double zref, double xref, double yref,
                            double tx, double ty, double p) const;
  virtual double ptAtOrigin(const LHCb::State& state) const;

protected:

private:
  double m_zMagnet;
};
#endif // PTTRANSPORTER_H
