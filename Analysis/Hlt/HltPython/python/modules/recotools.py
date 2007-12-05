# =============================================================================
""" @namespace recotools
@brief Functions that deal with reconstructed objects, like IP and vertex computations
@author Diego Martinez diego.martinez.santos@cern.ch
@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
@author Antonio Maria Perez-Calero Yzquierdo aperez@ecm.ub.es
@date 2007-11-28
"""
# =============================================================================


import math
import gaudimodule
import kintools

DEBUG = False

#---------------------------------------------------
def rIP(track,vertex):
    """ compute radial impact paramter
    NOTE: assumes that tracks in a 3D track!
    """
    state = track.firstState()
    tx = state.tx()
    ty = state.ty()
    xt = state.x()
    yt = state.y()
    zt = state.z()
    if (DEBUG): print " track  : x,y,z,tx,ty ",xt,yt,zt,tx,ty

    pos = vertex.position()
    xv = pos.x()
    yv = pos.y()
    zv = pos.z()
    if (DEBUG): print " vertex : x,y,z ",xv,yv,zv
    
    xt = xt+tx*(zv-zt)
    yt = yt+ty*(zv-zt)
    zt = zv
    rt = math.sqrt(xt*xt+yt*yt)
    tr = math.sqrt(tx*tx+ty*ty)
    cphi = xt/rt
    sphi = yt/rt
    rv = xv*cphi+yv*sphi
    if (DEBUG): print " track' : x,y,z,tx,ty ",xt,yt,zt,tx,ty
    if (DEBUG): print " track' : cphi,sphi,rt,tr ",cphi,sphi,rt,tr
    if (DEBUG): print " vertex': x,y,z,rv ",xv,yv,zv,rv

    Dr = rt-rv
    Dz = zt-zv
    dz = -(Dr*tr+Dz)/(1+tr*tr)
    RR = Dr+tr*dz
    ZZ = Dz+dz
    ip1 = -RR/sqrt(1+tr*tr)
    ip2 = math.sqrt(RR*RR+ZZ*ZZ)
    if (ZZ<0): ip1 = -1.*ip1
    if (ZZ>0): ip2 = -1.*ip2
    if (DEBUG): print " Dr,Dz,dz",Dr,Dz,dz
    if (DEBUG): print " RR,ZZ",RR,ZZ
    if (DEBUG): print " -- IP1 -- ",ip1
    if (DEBUG): print " -- IP2 -- ",ip2
    return ip2

#---------------------------------------------------
def IP(track,vertex):
    """ compute impact parameter
    """ 
    state = track.firstState()
    xt = state.x()
    yt = state.y()
    zt = state.z()
    tx = state.tx()
    ty = state.ty()
    qp = state.qOverP()
    pt = track.pt()
    pv = vertex.position()
    xv = pv.x()
    yv = pv.y()
    zv = pv.z()
    lbda = 1./math.sqrt(1.+tx*tx+ty*ty);
    if (DEBUG): print " track: x,y,z,tx,ty ",xt,yt,zt,tx,ty
    if (DEBUG): print " track: qp,pt,lambda ",qp,pt,lbda
    if (DEBUG): print " vertex: x,y,z ",xv,yv,zv

    xt = xt+tx*(zv-zt)
    yt = yt+ty*(zv-zt)
    zt = zv
    if (DEBUG): print " track': x,y,z,tx,ty ",xt,yt,zt,tx,ty

    dx   = xt - xv;
    dy   = yt - yv;
    dz   = zt - zv;
    if (DEBUG): print " Dz,Dy,Dz ",dx,dy,dz
  
    xx =  dy    - dz*ty;
    yy = -dx    + dz*tx;
    zz =  dx*ty - dy*tx;
    if (DEBUG): print " xx,yy,zz ",xx,yy,zz
  
    ip = lbda*math.sqrt(xx*xx+yy*yy+zz*zz);
    if (zz<0.): ip = -1.*ip;
    if (DEBUG): print " -- IP -- ",ip

    Dz = - (dx*tx+dy*ty+dz)/(1+tx*tx+ty*ty)
    XX = dx + tx*Dz
    YY = dy + ty*Dz
    ZZ = dz + 1.*Dz
    DD = math.sqrt(XX*XX+YY*YY+ZZ*ZZ)
    if (ZZ<0): DD = -1.*DD
    if (DEBUG): print " Dz, ", Dz
    if (DEBUG): print " XX,YY,ZZ ",XX,YY,ZZ,DD
    if (DEBUG): print " -- IP(DD) -- ",DD
    return DD

