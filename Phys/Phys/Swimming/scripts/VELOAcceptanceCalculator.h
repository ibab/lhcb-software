#include <math.h>
#include <limits>
#include <iostream>

class trackInVELOAcceptance{

	public :
		trackInVELOAcceptance(const double tx, const double ty) {
			m_tx = tx;
			m_ty = ty;
			n_z_stations = 21;
			r_min = 8.2;
			r_max = 40.;
			// Initialize the slow way
			double z_strip_A_temp[21] = { -175., -145., -115., -85., -55., -25., 5., 35., 65., 95., 125., 
							155., 185., 215., 245., 275., 435., 585., 635., 685., 735. };
			double z_strip_C_temp[21] = { -160., -130., -100., -70., -40., -10.,20., 50., 80.,110., 140., 
							170., 200., 230., 260., 300., 450., 600., 650., 700., 750. };
			for (int thisz = 0; thisz < n_z_stations; ++thisz) {
				z_strip_A[thisz] = z_strip_A_temp[thisz];
				z_strip_C[thisz] = z_strip_C_temp[thisz];				
			}
		}
		~trackInVELOAcceptance(){};

		int nHitsOnTrackInVELOAcceptance(double x, double y, double z) {
			double r = sqrt( x*x+y*y );
			std::pair<double,double> z1_min = std::make_pair(0.,0.); 
			std::pair<double,double> z1_max = std::make_pair(0.,0.);
  			int n1 = 0;
			// We need the intercept of the line coming from x, y, z, with slopes m_tx, m_ty
			// with radii r_min and r_max. There are two solutions for each radius
			//
			z1_min = solve_quad(x,y,z,r_min);
			z1_max = solve_quad(x,y,z,r_max);
			// Now, where are we?
			if (r > r_max) {
				// Are the crossing points ahead or behind us?
				if (	(z > z1_min.second) &&
					(z > z1_max.second) ) {
					// Current z value is ahead of both z values where we cross r_min/max
					// hence no hits on the VELO in the forward direction
					return n1;
				}
				// Do we cross r_min?
				if (fabs(z1_min.first+z1_min.second + 2*99999.) < std::numeric_limits<float>::epsilon() ) {
					// Do we cross the VELO at all?
					if (fabs(z1_max.first+z1_max.second + 2*99999.) < std::numeric_limits<float>::epsilon() ) {
						return n1;
					} else {
						// A/C side business is now tricky
						// For now approximate by averaging
						n1 =  n_mods_z_range( z1_max.first, z1_max.second, z_strip_A );
						n1 += n_mods_z_range( z1_max.first, z1_max.second, z_strip_C );
						n1 /= 2.;
					}
				} else {	
					// We started outside r_max so we must cross r_max to get to r_min
					if (x > 0) {
						//From A side to C side
						n1 += n_mods_z_range( z1_max.first, z1_min.first, z_strip_A );
						n1 += n_mods_z_range( z1_min.second, z1_max.second, z_strip_C );
					} else {
						//From C side to A side
						n1 += n_mods_z_range( z1_max.first, z1_min.first, z_strip_C );
						n1 += n_mods_z_range( z1_min.second, z1_max.second, z_strip_A );
					} 
				}
			} else if (r > r_min) {
				// Now we begin between r_min and r_max
				// We can either just cross r_max, or cross r_min, go through the space between
				// the sensors, cross r_min again, and finally cross r_max again
				//
				// Which direction are we going in?
				if (x*m_tx + y*m_ty > 0) { // Increasing r
					if ( x > 0 ) {
						n1 = n_mods_z_range( z, z1_max.second, z_strip_A );
					} else {
						n1 = n_mods_z_range( z, z1_max.second, z_strip_C );
					} 
				} else { // Decreasing r
					if ( x > 0 ) {
						// From A side to C side
						n1 =  n_mods_z_range( z, z1_min.first, z_strip_A );
						n1 += n_mods_z_range( z1_min.second, z1_max.second, z_strip_C );
					} else {
						// From C side to A side
						n1 =  n_mods_z_range( z, z1_min.first, z_strip_C );
						n1 += n_mods_z_range( z1_min.second, z1_max.second, z_strip_A );
					} 
				}
			} else {
				// Simplest case, we start inside r_min
				// We can only cross once ahead of us
				// Simplify by ignoring tracks which cross the A/C side
				if (( x + m_tx * ( z1_min.second - z ) ) > 0) {
					n1 = n_mods_z_range( z1_min.second, z1_max.second, z_strip_A );
				} else {
					n1 = n_mods_z_range( z1_min.second, z1_max.second, z_strip_C );
				} 
			}
			/*if (r < r_max) { 
				if (r > r_min) {
					z1_min = z;
					z1_max = z_from_radius( x, y, z, r_max );
				} else {
					z1_min = z_from_radius( x, y, z, r_min );
					z1_max = z_from_radius( x, y, z, r_max );
				}
				if (( x + m_tx * ( z1_min - z ) ) > 0) { 
					n1 = n_mods_z_range( z1_min, z1_max, z_strip_A );
				} else {  
					n1 = n_mods_z_range( z1_min, z1_max, z_strip_C );
				}	
			}*/ 
			return n1;
		}

	private :

		std::pair<double,double> solve_quad(double x0, double y0, double z0, double r1) {
			double coef_a = pow(m_tx,2) + pow(m_ty,2);
			double coef_b = 2*(x0 * m_tx + y0 * m_ty);
			double coef_c = pow(x0,2) + pow(y0,2) - pow(r1,2);
		
			if (4*coef_a*coef_c > pow(coef_b,2)) return std::make_pair(-99999.,-99999.);

			double temp = -0.5 * (coef_b + ((coef_b>0)-(coef_b<0)) * sqrt(coef_b*coef_b - 4*coef_a*coef_c));
			double x1 = temp;
			if (fabs(coef_a) > std::numeric_limits<float>::epsilon()) {
				x1 /= coef_a;
			} else {
				x1 = -99999.;
			}
			double x2 = coef_c / temp;

			// Order solutions in increasing z
			if (x1 < x2 ) return std::make_pair(x1+z0,x2+z0);
			else return std::make_pair(x2+z0,x1+z0);
			
		}

		double z_from_radius(double x0, double y0, double z0, double r1 ) {
			//returns z position where particle with direction tx, ty coming from x0, y0, z0 reaches radius r1
			double tr2 = pow(m_tx,2) + pow(m_ty,2);
			if (fabs(tr2) < std::numeric_limits<float>::epsilon()) return -1000.;
			double ratio = ( x0 * m_tx + y0 * m_ty ) / tr2;
			double rad = ratio * ratio + ( r1 * r1 - x0 * x0 - y0 * y0 ) / tr2;
			if (rad <=0) return -1000.;
			return z0 - ratio + sqrt( rad );
		}

		int n_mods_z_range(double z1, double z2, double* z_positions) {
			//returns number of modules with z coordinates between z1 and z2
			int n = 0;
			for (int thisz = 0; thisz < n_z_stations; ++thisz) {
				if ((z_positions[thisz] > z1)&&(z_positions[thisz] < z2)) ++n;
			}
			return n;
		}

		double z_strip_A[21];
		double z_strip_C[21];

		int    n_z_stations;

		double r_min;
		double r_max;
		
		double m_tx;
		double m_ty;
};
