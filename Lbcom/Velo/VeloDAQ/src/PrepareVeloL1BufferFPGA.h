// $Id: PrepareVeloL1BufferFPGA.h,v 1.2 2004-03-19 12:44:05 parkesb Exp $
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

  class sortClustersByID {
  public:
    bool operator () ( const L1VeloFPGACluster* first, 
                       const L1VeloFPGACluster* second ) const {
      if ( 0==first  ) return true;
      if ( 0==second ) return false;

      if ( first->sensor() != second->sensor() ) {
        return first->sensor() < second->sensor();
      }
      int type = first->sensor() % 8;

      //== Reversed Phi sensors: Increasing phi is decreasing strip number
      if ( 4 == type || 6 == type ) {
        return first->strip() > second->strip() ;
      }
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
