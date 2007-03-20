// $Id: TrackPtKick.h,v 1.4 2007-03-20 13:11:42 mneedham Exp $
#ifndef TRACKTOOLS_TRACKPTKICK_H 
#define TRACKTOOLS_TRACKPTKICK_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from LHCbKernel
#include "Kernel/IBIntegrator.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackPtKick.h"

/** @class TrackPtKick TrackPtKick.h TrackTools/TrackPtKick.h
 *  
 *  @author M. Needham
 *  @date   2000-08-16
 */
class TrackPtKick : public GaudiTool, virtual public ITrackPtKick {
public: 
  /// Standard constructor
  TrackPtKick( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual ~TrackPtKick( ); ///< Destructor

  StatusCode initialize();

  // Estimate the momentum P of a State
  virtual StatusCode calculate( LHCb::State* State ) const;

protected:

private:
  StatusCode determineFieldPolarity();

  IBIntegrator* m_bIntegrator;

  int m_FieldPolarity;
  
  double m_MomentumError;
  /// Define the parameters of the Z dependance
  std::vector<double> m_ParabolicCorrection;
  std::vector<double> m_resParams;
  double m_Constant;

};
#endif // TRACKTOOLS_TRACKPTKICK_H
