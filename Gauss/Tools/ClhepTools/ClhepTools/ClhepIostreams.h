// $Id: ClhepIostreams.h,v 1.1.1.1 2005-12-13 16:40:39 jpalac Exp $
#ifndef CLHEPTOOLS_CLHEPIOSTREAMS_H 
#define CLHEPTOOLS_CLHEPIOSTREAMS_H 1

// Include files

/** Global functions ClhepIostreams.h ClhepTools/ClhepIostreams.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2005-12-13
 */
std::ostream & operator<<(std::ostream & os, const HepGeom::Transform3D & t) {
  os << "\n" << t.xx() << "  " << t.xy() << "  " << t.xz() << "  " << t.dx() ; 
  os << "\n" << t.yx() << "  " << t.yy() << "  " << t.yz() << "  " << t.dy() ; 
  os << "\n" << t.zx() << "  " << t.zy() << "  " << t.zz() << "  " << t.dz() 
     << "\n";
  return os;
}

#endif // CLHEPTOOLS_CLHEPIOSTREAMS_H
