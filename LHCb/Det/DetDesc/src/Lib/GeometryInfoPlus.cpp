// Include files

// GaudiKernel
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
// DetDesc
#include "DetDesc/IPVolume_predicates.h"
#include "DetDesc/DetDesc.h"
#include "DetDesc/AlignmentCondition.h"
// STD and STL
#include <numeric>
#include <iterator>

#include "DetDesc/GeometryInfoPlus.h"


//-----------------------------------------------------------------------------
/// Implementation file for class : GeometryInfoPlus
/// Developed from Vanya's implementation GeometryInfo.
/// Needs to use standard "ideal" positions of PVolumes
/// as defined in XML detector description AND "deltas to
/// these volumes, as given in CondDB or XML conditions
/// catalogue.
/// Will need to hold containers of transformation matrices
/// of parents in order to obtain global transformations.
///
/// 2005-04-28 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// create "ghost"
GeometryInfoPlus::GeometryInfoPlus(IDetectorElement* de)
  :
  m_log                 (       0     ),
  m_gi_has_logical      (     false   ),
  m_gi_lvolumeName      (      ""     ),
  m_gi_lvolume          (       0     ),
  m_hasAlignment        (     false   ),
  m_alignmentPath       (      ""     ),
  m_hasAlignmentPath    (     false   ),
  m_alignmentCondition  (       0     ),
  m_matrix              (       0     ),
  m_idealMatrix         (       0     ),
  m_localIdealMatrix    (       0     ),
  m_localDeltaMatrix    (       0     ),
  m_matrixInv           (       0     ),
  m_idealMatrixInv      (       0     ),
  m_gi_has_support      (    false    ),
  m_gi_supportName      (      ""     ),
  m_gi_support          (       0     ),
  m_gi_supportPath      (             ),
  m_gi_supportNamePath  (      ""     ),
  m_gi_iDetectorElement (      de     ),
  m_gi_childLoaded      (    false    ),
  m_gi_childrens        (             ),
  m_gi_childrensNames   (             ),
  m_services            (      0      )
{
  if( 0 == de  )
    { throw GeometryInfoException("IDetectorElement* points to NULL!") ; }

  if ( initialize().isFailure() )
  { throw GeometryInfoException("Failed to initialize!") ; }


}
//=============================================================================
// create orphan
GeometryInfoPlus::GeometryInfoPlus(IDetectorElement*  de,
                                   const std::string& LogVol)
  :
  m_log                 (       0     ),
  m_gi_has_logical      (    true     ),
  m_gi_lvolumeName      (   LogVol    ),
  m_gi_lvolume          (       0     ),
  m_hasAlignment        (     false   ),
  m_alignmentPath       (      ""     ),
  m_hasAlignmentPath    (     false   ),
  m_alignmentCondition  (       0     ),
  m_matrix              (       0     ),
  m_idealMatrix         (       0     ),
  m_localIdealMatrix    (       0     ),
  m_localDeltaMatrix    (       0     ),
  m_matrixInv           (       0     ),
  m_idealMatrixInv      (       0     ),
  m_gi_has_support      (    false    ),
  m_gi_supportName      (      ""     ),
  m_gi_support          (       0     ) ,
  m_gi_supportPath      (             ),
  m_gi_supportNamePath  (      ""     ),
  m_gi_iDetectorElement (      de     ),
  m_gi_parentLoaded     (    false    ),
  m_gi_parent           (      0      ),
  m_gi_childLoaded      (    false    ) ,
  m_gi_childrens        (             ) ,
  m_gi_childrensNames   (             ),
  m_services            (      0      )
{
  if( 0 == de  )
    { throw GeometryInfoException("IDetectorElement* points to NULL!"    ) ; }

  if ( initialize().isFailure() )
  { throw GeometryInfoException("Failed to initialize!") ; }

}
//=============================================================================
/// create regular  with numeric replica path
GeometryInfoPlus::GeometryInfoPlus(IDetectorElement*            de,
                                   const std::string&           LogVol,
                                   const std::string&           Support,
                                   const ILVolume::ReplicaPath& replicaPath,
                                   const std::string& alignmentPath          )
  :
  m_log                 (       0     ),
  m_gi_has_logical      (    true     ),
  m_gi_lvolumeName      (   LogVol    ),
  m_gi_lvolume          (       0     ),
  m_hasAlignment        (     false   ),
  m_alignmentPath       (alignmentPath),
  m_hasAlignmentPath    (     false   ),
  m_alignmentCondition  (       0     ),
  m_matrix              (       0     ),
  m_idealMatrix         (       0     ),
  m_localIdealMatrix    (       0     ),
  m_localDeltaMatrix    (       0     ),
  m_matrixInv           (       0     ),
  m_idealMatrixInv      (       0     ),
  m_gi_has_support      (    true     ),
  m_gi_supportName      (   Support   ),
  m_gi_support          (       0     ) ,
  m_gi_supportPath      ( replicaPath ),
  m_gi_supportNamePath  (      ""     ),
  m_gi_iDetectorElement (      de     ),
  m_gi_parentLoaded     (    false    ),
  m_gi_parent           (      0      ),
  m_gi_childLoaded      (    false    ) ,
  m_gi_childrens        (             ) ,
  m_gi_childrensNames   (             ),
  m_services            (      0      )
{
  if( 0 == de  )
    { throw GeometryInfoException("IDetectorElement* points to NULL!"    ) ; }

  if ( initialize().isFailure() )
  { throw GeometryInfoException("Failed to initialize!") ; }

}
//=============================================================================
/// create regular  with name path
GeometryInfoPlus::GeometryInfoPlus( IDetectorElement*  de,
                                    const std::string& LogVol,
                                    const std::string& Support,
                                    const std::string& ReplicaNamePath,
                                    const std::string& alignmentPath   )
  :
  m_log                 (       0     ),
  m_gi_has_logical      (    true         ),
  m_gi_lvolumeName      (   LogVol        ),
  m_gi_lvolume          (       0         ),
  m_hasAlignment        (     false       ),
  m_alignmentPath       ( alignmentPath   ),
  m_alignmentCondition  (       0         ),
  m_matrix              (       0     ),
  m_idealMatrix         (       0     ),
  m_localIdealMatrix    (       0     ),
  m_localDeltaMatrix    (       0     ),
  m_matrixInv           (       0     ),
  m_idealMatrixInv      (       0     ),
  m_gi_has_support      (    true         ),
  m_gi_supportName      (   Support       ),
  m_gi_support          (       0         ) ,
  m_gi_supportPath      (                 ),
  m_gi_supportNamePath  ( ReplicaNamePath ),
  m_gi_iDetectorElement (      de         ),
  m_gi_parentLoaded     (    false        ),
  m_gi_parent           (      0          ),
  m_gi_childLoaded      (    false        ) ,
  m_gi_childrens        (                 ) ,
  m_gi_childrensNames   (                 ),
  m_services            (      0          ){
  if( 0 == de  )
    { throw GeometryInfoException("IDetectorElement* points to NULL!"    ) ; }

  if ( initialize().isFailure() )
  { throw GeometryInfoException("Failed to initialize!") ; }

}
//=============================================================================
StatusCode GeometryInfoPlus::initialize()
{

  m_services = DetDesc::services();
  const bool create = true;
  IUpdateManagerSvc *ums = updMgrSvc(create);

  m_hasAlignmentPath=!m_alignmentPath.empty();

  if( 0 == m_log ) m_log = new MsgStream(msgSvc(), "GeometryInfoPlus");
  if( log().level() <= MSG::VERBOSE ) {
    log() << MSG::VERBOSE << "initialize" << endmsg;
    log() << MSG::VERBOSE << "alignment path " << m_alignmentPath << endmsg;
  }
  
  if ( m_hasAlignmentPath ) {
    this->hasAlignmentCondition(true);
    if( log().level() <= MSG::VERBOSE )
      log() << MSG::VERBOSE << "Using AlignmentCondition with path "
            << m_alignmentPath << endmsg;

  }
  else {
    this->hasAlignmentCondition(false);
    m_alignmentCondition=0;
    if( log().level() <= MSG::VERBOSE )
      log() << MSG::VERBOSE
            << "No AlignmentCondition requested. Assigning identity transformation"
            << endmsg;
  }

  return registerCondition() && registerSupportGI() && ums->update(this);
}
//=============================================================================
bool GeometryInfoPlus::hasLVolume() const
{
  return m_gi_has_logical;
}
//=============================================================================
bool GeometryInfoPlus::hasSupport() const
{
  return m_gi_has_support;
}
//=============================================================================
const AlignmentCondition* GeometryInfoPlus::alignmentCondition() const
{
  return m_alignmentCondition;
}
//=============================================================================
const Gaudi::Transform3D& GeometryInfoPlus::toLocalMatrix() const
{
  return  *m_matrix;
}
//=============================================================================
const Gaudi::Transform3D& GeometryInfoPlus::toGlobalMatrix() const
{
  return *m_matrixInv;
}
//=============================================================================
const Gaudi::Transform3D& GeometryInfoPlus::toLocalMatrixNominal() const
{
  return *m_idealMatrix;
}
//=============================================================================
const Gaudi::Transform3D& GeometryInfoPlus::toGlobalMatrixNominal() const
{
  return *m_idealMatrixInv;
}
//=============================================================================
Gaudi::Transform3D GeometryInfoPlus::ownMatrix() const
{
  return Gaudi::Transform3D(this->ownToNominalMatrix() * this->ownToLocalMatrixNominal() );
}
//=============================================================================
Gaudi::XYZPoint GeometryInfoPlus::toLocal( const Gaudi::XYZPoint& globalPoint ) const
{
  Gaudi::XYZPoint localPoint = toLocalMatrix() * globalPoint;
  // Due to LHCb geometry, many measurements have Z=0 in local frame
  // Next line recovers precision lost in transformations, particularly on 32-bit
  if( fabs(localPoint.Z()) < 1.e-10 ) localPoint.SetZ(0.);
  return ( localPoint );
}
//=============================================================================
Gaudi::XYZPoint GeometryInfoPlus::toGlobal( const Gaudi::XYZPoint& localPoint  ) const
{
  return ( toGlobalMatrix() * localPoint  );
}
//=============================================================================
Gaudi::XYZVector GeometryInfoPlus::toLocal( const Gaudi::XYZVector& globalDirection ) const
{
  return ( toLocalMatrix() * globalDirection );
}
//=============================================================================
Gaudi::XYZVector GeometryInfoPlus::toGlobal( const Gaudi::XYZVector& localDirection  ) const
{
  return ( toGlobalMatrix() * localDirection  );
}
//=============================================================================
bool GeometryInfoPlus::isInside( const Gaudi::XYZPoint& globalPoint ) const
{
  return ( hasLVolume() && 0 != lvolume() ) ?
    lvolume()->isInside( toLocal( globalPoint ) ) :
    isInsideDaughter( globalPoint );
}
//=============================================================================
StatusCode GeometryInfoPlus::cache()
{

  if( log().level() <= MSG::VERBOSE )
    log() << MSG::VERBOSE << "cache() calculating matrices" << endmsg;
  return calculateMatrices();

}
//=============================================================================
StatusCode GeometryInfoPlus::calculateMatrices()
{

  if( log().level() <= MSG::VERBOSE )
    log() << MSG::VERBOSE << "calculateMatrices: clearing matrices" << endmsg;
  clearMatrices();

  IGeometryInfo* gi = this;
  std::vector<IGeometryInfo*> parentGeomInfos;

  if( log().level() <= MSG::VERBOSE )
    log() << MSG::VERBOSE << "Welcome to calculateMatrices for volume "
          << gi->lvolumeName() << " support " << m_gi_supportName
          << endmsg;
  do {
    Assert( 0 != gi,
            " GeometryInfoPlus::calculateMatrices() - support is not found!");
    Assert( 0 != gi->lvolume(),
            " GeometryInfoPlus::calculateMatrices() - support has no LVolume!");

    if( log().level() <= MSG::VERBOSE )
      log() << MSG::VERBOSE << "storing matrix for volume "
            << gi->lvolumeName()
            << endmsg;

    if (!idealMatrixLoaded()) {
      if( log().level() <= MSG::VERBOSE )
        log() << MSG::VERBOSE << "store ideal..." << endmsg;
      m_pvMatrices.push_back( gi->ownToLocalMatrixNominal() );
    } else {
      if( log().level() <= MSG::VERBOSE )
        log() << MSG::VERBOSE << "ideal already stored..." << endmsg;
    }

    if( log().level() <= MSG::VERBOSE )
      log() << MSG::VERBOSE << "store delta for " << endmsg;
    m_deltaMatrices.push_back( gi->ownToNominalMatrix() );

    parentGeomInfos.push_back(gi);

      if( log().level() <= MSG::VERBOSE )
        log() << MSG::VERBOSE << "get support IGeometryInfo* for "
              << gi->lvolumeName() << endmsg;

    gi = gi->supportIGeometryInfo();

  } while (0!=gi);

  if( log().level() <= MSG::VERBOSE )
    log() << MSG::VERBOSE << "calculateMatrices: stored "
          << m_pvMatrices.size() << " ideal and "
          << m_deltaMatrices.size() << " delta matrices for volumes "
          << endmsg;

  Assert(m_pvMatrices.size()==m_deltaMatrices.size(),
         "GeometryInfoPlus::calculateMatrices: \n different number of ideal and delta matrices");

  std::vector<IGeometryInfo*>::iterator it=parentGeomInfos.begin();
  for (; it!=parentGeomInfos.end(); ++it) {
    if( log().level() <= MSG::VERBOSE )
      log() << MSG::VERBOSE << "\t" << (*it)->lvolumeName()
            << endmsg;
  }

  return combineMatrices(deltaBegin(), deltaEnd(),
                         idealBegin(), idealEnd());

}
//=============================================================================
StatusCode GeometryInfoPlus::combineMatrices(matrix_iterator deltaFirst,
                                             matrix_iterator deltaEnd,
                                             matrix_iterator pvFirst,
                                             matrix_iterator pvEnd)
{
  if ( !idealMatrixLoaded() ) calculateIdealMatrix(pvFirst, pvEnd);
  return calculateFullMatrices(deltaFirst, deltaEnd, pvFirst);
}
//=============================================================================
StatusCode GeometryInfoPlus::calculateFullMatrices(matrix_iterator deltaFirst,
                                                   matrix_iterator deltaEnd,
                                                   matrix_iterator pvFirst)
{

  Gaudi::Transform3D init;
  m_matrix =
    new Gaudi::Transform3D( std::inner_product(deltaFirst,
                                           deltaEnd,
                                           pvFirst,
                                           init,
                                           std::multiplies<Gaudi::Transform3D>(),
                                           std::multiplies<Gaudi::Transform3D>()
                                           )
                        );

  // Recover precision in cases of rotations by pi or halfpi
  double xx, xy, xz, dx, yx, yy, yz, dy, zx, zy, zz, dz;
  m_matrix->GetComponents( xx, xy, xz, dx, yx, yy, yz, dy, zx, zy, zz, dz );
  if( fabs(xx) < 1.e-15 ) xx = 0.;
  if( fabs(xy) < 1.e-15 ) xy = 0.;
  if( fabs(xz) < 1.e-15 ) xz = 0.;
  if( fabs(dx) < 1.e-12 ) dx = 0.;
  if( fabs(yx) < 1.e-15 ) yx = 0.;
  if( fabs(yy) < 1.e-15 ) yy = 0.;
  if( fabs(yz) < 1.e-15 ) yz = 0.;
  if( fabs(dy) < 1.e-12 ) dy = 0.;
  if( fabs(zx) < 1.e-15 ) zx = 0.;
  if( fabs(zy) < 1.e-15 ) zy = 0.;
  if( fabs(zz) < 1.e-15 ) zz = 0.;
  if( fabs(dz) < 1.e-12 ) dz = 0.;
  m_matrix->SetComponents( xx, xy, xz, dx, yx, yy, yz, dy, zx, zy, zz, dz );
  
  m_matrixInv=new Gaudi::Transform3D( toLocalMatrix().Inverse() );

  // Recover precision
  m_matrixInv->GetComponents( xx, xy, xz, dx, yx, yy, yz, dy, zx, zy, zz, dz );
  if( fabs(xx) < 1.e-15 ) xx = 0.;
  if( fabs(xy) < 1.e-15 ) xy = 0.;
  if( fabs(xz) < 1.e-15 ) xz = 0.;
  if( fabs(dx) < 1.e-12 ) dx = 0.;
  if( fabs(yx) < 1.e-15 ) yx = 0.;
  if( fabs(yy) < 1.e-15 ) yy = 0.;
  if( fabs(yz) < 1.e-15 ) yz = 0.;
  if( fabs(dy) < 1.e-12 ) dy = 0.;
  if( fabs(zx) < 1.e-15 ) zx = 0.;
  if( fabs(zy) < 1.e-15 ) zy = 0.;
  if( fabs(zz) < 1.e-15 ) zz = 0.;
  if( fabs(dz) < 1.e-12 ) dz = 0.;
  m_matrixInv->SetComponents( xx, xy, xz, dx, yx, yy, yz, dy, zx, zy, zz, dz );

  if( log().level() <= MSG::VERBOSE )
    log() << MSG::VERBOSE << "calculated full matrices" << endmsg;

  return StatusCode::SUCCESS;

}
//=============================================================================
void GeometryInfoPlus::calculateIdealMatrix(matrix_iterator pvFirst,
                                            matrix_iterator pvEnd)
{
  Gaudi::Transform3D init;

  m_idealMatrix =
    new Gaudi::Transform3D( std::accumulate(pvFirst,
                                        pvEnd,
                                        init,
                                        std::multiplies<Gaudi::Transform3D>()));
  m_idealMatrixInv = new Gaudi::Transform3D( m_idealMatrix->Inverse());

}
//=============================================================================
StatusCode GeometryInfoPlus::ownToOffNominalMatrix(const Gaudi::Transform3D& newDelta)
{
  // Need to do this depending on whether there is an
  // AlignmentCondition present. So check for that, and if there is,
  // also change the matrix in the AlignmentCondition data member.
  if (this->hasAlignmentCondition()) myAlignmentCondition()->offNominalMatrix(newDelta );
  return setLocalOffNominalDeltaMatrix(newDelta);
}
//=============================================================================
StatusCode GeometryInfoPlus::ownToOffNominalParams(const std::vector<double>& trans,
                                                 const std::vector<double>& rot,
                                                 const std::vector<double>& pivot)
{
  // should also make children re-calculate matrices by calling their
  // calculateMatrices methods iteratively.

  if (this->hasAlignmentCondition()) {

    return (myAlignmentCondition()->setOffNominalTransformation(trans, rot, pivot) ) ?
      setLocalOffNominalDeltaMatrix(myAlignmentCondition()->offNominalMatrix()) :
      StatusCode::FAILURE;
  }

  return StatusCode::FAILURE;

}

