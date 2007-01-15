#ifndef CaloUtils_CaloMomentum_H
#define CaloUtils_CaloMomentum_H 1

// Include files
#include <vector>
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "Event/CaloPosition.h"
//RecEvent
#include "Event/RecVertex.h"
#include "Event/ProtoParticle.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"


namespace LHCb 
{

  /** @class CaloMomentum CaloMomentum.h
   *
   * 
   */
  
  class CaloMomentum
  {
  public:
    

    /// 4-momentum (px,py,pz,e)
    typedef Gaudi::LorentzVector Vector;
    ///  3D point (x0,y0,z0) 
    typedef Gaudi::XYZPoint      Point;
    /// 4x4 Covariance matrix (px,py,pz,e)
    typedef Gaudi::SymMatrix4x4 MomCovariance;
    /// 3x3 Covariance matrix (x0,y0,z0)
    typedef Gaudi::SymMatrix3x3 PointCovariance;
    /// 4x3 pos-mom  covariance matrix off-diagonal block 
    typedef Gaudi::Matrix4x3 MomPointCovariance;
    /// vector of CaloPositions
    typedef std::vector<LHCb::CaloPosition> CaloPositions;
    
    /// indices to access Px,Py,Pz,E
    enum Index{ Px = 0,
                Py = 1,
                Pz = 2,
                E  = 3 };

    enum Parameter{ Momentum = 1,
                     Covariance=2};

    enum Status{ OK = 0,
                 NullCaloCluster=1,
                 NullCaloPos=2,
                 NullCaloHypo=4,
                 NullProtoPart=8,
                 ChargedProtoPart=16,
                 EmptyHypoRef=32,
                 MultipleHypoRef=64,
                 NullPart=128,
                 ChargedParticle=256,
                 PartIsNotCaloObject=512
    };
    
    enum Flag{ Empty=0,
               MomentumEvaluated=1,
               CovarianceEvaluated=2,
               FromCaloCluster=4,
               FromCaloPos=8,
               FromCaloHypoRef=16,
               FromCaloHypo=32,
               FromProtoPart=64,
               FromPartDaughters=128,
               FromPart=256,
               ParticleUpdated = 512,
               ParticleTreeUpdated = 1024,
               NewPointCovariance=2048,
               NewReferencePoint=4096
  };   
    /// Default Constructor
    CaloMomentum() : m_caloPositions(),
                     m_point(), m_pointCovMatrix(),
                     m_momentum(), m_momCovMatrix(),
                     m_momPointCovMatrix(),
                     m_status(LHCb::CaloMomentum::OK),
                     m_flag(LHCb::CaloMomentum::Empty){}
    


    // Special constructors
    CaloMomentum(const LHCb::CaloPosition* calo );
    CaloMomentum(const LHCb::CaloPosition* calo , const Point point);
    CaloMomentum(const LHCb::CaloPosition* calo , const Point point, const PointCovariance cov);
    CaloMomentum(const LHCb::ProtoParticle* proto);
    CaloMomentum(const LHCb::ProtoParticle* proto, const Point point, const PointCovariance cov);
    CaloMomentum(const LHCb::ProtoParticle* proto, const Point point);
    CaloMomentum(const LHCb::CaloHypo* hypo);
    CaloMomentum(const LHCb::CaloHypo* hypo, const Point point);
    CaloMomentum(const LHCb::CaloHypo* hypo, const Point point, const PointCovariance cov);
    CaloMomentum(const LHCb::CaloCluster* cluster);
    
    
    /// Default Destructor
    virtual ~CaloMomentum() {}
    
    
    // Getters
    CaloPositions&        caloPositions()  { return m_caloPositions;   }
    Point&                referencePoint() { return m_point;           };
    PointCovariance&      pointCovMatrix() { return m_pointCovMatrix;  };
    int                   status()         { return m_status;          }; 
    int                   flag()           { return m_flag;            };
    const Vector               momentum();
    const MomCovariance        momCovMatrix();
    const MomPointCovariance   momPointCovMatrix();

        
    // Setters
    void setCaloPositions(const CaloPositions value){ m_caloPositions = value;}
    void resetCaloPositions(){m_caloPositions.clear();}
    //
    void addCaloPosition(const LHCb::CaloPosition*    pos);
    void addCaloPosition(const LHCb::CaloCluster*    clus);
    void addCaloPosition(const LHCb::CaloHypo*       hypo);
    void addCaloPosition(const LHCb::ProtoParticle*  proto);
    //
    void setReferencePoint(const Point point);
    void setReferencePoint(const Point point, const PointCovariance cov);
    void setPosCovMatrix(const PointCovariance cov); 
    void setReferencePoint(const LHCb::RecVertex* vertex);
    //
    void addToStatus(Status status) { m_status |= status;   };
    void addToFlag(Flag flag) { m_flag |= flag;            };
    // Evaluators
    void evaluate(int param);
    void evaluate(){ this->evaluate(Momentum & Covariance);  }
    // Shortcuts
    const double e(){return momentum().E() ; }
    const double px(){return momentum().Px() ; }
    const double py(){return momentum().Py() ; }
    const double pz(){return momentum().Pz() ; }
    const double pt(){return momentum().Pt() ; }
    const double mass(){return momentum().M() ; }
    const double emass();
    
    
      

  protected:
    
  private:
    
    CaloPositions      m_caloPositions;
    Point              m_point;
    PointCovariance    m_pointCovMatrix;
    Vector             m_momentum;   
    MomCovariance      m_momCovMatrix; 
    MomPointCovariance m_momPointCovMatrix;
    int m_status;
    int m_flag;
    
  }; // class CaloMomentum
} // namespace LHCb;
// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------


inline const LHCb::CaloMomentum::Vector LHCb::CaloMomentum::momentum() 
{
  this->evaluate( LHCb::CaloMomentum::Momentum );
  return m_momentum;
}

inline const LHCb::CaloMomentum::MomCovariance LHCb::CaloMomentum::momCovMatrix() 
{
  this->evaluate(LHCb::CaloMomentum::Covariance);
  return m_momCovMatrix;
} 

inline const LHCb::CaloMomentum::MomPointCovariance LHCb::CaloMomentum::momPointCovMatrix() 
{
  this->evaluate(LHCb::CaloMomentum::Covariance);
  return m_momPointCovMatrix;
} 

inline void LHCb::CaloMomentum::setReferencePoint(const Point point)
{
  m_point = point; 
  this->addToFlag( LHCb::CaloMomentum::NewReferencePoint);
} ;

inline void LHCb::CaloMomentum::setPosCovMatrix(const PointCovariance cov)
{
  m_pointCovMatrix= cov;
  this->addToFlag( LHCb::CaloMomentum::NewPointCovariance);
} ;

inline void LHCb::CaloMomentum::setReferencePoint(const Point point, const PointCovariance cov)
{ 
  m_point = point; m_pointCovMatrix= cov;
  this->addToFlag( LHCb::CaloMomentum::NewReferencePoint);
  this->addToFlag( LHCb::CaloMomentum::NewPointCovariance);
} 

inline void LHCb::CaloMomentum::setReferencePoint(const LHCb::RecVertex* vertex)
{ 
  m_point = vertex->position(); 
  m_pointCovMatrix= vertex->covMatrix();
  this->addToFlag( LHCb::CaloMomentum::NewReferencePoint);
  this->addToFlag( LHCb::CaloMomentum::NewPointCovariance);
} 

#endif //CaloUtils_CaloMomentum_H
