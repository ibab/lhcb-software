#include <math.h>
#include <limits>
#include <vector>
#include <utility>
#include <map>
#include <string>
#include <VELOAcceptanceCalculator.h>
#include <iostream>

class SwimVELOAcceptance{
	public :
		SwimVELOAcceptance(	double swimgranularity,
					double stepreduction,
					int    nsteps,
					double swimmin,
					double swimmax,
					double x, double y, double z, 
					double px, double py, double pz,
					double tx, double ty ){

			// Initialize the variables
			m_swimgranularity = swimgranularity;
			m_stepreduction = stepreduction;
			m_nsteps = nsteps;
			m_swimmin = swimmin;
			m_swimmax = swimmax;

			m_minhits["Hlt1"]    = 10;
			//m_minhits["Hlt2"]    = 6;
			m_minhits["Offline"] = 6;

			m_parent_x  = x;
			m_parent_y  = y;
			m_parent_z  = z;
			m_parent_px = px;
			m_parent_py = py;
			m_parent_pz = pz;
			m_child_tx  = tx;
			m_child_ty  = ty;

			m_veloAccHelper = new trackInVELOAcceptance(m_child_tx,m_child_ty);
		}
		~SwimVELOAcceptance(){};
		
		// Helper routines
		void setHitLimits(std::string level, int minhits_level){
			m_minhits[level]    = minhits_level;
		}
		// The main code
		std::map< std::string, std::vector< std::pair<double,bool> > > getTurningPoints() {
			// Begin by getting the rough turning points
			m_roughtps = m_getRoughTurningPoints();
			// Now refine them
			double currentgranularity = m_swimgranularity;
			for (int refinement_step = 0; refinement_step < m_nsteps; ++refinement_step) {
				currentgranularity /= m_stepreduction;
				for (std::map< std::string, int >::iterator level = m_minhits.begin(); level != m_minhits.end(); ++level) {
					m_roughtps[(*level).first] = m_refineTurningPoints(m_roughtps[(*level).first],currentgranularity,(*level).first);
				}
			}
			return m_roughtps;
		}
	private :
		// The rough TP loop
		std::map< std::string, std::vector< std::pair<double,bool> > > m_getRoughTurningPoints() {
			std::map< std::string, std::vector< std::pair<double,bool> > > roughtps; roughtps.clear();
			for (double swimstep = m_swimmax; swimstep > (m_swimmin-m_swimgranularity); swimstep -= m_swimgranularity) {
				double parent_x = m_parent_x + swimstep*m_parent_px;
				double parent_y = m_parent_y + swimstep*m_parent_py;
				double parent_z = m_parent_z + swimstep*m_parent_pz;

				int nhits = 2.*m_veloAccHelper->nHitsOnTrackInVELOAcceptance(parent_x,parent_y,parent_z);

				//cout << "Hits on track are " << nhits << " " << parent_x << " " << parent_y << " " << parent_z << endl;
	
				for (std::map< std::string, int >::iterator level = m_minhits.begin(); level != m_minhits.end(); ++level) {
					bool passed = false;
					if (nhits >= (*level).second) passed = true;	

					if (	(fabs(swimstep-m_swimmin) < std::numeric_limits<float>::epsilon() ) || 
						(fabs(swimstep-m_swimmax) < std::numeric_limits<float>::epsilon() ) ) {
						roughtps[(*level).first].push_back(std::make_pair(-1.*swimstep,passed));
					} else if (passed != roughtps[(*level).first].back().second) {
						roughtps[(*level).first].push_back(std::make_pair(-1.*swimstep,passed));
					}
					//cout << (*level).first << " " << passed << " " << roughtps[(*level).first].back().second << endl;
				}
			}
			for (std::map< std::string, std::vector< std::pair<double,bool> > >::iterator 	level = roughtps.begin(); 
													level != roughtps.end(); 
													++level) {
				/*cout << (*level).first << " rough turning points" << endl;
				for (std::vector< std::pair<double,bool> >::iterator 	iter = (*level).second.begin(); 
											iter != (*level).second.end(); 
											++iter) {
       			 		cout << (*iter).first << " " << (*iter).second << endl;
      				}
				cout << "=========================" << endl;*/
			}
			return roughtps;
		}
		// The refinement loop
		std::vector< std::pair<double,bool> > m_refineTurningPoints(	std::vector< std::pair<double,bool> > tpstorefine,
									    	double currentgranularity,
										std::string level) {
			// Special case : do not refine an empty turning point array
			if (tpstorefine.size() == 2) return tpstorefine;
			
			std::vector< std::pair<double,bool> > roughtps; roughtps.clear();
			roughtps.push_back(tpstorefine.front());	
			for (unsigned int thistp = 1; thistp < tpstorefine.size()-1; thistp++) {
				double swimmin = tpstorefine[thistp].first + currentgranularity*m_stepreduction;
				double swimmax = tpstorefine[thistp].first - currentgranularity*m_stepreduction;
				//cout << swimmin << " " << swimmax << endl;
				for (double swimstep = -1*swimmax; swimstep > -1*swimmin; swimstep -= currentgranularity) {
					double parent_x = m_parent_x + swimstep*m_parent_px;
					double parent_y = m_parent_y + swimstep*m_parent_py;
					double parent_z = m_parent_z + swimstep*m_parent_pz;

					int nhits = 2.*m_veloAccHelper->nHitsOnTrackInVELOAcceptance(parent_x,parent_y,parent_z);

					//cout << nhits << " " << parent_x << " " << parent_y << " " << parent_z << endl;
					bool passed = false;
					if (nhits >= m_minhits[level]) passed = true;
	
					if (passed != roughtps.back().second) {
						roughtps.push_back(std::make_pair(-1.*swimstep,passed));
					}
				}
			}
			roughtps.push_back(tpstorefine.back());
			return roughtps;
		}
		// The various variables
		double m_swimgranularity;
		double m_stepreduction;
		int    m_nsteps;
		double m_swimmax;
		double m_swimmin;

		std::map< std::string, int > m_minhits;

		double m_parent_x,m_parent_y,m_parent_z;
		double m_parent_px,m_parent_py,m_parent_pz;
		double m_child_tx,m_child_ty;

		// The main vector of turning points to be built
		std::map< std::string, std::vector< std::pair<double,bool> > > m_roughtps;
		//std::vector< std::pair<double,bool> > m_finaltps;

		// The helper class for the VELO geometry calculations
		trackInVELOAcceptance* m_veloAccHelper; 
};
