// $Id: VisPrimVertTool.h,v 1.2 2007-05-29 08:48:16 cattanem Exp $
#ifndef VISPRIMVERTTOOL_VISPRIMVERTTOOL_H 
#define VISPRIMVERTTOOL_VISPRIMVERTTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from LHCb 
#include "MCInterfaces/IVisPrimVertTool.h"

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

  };
#endif // VISPRIMVERTTOOL_VISPRIMVERTTOOL_H
