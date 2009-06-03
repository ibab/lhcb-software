// $Id: ENNRingFinder.cpp,v 1.8 2009-06-03 08:52:59 jonrob Exp $
// Include files

// STL
#include <algorithm>
#include <cmath>
#include <iostream>

// local
#include "ENNRingFinder.h"

using namespace Rich::Rec::ENNRingFinder;

double Finder::hitProbability( const Ring & /* ring */, 
                               const Hit & hit )
{
  const double prob = ( hit.nAssRings > 0 ? 1.0/hit.nAssRings : 0.0 );
  return prob;
}

void Finder::FindRings()
{
  Ring::Vector & Rings = rings();
  Hit::Vector  & Hits  = hits();

  typedef Hit::Vector::iterator iH;
  typedef Hit::PtnVector::iterator iP;

  Rings.clear();
  std::sort( Hits.begin(), Hits.end() );

  Hit::PtnVector SearchArea;
  Hit::PtnVector PickUpArea;
  SearchArea.reserve(40);
  PickUpArea.reserve(40);
  
  // MAIN LOOP OVER HITS

  iH ileft = Hits.begin(), iright = ileft, i = ileft;

  for ( ; i != Hits.end(); ++i )
  {
    
    if ( i->busy >= 1 ) continue; // already found hit

    const double left  = i->x - config().areaSize();
    const double right = i->x + config().areaSize();

    while ( ileft->x < left ) ++ileft;
    while ( iright != Hits.end() && iright->x < right ) ++iright;

    SearchArea.clear();
    PickUpArea.clear();

    double X(0), Y(0), R(0), R2(0);
    int NRingHits(1);
    double Dmax(0);
    double S0(0), S1(0), S2(0), S3(0), S4(0), S5(0), S6(0), S7(0);
    int NAreaHits(0);

    { // initialize hits in the search area

      S0 = S1 = S2 = S3 = S4 = 0.;

      for( iH j = ileft; j != iright; ++j )
      {
        if ( j == i ) continue;
        j->ly = j->y - i->y;
        if( std::fabs(j->ly) > config().areaSize() ) continue;
        j->lx = j->x - i->x;
        j->S0 = j->lx * j->lx;
        j->S1 = j->ly * j->ly;
        j->lr2 = j->S0 + j->S1;
        if( j->lr2 > config().areaSize2() ) continue;
        ++NAreaHits;
        if( j->busy >= 13 )
        {
          PickUpArea.push_back( &*j );
          continue;
        }
        SearchArea.push_back( &*j );

        const double &lr2 = j->lr2;
        const double lr   = std::sqrt(lr2);
        if( lr > Dmax ) Dmax = lr;

        j->S2 = j->lx * j->ly;
        j->S3 = j->lx * lr2;
        j->S4 = j->ly * lr2;
        j->C = -lr/2;

        if( lr > 1.E-4 )
        {
          const double w = 1./lr;
          const double w2 = w*w;
          j->Cx = w*j->lx;
          j->Cy = w*j->ly;
          S0 += w2*j->S0;
          S1 += w2*j->S1;
          S2 += w2*j->S2;
          S3 += w2*j->S3;
          S4 += w2*j->S4;
        }
        else 
        {
          j->Cx = j->Cy = 0;
          S0 += j->S0;
          S1 += j->S1;
          S2 += j->S2;
          S3 += j->S3;
          S4 += j->S4;
        }
      }

      if ( NAreaHits+1 < config().minRingHits() ) continue;

    }// end of initialization of the search area

    // loop for minimization of E and noise rejection
    do
    {
      // calculate parameters of the current ring
      double tmp = S0*S1-S2*S2;
      if( std::fabs(tmp) < 1.E-10 ) break;
      tmp = 0.5/tmp;
      X = (S3*S1 - S4*S2)*tmp;
      Y = (S4*S0 - S3*S2)*tmp;
      R2 = X*X + Y*Y;
      R = std::sqrt( R2 );

      const double Dcut = Dmax * config().rejectionFact(); // cut for noise hits
      const double RingCut = 4.0 * config().hitSize() * ( config().hitSize() + R ); // closeness
      S0 = S1 = S2 = S3 = S4 = 0.0;
      NRingHits = 1;
      NAreaHits = 0;
      Dmax = -1.;
      for( iP j = SearchArea.begin(); j != SearchArea.end(); ++j )
      {
        const double dx = (*j)->lx - X;
        const double dy = (*j)->ly - Y;
        const double d2 = std::fabs( dx*dx + dy*dy - R2 );
        (*j)->on_ring = ( d2 <= RingCut );
        if( (*j)->on_ring )
        {
          ++NRingHits;
          S0 += (*j)->S0;
          S1 += (*j)->S1;
          S2 += (*j)->S2;
          S3 += (*j)->S3;
          S4 += (*j)->S4;
        }
        else
        {
          const double dp = std::fabs( (*j)->C + (*j)->Cx*X + (*j)->Cy*Y );
          if( dp > Dcut ) continue;
          if( dp > Dmax ) Dmax = dp;
          ++NAreaHits;
          const double w = 1./d2;
          S0 += w*(*j)->S0;
          S1 += w*(*j)->S1;
          S2 += w*(*j)->S2;
          S3 += w*(*j)->S3;
          S4 += w*(*j)->S4;
        }
      }
    }
    while ( Dmax > 0 && NRingHits + NAreaHits >= config().minRingHits() );

    // store the ring if it is found

    if( NRingHits < config().minRingHits() || R2 > config().r2Max() || R2 < config().r2Min() ) continue;

    { // final fit of 3 parameters (X,Y,R)
      int n = 1;
      S0 = S1 = S2 = S3 = S4 = S5 = S6 = S7 = 0.0;
      for( iP j = SearchArea.begin(); j != SearchArea.end(); ++j )
      {
        if( !(*j)->on_ring ) continue;
        S0 += (*j)->S0;
        S1 += (*j)->S1;
        S2 += (*j)->S2;
        S3 += (*j)->S3;
        S4 += (*j)->S4;
        S5 += (*j)->lx;
        S6 += (*j)->ly;
        S7 += (*j)->lr2;
        ++n;
      }
      const double s0 = S6*S0-S2*S5;
      const double s1 = S0*S1-S2*S2;
      const double s2 = S0*S4-S2*S3;
      if( std::fabs(s0) < 1.E-6 || std::fabs(s1) < 1.E-6 ) continue;
      const double tmp = s1*(S5*S5-n*S0)+s0*s0;
      const double A = ( ( S0*S7-S3*S5 )*s1-s2*s0 ) / tmp;
      Y = (s2 + s0*A )/s1/2;
      X = ( S3 + S5*A - S2*Y*2 )/S0/2;
      R2 = X*X+Y*Y-A;
      if( R2 < 0 ) continue;
      R = std::sqrt( R2 );
    }// end of the final fit

    if ( R2 > config().r2Max() || R2 < config().r2Min() ) continue;

    Rings.push_back( Ring(i->x+X,i->y+Y,R,1) );
    Ring & ring = Rings.back();
    ring.Hits.push_back(&*i);
    for( iP j = SearchArea.begin(); j != SearchArea.end(); ++j )
    {
      const double dx = (*j)->lx - X;
      const double dy = (*j)->ly - Y;
      const double d = std::fabs( std::sqrt(dx*dx+dy*dy) - R );
      if( d <= config().hitSigma() )
      {
        ring.chi2 += d*d;
        ring.Hits.push_back(*j);
        ring.NHits++;
      }
    }
    for( iP j = PickUpArea.begin(); j != PickUpArea.end(); ++j )
    {
      const double dx = (*j)->x - ring.x;
      const double dy = (*j)->y - ring.y;
      const double d = std::fabs( std::sqrt(dx*dx+dy*dy) - ring.r );
      if( d <= config().hitSigma() )
      {
        ring.chi2 += d*d;
        ring.Hits.push_back(*j);
        ring.NHits++;
      }
    }
    if( ring.NHits < config().minRingHits() )
    {
      Rings.pop_back();
      continue;
    }
    ring.chi2 = ring.chi2 / ( ring.NHits - 3 )/.3/.3;
    for( iP j = ring.Hits.begin(); j != ring.Hits.end(); ++j )
    {
      if( (*j)->busy<ring.NHits ) (*j)->busy = ring.NHits;
    }

  }// END OF THE MAIN LOOP OVER HITS

  // SELECTION OF RINGS

  typedef Ring::Vector::iterator iR;

  for( iH i = Hits.begin(); i != Hits.end(); ++i ) i->busy = 0;
  for( iR i = Rings.begin(); i != Rings.end(); ++i )
  {
    i->skip = i->on = 0;
    i->NOwn = i->NHits;
    if( ( i->NHits < config().minRingHits() ) ||
        ( i->NHits <= 6 && i->chi2 > .3 ) )
    {
      i->skip = 1;
    }
  }

  do
  {
    iR best = Rings.end();
    int bestOwn = 0;
    double bestChi2 = 1.E20;
    for( iR i = Rings.begin(); i != Rings.end(); ++i )
    {
      if( i->skip ) continue;
      if( ( i->NOwn < 1.0*(config().minRingHits()) ) ||
          ( i->NHits < 10 && i->NOwn < 1.00*i->NHits ) )
      {
        i->skip = 1;
        continue;
      }
      if( ( i->NOwn > 1.2*bestOwn ) ||
          ( i->NOwn >= 0.8*bestOwn && i->chi2 < bestChi2 ) )
      {
        bestOwn = i->NOwn;
        bestChi2 = i->chi2;
        best = i;
      }
    }
    if ( best == Rings.end() ) break;
    best->skip = 1;
    best->on = 1;
    for( iP i = best->Hits.begin(); i != best->Hits.end(); ++i )
    {
      (*i)->busy = 1;
    }
    for( iR i = Rings.begin(); i != Rings.end(); ++i )
    {
      if( i->skip ) continue;
      const double dist = i->r+best->r+2*config().hitSigma();
      if( std::fabs(i->x-best->x) > dist ||
          std::fabs(i->y-best->y) > dist ) continue;
      i->NOwn = 0;
      for( iP j = i->Hits.begin(); j != i->Hits.end(); ++j )
      {
        if( !(*j)->busy ) i->NOwn++;
      }
    }
  }
  while(true);

  // CRJ : Count number of rings each hit is associated with
  for ( iR r = Rings.begin(); r != Rings.end(); ++r )
  {
    for ( iP h = r->Hits.begin(); h != r->Hits.end(); ++h )
    {
      ++((*h)->nAssRings);
    } 
  }

}
