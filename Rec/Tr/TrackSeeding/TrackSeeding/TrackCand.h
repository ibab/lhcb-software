#ifndef TrackCand_h
#define TrackCand_h
/*
 * Class TrackCand
 * author H.J. Bulten
 * Feb 21, 2006
 *
 * helper class; basically just a structure, to keep track of intermediate
 * hits in the seeding
 *
*/ 

#include <vector>
#include "TrackSeeding/TrackSeedHit.h"

class TrackCand {
   public:
      // default constructors, destructors, assignment operators
      TrackCand() { x_linefit=true;};
      ~TrackCand() {};
      void reset(); // clear all  arrays
      void resetX(); // clear X arrays
      void resetY(); // clear Y arrays
      void xHits(std::vector<TrackSeedHit>& vec); // return non-skipped hits in vec
      void yHits(std::vector<TrackSeedHit>& vec); // return non-skipped hits in vec
      void xParams(std::vector<double>& vec); // return fit parameters
      void yParams(std::vector<double>& vec); // return fit parameters
      void deleteXHit(unsigned int hitnum);
      void deleteYHit(unsigned int hitnum);
      void addXHit(const TrackSeedHit& hit, double xpos, double zpos, double error);
      void addYHit(const TrackSeedHit& hit, double xpos, double zpos, double error);
      void setXFitParams(const std::vector<double> & pars) {xfitpars=pars;};
      void setYFitParams(const std::vector<double> & pars) {yfitpars=pars;};
      double refitXHits(bool linefit=true); // when y is known, X may be refitted
      double refitYHits(); // when x is known, Y may be refitted
      unsigned int xSkipped() const { return skipped_x.size();};
      unsigned int ySkipped() const { return skipped_y.size();};

   private:
      std::vector<TrackSeedHit> xhits;
      std::vector<TrackSeedHit> yhits;
      std::vector<double> x;
      std::vector<double> x_error;
      std::vector<double> z_x;
      std::vector<double> xfitpars;
      std::vector<double> y;
      std::vector<double> y_error;
      std::vector<double> z_y;
      std::vector<double> yfitpars;
      std::vector<unsigned int> skipped_x;
      std::vector<unsigned int> skipped_y;
      bool x_linefit; // true -> linefit; false -> parabolafit
};

#endif
