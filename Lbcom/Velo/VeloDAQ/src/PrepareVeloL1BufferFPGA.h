// $Id: PrepareVeloL1BufferFPGA.h,v 1.5 2004-12-06 10:36:38 dhcroft Exp $
#ifndef PREPAREVELOL1BUFFERFPGA_H 
#define PREPAREVELOL1BUFFERFPGA_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// from VeloDet
#include "VeloDet/DeVelo.h"

// local
#include "L1VeloFPGACluster.h"

/** @class PrepareVeloL1BufferFPGA PrepareVeloL1BufferFPGA.h
 *  Prepare the Velo Cluster container for fast access.
 *
 *  @author Olivier Callot
 *  @date   2003-04-11
 */
class PrepareVeloL1BufferFPGA : public Algorithm {
public:
  /// Standard constructor
  PrepareVeloL1BufferFPGA( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrepareVeloL1BufferFPGA( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  Rndm::Numbers m_gaussDist;
  
  class sortClustersByPosition {
    DeVelo* m_velo;	  
  public :  
    explicit sortClustersByPosition(DeVelo* de) : m_velo(de) {}
    bool operator () ( const L1VeloFPGACluster* first, 
                       const L1VeloFPGACluster* second ) const {
      if ( 0==first  ) return true;
      if ( 0==second ) return false;
      
      // sort by station
      if ( first->sensor() != second->sensor() ) {
        return (m_velo->sensorIndex(first->sensor()) < 
                m_velo->sensorIndex(second->sensor()) );
      }
      // sort by strip
      if (m_velo->isPhiSensor(first->sensor())){
        if (m_velo->isDownstreamSensor(first->sensor())){ 
          //== Flipped Phi sensors: Increasing phi is decreasing strip number
          return first->strip() > second->strip() ;
        }
      }
      // else a non-flipped phi or an R sesnor
      return first->strip() < second->strip() ;
    }        

  };

protected:

private:
  double      m_hitthreshold_ston;  ///< Hit Threshold (S/N) 
  int         m_doslopecorrection;  ///< Do a linear or flat CM correction 
  double      m_cluthreshold_ston;  ///< Cluster Threshold (S/N) 
  double      m_relthreshold_neig;  ///< Relative signal of neighbouring strip
  double      m_chargeThreshold; ///< bit 13 set for high charge clusters

  DeVelo*          m_velo;          ///< Detector element
};
#endif // PREPAREVELOL1BUFFERFPGA_H
