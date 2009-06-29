import GaudiPython
Math = GaudiPython.gbl.ROOT.Math
# neet to get this first so that all of the other stuff is understood!
# seems to work since Transform3D isn't a templated class.
Transform3D = Math.Transform3D
Plane3D     = Math.Plane3D
Rotation3D  = Math.Rotation3D
RotationZYX = Math.RotationZYX
RotationX   = Math.RotationX
RotationY   = Math.RotationY
RotationZ   = Math.RotationZ
Quaternion  = Math.Quaternion
EulerAngles = Math.EulerAngles
AxisAngle   = Math.AxisAngle
# 3D vectors and points (double precision)
XYZVector        = Math.XYZVector
XYZPoint         = Math.XYZPoint
Polar3DVector    = Math.Polar3DVector
Polar3DPoint     = Math.Polar3DPoint
RhoEtaPhiVector  = Math.RhoEtaPhiVector
RhoEtaPhiPoint   = Math.RhoEtaPhiPoint
RhoZPhiVector    = Math.RhoZPhiVector
RhoZPhiPoint     = Math.RhoZPhiPoint
# 3D vectors and points (float)
XYZVectorF        = Math.XYZVectorF
XYZPointF         = Math.XYZPointF
Polar3DVectorF    = Math.Polar3DVectorF
Polar3DPointF     = Math.Polar3DPointF
RhoEtaPhiVectorF  = Math.RhoEtaPhiVectorF
RhoEtaPhiPointF   = Math.RhoEtaPhiPointF
RhoZPhiVectorF    = Math.RhoZPhiVectorF
RhoZPhiPointF     = Math.RhoZPhiPointF
# Lorentz vectors
XYZTVector       = Math.XYZTVector      # LorentzVector<PxPyPzE4D<double> >
PxPyPzEVector    = Math.PxPyPzEVector   # LorentzVector<PxPyPzE4D<double> >
PxPyPzMVector    = Math.PxPyPzMVector   # LorentzVector<PxPyPzM4D<double> >
PtEtaPhiEVector  = Math.PtEtaPhiEVector # LorentzVector<PtEtaPhiE4D<double> >
PtEtaPhiMVector  = Math.PtEtaPhiMVector # LorentzVector<PtEtaPhiM4D<double> >
# Lorentz vectors (float)
XYZTVectorF       = Math.XYZTVectorF    # LorentzVector<PxPyPzE4D<float> >
#
Line = GaudiPython.gbl.Gaudi.Math.Line
XYZLine        = Line(XYZPoint, XYZVector)
Polar3DLine    = Line(Polar3DPoint, Polar3DVector)
RhoEtaPhiLine  = Line(RhoEtaPhiPoint, RhoEtaPhiVector)
RhoZPhiLine    = Line(RhoZPhiPoint, RhoZPhiVector)
#
XYZLineF       = Line(XYZPointF, XYZVectorF)
Polar3DLineF   = Line(Polar3DPointF, Polar3DVectorF)
RhoEtaPhiLineF = Line(RhoEtaPhiPointF, RhoEtaPhiVectorF)
RhoZPhiLineF   = Line(RhoZPhiPointF, RhoZPhiVectorF)
#
GeomFun = GaudiPython.gbl.Gaudi.Math.GF
XYZGeomFun = GeomFun(XYZPoint, XYZLine, Plane3D)
"""

# these are the real definitions.

XYZVector = Math.DisplacementVector3D('ROOT::Math::Cartesian3D<double>,ROOT::Math::DefaultCoordinateSystemTag')

XYZPoint = Math.PositionVector3D('ROOT::Math::Cartesian3D<double>,ROOT::Math::DefaultCoordinateSystemTag')

Polar3DVector = Math.DisplacementVector3D('ROOT::Math::Polar3D<double>,ROOT::Math::DefaultCoordinateSystemTag')
Polar3DPoint = Math.PositionVector3D('ROOT::Math::Polar3D<double>,ROOT::Math::DefaultCoordinateSystemTag')

RhoEtaPhiVector = Math.DisplacementVector3D('ROOT::Math::CylindricalEta3D<double>,ROOT::Math::DefaultCoordinateSystemTag')
RhoEtaPhiPoint = Math.PositionVector3D('ROOT::Math::CylindricalEta3D<double>,ROOT::Math::DefaultCoordinateSystemTag')

RhoZPhiVector = Math.DisplacementVector3D('ROOT::Math::Cylindrical3D<double>,ROOT::Math::DefaultCoordinateSystemTag')
RhoZPhiPoint = Math.PositionVector3D('ROOT::Math::Cylindrical3D<double>,ROOT::Math::DefaultCoordinateSystemTag')

# floating point types
XYZVectorF = Math.DisplacementVector3D('ROOT::Math::Cartesian3D<float>,ROOT::Math::DefaultCoordinateSystemTag')
XYZPointF = Math.PositionVector3D('ROOT::Math::Cartesian3D<float>,ROOT::Math::DefaultCoordinateSystemTag')

Polar3DVectorF = Math.DisplacementVector3D('ROOT::Math::Polar3D<float>,ROOT::Math::DefaultCoordinateSystemTag')
Polar3DPointF = Math.PositionVector3D('ROOT::Math::Polar3D<float>,ROOT::Math::DefaultCoordinateSystemTag')

RhoEtaPhiVectorF = Math.DisplacementVector3D('ROOT::Math::CylindricalEta3D<float>,ROOT::Math::DefaultCoordinateSystemTag')
RhoEtaPhiPointF = Math.PositionVector3D('ROOT::Math::CylindricalEta3D<float>,ROOT::Math::DefaultCoordinateSystemTag')

RhoZPhiVectorF = Math.DisplacementVector3D('ROOT::Math::Cylindrical3D<float>,ROOT::Math::DefaultCoordinateSystemTag')
RhoZPhiPointF = Math.PositionVector3D('ROOT::Math::Cylindrical3D<float>,ROOT::Math::DefaultCoordinateSystemTag')


"""
