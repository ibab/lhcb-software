#ifndef PATVTTTRACK_H
#define PATVTTTRACK_H 1

// Include files
#include "Event/Track.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "PatKernel/PatTTHit.h"


  /** @class PatVTTTrack PatVTTTrack.h
   *  Holds the candidate track while building it.
   *  Based on the original implementation by Olivier Callot.
   *
   *  @author Luis Fernandez
   *  @date   2005-07-27
   *  Taken over by Mariusz Witek and modified to have single version on-line off-line
   *  @date   2006-10-07
   */

  class PatVTTTrack {

  public:

    /** @class LocalHit PatVTTTrack.h
     *
     *  Hit for PatVTTTrack
     *
     *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
     *  @date   2007-06-01
     */

    class LocalHit {
    public:

      LocalHit() : m_hit(0) {};

      LocalHit( PatTTHit* hit, double dist ) {
        m_hit = hit;
        m_dist = dist;
        m_inFourLayersSol = false;
      }

      ~LocalHit () { } ;

      double distance() const { return m_dist; }
      PatTTHit* hit()        { return m_hit; }

      void setInFourLayersSolution(bool inFour) { m_inFourLayersSol = inFour; }
      bool inFourLayersSolution() const { return m_inFourLayersSol; }

      class increasingByDistance  {
      public:
        bool operator() (const LocalHit& first, const LocalHit& second ) const {
          return fabs(first.distance()) < fabs(second.distance()) ;
        }
      };

    private:
      PatTTHit*  m_hit;
      double  m_dist;
      bool    m_inFourLayersSol;
    };

    typedef std::vector<LocalHit> LocalHits;
    typedef std::vector<LocalHit>::iterator LocalHitIterators;

  public:
    /// Standard constructor
    PatVTTTrack( const LHCb::Track* tr ) {
      m_track  = tr;
      m_fittrack  = 0;
      const LHCb::State & trState = *(tr->stateAt(LHCb::State::EndVelo));
      m_origin = trState.position( );
      m_slope  = trState.slopes();
      m_list.reserve(100);
      m_clusters.reserve(8); // max of 2 per layer
      m_dx = 0.;
      m_dxvar = 0.;
      m_qOverP = 0.;
      m_fitFlag = 0;
      m_chi2PerDoF=0.;
      m_maskInTTRegionDeadZones = 0;
      m_maskTTLayersFired = 0;
      m_nTTLayersFired = 0;
      m_badCandidate = false;
      m_unique = false;

      // LUT between mask and number of layers fired.
      // E.g. m_LUT[14] <-> 1110 -> 3 layers
      m_LUT.reserve(16);
      m_LUT[15] = 4;
      m_LUT[14] = m_LUT[13] = m_LUT[11] = m_LUT[7] = 3;
      m_LUT[12] = m_LUT[10] = m_LUT[9] = m_LUT[6] = m_LUT[5] = m_LUT[3] = 2;
      m_LUT[8] = m_LUT[4] = m_LUT[2] = m_LUT[1] = 1;
      m_LUT[0] = 0;

    };

    virtual ~PatVTTTrack( ) {}; ///< Destructor

    const LHCb::Track* track()       const { return m_track; }

    LHCb::Track* fitTrack()       const { return m_fittrack; }
    void setFitTrack(LHCb::Track* pTr)       { m_fittrack = pTr; }

    double xAtZ( double z ) const {
      return m_origin.x() + m_slope.x() * ( z - m_origin.z() ) ;
    }

    double yAtZ( double z ) const {
      return m_origin.y() + m_slope.y() * ( z - m_origin.z() ) ;
    }

    void storeHit( double dist, PatTTHit* hit ) {
      LocalHit dum( hit, dist );
      m_list.push_back( dum );
    }

    void sortHits() {
      std::sort( m_list.begin(), m_list.end(), LocalHit::increasingByDistance() );
    }

    void setDx(double dx) { m_dx = dx; }

    double slopeY() const { return m_slope.y(); }

    double Dx() const { return m_dx; }

    void setDxVar(double dxvar) { m_dxvar = dxvar; }

    double DxVar() const { return m_dxvar; }

    void setTTRegionDeadZonesMask(int maskInTTRegionDeadZones) { m_maskInTTRegionDeadZones = maskInTTRegionDeadZones; }

    int TTRegionDeadZonesMask() const { return m_maskInTTRegionDeadZones; }

    void setTTLayersFiredMask(int maskTTLayersFired) { m_maskTTLayersFired = maskTTLayersFired; }

    int TTLayersFiredMask() const { return m_maskTTLayersFired; }

    int nTTLayersFired() const { return m_nTTLayersFired; }

    void setNTTLayersFired(int maskTTLayersFired) { m_nTTLayersFired = m_LUT[maskTTLayersFired];}

    void storeClusters(){
      for(LocalHits::iterator iLocalHits = m_list.begin(); iLocalHits != m_list.end(); iLocalHits++){
        m_clusters.push_back((*iLocalHits).hit());
      }
    }

    PatTTHits& clusters() { return m_clusters;}
    const PatTTHits& clusters()const { return m_clusters;}

    void setBadCandidate(bool badCandidate){ m_badCandidate = badCandidate;}

    bool badCandidate() const { return m_badCandidate;}

    void setUnique(bool unique){ m_unique = unique;}

    bool unique() const { return m_unique;}

    void setChi2PerDoF(double chi2) { m_chi2PerDoF = chi2; }
    double chi2PerDoF() const { return m_chi2PerDoF; }

    void setQOverP(double qp) { m_qOverP = qp; }
    double qOverP() const { return m_qOverP; }

    void setFitFlag(int flag) { m_fitFlag = flag; }
    double fitFlag() const { return m_fitFlag; }

    //=========================================================================
    // Select the best list of sorted hits...
    //=========================================================================
    void bestLists(double tol, double tol_factor,
                   std::vector<PatTTHits> & hitsSolutions, 
                   IMessageSvc* msgSvc, std::string source, bool isDebug ){

      // The vector of local hits for bestLists
      std::vector<std::vector<LocalHitIterators> > LocalHitsLists;
      LocalHitsLists.reserve(10); // reserve in case of many solutions for this Velo track

      // Vector of local hits without more than 2 on the same layer:
      std::vector<LocalHitIterators> myLocalHits;
      myLocalHits.reserve(8); // max of 2 per layer

      // Iterators: begin and end
      LocalHitIterators itB, itE;
      LocalHitIterators itStoreB, itStoreE;
      itB = itE = m_list.begin();

      // Layers 0, 1, 2, 3 and stations 4 (TTa), 5 (TTb)
      std::vector<LocalHitIterators> stationsLocalHits[6];

      int planeCode;
      int maskPlanes;
      int nLayersFired;

      while(itB!= m_list.end()){

        // set tolerances. They depend on distance (i.e. momentum).

        // tolerance for the grouping of clusters in different stations
        double globalTol      = tol        + tol_factor*fabs(itB->distance());

        // tolerance for the grouping of clusters in the same station
        double sameStationTol = tol * 0.5  + tol_factor*fabs(itB->distance()) * 0.33333;

        // tolerance for the grouping of clusters on the same layer
        double  sameLayerTol   = tol * 0.25;

        // Ignore LocalHits already used in a four layers fired solution
        //      if(!itB->inFourLayersSolution())

        // Take all. Better for off-line MW
        if( 1 ){

          // Find the LocalHits
          itE = itB;

          for(int i = 0; i< 6; ++i) stationsLocalHits[i].clear();

          maskPlanes = 0;

          bool foundSolution = false;
          nLayersFired = 0;

          while( itE != m_list.end() ){

            // Global tolerance. Hits are sorted, no need for fabs!
            //   if(globalTol < itE->distance() - itB->distance()) break;

            // Global tolerance. Hits are sorted using fabs
            // for off-line construct all combinations. MW
            double db = itB->distance();
            double de = itE->distance();
            double dist = fabs(itE->distance() - itB->distance());
            if(db*de>0.0 && globalTol < dist) break;
            if(db*de<0.0) {
              itE++;
              continue;
            }
            // Layer and station to which this hit belongs
            planeCode = itE->hit()->planeCode();

            // Same station TTa or TTb tolerance
            if(planeCode<2){ // TTa
              if(!stationsLocalHits[4].empty()){
                if(sameStationTol < itE->distance() - stationsLocalHits[4].front()->distance()){
                  break;
                } // tolerance
              }
            }
            else{ // TTb
              if(!stationsLocalHits[5].empty()){
                if(sameStationTol < itE->distance() - stationsLocalHits[5].front()->distance()){
                  break;
                } // tolerance
              }
            }

            // Same layer tolerances and maximum of 2 compatible clusters on same layer
            int n = stationsLocalHits[planeCode].size();
            // Ignore solution if already 2 clusters on this layer
            if(n >= 2) break;

            if(n > 0){
              if(sameLayerTol < itE->distance() - stationsLocalHits[planeCode].front()->distance()){
                break;
              } // tolerance
            }

            // Fill lists
            stationsLocalHits[planeCode].push_back(itE); // layers
            if(planeCode<2) stationsLocalHits[4].push_back(itE); // TTa
            else stationsLocalHits[5].push_back(itE); // TTb

            // Require 3 or 4 planes to be fired until no more compatible clusters
            // -> the solution with 4 planes if any is found first
            maskPlanes |= itE->hit()->mask();

            if(m_LUT[maskPlanes] >= 3){

              // Passes all tolerances and has enough clusters
              foundSolution = true;
              itStoreB = itB;
              itStoreE = itE;

            } // if at least three planes fired

            ++itE;
          } // itE

          if(foundSolution){

            nLayersFired = m_LUT[maskPlanes];

            if(isDebug){
              MsgStream msg = MsgStream( msgSvc, source );
              msg << MSG::DEBUG << "Found a solution with nLayersFired: " << nLayersFired
                  << " with first hit dist: "
                  << format(" %6.2f(%1d) ", itStoreB->distance(), itStoreB->hit()->planeCode())
                  << " , last hit dist: "
                  << format(" %6.2f(%1d) ", itStoreE->distance(), itStoreE->hit()->planeCode())
                  << endmsg;
            }

            // List of local hits. Not more than 2 on the same layer. 
            // TODO. Decrease to 1 per layer. 2 per layer is historical for TT with overlaping sensors.
            myLocalHits.clear();

            std::vector<LocalHitIterators>::const_iterator it;
            for (int j=0; j!=4; ++j ){
                for (it = stationsLocalHits[j].begin(); it != stationsLocalHits[j].end(); ++it){
                    
                    if(nLayersFired == 4) (*it)->setInFourLayersSolution(true);
                    myLocalHits.push_back(*it);
                }
            }

            LocalHitsLists.push_back(myLocalHits);
          } // foundSolution
        } // if(!itB->inFourLayersSolution()){

        ++itB;
      } // itB

      if(isDebug){
        MsgStream msg = MsgStream( msgSvc, source );
        msg << MSG::DEBUG << "Number of solutions: " << LocalHitsLists.size() << endmsg;
      }

      // Now create the different PatTTHit combinations and add them to the vector of solutions
      std::vector<std::vector<LocalHitIterators> >::iterator iLocalHitsLists;
      for(iLocalHitsLists = LocalHitsLists.begin(); iLocalHitsLists != LocalHitsLists.end(); ++iLocalHitsLists){

        // The compatible clusters made from these LocalHits
        PatTTHits hitsCandidate;
        hitsCandidate.reserve(8);

        std::vector<LocalHitIterators>::iterator iSub;
        for(iSub = (*iLocalHitsLists).begin(); iSub != (*iLocalHitsLists).end(); ++iSub){
          hitsCandidate.push_back((*iSub)->hit());
        }

        if(isDebug){
          MsgStream msg = MsgStream( msgSvc, source );
          msg << MSG::DEBUG << "  with hits: ";
          for(iSub = (*iLocalHitsLists).begin(); iSub != (*iLocalHitsLists).end(); ++iSub){
            msg << format(" %6.2f(%1d) ", (*iSub)->distance(), (*iSub)->hit()->planeCode());
          }
          msg << endmsg;
        }

        // Add all the solutions
        hitsSolutions.push_back(hitsCandidate);
      }
    } // bestLists

    void printLists( MsgStream& msg ) {

      LocalHitIterators itL;

      /*
        msg << endmsg << "Hits : ";
        for ( itL = m_list.begin(); m_list.end() != itL; itL++ ) {
        LHCb::STChannelID id = (*itL).hit()->lhcbID().stID();
        }
      */

      int ll = 10;
      msg << std::endl << "Hits : ";
      for ( itL = m_list.begin(); m_list.end() != itL; itL++ ) {
        double dist = (*itL).distance();
        msg << format("%6.2f(%1d) ", dist, (*itL).hit()->planeCode());
        --ll;
        if ( 0 == ll ) {
          msg << std::endl << "       ";
          ll = 10;
        }
      }
      msg << endmsg;
    }

  protected:

  private:
    const LHCb::Track*  m_track;
    LHCb::Track*  m_fittrack;
    Gaudi::XYZPoint m_origin;
    Gaudi::XYZVector m_slope;
    LocalHits m_list;
    PatTTHits m_clusters;
    double m_dx;
    double m_dxvar;
    double m_chi2PerDoF;
    double m_qOverP;
    int m_fitFlag; // 0 = no fit, 1 = p from dx, 2 = simple fit, 3 = Kalman
    int m_maskInTTRegionDeadZones;
    int m_maskTTLayersFired;
    int m_nTTLayersFired;
    bool m_badCandidate;
    bool m_unique;
    std::vector<int> m_LUT;

  };

#endif // PATVTTTRACK_H

