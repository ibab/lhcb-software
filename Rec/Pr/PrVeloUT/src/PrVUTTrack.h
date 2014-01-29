#ifndef PRVUTTRACK_H
#define PRVUTTRACK_H 1

// Include files
#include "Event/Track.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "PrKernel/PrUTHit.h"


  /** @class PrVUTTrack PrVUTTrack.h
   *  Holds the candidate track while building it.
   *  Based on the original implementation by Olivier Callot.
   *
   *  @author Luis Fernandez
   *  @date   2005-07-27
   *  Taken over by Mariusz Witek and modified to have single version on-line off-line
   *  @date   2006-10-07
   */

  class PrVUTTrack {

  public:

    /** @class LocalHit PrVUTTrack.h
     *
     *  Hit for PrVUTTrack
     *
     *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
     *  @date   2007-06-01
     */

    class LocalHit {
    public:

      LocalHit() : m_hit(0) {};

      LocalHit( PrUTHit* hit ) {
        m_hit = hit;
        m_x = hit->x();
        m_z = hit->z();
      }

      ~LocalHit () { } ;

      PrUTHit* hit()   { return m_hit; }
      float x() const { return m_x; }
      float z() const { return m_z; }

      class increasingX  {
      public:
        bool operator() (const LocalHit& first, const LocalHit& second ) const {
          return first.x() < second.x() ;
        }
      };

      class lowerBoundX  {
      public:
        bool operator() (const LocalHit& first, const double value ) const {
          return first.x() < value ;
        }
      };

    private:
      PrUTHit* m_hit;
      float m_x;
      float m_z;


    };

    typedef std::vector<LocalHit> LocalHits;
    typedef std::vector<LocalHit>::iterator LocalHitIterators;

  public:
    /// Standard constructor
    PrVUTTrack( LHCb::Track* tr ) {
      m_track  = tr;
      m_fittrack  = 0;
      const LHCb::State& trState = tr->hasStateAt(LHCb::State::EndVelo) ?
        *(tr->stateAt(LHCb::State::EndVelo)) :
        (tr->closestState(LHCb::State::EndVelo)) ;

      m_origin = trState.position( );
      m_slope  = trState.slopes();
      m_list.reserve(4);
      m_clusters.reserve(4); // max of 1 per layer
      m_allHits.resize(4);
      for(int i = 0; i<4 ; i++){
        m_allHits[i].reserve(30);
      }
      m_fourLayerSolution = false;
      m_qOverP = 0.;
      m_fitFlag = 0;
      m_chi2PerDoF=0.;
      m_maskInUTRegionDeadZones = 0;
      m_maskUTLayersFired = 0;
      m_nUTLayersFired = 0;
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

    virtual ~PrVUTTrack( ) {}; ///< Destructor

    LHCb::Track* track()       const { return m_track; }

    LHCb::Track* fitTrack()       const { return m_fittrack; }
    void setFitTrack(LHCb::Track* pTr)       { m_fittrack = pTr; }

    double xAtZ( double z ) const {
      return m_origin.x() + m_slope.x() * ( z - m_origin.z() ) ;
    }

    double yAtZ( double z ) const {
      return m_origin.y() + m_slope.y() * ( z - m_origin.z() ) ;
    }
    
    void storeHit( PrUTHit* hit ,double planeCode) {
      LocalHit dum( hit );
      m_allHits[planeCode].push_back( dum );
    }

    void storeHit( PrUTHit* hit ) {
      LocalHit dum( hit );
      m_list.push_back( dum );
    }

    double slopeY() const { return m_slope.y(); }

    void setUTRegionDeadZonesMask(int maskInUTRegionDeadZones) { m_maskInUTRegionDeadZones = maskInUTRegionDeadZones; }

    int UTRegionDeadZonesMask() const { return m_maskInUTRegionDeadZones; }

    void setUTLayersFiredMask(int maskUTLayersFired) { m_maskUTLayersFired = maskUTLayersFired; }

    int UTLayersFiredMask() const { return m_maskUTLayersFired; }

    int nUTLayersFired() const { return m_nUTLayersFired; }

    void setNUTLayersFired(int maskUTLayersFired) { m_nUTLayersFired = m_LUT[maskUTLayersFired];}

    void storeClusters(){
      for(LocalHits::iterator iLocalHits = m_list.begin(); iLocalHits != m_list.end(); iLocalHits++){
        m_clusters.push_back((*iLocalHits).hit());
      }
    }

    PrUTHits& clusters() { return m_clusters;}

    void setBadCandidate(bool badCandidate){ m_badCandidate = badCandidate;}

    bool badCandidate() const { return m_badCandidate;}

    void setUnique(bool unique){ m_unique = unique;}

    bool unique() const { return m_unique;}

    void setChi2PerDoF(double chi2) { m_chi2PerDoF = chi2; }
    double chi2PerDoF() { return m_chi2PerDoF; }

    void setQOverP(double qp) { m_qOverP = qp; }
    double qOverP() { return m_qOverP; }

    void setFitFlag(int flag) { m_fitFlag = flag; }
    double fitFlag() { return m_fitFlag; }
    
    //=========================================================================
    // Form clusters
    //=========================================================================

    void formClusters(std::vector<LocalHits> & clusters, double tol, bool forward){

      if(!forward){
        std::reverse(m_allHits.begin(),m_allHits.end());
      }

      LocalHits clusterCandidate;
      clusterCandidate.reserve(4);

      // Loop over First Layer
      LocalHitIterators ilayer0_end =  m_allHits[0].end();
      for(LocalHitIterators ilayer0 = m_allHits[0].begin(); ilayer0_end != ilayer0 ; ++ilayer0){
              
        float xhitLayer0 = ilayer0->x();
        float zhitLayer0 = ilayer0->z();
        
        // Loop over Second Layer
        LocalHitIterators ilayer1_end =  m_allHits[1].end();
        for(LocalHitIterators ilayer1 = m_allHits[1].begin(); ilayer1_end != ilayer1 ; ++ilayer1){
          
          float xhitLayer1 = ilayer1->x();
          float zhitLayer1 = ilayer1->z();
          float tx = (xhitLayer1 - xhitLayer0)/(zhitLayer1 - zhitLayer0);

          if(fabs(tx)>0.3) continue;

          clusterCandidate.clear();
          clusterCandidate.push_back(*ilayer0);
          clusterCandidate.push_back(*ilayer1);
          
          //Find upper and lower bounds of tolerance
          float zhitLayer2 = (*(m_allHits[2].begin())).z();
          float xextrapLayer2 = xhitLayer1 + tx*(zhitLayer2-zhitLayer1);
          
          // Loop over Third Layer
          LocalHitIterators ilayer2 =  
            std::lower_bound(m_allHits[2].begin(),m_allHits[2].end(),xextrapLayer2-tol,LocalHit::lowerBoundX());
          LocalHitIterators ilayer2_end = m_allHits[2].end();

          while(ilayer2!= ilayer2_end && (*ilayer2).x() < xextrapLayer2+tol){
            
            clusterCandidate.push_back(*ilayer2);
            if(!m_fourLayerSolution){  
              clusters.push_back(clusterCandidate);
            }

            //Find upper and lower bounds of tolerance
            float zhitLayer3 = (*(m_allHits[3].begin())).z();
            float tx4 = ((*ilayer2).x()-(*ilayer1).x())/((*ilayer2).z()-(*ilayer1).z());
            float xextrapLayer3 = xhitLayer1 + tx4*(zhitLayer3-zhitLayer1);
            
            // Loop over Fourth Layer
            LocalHitIterators ilayer3 =  
              std::lower_bound(m_allHits[3].begin(),m_allHits[3].end(),xextrapLayer3-tol/2.0,LocalHit::lowerBoundX());
            LocalHitIterators ilayer3_end = m_allHits[3].end();
              
            while(ilayer3!= ilayer3_end && (*ilayer3).x() < xextrapLayer3+tol/2.0){
              
              if(!m_fourLayerSolution){  
                m_fourLayerSolution = true;
                clusters.pop_back();
              }
              
              clusterCandidate.push_back(*ilayer3);
              clusters.push_back(clusterCandidate);
              clusterCandidate.pop_back();
              
              ++ilayer3;
            }//layer3
            clusterCandidate.pop_back();
            
            ++ilayer2;
          }//layer2
          
          // Loop over Fourth Layer
          if(!m_fourLayerSolution){ 
            
            //Find upper and lower bounds of tolerance
            float zhitLayer3 = (*(m_allHits[3].begin())).z();
            float xextrapLayer3 = xhitLayer1 + tx*(zhitLayer3-zhitLayer1);
            
            LocalHitIterators ilayer3 =  
              std::lower_bound(m_allHits[3].begin(),m_allHits[3].end(),xextrapLayer3-tol,LocalHit::lowerBoundX());
            LocalHitIterators ilayer3_end = m_allHits[3].end();

            while(ilayer3!= ilayer3_end && (*ilayer3).x() < xextrapLayer3+tol){
              
              clusterCandidate.push_back(*ilayer3);
              clusters.push_back(clusterCandidate);
              clusterCandidate.pop_back();
              
              ++ilayer3;
            }//layer3
          }//!m_fourLayerSolution
        }//layer1
      }//layer0
    }//form clusters
    
    
    //=========================================================================
    // Select the best list of sorted hits...
    //=========================================================================
    void bestLists(double tol,
                   std::vector<PrUTHits> & hitsSolutions, 
                   IMessageSvc* msgSvc, std::string source){
      
      MsgStream msg = MsgStream( msgSvc, source );

      for(unsigned int i = 0;i<m_allHits.size();i++){  
        std::sort(m_allHits[i].begin(),m_allHits[i].end(),LocalHit::increasingX()); 
      }
      
      std::vector<LocalHits>forwardClusters;
      forwardClusters.reserve(100);
      formClusters(forwardClusters,tol,true);

      if(!m_fourLayerSolution){
        std::vector<LocalHits>backwardClusters;
        backwardClusters.reserve(50);
        formClusters(backwardClusters,tol,false);
        forwardClusters.insert( forwardClusters.end(), backwardClusters.begin(), backwardClusters.end() );
      }
      
      std::vector<LocalHits>::iterator iLocalHits;
      for(iLocalHits = forwardClusters.begin(); iLocalHits != forwardClusters.end(); ++iLocalHits){
   
        PrUTHits hitsCandidate;
        hitsCandidate.reserve(4);
        
        LocalHits::iterator iSub;
        for(iSub = (*iLocalHits).begin(); iSub != (*iLocalHits).end(); ++iSub){
          hitsCandidate.push_back((*iSub).hit());
        }
        
        hitsSolutions.push_back(hitsCandidate);
      }
    } // bestLists

    

  protected:

    

  private:
    LHCb::Track*  m_track;
    LHCb::Track*  m_fittrack;
    Gaudi::XYZPoint m_origin;
    Gaudi::XYZVector m_slope;
    LocalHits m_list;
    std::vector<LocalHits> m_allHits;
    PrUTHits m_clusters;
    bool m_fourLayerSolution;
    double m_chi2PerDoF;
    double m_qOverP;
    int m_fitFlag; // 0 = no fit, 1 = p from dx, 2 = simple fit, 3 = Kalman
    int m_maskInUTRegionDeadZones;
    int m_maskUTLayersFired;
    int m_nUTLayersFired;
    bool m_badCandidate;
    bool m_unique;
    std::vector<int> m_LUT;

  };

#endif // PRVUTTRACK_H

