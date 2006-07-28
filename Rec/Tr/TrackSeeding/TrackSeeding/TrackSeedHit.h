#ifndef TrackSeedHit_h
#define TrackSeedHit_h

#include "TrackSeeding/SeedHit.h"

/*
 author: H.J. Bulten
 modified: 4-7-2006
 TrackSeedHit contains a pointer to SeedHit and the ambiguity information.
*/


using namespace std;
class TrackSeedHit {
	public:

// static member declarations :

// millions of fits per event, one fit
// should take a fraction of a microsecond. Therefore I do not make use of
// external fitting libraries
//
        static double line_fit(const vector<double>& zcoord, const vector<double>& xcoord, const  vector<double>& error, vector<double>& params, vector<unsigned int>& removedhits);
        static double parabola_fit(const vector<double>& zcoord, const vector<double>& xcoord, const  vector<double>& error, vector<double>& params, vector<unsigned int>& removedhits);

// constructors
	TrackSeedHit() {};
	TrackSeedHit(SeedHit* ptr, bool ambig=false) {
              m_SeedHit = ptr;
	      m_ambig=ambig;
        };
//
// public member functions
  	inline bool isIT() const { return m_SeedHit->m_isIT;}; // always check it, I only set it true if hit and plane are correctly filled.
  	inline bool isOT() const { return m_SeedHit->m_isOT;}; // always check it, I only set it true if hit and plane are correctly filled.

	double x() const { return m_SeedHit->x();}; // return x-value OT center wire/ IT cluster 
        double x(bool ambig) const {return m_SeedHit->x(ambig);}; // return x-value OT 
	double x(double yy) const {return m_SeedHit->x(yy);}; // return x-value wire/IT cluster at height y
	double x(bool left_ambig, double yy) const {return m_SeedHit->x(left_ambig,yy);};
	double y() const {return m_SeedHit->y();}; // return y-value OT center wire/ IT cluster 
	double y(double xx) const {return m_SeedHit->y(xx);}; // return y-value wire/IT cluster at x-pos xx
	double y(bool left_ambig, double xx) const {return m_SeedHit->y(left_ambig,xx);}; //return y_value OT for ambiguity bool
	double z() const {return m_SeedHit->z();}; // return z-value OT center wire/ IT cluster 
        double z(double xx, double yy) const {return m_SeedHit->z(xx,yy);}; // return z-value OT for position xx, yy
        inline double y_min() const {return m_SeedHit->m_ymin;}; // return minimum y
        inline double y_max() const {return m_SeedHit->m_ymax;}; // return maximum y
	
	inline double error() const { return m_SeedHit->m_error;};

	bool isStereo() const { return m_SeedHit->isStereo(); }; // stereo : more than 20 mrad deviating from an X-plane
        inline double tanY() const { return m_SeedHit->xoff[6];}; // dy/dx = xoff[6]
        inline unsigned int station() const { return m_SeedHit->m_station;}; // station 1,2, or 3
	inline bool ambiguity() const { return m_ambig;}; // for OT
 	inline void setAmbiguity(bool a) {m_ambig = a;};  // for OT
        inline LHCb::STLiteCluster* getItRef() {return m_SeedHit->m_ITRef;}; // to get back the hits
        inline LHCb::OTTime* getOtRef() {return m_SeedHit->m_OTRef;}; // to get back the hits

// private data members
	private:
        SeedHit* m_SeedHit;
	bool m_ambig;
};

#endif
