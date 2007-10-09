// $Id: PatFwdPlaneCounter.h,v 1.1.1.1 2007-10-09 18:23:10 smenzeme Exp $
#ifndef PATFWDPLANECOUNTER_H
#define PATFWDPLANECOUNTER_H 1

// Include files
#include "TfKernel/HitExtension.h"
#include "TfKernel/LineHit.h"


  /** @class PatFwdPlaneCounter PatFwdPlaneCounter.h
   *  Small class to count how many different planes are in a list.
   *
   *  @author Olivier Callot
   *  @date   2005-04-11 Initial version
   *  @date   2007-08-20 Update for A-Team framework
   */

  template <class Hit>
  class PatFwdPlaneCounter
  {

  public:

    /// Standard constructor
    PatFwdPlaneCounter(typename std::vector< Hit* >::const_iterator first,
                       typename std::vector< Hit* >::const_iterator last )
      : m_planeList(12,0)
      , m_nbDifferent(0)
    {
      for ( typename std::vector< Hit* >::const_iterator ith = first;
            last != ith; ++ith ) {
        if ( !(*ith)->isSelected() ) continue;
        int plane = (*ith)->planeCode();
        if ( 0 == m_planeList[plane] ) m_nbDifferent++;
        m_planeList[plane]++;
      }
    };

    virtual ~PatFwdPlaneCounter( ) {}; ///< Destructor

    int nbDifferent() const { return m_nbDifferent; }

    int addHit( Hit* hit ) {
      if ( !hit->isSelected() ) return m_nbDifferent;
      int plane = hit->planeCode();
      if ( 0 == m_planeList[plane] ) {
        m_nbDifferent++;
      }
      m_planeList[plane]++;
      return m_nbDifferent;
    }

    int removeHit( Hit* hit ) {
      if ( !hit->isSelected() ) return m_nbDifferent;
      int plane = hit->planeCode();
      m_planeList[plane]--;
      if ( 0 == m_planeList[plane] ) m_nbDifferent--;
      return m_nbDifferent;
    }

    int maxPlane () {
      int l = -1;
      int max = 0;
      for ( unsigned int kk = 0; m_planeList.size() > kk; ++kk ) {
        if ( m_planeList[kk] > max ) {
          max = m_planeList[kk];
          l   = kk;
        }
      }
      return l;
    }

    int nbInPlane( int plane ) const { return m_planeList[plane]; }


    int nbStereo() const {
      int nb = 0;
      if ( 0 < m_planeList[1] ) ++nb;
      if ( 0 < m_planeList[2] ) ++nb;
      if ( 0 < m_planeList[5] ) ++nb;
      if ( 0 < m_planeList[6] ) ++nb;
      if ( 0 < m_planeList[9] ) ++nb;
      if ( 0 < m_planeList[10] ) ++nb;
      return nb;
    }


  protected:

  private:
    std::vector<int> m_planeList;
    int m_nbDifferent;

  };

#endif // PATFWDPLANECOUNTER_H
