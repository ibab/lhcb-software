#include "TrackSeeding/TrackSeedHit.h"
#include "STDet/DeSTLayer.h"
#include "STDet/DeSTSector.h"
#include "OTDet/DeOTModule.h"
#include "Kernel/Trajectory.h"
//#include "GaudiKernel/Transform3D.h"

using namespace std; // for the vectors

/*
 Class TrackSeedHit follows LTHit, from longtracks, but can cope with alignments.
 Therefore I include a link to TrackSeedPlaneDef, so as to easily be able to track
 with different alignments of OT/IT modules. 
 the class is not intended to go outside the seeding algorithm, so I do not inherit
 from LHCb containers.
 
 author: H.J. Bulten
 created: 15-12-2005
 last modification: 4-7-2006 split up ambiguity and hit; hit info in pointer to  SeedHit. This since billions of TrackSeedHits are made but only thousands of 
 SeedHits in 1 event. The ambiguity info needs to be duplicated, but a pointer
 to SeedHit is enough
*/

// the static members:


//
// line_fit can be called from outside, it is a static member function
// vectors with z-coordinates, hit coordinates and errors contain the hits
// to minimize the cost of removing hits from these list, a vector with
// the indices of hits with a chi-2 larger than 3 can be supplied in vector removed
// indices that are in the vector removed in the call to the method will be skipped
// the index of the hit with the largest deviation, if that deviation is above
// three sigma, will be added to the vector removed. It is for the calling
// method to decide whether the fit will be redone without that hit.
// 
// return value: chi2 of the fit, if successful
// chi2 = -1,-2,-3,-4 if failure (inconsistent input) :
//    it may signal a wrong number of hits,errors and positions
//    it may be that there is an error that is 0, smaller than 10^-30
//    it may be that the difference in z-positions is smaller than 1 mm for all
// 	considered points. Then the slope cannot be fitted reliably
//
// params 0 = average z
// params 1 = x at that z
// params 2 = uncertainty in x
// params 3 = slope dz/dx
// params 4 = uncertainty in slope
//
  
