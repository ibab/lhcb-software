// $Id: L1VeloFPGACluster.h,v 1.1.1.1 2004-03-16 14:28:12 cattanem Exp $
#ifndef L1VeloFPGACLUSTER_H 
#define L1VeloFPGACLUSTER_H 1

// Include files
  
class MCVeloFE;
class MCParticle;

/** @class L1VeloFPGACluster L1VeloFPGACluster.h 
 *  
 *
 *  @author Niels Tuning
 *  @date   17/05/2002
 */
class L1VeloFPGACluster {
public:
  /// Standard constructor
  L1VeloFPGACluster( ) {}; 
  ///
  L1VeloFPGACluster( int sensor, 
               int strip, 
               int nStrips, 
               int nStripsFE, 
                 std::vector<MCVeloFE*> mcVeloFEinCluster) {
    m_sensor            = sensor;
    m_strip             = strip;
    m_nStrips           = nStrips;
    m_nStripsFE         = nStripsFE;
    m_mcVeloFEinCluster = mcVeloFEinCluster;
    
  }
  ///
  virtual ~L1VeloFPGACluster( ) {}; ///< Destructor

  /// Return sensor
  int sensor()                    const { return m_sensor; }
  /// Return strip
  int strip()                     const { return m_strip; }
  /// Return width of cluster
  int nStrips()                   const { return m_nStrips; }
  /// Return nr of strips with a MCVeloFE associated to it
  int nStripsFE()                 const { return m_nStripsFE; }
  /// Return MCVeloFE of first strip of cluster. If it doesn't exist,
  /// try the other strip(s).
  std::vector<MCVeloFE*> mcVeloFEinCluster()  { return m_mcVeloFEinCluster; }; 

private:
  int m_sensor;
  int m_strip;
  int m_nStrips;
  int m_nStripsFE;
  std::vector<MCVeloFE*> m_mcVeloFEinCluster; 

};
#endif // L1VELOFPGACLUSTER_H
