// $Id: RichG4ReconFlatMirr.h,v 1.1 2004-02-04 13:53:01 seaso Exp $
#ifndef RICHANALYSIS_RICHG4RECONFLATMIRR_H 
#define RICHANALYSIS_RICHG4RECONFLATMIRR_H 1

// Include files

/** @class RichG4ReconFlatMirr RichG4ReconFlatMirr.h RichAnalysis/RichG4ReconFlatMirr.h
 *  
 *
 *
 *
 *  @author Sajan EASO
 *  @date   2003-09-11
 */
class RichG4ReconFlatMirr {
public:
  /// Standard constructor
  RichG4ReconFlatMirr( ); 

  virtual ~RichG4ReconFlatMirr( ); ///< Destructor
  void setRich1FlatMirrorParam( );
  void setRich2FlatMirrorParam( );
  
  HepPoint3D FlatMirrorReflect(HepPoint3D HitCoordQw , 
                                int FlatMirrorType);

protected:

private:
  
  int m_RichNumFlatMirrorOrientationParam;
  int m_RichNumFlatMirrorLocationTypes;
  
  std::vector<std::vector<double> > m_RichFlatMirrorNominalOrientation;

};
#endif // RICHANALYSIS_RICHG4RECONFLATMIRR_H
