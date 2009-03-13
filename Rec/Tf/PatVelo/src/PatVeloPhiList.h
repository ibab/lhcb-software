// $Id: PatVeloPhiList.h,v 1.4 2009-03-13 16:17:32 ocallot Exp $
#ifndef TF_PATVELOPHILIST_H 
#define TF_PATVELOPHILIST_H 1

// Include files
#include "PatVeloHit.h"

namespace Tf {
  /** @class PatVeloPhiList PatVeloPhiList.h
   *  
   *  List of Velo Phi hits
   *
   *  @author Olivier Callot
   *  @date   2005-06-10
   */
  class PatVeloPhiList {
    public: 
      /// Standard constructor
      PatVeloPhiList( );

      /// initialising constructor
      PatVeloPhiList( double phi, PatVeloPhiHit* coord ) ;

      virtual ~PatVeloPhiList( ) {}; ///< Destructor

      /// Zero out all values in this list
      void initialize ( double phi, PatVeloPhiHit* coord );

      /// distance^2 in 2D between track extrapolation and x,y position
      double distSquared( double x, double y );

      /// compare new coord to last extrapolation/coord
      bool isCloser( double xPred, double yPred, PatVeloPhiHit* coord );

      /// extrapolate to new Z position
      void setExtrapolation( double z, double r, double tol, double tol2 );

      /// add the candidate to the list (if there is one) and refit
      void saveCandidate( );

      // some inline accessors
      inline double phi()         const { return m_phi; }
      inline int size()           const { return m_coord.size(); }
      inline std::vector<PatVeloPhiHit*>& coords() { return m_coord; }
      inline double predX()       const { return m_predX; }
      inline double predY()       const { return m_predY; }
      inline bool valid()         const { return m_valid; }
      inline double posX( )       const { return m_posX ;  }
      inline double slopeX( )     const { return m_slopeX; }
      inline double posY( )       const { return m_posY ;  }
      inline double slopeY( )     const { return m_slopeY; }

      /// set if the list of phi clusters is valid
      inline void setValid( bool valid )    { m_valid = valid; }

      /// Calculate chiSq for this combination of clusters
      double chiSq() { 
        if ( 0 < m_chi2 ) return m_chi2;
        return computeChi2();
      }
      double computeChi2();

    /// Compute the quality factor
    double qFactor() {
      if ( -100 <= m_qFact ) return m_qFact;
      m_qFact = chiSq() - 0.5 * nbUnused();
      return m_qFact;
    }    


      /// Number of uniquely used clusters
      int nbUnused();

      /// force recomputation of chi2
      void addExtraPhi( PatVeloPhiHit* coord ) {
        m_coord.push_back( coord );
        m_chi2 = -1;
        m_qFact = -1000.;
     }

    private:
      bool       m_valid;   ///< is the combination OK 
      double     m_phi;     ///< Phi of the track at "current" z
      double     m_cosPhi;  ///< cos(phi) for convience
      double     m_sinPhi;  ///< sin(phi) for convience
      PatVeloPhiHit* m_bestCoord; ///< At current Z best current coord
      std::vector<PatVeloPhiHit*> m_coord; ///< coord list of used coords

      double m_posX;        ///< X of track at last z
      double m_slopeX;      ///< slope in X of track at last z
      double m_posY;        ///< Y of track at last z
      double m_slopeY;      ///< slope in Y of track at last z
      double m_lastZ;       ///< last Z position 

      double m_predX;       ///< predicted X of track at current z
      double m_predY;       ///< predicted Y of track at current z
      double m_tol;         ///< current tolerance between coord and extrapolation
      double m_chi2;        ///< current chiSq
    double m_qFact;       ///< Quality factor, from chi2 and nbUnused.


      // local fit parameters
      double m_s0, m_sx, m_sz, m_sxz, m_sz2, m_t0, m_ty, m_tz,  m_tyz, m_tz2;

  };

  //typedef std::vector<PatVeloPhiList> PatVeloPhiLists;

  class PatVeloPhiLists{
    public:
      typedef std::vector<PatVeloPhiList>::iterator iterator;
      typedef std::vector<PatVeloPhiList>::const_iterator const_iterator;

      // initilaise some values
      PatVeloPhiLists(): m_patVeloPhiVec(64), m_finish(m_patVeloPhiVec.begin()){};
      PatVeloPhiLists(int size): m_patVeloPhiVec(size),
                                 m_finish(m_patVeloPhiVec.begin()) {};

      inline iterator begin() { return m_patVeloPhiVec.begin(); }
      inline iterator   end() { return m_finish; }
      inline size_t    size() { return m_finish - m_patVeloPhiVec.begin(); }
      inline bool     empty() { return m_finish == m_patVeloPhiVec.begin(); }
      inline void     clear() { m_finish = m_patVeloPhiVec.begin(); }

      void addEntry( const double & phi, PatVeloPhiHit* coord ){
        // bit to resize underlying vector if required
        if( m_finish == m_patVeloPhiVec.end() ){
          unsigned int tSize = m_patVeloPhiVec.size();
          m_patVeloPhiVec.resize(2*tSize);
          m_finish = m_patVeloPhiVec.begin() + tSize;      
        }
        // reuse the entry pointed to by m_finish and increment
        m_finish->initialize(phi,coord);
        ++m_finish;
      }

    private:
      std::vector<PatVeloPhiList> m_patVeloPhiVec;
      iterator m_finish;
  };
}  

#endif // TF_PATVELOPHILIST_H
