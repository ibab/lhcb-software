// $Id: VisPrimVertTool.h,v 1.2 2006-10-11 14:19:20 jpalac Exp $
#ifndef VISPRIMVERTTOOL_VISPRIMVERTTOOL_H 
#define VISPRIMVERTTOOL_VISPRIMVERTTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// local 
#include "Kernel/IVisPrimVertTool.h"

//class IParticlePropertySvc;
/** @class VisPrimVertTool VisPrimVertTool.h VisPrimVertTool.h
 *  
 *  v1.3
 *  @author Christian Jacoby (LPHE, EPFL, Lausanne)
 *  @date   2004-03-18
 */
class VisPrimVertTool : public GaudiTool, virtual public IVisPrimVertTool {
public:
  /// Standard constructor
  VisPrimVertTool( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent );

  virtual ~VisPrimVertTool( ){}; ///< Destructor

  
  /// Initialize
  StatusCode initialize();


  long countVertices() const ;
  bool isVisible(const LHCb::MCVertex* pv )const {
    return (countVisTracks(pv) >= m_nTracks) ; };
  bool isB( const LHCb::MCVertex* )const ;
  long countVisTracks( const LHCb::MCVertex* )const ;

protected:

private:

  long m_nTracks;
  bool m_veloAndSeed;
  //  IParticlePropertySvc* m_ppSvc;  ///<  Reference to Particle Property Service

  };
#endif // VISPRIMVERTTOOL_VISPRIMVERTTOOL_H