double TrackSeedHit::line_fit(const vector<double>& zcoord , 
const vector<double>& xcoord, const vector<double>& error,
vector<double>& params, vector<unsigned int>& removed) {

   double chi2=-1;
   if ((zcoord.size() - removed.size()) <2) return chi2;
   if (xcoord.size() != zcoord.size()) return chi2;
   if (error.size() != zcoord.size()) return chi2;
   if (params.size()<5) params.resize(5);
   if (zcoord.size() == 2) {
// special case, happens often. Speed up fit for this
// do not check for removed.size; the cases that zcoord.size-removed.size==2 will be slower
      double sum=0;
      double av=0;
      double tot=0;
      for (int i=0; i<2; i++) {
	 double err = 1/(error[i]*error[i]+1e-30);
	 sum+=xcoord[i]*err;
	 av+=zcoord[i]*err;
	 tot+=err;
      }
      params[0]=av/tot;
      params[1]=sum/tot;
      params[2]=1/sqrt(tot);
      double zdiv=zcoord[0]-zcoord[1];
      if (fabs(zdiv)>1) { // more than 1 mm distance between planes
 	params[3]=(xcoord[0]-xcoord[1])/zdiv;
	params[4]=params[2]/fabs(zdiv);
      } else params[3]=params[4]=0;
      return 0.0;
   }
	 
   double zav=0;
   double xav=0;
   double lambda=0;
   double sigsum =0;
   double lambsum=0;
   for (unsigned int i=0; i<error.size(); i++ ) {
      if (error[i]<1e-30) return chi2;
      double sig2=1/error[i]/error[i];
      for (unsigned int j=0; j<removed.size(); j++) if (removed[j]==i) sig2=0; // do not consider this point
      sigsum+=sig2;
      zav+=sig2*zcoord[i];
      xav+=sig2*xcoord[i];
   }
   zav/=sigsum; // weighted average z-position
   xav/=sigsum; // weighted average of x of all hits
   for (unsigned int i=0; i<zcoord.size(); i++) {
      bool takehit=true;
      for (unsigned int j=0; j<removed.size(); j++) if (removed[j]==i) takehit=false;
      double zdiv = zcoord[i]-zav;
      if ((fabs(zdiv)>1) && takehit) {
         double siglambda = (1 - 1/error[i]/error[i]/sigsum)*error[i]/zdiv;
	 double tmp = 1/siglambda/siglambda;
	 lambda+=(xcoord[i]-xav)/(zcoord[i]-zav)*tmp;
	 lambsum+=tmp;
      }
   }
   if (lambsum>0) lambda/=lambsum;
   else return chi2;
   params[0]=zav; // z-point around which line has been fitted
   params[1]=xav; // x-value at that z
   params[2]=(1/sqrt(sigsum)); // uncertainty in that x-value
   params[3]=(lambda); // slope
   params[4]=(1/sqrt(lambsum)); // uncertainty in slope
   chi2=0;
   if (zcoord.size() - removed.size() >2) {
      double maxdev=0;
      int removeint=-1;
      for (unsigned int i=0; i<zcoord.size(); i++) {
         bool takehit=true;
         for (unsigned int j=0; j<removed.size(); j++) if (removed[j]==i) takehit=false;
         if (takehit) {
            double temp=(xcoord[i]-xav-(zcoord[i]-zav)*lambda)/error[i];
	    temp*=temp;
            chi2+=temp;
	    if (temp>maxdev) { 
	       maxdev=temp;
	       removeint=i;
	    }
	 }
      }
      if (maxdev>9) removed.push_back(removeint); // the hit at index removeint is more than 3 standard dev's away
      chi2=chi2/(zcoord.size()-removed.size()-2);
   }
   return chi2;
}
      
//
// parabola_fit can be called from outside, it is a static member function
// vectors with z-coordinates, hit coordinates and errors contain the hits
// to minimize the cost of removing hits from these list, a vector with
// the indices of hits with a chi-2 larger than 3 can be supplied in vector removed
// indices that are in the vector removed in the call to the method will be skipped
// the index of the hit with the largest deviation, if that deviation is above
// three sigma, will be added to the vector removed. It is for the calling
// method to decide whether the fit will be redone without that hit.
// 

