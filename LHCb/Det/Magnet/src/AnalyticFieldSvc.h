#ifndef ANALYTICFIELDSVC_H
#define ANALYTICFIELDSVC_H 1

// Include files
#include "GaudiKernel/Service.h"

#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "MagVec.h"
#include "MagMat.h"

// Forward declarations
template <class TYPE> class SvcFactory;


/** @class AnalyticFieldSvc AnalyticFieldSvc.h
 *  
 *  
 *
 *  @authors Geraldine Conti, Adlene Hicheur
 *  @date   2007-03
 */



class AnalyticFieldSvc : public Service,
                         virtual public IMagneticFieldSvc {

protected:
  
  /// Standard Constructor.
  /// @param  name   String with service name
  /// @param  svc    Pointer to service locator interface
  AnalyticFieldSvc( const std::string& name, ISvcLocator* svc );
  
  /// Virtual destructor.
  virtual ~AnalyticFieldSvc();

public:

  /// Initialise the service (Inherited Service overrides) 
  virtual StatusCode initialize();
  
  /// Finalise the service.
  virtual StatusCode finalize();
  
  /** Query the available interfaces.
   * @param riid Requested interface ID
   * @param ppvInterface Pointer to requested interface
   * @return StatusCode indicating SUCCESS or FAILURE.
   */
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface      );
  
  /** IAnalyticFieldSvc interface.
   * @param[in]  xyz Point at which magnetic field vector will be given
   * @param[out] fvec Magnectic field vector.
   * @return StatusCode SUCCESS if calculation was performed.
   */
  virtual StatusCode fieldVector( const Gaudi::XYZPoint&  xyz, 
                                        Gaudi::XYZVector& fvec ) const;

private:

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<AnalyticFieldSvc>;

  /// Fills Q, the field vector
  //  void fieldGrid( const Gaudi::XYZPoint& xyz, Gaudi::XYZVector& fvec ) const;

  StatusCode GetParam();       ///< Reads the field map parameterizations from file 
void Bcalculation(const Gaudi::XYZPoint&  point, 
		   Gaudi::XYZVector& bf ) const; ///< Method for field calculation depending on the region
 double EvaluateField(MagVec& pos, MagMat& bmap) const; ///< Compute the field analytically from the parameterization

  void ClearMaps();
  
  
  std::string m_filename[3];           ///< Analytic field file names
  std::vector<MagMat*> Bxmap;       ///< Parameterization vector for Bx map
  std::vector<MagMat*> Bymap;       ///< Parameterization vector for By map
  std::vector<MagMat*> Bzmap;       ///< Parameterization vector for Bz map

  std::vector<double> zmin;           ///< Vector containing the lower z-limit of each region
  std::vector<double> zmax;           ///< Vector containing the upper z-limit of each region

  double m_zOffSet;              ///< The z offset
  

  bool                m_useConstField;    ///< Job option to use constant field
  std::vector<double> m_constFieldVector; ///< Option for constant field value
  double              m_scaleFactor;      ///< Option for field scaling factor

  int m_nREG,m_nREGmin,m_nREGmax;
  
};

#endif  // ANALYTICFIELDSVC_H