//=============================================================================
StatusCode GeometryInfoPlus::setLocalOffNominalDeltaMatrix(const Gaudi::Transform3D&
                                                           newDelta)
{

  // should also make children re-calculate matrices by calling their
  // calculateMatrices methods iteratively.

  if (m_deltaMatrices.empty()) {
    log() << MSG::WARNING << "setLocalDeltaMatrix set failed!" << endmsg;
    return StatusCode::FAILURE;
  }

  if( 0 != m_localDeltaMatrix ) {
    delete m_localDeltaMatrix;
    m_localDeltaMatrix=0;
  }

  if( log().level() <= MSG::VERBOSE )
    log() << MSG::VERBOSE << "updating local delta matrix" << endmsg;
  m_localDeltaMatrix = new Gaudi::Transform3D(newDelta.Inverse());
  m_deltaMatrices[0] = *m_localDeltaMatrix;

  if (!calculateFullMatrices(deltaBegin(),
                             deltaEnd(),
                             idealBegin()) ) return StatusCode::FAILURE;

  IUpdateManagerSvc *ums = updMgrSvc();
  ums->invalidate(this);
  return ums->newEvent();
}
//=============================================================================
StatusCode GeometryInfoPlus::updateChildren()
{
  for (iGInfo_iterator gi = this->childBegin(); gi!=this->childEnd(); ++gi) {
    (*gi)->cache();
    (*gi)->updateChildren();
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
void GeometryInfoPlus::clearMatrices()
{

  if (!m_pvMatrices.empty()) m_pvMatrices.clear();
  if (!m_deltaMatrices.empty()) m_deltaMatrices.clear();

  if( 0 != m_matrix ) {
    delete m_matrix;
    m_matrix = 0;
  }
  if( 0 != m_idealMatrix ) {
    delete m_idealMatrix;
    m_idealMatrix = 0;
  }

  if( 0 != m_localIdealMatrix ) {
    delete m_localIdealMatrix;
    m_localIdealMatrix = 0;
  }
  if( 0 != m_localDeltaMatrix ) {
    delete m_localDeltaMatrix;
    m_localDeltaMatrix = 0;
  }

  if( 0 != m_matrixInv ) {
    delete m_matrixInv;
    m_matrixInv = 0;
  }

  if( 0 != m_idealMatrixInv ) {
    delete m_idealMatrixInv;
    m_idealMatrixInv = 0;
  }

}
//=============================================================================
// FIXME: to be removed
StatusCode GeometryInfoPlus::getAlignmentCondition()
{
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode GeometryInfoPlus::registerCondition()
{
  if( log().level() <= MSG::VERBOSE )
    log() << MSG::VERBOSE << "registerCondition" << endmsg;
  updMgrSvc()->registerCondition(this,
                                 m_alignmentPath,
                                 &GeometryInfoPlus::cache,
                                 m_alignmentCondition);
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode GeometryInfoPlus::registerSupportGI()
{
  if( log().level() <= MSG::VERBOSE )
    log() << MSG::VERBOSE << "registerSupportGI" << endmsg;
  IGeometryInfo* gi = this->supportIGeometryInfo();

//   return (gi) ? m_ums->registerCondition(this, gi, &IGeometryInfo::cache) :
//     StatusCode::SUCCESS;
  if (gi) {
    if( log().level() <= MSG::VERBOSE )
      log() << MSG::VERBOSE << "register parent GI!" << endmsg;
    updMgrSvc()->registerCondition(this, gi, &IGeometryInfo::cache);
    if( log().level() <= MSG::VERBOSE )
      log() << MSG::VERBOSE << "Registered" << endmsg;
  } else {
    if( log().level() <= MSG::VERBOSE )
      log() << MSG::VERBOSE << "No parent " << endmsg;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
const Gaudi::Transform3D& GeometryInfoPlus::ownToLocalMatrixNominal() const
{

  if (0!=m_localIdealMatrix) {
    if( log().level() <= MSG::VERBOSE )
      log() << MSG::VERBOSE << "Ideal matrix for " << m_gi_supportNamePath
            << " already present" << endmsg;
    return *m_localIdealMatrix;
  }

  IGeometryInfo* gi = supportIGeometryInfo();

  if( ( 0==gi || !hasLVolume()) || !hasSupport() ||
      ( this == ( const IGeometryInfo*) gi ) ) {
    if( log().level() <= MSG::VERBOSE )
      log() << MSG::VERBOSE << "localIdealMatrix: assigning identity matrix "
            << endmsg;
    m_localIdealMatrix = new Gaudi::Transform3D();
  } else {

    const ILVolume* lv = gi->lvolume();

    ILVolume::PVolumePath volumePath;

    StatusCode sc = lv->traverse( supportPath().begin(),
                                  supportPath().end(),
                                  volumePath );

    Assert( sc.isSuccess()  ,
            " GeometryInfoPlus::localIdealMatrix(): could not traverse the path!");
    m_localIdealMatrix =  accumulateMatrices(volumePath);
  }

  return  *m_localIdealMatrix;

}
//=============================================================================
const Gaudi::Transform3D& GeometryInfoPlus::ownToNominalMatrix() const
{
  if (0!=m_localDeltaMatrix) return *m_localDeltaMatrix;

  m_localDeltaMatrix = (this->hasAlignmentCondition())      ?
    new Gaudi::Transform3D(myAlignmentCondition()->toNominalMatrix()) :
    new Gaudi::Transform3D();

  return *m_localDeltaMatrix;

}
//=============================================================================
Gaudi::Transform3D GeometryInfoPlus::ownToOffNominalMatrix() const
{
  return ownToNominalMatrix().Inverse();
}
//=============================================================================
IGeometryInfo* GeometryInfoPlus::supportIGeometryInfo() const
{

  if ( !hasSupport() ) return 0;
  if( log().level() <= MSG::VERBOSE )
    log() << MSG::VERBOSE << "supportIGeometryInfo with support "
          << m_gi_supportName << endmsg;

  if ( 0 == m_gi_support ) {
    IGeometryInfo* gi = 0;
    if( std::string("") != m_gi_supportName ) {
      gi = geoByName( m_gi_supportName );
    }
    m_gi_support=gi;
  }

  if( log().level() <= MSG::VERBOSE ) {
    if (0!=m_gi_support) {
      log() << MSG::VERBOSE
            << "supportIGeometryInfo return IGeometryInfo* with volume "
            << m_gi_support->lvolumeName();
    } else {
       log() << MSG::VERBOSE
             << "supportIGeometryInfo found no IGeometryInfo* with support "
             << m_gi_supportName << endmsg;
    }
  }
  
  return  m_gi_support;

}
//=============================================================================
Gaudi::Transform3D* GeometryInfoPlus::accumulateMatrices(const ILVolume::PVolumePath& volumePath) const
{
  Gaudi::Transform3D init, return_value;
  return_value=std::accumulate( volumePath.begin() ,
                                volumePath.end  () ,
                                init               ,
                                IPVolume_accumulateMatrix() );
  return new Gaudi::Transform3D(return_value);

}

//=============================================================================
const ILVolume::ReplicaPath& GeometryInfoPlus::supportPath() const
{
  if( log().level() <= MSG::VERBOSE )
    log() << MSG::VERBOSE << "supportPath is " << m_gi_supportName << endmsg;
  if(!m_gi_has_support||!m_gi_supportPath.empty()) {
    return m_gi_supportPath;
  }


  // try to extract the replica path from its name (parse the name path)
  if( 0 == m_gi_support ) { m_gi_support = geoByName( m_gi_supportName ) ; }
  ///
  const ILVolume* lv = m_gi_support->lvolume();
  std::string aux( m_gi_supportNamePath );
  while( 0 != aux.size() && 0 != lv )
    {
      // find separator
      std::string::size_type pos     = aux.find_first_of('/');
      std::string            physVol ("") ;
      if( std::string::npos == pos )
        { physVol  = aux                   ; aux=""                       ; }
      else
        { physVol = aux.substr( 0 , pos )  ; aux = aux.substr( pos + 1  ) ; }
      if( 0 != physVol.size() )
        {
          // get physical volume by name
          const IPVolume* pv = (*lv)[ physVol ];
          // find this name in the sequence
          ILVolume::PVolumes::const_iterator it =
            std::find( lv->pvBegin() , lv->pvEnd() , pv ) ;
          Assert( lv->pvEnd() != it ,
                  "GeometryInfoPlus:: error during retrieve of Replica Path" );
          // extract index and put it into replica path

          m_gi_supportPath.push_back( it - lv->pvBegin() );
          lv = (*it)->lvolume();
        }
    }
  //
  return m_gi_supportPath;
  //
}
//=============================================================================
IGeometryInfo* GeometryInfoPlus::geoByName( const std::string& name ) const
{
  if( log().level() <= MSG::VERBOSE )
    log() << MSG::VERBOSE << "geoByName name is " << name << endmsg;

  IGeometryInfo* gi = 0;
  SmartDataPtr<IDetectorElement> ptr( dataSvc() , name );
  if ( 0 != ptr ) {
    gi = ptr->geometry();
    if( log().level() <= MSG::VERBOSE )
      log() << MSG::VERBOSE << "geoByName: Found detector element for "
            << name << endmsg;
  }


  return gi;
}
//=============================================================================
StatusCode GeometryInfoPlus::fullGeoInfoForPoint
( const Gaudi::XYZPoint&        point      ,
  const int                level      ,
  IGeometryInfo*&          start      ,
  ILVolume::PVolumePath&   volumePath )
{
  // reset output values
  start = 0         ;
  if( !volumePath.empty() )            { volumePath.clear();          }
  if( !isInside( point )  )            { return StatusCode::FAILURE ; }
  //
  IGeometryInfo* gi = belongsTo( point , -1 );
  if( 0 == gi || 0 == gi->lvolume() )  { return StatusCode::FAILURE ; }
  start = gi;
  //
  return
    gi->lvolume()->belongsTo( gi->toLocal( point )  ,
                              level , volumePath );
}
//=============================================================================
StatusCode GeometryInfoPlus::fullGeoInfoForPoint
( const Gaudi::XYZPoint&        point      ,
  const int                level      ,
  std::string&             start      ,
  ILVolume::PVolumePath&   volumePath )
{
  // reset output values
  start = std::string("");
  if( !volumePath.empty() ) { volumePath.clear()       ; }
  if( !isInside( point ) ) { return StatusCode::FAILURE; }
  //
  IGeometryInfo* gi = belongsTo    ( point , -1 );
  if( 0 == gi || 0 == gi->lvolume() ) { return StatusCode::FAILURE ; }
  start             = belongsToPath( point , -1 );
  //
  return
    gi->lvolume()->belongsTo( gi->toLocal( point ) ,
                              level , volumePath );
  //
}
//=============================================================================
StatusCode GeometryInfoPlus::fullGeoInfoForPoint
( const Gaudi::XYZPoint&        point      ,
  const int                level      ,
  IGeometryInfo*&          start      ,
  ILVolume::ReplicaPath&   replicaPath )
{
  // reset output values
  start = 0         ;
  if( !replicaPath.empty() )            { replicaPath.clear();         }
  if( !isInside( point )   )            { return StatusCode::FAILURE ; }
  //
  IGeometryInfo* gi = belongsTo( point , -1 );
  if( 0 == gi || 0 == gi->lvolume() )   { return StatusCode::FAILURE ; }
  start = gi;
  //
  return
    gi->lvolume()->belongsTo( gi->toLocal( point )  ,
                              level , replicaPath );
}
//=============================================================================
StatusCode GeometryInfoPlus::fullGeoInfoForPoint
( const Gaudi::XYZPoint&        point      ,
  const int                level      ,
  std::string&             start      ,
  ILVolume::ReplicaPath&   replicaPath )
{
  // reset output values
  start = std::string("");
  if( !replicaPath.empty() ) { replicaPath.clear()       ; }
  if( !isInside( point )   ) { return StatusCode::FAILURE; }
  //
  IGeometryInfo* gi = belongsTo    ( point , -1 );
  if( 0 == gi || 0 == gi->lvolume() ) { return StatusCode::FAILURE ; }
  start             = belongsToPath( point , -1 );
  //
  return
    gi->lvolume()->belongsTo( gi->toLocal( point ) ,
                              level , replicaPath );
}
//=============================================================================
std::string GeometryInfoPlus::belongsToPath( const Gaudi::XYZPoint& globalPoint )
{
  if( !isInside( globalPoint )                     )
    { return std::string(""); }
  if( !childLoaded() && loadChildren().isFailure() )
    { return std::string(""); }
  //
  IGeometryInfo::IGIChildrens::const_iterator it  =  ///< look through children
    std::find_if( childBegin() , childEnd  () ,
                  IGeometryInfo_isInside(globalPoint) ) ;
  //
  return
    ( childEnd() == it ) ? std::string("") :
    *(m_gi_childrensNames.begin()+(it-childBegin()));
}
//=============================================================================
std::string GeometryInfoPlus::belongsToPath( const Gaudi::XYZPoint& globalPoint,
                                             const int         level       )
{
  if( 0 == level ){  return detElem()->name() ; }
  IGeometryInfo* gi =  belongsTo( globalPoint );
  return
    ( 0 == gi ) ? detElem()->name() :
    gi->belongsToPath( globalPoint , level - 1 ) ;
  //
}
//=============================================================================
IGeometryInfo* GeometryInfoPlus::belongsTo( const Gaudi::XYZPoint& globalPoint )
{
  if( !isInside( globalPoint )                     ) { return 0; }
  if( !childLoaded() && loadChildren().isFailure() ) { return 0; }
  //
  IGeometryInfo::IGIChildrens::const_iterator it  =
    std::find_if( childBegin() , childEnd  () ,
                  IGeometryInfo_isInside(globalPoint) ) ;
  //
  return ( childEnd() == it ? 0 : *it );
}
//=============================================================================
IGeometryInfo* GeometryInfoPlus::belongsTo( const Gaudi::XYZPoint& globalPoint ,
                                            const int         level       )
{
  if( level == 0 ) { return this;  } // do not look throug  daughters!
  IGeometryInfo* gi = belongsTo( globalPoint );
  return  ( ( 0 == gi ) ? this : gi->belongsTo( globalPoint , level - 1 ) );
}
//=============================================================================
StatusCode GeometryInfoPlus::loadChildren() const
{
  ///
  m_gi_childrensNames.clear();
  m_gi_childrens.clear();
  m_gi_childLoaded = false;
  ///
  Assert( 0 != m_gi_iDetectorElement,
          "GeometryInfoPlus::loadChildren IDetectorElement is not availble!");
  ///
  IDetectorElement::IDEContainer& IDEchildrens =
    m_gi_iDetectorElement->childIDetectorElements();
  IDetectorElement::IDEContainer::iterator it = IDEchildrens.begin();
  while( IDEchildrens.end() != it )
    {
      IDetectorElement* ide = *it++;
      try{
        IGeometryInfo* igi   = 0 ;
        DataObject*    pObj  = 0 ;
        std::string    path("")  ;
        if( 0 != ide  ) { igi  = ide->geometry();                }
        if( 0 != ide  ) { pObj = dynamic_cast<DataObject*>(ide); }
        if( 0 != pObj ) {
          IRegistry* pReg = pObj->registry();
          if (0!=pReg) path=pReg->identifier();
        }
        if( 0 != igi &&  0 != path.size() )
          { m_gi_childrensNames.push_back( path );
          m_gi_childrens.push_back( igi  ); }
      }
      catch( const GaudiException& Exception )
        {  Assert( false ,
                   "GeometryInfoPlus::loadChildren(): exception caught!" ,
                   Exception ) ; }   ///  throw new exception
      catch(...)
        {  Assert( false ,
                   "GeometryInfoPlus::loadChildren(): unknown exception caught!") ;
        }   ///  throw new exception
    }
  //
  m_gi_childLoaded = true;
  //
  return StatusCode::SUCCESS;
}
//=============================================================================
IGeometryInfo*  GeometryInfoPlus::reset()
{
  /// reset logical volume
  m_gi_lvolume = 0;

  /// reset matrices
  if( 0 != m_matrix ) {
    delete m_matrix;
    m_matrix    = 0 ;
  }

  if( 0 != m_matrixInv ) {
    delete m_matrixInv ;
    m_matrixInv = 0 ;
  }

  /// reset support
  m_gi_support      = 0      ;

  /// reset parent
  m_gi_parentLoaded = false  ;
  m_gi_parent       = 0      ;
  /// reset daughters
  std::for_each( m_gi_childrens.begin () ,
                 m_gi_childrens.end   () ,
                 std::mem_fun(&IGeometryInfo::reset) );
  m_gi_childrens.clear()      ;
  m_gi_childrensNames.clear() ;
  m_gi_childLoaded  = false   ;
  ///
  return this;
}
//=============================================================================
IGeometryInfo* GeometryInfoPlus::parentIGeometryInfo()
{
  if( !hasSupport() )  { return 0; }
  if( log().level() <= MSG::VERBOSE )
    log() << MSG::VERBOSE << "parentIGeometryInfo with support "
          << m_gi_supportName << endmsg;

  if( 0 == m_gi_support )
  {
    IGeometryInfo* gi = 0;
    if( std::string("") != m_gi_supportName )
    { gi = geoByName( m_gi_supportName ); }
    if( 0 !=  gi ) { m_gi_support = gi ; }
  }
  return m_gi_support;
}
//=============================================================================
const IGeometryInfo* GeometryInfoPlus::parentIGeometryInfo() const
{
  if( m_gi_parentLoaded ) { return m_gi_parent; }
  Assert( 0 != m_gi_iDetectorElement ,
          "GeometryInfoPlus:: IDetectorElement is not available!" );

  IDetectorElement* ide =
    m_gi_iDetectorElement->parentIDetectorElement();
  Assert( 0 != ide                   ,
          "GeometryInfoPlus:: parent           is not available!" );
  const IGeometryInfo* igi = ide->geometry();
  Assert( 0 != igi                   ,
          "GeometryInfoPlus:: parent geometry  is not available!" );
  return igi;
}
//=============================================================================
const ILVolume* GeometryInfoPlus::lvolume() const
{
  if( !hasLVolume() )     { return  0              ; }
  if( 0 != m_gi_lvolume ) { return  m_gi_lvolume          ; }
  try
  { m_gi_lvolume  = findLogical() ; }
  catch ( const GaudiException Exception )
    { Assert( false ,
              "GeometryInfoPlus::lvolume() : exception caught" , Exception ) ; }
  catch (...)
    { Assert( false ,
              "GeometryInfoPlus::lvolume() : unknown exception caught" ) ; }
  ///
  Assert( 0 != m_gi_lvolume ,
          "GeometryInfoPlus::lvolume() : unable to load ILVolume!" );
  return m_gi_lvolume;
}
//=============================================================================
StatusCode GeometryInfoPlus::location( IGeometryInfo*& start ,
                                   ILVolume::ReplicaPath& replicaPath ) const
{
  replicaPath.clear();

  replicaPath.insert( replicaPath.end() ,
                      supportPath().begin() , supportPath().end() );

  if( !hasSupport() )  { start = 0; return StatusCode::SUCCESS; }
  //
  if( 0 == m_gi_support )
    {
      IGeometryInfo* gi = 0;
      if( std::string("") != m_gi_supportName )
        { gi = geoByName( m_gi_supportName ); }
      if( 0 !=  gi ) { m_gi_support = gi ; }
    }

  start = m_gi_support;
  if( 0 == start ) { replicaPath.clear(); }

   return  ( ( 0 != start ) ? StatusCode::SUCCESS : StatusCode::FAILURE );
}
//============================================================================
StatusCode GeometryInfoPlus::location( std::string& start ,
                                   ILVolume::ReplicaPath& replicaPath ) const
{
  start = m_gi_supportName;
  replicaPath.clear();
  replicaPath.insert( replicaPath.end() ,
                      supportPath().begin() , supportPath().end() );

  return StatusCode::SUCCESS;
}
//============================================================================
const ILVolume* GeometryInfoPlus::lvolume( IGeometryInfo* start,
                                       const ILVolume::ReplicaPath& replicaPath )
{
  if( 0 == start || 0 == start->lvolume() ){ return 0; }
  //
  const ILVolume* lv = start->lvolume();
  ILVolume::PVolumePath volumePath;
  if( lv->traverse( replicaPath.begin()  ,
                    replicaPath.end  ()  ,
                    volumePath           ).isFailure() ) { return 0; }
  //
  if( volumePath.empty() ) { return lv; }
  //
  const IPVolume* pv = *(volumePath.rbegin()); // get "the last" element
  //
  return ( 0 == pv ? 0 : pv->lvolume() );
}
//=============================================================================
std::string GeometryInfoPlus::lvolumePath( IGeometryInfo* start,
                                           const ILVolume::ReplicaPath&
                                           replicaPath )
{
  // starting point for address is invalid!
  if( 0 == start || 0 == start->lvolume() ){ return std::string("") ; }
  //
  const ILVolume* lv = start->lvolume();
  ILVolume::PVolumePath volumePath;
  if( lv->traverse( replicaPath.begin()  ,
                    replicaPath.end  ()  ,
                    volumePath           ).isFailure() ) { return 0; }
  //
  if( volumePath.empty() ) { return start->lvolumeName(); }
  //
  const IPVolume* pv = *(volumePath.rbegin()); // get "the last" element
  //
  return ( 0 == pv ? std::string("") : pv->lvolumeName() );
}
//=============================================================================
const ILVolume* GeometryInfoPlus::lvolume( const std::string& start,
                                           const ILVolume::ReplicaPath&
                                           replicaPath )
{
  if( std::string("") == start ) { return 0; }
  //
  IGeometryInfo* gi = geoByName( start );
  //
  return 0 == gi ? 0 : lvolume( gi , replicaPath );
}
//=============================================================================
std::string GeometryInfoPlus::lvolumePath( const std::string& start,
                                           const ILVolume::ReplicaPath&
                                           replicaPath )
{
  if( std::string("") == start ) { return std::string(""); }
  //
  IGeometryInfo* gi = geoByName( start );
  //
  return 0 == gi ? std::string("") : lvolumePath( gi , replicaPath );
}
//=============================================================================
// StatusCode  GeometryInfoPlus::reset( const int /*Level*/ )
// {
//   this->reset();
//   return StatusCode::SUCCESS;
// }
//=============================================================================

//=============================================================================
std::ostream& GeometryInfoPlus::printOut( std::ostream& os ) const
{
  try {
    os << "GeometryInfoPlus::name=" <<  detElem()->name() << ";" ;
    if( !hasLVolume() ) { os << " no LVolume associated ('ghost');"; }
    else
      {
        os << " LVolume=" << lvolumeName() << ";";
        if( !hasSupport() ) { os << "('orphan');"; }
        else
          {
            os << " supported by " << m_gi_supportName << " with ReplicaPath=(";
            std::copy( m_gi_supportPath.begin() ,
                       m_gi_supportPath.end() ,
                       std::ostream_iterator<ILVolume::ReplicaType>(os,",") );
            os << ") ";
            os << "(" << m_gi_supportNamePath << ");";
          }
      }
  } catch (...) {
    os << " !!! Unable to print GeometryInfo out !!!";
  }
  //
  return os << std::endl;
}
//=============================================================================
MsgStream& GeometryInfoPlus::printOut( MsgStream& os ) const
{
  try {
    os << "GeometryInfoPlus::name=" << detElem()->name() << ";" ;
    if (!hasLVolume()) {
      os << " no LVolume associated ('ghost');";
    } else {
      os << " LVolume=" << lvolumeName() << ";";
      if (!hasSupport()) {
        os << "('orphan');";
      } else {
        os << " supported by " << m_gi_supportName << " with ReplicaPath=(";
        ILVolume::ReplicaPath::const_iterator ci = supportPath().begin();
        while( supportPath().end() != ci ) { os << *ci++ << "," ; }
        os << ") ";
        os << "(" << m_gi_supportNamePath << ");";
      }
    }
  } catch (...) {
    os << " !!! Unable to print GeometryInfo out !!!";
  }
  //
  return os << endmsg ;
}
//=============================================================================
ILVolume* GeometryInfoPlus::findLogical() const
{
  ILVolume* lv = 0;
  if( !hasLVolume() ) { return lv; }
  //
  try
    {
      SmartDataPtr<ILVolume> ptr( dataSvc() , lvolumeName() );
      if( 0 != ptr ) { lv = ptr ; }
    }
  catch ( const GaudiException& Exception )
    {  Assert( false ,
               "GeometryInfoPlus::findLogical(), exception in retrieving LV=" +
               lvolumeName() , Exception ); }   ///  throw new exception
  catch (...)
    {  Assert( false ,
               "GeometryInfoPlus::findLogical(), exception in retrieving LV=" +
               lvolumeName() ); }   ///  throw new exception
  ///
  Assert( 0 != lv ,
          "GeometryInfoPlus::findLogical(), unable to retrieve LV=" +
          lvolumeName() ) ;
  ///
  return lv;
}
//=============================================================================
StatusCode GeometryInfoPlus::queryInterface( const InterfaceID& ID,
                                             void** ppI )
{
  if ( 0 == ppI ) { return StatusCode::FAILURE; }
  *ppI = 0 ;
  if      ( IGeometryInfo::interfaceID()  == ID )
    { *ppI = static_cast<IGeometryInfo*> ( this ) ; }
  else if ( IInterface:: interfaceID()    == ID )
    { *ppI = static_cast<IInterface*>    ( this ) ; }
  else
    { return StatusCode::FAILURE                  ; }
  /// add the reference
  addRef();
  ///
  return StatusCode::SUCCESS;
}
//=============================================================================
// Destructor
//=============================================================================
GeometryInfoPlus::~GeometryInfoPlus()
{
  //
  m_gi_lvolume = 0 ;
  //
  clearMatrices();
  //
  m_gi_support = 0;
  //
  m_gi_childLoaded = false;
  m_gi_childrensNames.clear();
  m_gi_childrens.clear();
  //
  if( 0 != m_log ) {
    delete m_log;
    m_log = 0;
  }

  updMgrSvc()->unregister(this);
  m_services->release();
}
//=============================================================================