#---------------------------------------------------
def ipError( pt ):
    """ Returns IP uncertainty from the parameterization published in LHCb-2005-012.
    @param pt Transverse momentum of the track
    @author Hugo Ruiz, hugo.ruiz@cern.ch
    """
    x = fabs(1/pt)
    return 0.0223869 - (0.00424545*x)+ (0.0175535*x**2) -(0.005477*x**3)+ (0.00104437*x**4) -(0.000104878*x**5)+ (0.00000418932*x**6)


#---------------------------------------------------
def getLists(track):
    """ Returns position and momentum of a track as python lists
    @param track Input track
    @returns [posx,posy,posz],[momx,momy,momz]
    @author Antonio Perez-Calero Izquierdo aperez@ecm.ub.es
    """
    pos=track.position()
    mom=track.momentum()

    posx=pos.Coordinates().x()
    posy=pos.Coordinates().y()
    posz=pos.Coordinates().z()

    momx=mom.Coordinates().x()
    momy=mom.Coordinates().y()
    momz=mom.Coordinates().z()

    return [posx,posy,posz],[momx,momy,momz]

#---------------------------------------------------
def produceVertex(track1,track2, returnDoca = False):
    """ Produces a vertex made out of two tracks.
    @param track1 First track
    @param track2 Second track
    @param returnDoca If set to True, DOCA of the vertex is also returned
    @returns Vertex or Vertex and DOCA, depending on value of returnDoca
    @author Antonio Perez-Calero Izquierdo aperez@ecm.ub.es
    """ 

    pos1,mom1=getLists(track1)
    pos2,mom2=getLists(track2)

    vertex=gaudimodule.gbl.LHCb.RecVertex()
    vertex_cord=gaudimodule.gbl.Gaudi.XYZPoint()
    vertex.addToTracks(track1)
    vertex.addToTracks(track2)

    v0=(pos1[0]-pos2[0],pos1[1]-pos2[1],pos1[2]-pos2[2])

    tol = 1.e-13

    d02=kintools.dot(v0,mom2)
    d21=kintools.dot(mom1,mom2)
    d01=kintools.dot(v0,mom1)
    d22=kintools.dot(mom2,mom2)
    d11=kintools.dot(mom1,mom1)

    den=d11*d22-d21*d21
    if fabs(den)<tol:
        vertex_cord.SetXYZ(0,0,-8000)
        doca=1000000.
    else:
        num=d02*d21-d01*d22
        mu1=num/den
        mu2=(d02+d21*mu1)/d22

        point1=[pos1[0]+mu1*mom1[0],pos1[1]+mu1*mom1[1],pos1[2]+mu1*mom1[2]]
        point2=[pos2[0]+mu2*mom2[0],pos2[1]+mu2*mom2[1],pos2[2]+mu2*mom2[2]]

        #print point1,point2
        P1P2=(point1[0]-point2[0],point1[1]-point2[1],point1[2]-point2[2])
        doca=math.sqrt(kintools.dot(P1P2,P1P2))

        coord_x=0.5*(point1[0]+point2[0])
        coord_y=0.5*(point1[1]+point2[1])
        coord_z=0.5*(point1[2]+point2[2])

        vertex_cord.SetXYZ(coord_x,coord_y,coord_z)

    vertex.setPosition(vertex_cord)

    if returnDoca:
        return vertex,doca
    else:
        return vertex
