# Functions to compute the VELO geometric acceptance 
from math import exp, sqrt, cos, asin, floor, fabs

__all__ = ["trackInVELOAcceptance"]

z_strip_A = [ -175., -145., -115., -85., -55., -25., 5., 35., 65., 95., 125., 155., 185., 215., 245., 275., 435., 585., 635., 685., 735. ]
z_strip_C = [ -160., -130., -100., -70., -40., -10.,20., 50., 80.,110., 140., 170., 200., 230., 260., 300., 450., 600., 650., 700., 750. ]
r_min = 8.2
r_max = 40.

def z_from_radius( x0, y0, z0, tx, ty, r1 ):
  # returns z position where particle with direction tx, ty coming from x0, y0, z0 reaches radius r1
  tr2 = tx * tx + ty * ty
  if 0 == tr2: 
    return -1000.
  ratio = ( x0 * tx + y0 * ty ) / tr2
  #rad = ratio * ratio + r1 * r1 - x0 * x0 - y0 * y0
  rad = ratio * ratio + ( r1 * r1 - x0 * x0 - y0 * y0 ) / tr2
  if rad <= 0: 
    return -1000.
  return z0 - ratio + sqrt( rad )

def n_mods_z_range( z1, z2, z_positions ):
  # returns number of modules with z coordinates between z1 and z2
  n = 0
  for z in z_positions:
    if z > z1 and z < z2: n += 1
  return n

def _trackInVELOAcceptance( x, y, z, tx1, ty1, n_min ):
  r = sqrt( x*x+y*y )
  n1 = 0
  if r < r_max : 
    if r > r_min:
      z1_min = z
      z1_max = z_from_radius( x, y, z, tx1, ty1, r_max )
    else:  
      z1_min = z_from_radius( x, y, z, tx1, ty1, r_min )
      z1_max = z_from_radius( x, y, z, tx1, ty1, r_max )
    if ( x + tx1 * ( z1_min - z ) ) > 0: 
      n1 = n_mods_z_range( z1_min, z1_max, z_strip_A )
    else:  
      n1 = n_mods_z_range( z1_min, z1_max, z_strip_C )
  if n1 >= n_min:
    return 1
  return 0    

def trackInVELOAcceptance( parent, track, minhits) :
  x  = parent.endVertex().position().x()
  y  = parent.endVertex().position().y()
  z  = parent.endVertex().position().z()  
  tx = track.slopes().x()
  ty = track.slopes().y()
  return _trackInVELOAcceptance(x,y,z,tx,ty,minhits)

