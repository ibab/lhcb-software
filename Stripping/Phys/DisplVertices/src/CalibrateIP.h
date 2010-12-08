// $Id: $
#ifndef CALIBRATEIP_H 
#define CALIBRATEIP_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrackInterfaces/ITrackVertexer.h"

#include "Event/Particle.h"

#include "AIDA/IHistogram1D.h"

// from ROOT
#include "TH1.h"
#include "TF1.h"

//a class to hold beam line infos
#include "BeamLineInfo.h"

/** @class CalibrateIP CalibrateIP.h
 *  
 * Compute the beam line position and store infos
 * THIS IS INTENDED FOR DEBUGGING/TESTING AND OFFLINE MC STUDIES 
 * DO NOT USE ONLINE
 *
 *  @author Neal Gauvin
 *  @date   2009-09-17
 */


class CalibrateIP : public GaudiHistoAlg {
public: 
  /// Standard constructor
  CalibrateIP( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CalibrateIP( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  class HistoPVPos {
  public: 
    /// Standard constructor
    HistoPVPos(){};
    HistoPVPos( CalibrateIP*, std::string );
    /// Destructor
    virtual ~HistoPVPos( ){
      //This should not be needed
//       delete hx; delete hy; delete hz;
//       delete hxx; delete hxy; delete hxz;
//       delete hyy; delete hyz; delete hzz;
    };

    void Reset(){
      hx->reset();
      hy->reset();
      hz->reset();
      hxx->reset();
      hyy->reset();
      hzz->reset();
      hxy->reset();
      hxz->reset();
      hyz->reset();      
    };
    
    void Fill( double & x, double & y, double & z ){
      hx->fill( x ); hy->fill( y ); hz->fill( z );
      hxx->fill( x*x ); hyy->fill( y*y ); hzz->fill( z*z );
      hxy->fill( x*y ); hyz->fill( y*z ); hxz->fill( x*z );      
    }

    void Add( HistoPVPos & HA ){
      hx->add( *(HA.hx) ); hy->add( *(HA.hy) ); hz->add( *(HA.hz) );
      hxx->add( *(HA.hxx) ); hyy->add( *(HA.hyy) ); hzz->add( *(HA.hzz) );
      hxy->add( *(HA.hxy) ); hyz->add( *(HA.hyz) ); hxz->add( *(HA.hxz) );
    }    

    ///AIDA histograms
    AIDA::IHistogram1D*  hx;
    AIDA::IHistogram1D*  hy;
    AIDA::IHistogram1D*  hz;
    AIDA::IHistogram1D*  hxx;
    AIDA::IHistogram1D*  hxy;
    AIDA::IHistogram1D*  hxz;
    AIDA::IHistogram1D*  hyy;
    AIDA::IHistogram1D*  hyz;
    AIDA::IHistogram1D*  hzz;
  };


protected:
  
private:

  /// Compute the Beam Line
  virtual StatusCode ComputeBeamLine( HistoPVPos &  ); 
  void SaveBeamLine(); ///< Save the beam line in TES
  void GetLongs( const LHCb::RecVertex*, std::vector<const LHCb::Track*> & );
  void SmearPV( Gaudi::XYZPoint & );

  ToolHandle<ITrackVertexer> m_vertexer ;

  std::string m_PVContainerName; ///< where the PVs are located
  std::string m_outloc;          ///< where the beam line is saved on TES

  bool m_refit; ///< refit the PV with long tracks only
  bool m_local; ///< work in local Velo frame
  bool m_histo; ///< compute IP from fitting histograms
  bool m_smear; ///< smear vertex artificially

  Gaudi::Transform3D m_toVeloFrame; ///< to transform to local velo frame
  Gaudi::Transform3D m_toGlobalFrame; ///< to transform from local velo frame

  HistoPVPos H; ///< histogramms of PV positions
  HistoPVPos HT; ///< temp histogramms of PV positions

  int m_nbevent;
  int m_bin; ///< histo binning
  double m_beamstab; ///< threshold to consider beam stable

  ///cuts on the pv / bounds on histograms
  double m_minz;
  double m_maxz;
  double m_maxx;
  double m_maxy;
  double m_maxchi;
  int m_minlongs;
  int m_mintrks;

  BeamLineInfo * m_Beam;           ///<The beam line
  std::vector<double> m_BLInitPos; ///< Beam line initial position
  std::vector<double> m_BLInitDir; ///< Beam line initial direction
  int m_cycle; ///< compute beam position every m_cycle events

};


#endif // CALIBRATEIP_H
