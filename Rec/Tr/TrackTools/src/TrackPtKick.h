// $Id: TrackPtKick.h,v 1.2 2005-09-07 12:32:10 cattanem Exp $
#ifndef TRACKTOOLS_TRACKPTKICK_H 
#define TRACKTOOLS_TRACKPTKICK_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackPtKick.h"            // Interface

// from LHCbKernel
#include "Kernel/IBIntegrator.h"

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
  virtual StatusCode calculate( State* State ) const;

protected:

private:
  void determineFieldPolarity();

  IBIntegrator* m_bIntegrator;

  int m_FieldPolarity;
  
  double m_MomentumError;
  /// Define the parameters of the Z dependance
  std::vector<double> m_ParabolicCorrection;
  double m_Constant;

};
#endif // TRACKTOOLS_TRACKPTKICK_H
