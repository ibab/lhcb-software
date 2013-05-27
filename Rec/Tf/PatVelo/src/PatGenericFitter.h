#ifndef TF_PAT_PATGENERICFITTER_H 
#define TF_PAT_PATGENERICFITTER_H 1

// Math definitions
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"

// from Gaudi
#include "GaudiKernel/StatusCode.h"

#include "PatVeloRHitManager.h"
#include "PatVeloPhiHitManager.h"
#include "CircularRangeUtils.h"

namespace Tf {
  /** @class PatGenericFitter PatGenericFitter.h
   *
   *  @author Tomas LASTOVICKA
   *  @date   2005-10-18
   */

  class PatGenericFitter {
    public: 

      /// Standard constructor
      PatGenericFitter( PatVeloRHitManager*, PatVeloPhiHitManager*, bool );  
      ~PatGenericFitter(); ///< Destructor

      /// The main fittig function
      StatusCode fit();  

      void reset();  

      /// Add a pair of clusters, they are allowed to be NULL
      void addPair( PatVeloRHit*, PatVeloPhiHit* );  

      inline int entries()  const { return m_rCoord.size(); }

      inline double getX0() const { return m_par[0]; }
      inline double getY0() const { return m_par[1]; }
      inline double getSX() const { return m_par[2]; }
      inline double getSY() const { return m_par[3]; }
      inline double getX( double z ) const { return m_par[0] + z*m_par[2]; }
      inline double getY( double z ) const { return m_par[1] + z*m_par[3]; }    

      inline void setValid( bool flag) { m_valid = flag; }
      inline bool isValid() const      { return m_valid; }    

      std::vector<PatVeloRHit*>&   rCoords()   { return m_rCoord; }
      std::vector<PatVeloPhiHit*>& phiCoords() { return m_pCoord; }

      inline const Gaudi::SymMatrix4x4& covariance() { return m_matrix; }

      /// Tests if track seeds share clusters - lite version
      bool shareClustersLite( PatGenericFitter* );

      /// Last sensor on track
      double lastSensorZ();
      double firstSensorZ();  
      const DeVeloSensor* lastSensor();  

      /// All clusters are marked as used
      void markUsed();  

      void sortCoordinates(); 

      double rExtrap( double );  
      double pExtrap( double );

      double maxVariation();

    private:

      double pseudoPhi(const PatVeloPhiHit* hit);
      
    private:

      PatVeloRHitManager*   m_rHitManager;
      PatVeloPhiHitManager* m_phiHitManager;

      CircularRangeUtils<double> m_angleUtils;
      
      Gaudi::SymMatrix4x4 m_matrix;
      Gaudi::Vector4 m_vector;
      Gaudi::Vector4 m_par;
      Gaudi::Vector4 m_par_old;

      bool m_valid;
      bool m_align;  

      std::vector<PatVeloRHit*>   m_rCoord;
      std::vector<PatVeloPhiHit*> m_pCoord;  

      void fillMatrix();
      bool solve();

  };
}

#endif // TF_PAT_PATGENERICFITTER_H
