# module with functions that deals LHCb RC objects
# author: D. Martinez, J.A. Hernando

from math import *

DEBUG = False

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
    rt = sqrt(xt*xt+yt*yt)
    tr = sqrt(tx*tx+ty*ty)
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
    ip2 = sqrt(RR*RR+ZZ*ZZ)
    if (ZZ<0): ip1 = -1.*ip1
    if (ZZ>0): ip2 = -1.*ip2
    if (DEBUG): print " Dr,Dz,dz",Dr,Dz,dz
    if (DEBUG): print " RR,ZZ",RR,ZZ
    if (DEBUG): print " -- IP1 -- ",ip1
    if (DEBUG): print " -- IP2 -- ",ip2
    return ip2

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
    lbda = 1./sqrt(1.+tx*tx+ty*ty);
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
  
    ip = lbda*sqrt(xx*xx+yy*yy+zz*zz);
    if (zz<0.): ip = -1.*ip;
    if (DEBUG): print " -- IP -- ",ip

    Dz = - (dx*tx+dy*ty+dz)/(1+tx*tx+ty*ty)
    XX = dx + tx*Dz
    YY = dy + ty*Dz
    ZZ = dz + 1.*Dz
    DD = sqrt(XX*XX+YY*YY+ZZ*ZZ)
    if (ZZ<0): DD = -1.*DD
    if (DEBUG): print " Dz, ", Dz
    if (DEBUG): print " XX,YY,ZZ ",XX,YY,ZZ,DD
    if (DEBUG): print " -- IP(DD) -- ",DD
    return DD