double TrackSeedHit::parabola_fit(const vector<double>& zcoord,const vector<double>& xcoord, const  vector<double>& error, vector<double>& params, vector<unsigned int>& removed) {
/*
   The parabola x = params[0] + params[1]*zcoord + params2*zcoord*zcoord is
   fitted. the hits have position xcoord and uncertainty error. The error
   correlation matrix is given in params[3..8]
   params 3 = variance p0
   params 4 = variance p1
   params 5 = variance p2
   params 6 = error correlation p0-p1
   params 7 = correlation p0-p2
   params 8 = correlation p1-p2
 Note, that the parameters will be correlated!. 

   Fit method: following numerical recipes, ch. 15.4 (www.nr.com)
   using the Cholesky decomposition to invert the positive-definite error matrix
  
*/
   double beta[3];
   double a[5];
   double chi2 = -1;
   if (zcoord.size() - removed.size() <3) return chi2;
   if (xcoord.size() != zcoord.size()) return chi2;
   if (error.size() != zcoord.size()) return chi2;
   if (params.size()<9) params.resize(9);
   for (int i=0; i<3; i++) beta[i]=0;
   for (int i=0; i<5; i++) a[i]=0;
   for (unsigned int i=0; i<zcoord.size(); i++ ) {
      bool skip = false;
      for (unsigned int k=0; ((k<removed.size()) && !skip);k++) skip=(removed[k]==i);
      if (skip) continue;
      if (error[i]<1e-30) return chi2;
      double sig2=1.0/error[i]/error[i]; 
      a[0] += sig2;
      beta[0]+=xcoord[i]*sig2; 
      for (int j=1; j<5; j++) {
         sig2*= zcoord[i];
	 a[j]+= sig2;
	 if (j<3) beta[j]+= xcoord[i]*sig2;
      }
   }
// solve for parameters : the matrix equation a_ij * params[j] = b[j] gives
// the parameters
// we need thus the inverse of matrix a_ij. Since this matrix is positive-definite,
// we can use the stable and fast Cholesky decomposition. This is 3 times faster
// then Gauss-Jordan. See Numerical Recipes, section 2.9 
// a_ij = chol * chol_transposed, where chol is a matrix with only elements
// below and on the diagonal
// for speed, I do not use arrays but write out all elements
    double chol00 =sqrt(a[0]);
    double chol10 = a[1]/chol00;
    double chol20 = a[2]/chol00;
    double chol11 = sqrt(a[2]-chol10*chol10);
    double chol21 = (a[3]-chol10*chol20)/chol11;
    double chol22 = sqrt(a[4]-chol20*chol20-chol21*chol21);
// (You may verify that chol * chol_transpose = a_ij, with a_ij=a[i+j] )
// e.g. a_21 = chol20*chol10+chol21*chol11+chol22*chol12(=0) = a[3]
   
   // solve A_ij*params[j] = b[j]
   // first solve chol*y[j]= b[j]
   // then solve chol_transpose*p[j] = y[j]
   double yy0=beta[0]/chol00;
   double y1=(beta[1]-chol10*yy0)/chol11;
   double y2=(beta[2]-chol21*y1-chol20*yy0)/chol22;

// now solve chol_transpose*p[j] = y[j]
   params[2]=y2/chol22;
   params[1]=(y1-chol21*params[2])/chol11;
   params[0]=(yy0-chol20*params[2]-chol10*params[1])/chol00;

   // finally, provide the uncertainties on the parameters p0,p1,p2
   // replace cholesky matrix Lt with its inverse
   chol00=1/chol00;
   chol10 = -chol10*chol00/chol11; // chol00 is already updated
   chol11=1/chol11;
   chol20 = -chol20*chol00 - chol21*chol10; // note that chol10 is updated already
   chol20 /= chol22;
   chol22 = 1/chol22;
   chol21 = -chol21*chol11*chol22;
   // alpha-inverse = LT_inv * L*inv
   params[3]= chol00*chol00;
   params[4]= chol11*chol11; // variance on p1
   params[5] = chol22*chol22;
   params[6] = chol10*chol00; // p0-p1 correlation
   params[7] = chol20*chol20; // p0-p2 correlation
   params[8] = chol20*chol10+chol21*chol11; // p1-p2 correlation

// calculate chi-2
// also add the hit with the maximum deviation to the removed list, if 
// that deviation is more than 3 sigma
   chi2=0;
   if (zcoord.size() - removed.size() >3) {
      double maxdev=0;
      int removeint=-1;
      for (unsigned int i=0; i<zcoord.size(); i++) {
         bool skip=false;
         for (unsigned int j=0; (j<removed.size()) && !skip ; j++) skip=(removed[j]==i);
         if (skip) continue;
         double dev= (xcoord[i]-params[0]-params[1]*zcoord[i]-params[2]*zcoord[i]*zcoord[i])/error[i];
	 dev*=dev;
         chi2+=dev;
	 if (dev>maxdev) { 
	    maxdev=dev;
	    removeint=i;
	 }
      }
      if (maxdev>9) removed.push_back(removeint); // the hit at index removeint is more than 3 standard dev's away
      chi2=chi2/(zcoord.size()-removed.size()-2.999999999999); // for the hypothetical case that the previous statement removed the last d.o.f, I subtract 2.99999999 instead of 3.
   }
   return chi2;
}
