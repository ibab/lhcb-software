// $Id: PatFwdTrackCandidate.h,v 1.6 2009-01-21 09:06:17 cattanem Exp $
#ifndef PATFWDTRACKCANDIDATE_H
#define PATFWDTRACKCANDIDATE_H 1

#include <algorithm>

// Include files
#include "Event/Track.h"
#include "PatKernel/PatForwardHit.h"

  /** @class PatFwdTrackCandidate PatFwdTrackCandidate.h
   *  Local track candidate. Basically, the Velo track, list of PatTCoord
   *  and T-station track parameters
   *
   *  @author Olivier Callot
   *  @date   2005-07-06 Initial version
   *  @date   2007-08-20 Update for A-Team framework 
   */

  class PatFwdTrackCandidate {
      private:
	  struct isNotSel {
	      bool operator()(const PatFwdHit* h) const
	      { return !h->isSelected(); }
	  };
  public:
    /// Standard constructor
    PatFwdTrackCandidate( const LHCb::Track * tr, PatFwdHits& coords) :
	m_track(tr), m_coords(coords)
    { init(); }

    PatFwdTrackCandidate( const LHCb::Track* tr ) :
	m_track(tr)
    { m_coords.reserve(32); init(); }

    void cleanCoords()
    {
	// Manuel Schiller 2012-02-06: avoid allocation of temp vector
	m_coords.erase(
		std::remove_if(m_coords.begin(), m_coords.end(), isNotSel()),
		m_coords.end());
    }

  private:
    void init() {
      const LHCb::State& state = *(m_track->stateAt(LHCb::State::EndVelo));
      m_x0  = state.x();
      m_y0  = state.y();
      m_z0  = state.z();
      m_tx  = state.tx();
      m_ty  = state.ty();
      m_tx2 = m_tx * m_tx;
      m_ty2 = m_ty * m_ty;
      m_qOverP = state.qOverP();

      m_ax = 0.;
      m_bx = 0.;
      m_cx = 0.;
      m_dx = 0.;
      m_ay = 0.;
      m_by = 0.;
      m_cosAfter = 1.;
      m_fitted = false;
      m_quality = 0.;
      m_cand1stquality = 0.;
      m_cand2ndquality = 0.;

      m_nbIT = 0;
      m_nbOT = 0;

      m_chi2PerDoF = 0.;
      m_nDoF = 0;
    }
  public:

    virtual ~PatFwdTrackCandidate( ) {}; ///< Destructor

    const LHCb::Track* track() const   { return m_track; }
    double xStraight( double z) const { return m_x0 + (z-m_z0) * m_tx; }
    double yStraight( double z) const { return m_y0 + (z-m_z0) * m_ty; }
    double qOverP()             const { return m_qOverP; }

    double slX()                const { return m_tx;                 }
    double slY()                const { return m_ty;                 }
    double slX2()               const { return m_tx2;                }
    double slY2()               const { return m_ty2;                }

    double sinTrack()           const { return sqrt( 1. - 1./(1.+m_tx2 + m_ty2) ); }


    PatFwdHits::iterator coordBegin()  { return m_coords.begin(); }
    PatFwdHits::iterator coordEnd()    { return m_coords.end(); }
    void addCoord( PatFwdHit* coord ) { m_coords.push_back( coord ); }
    void addCoords( PatFwdHits& coords )
    {
	// Manuel Schiller 2012-02-06: make sure we have space in m_coords
	m_coords.reserve(m_coords.size() + coords.size());
	m_coords.insert(m_coords.end(), coords.begin(), coords.end());
    }
    PatFwdHits& coords() { return m_coords; }

    void setParameters( double ax, double bx, double cx, double dx, double ay, double by ) {
      m_ax = ax;
      m_bx = bx;
      m_cx = cx;
      m_dx = dx;
      m_ay = ay;
      m_by = by;
      m_cosAfter =  1. / sqrt( 1. +  m_bx * m_bx  );
      m_fitted = true;
    }

    bool fitted()    const { return m_fitted; }

    void updateParameters( double dax, double dbx, double dcx,
                           double ddx=0., double day=0., double dby= 0.  ) {
      m_ax += dax;
      m_bx += dbx;
      m_cx += dcx;
      m_dx += ddx;
      m_ay += day;
      m_by += dby;
      m_cosAfter =  1. / sqrt( 1. +  m_bx * m_bx  );
    }

    double x( double dz ) const { return m_ax + dz * ( m_bx + dz * ( m_cx + dz * m_dx ) ); }
    double y( double dz ) const { return m_ay + dz *   m_by; }

    double xSlope( double dz ) const { return m_bx + dz * ( 2 * m_cx + 3 * dz * m_dx ); }

    double ySlope( double  ) const { return m_by; }

    double xMagnet( double dz ) const { return m_ax + dz * m_bx; }

    double cosAfter()          const { return  m_cosAfter; }

    double dSlope()            const { return m_bx - m_tx; }

    int setSelectedCoords() {
      PatFwdHits::iterator itH;
      int nbSelected = 0;
      for ( itH = m_coords.begin(); m_coords.end() != itH ; ++itH ) {
        if ( (*itH)->isIgnored() ) {
          (*itH)->setSelected( false );
        } else  {
          (*itH)->setSelected( true );
          nbSelected++;
        }
      }
      return nbSelected;
    }


    void setQuality( double quality )  { m_quality = quality; }
    double quality()          const    { return m_quality; }

    void setChi2PerDoF( double chi2 )  { m_chi2PerDoF = chi2; }
    double chi2PerDoF()       const    { return m_chi2PerDoF; }

    void setNDoF( int nDoF )     { m_nDoF = nDoF; }
    int nDoF()          const    { return m_nDoF; }

    double bx()         const    { return m_bx;   }

    int nbIT()          const { return m_nbIT; }
    int nbOT()          const { return m_nbOT; }
    void setNbIT( int nb )    { m_nbIT = nb; }
    void setNbOT( int nb )    { m_nbOT = nb; }

    void setCand1stQuality( double cand1stquality){ 
      m_cand1stquality = cand1stquality; 
    }
    double cand1stquality() const { return m_cand1stquality; }
    
    void setCand2ndQuality( double cand2ndquality){ 
      m_cand2ndquality = cand2ndquality; 
    }
    double cand2ndquality() const { return m_cand2ndquality; }

  protected:

  private:
    const LHCb::Track* m_track;
    double m_x0;
    double m_y0;
    double m_z0;
    double m_tx;
    double m_ty;
    double m_qOverP;
    double m_tx2;
    double m_ty2;

    PatFwdHits m_coords;

    double m_ax;
    double m_bx;
    double m_cx;
    double m_dx;
    double m_ay;
    double m_by;
    double m_cosAfter;
    bool   m_fitted;
    double m_quality;
    double m_cand1stquality;
    double m_cand2ndquality;

    int    m_nbIT;
    int    m_nbOT;

    double m_chi2PerDoF;
    int    m_nDoF;
  };
#endif // PATFWDTRACKCANDIDATE_H
