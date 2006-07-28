#ifndef SeedHit_h
#define SeedHit_h

//#include "Seeding/TrSeedPlaneDef.h"
#include "TrackSeeding/TrackSeedPoint.h"
#include "STDet/DeSTDetector.h"
#include "OTDet/DeOTDetector.h"
#include "Kernel/STChannelID.h"
#include "Event/STLiteCluster.h"
#include "Event/OTTime.h"
#include <vector>

/*
 Class SeedHit follows LTHit, from longtracks, but can cope with alignments.
 Therefore I include a link to TrackSeedPlaneDef, so as to easily be able to track
 with different alignments of OT/IT modules. 
 the class is not intended to go outside the seeding algorithm, so I do not
 inherit from LHCb containers.
 
 author: H.J. Bulten
 created: 15-12-2005
 modified 4-7-2006: I split up the hit information and the ambiguity. SeedHit 
 will be created once per event; TrackSeedHit will contain a pointer to SeedHit and
 a boolean, representing the ambiguity, and will lay on the track candidate.
 For an event with 10,000 OT hits, there will be far more than a billion
 TrackSeedHit combinations that need to be considered.
 modified 20-7-2006. There were so many (up to billion/evt) calls to y() and x()
 that I do not calculate the position from the trajectory, but precompile functions
 taking into account the wire drift time and ambiguity. Class TrackSeedPlaneDef is now obsolete

*/


using namespace std;
class SeedHit {
	public:
	friend class TrackSeedHit;

// static member declarations :

	static double OT_wiredrifttime; // to calculate OT drift distance
	static double OT_driftspeed; // to calculate OT drift distance
	static double OT_cellradius; // to calculate OT drift distance
	static double Y_margin; // extra margin above/below plane for acceptance
	static double OTResolution; // from OT Det
	static double ITResolution; // needs to be modified later

//
// two static member functions that may be called without making a TrackSeedHit
// for fitting hits
// they require vectors containing the z-position, hit position and uncertainty,
// a supplied vector for the return parameters (fit parameters and error
// correlation matrix) and a vector (may be empty) of indices of hits that are
// not considered in the fit. This last vector is done for speed reasons;
// I did not want to remove hits from the first three vectors before fitting is
// finished
//
// the functions are optimized for speed, a parabolic fit through 20 points 
// should take a fraction of a microsecond. Therefore I do not make use of
// external fitting libraries
//
        static double line_fit(const vector<double>& zcoord, const vector<double>& xcoord, const  vector<double>& error, vector<double>& params, vector<unsigned int>& removedhits);
        static double parabola_fit(const vector<double>& zcoord, const vector<double>& xcoord, const  vector<double>& error, vector<double>& params, vector<unsigned int>& removedhits);

// constructors
	SeedHit();
	SeedHit(LHCb::STLiteCluster* itclus, DeSTDetector* itdet);
	SeedHit(LHCb::OTTime* ott, DeOTDetector* otdet);
// use default copy/assignment constructor/ default destructor
//	TrackSeedHit(const TrackSeedHit& hit);
//	TrackSeedHit& operator=(const TrackSeedHit& hit);
//	~TrakSeedHit();
//
// public member functions
  	inline bool isIT() const { return m_isIT;}; // always check it, I only set it true if hit and plane are correctly filled.
  	inline bool isOT() const { return m_isOT;}; // always check it, I only set it true if hit and plane are correctly filled.

	inline double x() const { return xoff[0];}; // return x-value OT center wire/ IT cluster 
        inline double x(bool ambig) const {
	   if (ambig) return (xoff[1]+yoff[0]*xoff[2]);
	   else return (xoff[3]+yoff[0]*xoff[4]);
        }; // return x-value for center wire OT at ambiguity ambig
	inline double x(double yy) const { return (xoff[5]+yy*xoff[6]); }; // return x-value wire/IT cluster at height y
	inline double x(bool ambig, double yy) const {
	   if (ambig) return (xoff[1]+yy*xoff[2]);
	   else return (xoff[3]+yy*xoff[4]);
	}
	inline double y() const {return yoff[0];}; // return y-value OT center wire/ IT cluster 
	inline double y(double xx) const { return (yoff[5]+xx*yoff[6]);}; // return y-value wire/IT cluster at x-pos xx
	inline double y(bool ambig, double xx) const {
	   if (ambig) return (yoff[1]+xx*yoff[2]);
	   else return (yoff[3]+xx*yoff[4]);
	}; //return y_value OT for ambiguity bool at position x
	inline double z() const { return zoff[0];}; // return z-value OT center wire/ IT cluster 
        inline double z(double xx, double yy) const{ return (zoff[1]+xx*zoff[2]+yy*zoff[3]);};
	
	inline double error() const { return m_error;};

	inline bool isStereo() const { return (fabs(xoff[6])>1e-3);}; // stereo hit.
        inline unsigned int station() const { return m_station;}; // station 1,2, or 3
	inline bool ambiguity() const { return m_ambig;}; // for OT
 	inline void setAmbiguity(bool a) {m_ambig = a;};  // for OT
        LHCb::STLiteCluster* getItRef() {return m_ITRef;}; // to get back the hits
        LHCb::OTTime* getOtRef() {return m_OTRef;}; // to get back the hits

// private data members
	private:
	LHCb::STLiteCluster* m_ITRef;
	LHCb::OTTime* m_OTRef;
	double zoff[4]; // z0, 3* formula for z given x and y
	double yoff[7]; // y0, 3* formula for z given x
	double xoff[7]; // x0, 3* formula for z given y
        unsigned int m_station; // station 1,2, or 3
        double m_x; // also used in time for spill-over killing
	double m_error;
	double m_ymin; // stored instead of calculated because of speed. there are billions of calls to this
	double m_ymax; // stored instead of calculated because of speed. there are billions of calls to this
	bool m_isIT;
	bool m_isOT;
	bool m_ambig;
};

#endif
