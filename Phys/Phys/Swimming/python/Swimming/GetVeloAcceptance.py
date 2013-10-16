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

# Old function used for swimming 2010 data
# Now obsolete, should not be used, left for legacy reasons
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
# End obsolete code
# ---------------------------------------------------------

def solve_quad(x0,y0,z0,tx,ty,r1) :
  coef_a = tx*tx + ty*ty
  coef_b = 2*(x0 * tx + y0*ty)
  coef_c = x0*x0 + y0*y0 - r1*r1
  if (4*coef_a*coef_c > coef_b*coef_b) :
    return (-99999.,-99999.) 
  temp = -0.5 * (coef_b + (coef_b/fabs(coef_b))*sqrt(coef_b*coef_b - 4*coef_a*coef_c))
  x1 = temp
  if fabs(coef_a) > 0 :
    x1 /= coef_a
  else :
    x1 = -99999.
  x2 = coef_c/temp
  if x1 < x2 :
    return (x1+z0,x2+z0)
  else : 
    return (x2+z0,x1+z0) 

def nHitsOnTrackInVELOAcceptance(x, y, z, tx, ty) :
  r = sqrt( x*x+y*y )
  z1_min = (0.,0.)
  z1_max = (0.,0.)
  n1 = 0.
  # We need the intercept of the line coming from x, y, z, with slopes m_tx, m_ty
  # with radii r_min and r_max. There are two solutions for each radius
  z1_min = solve_quad(x,y,z,tx,ty,r_min)
  z1_max = solve_quad(x,y,z,tx,ty,r_max)
  # Now, where are we?
  if (r > r_max) :
    # Are the crossing points ahead or behind us? 
    if (    (z > z1_min[1]) and (z > z1_max[1]) ) : 
      # Current z value is ahead of both z values where we cross r_min/max
      # hence no hits on the VELO in the forward direction
      return n1
    # Do we cross r_min?
    if not (fabs(z1_min[0]+z1_min[1] + 2*99999.) > 0.) :
      # Do we cross the VELO at all?
      if not (fabs(z1_max[0]+z1_max[1] + 2*99999.) > 0.) :
        return n1
      else :
        # A/C side business is now tricky
        # For now approximate by averaging
        n1 =  n_mods_z_range( z1_max[0], z1_max[1], z_strip_A )
        n1 += n_mods_z_range( z1_max[0], z1_max[1], z_strip_C )
        n1 /= 2.
    else :   
      # We started outside r_max so we must cross r_max to get to r_min 
      if x > 0 :
        # From A side to C side
        n1 += n_mods_z_range( z1_max[0], z1_min[0], z_strip_A )
        n1 += n_mods_z_range( z1_min[1], z1_max[1], z_strip_C )
      else :
        # From C side to A side
        n1 += n_mods_z_range( z1_max[0], z1_min[0], z_strip_C )
        n1 += n_mods_z_range( z1_min[1], z1_max[1], z_strip_A )
  elif (r > r_min) :
    # Now we begin between r_min and r_max
    # We can either just cross r_max, or cross r_min, go through the space between
    # the sensors, cross r_min again, and finally cross r_max again
    # 
    # Which direction are we going in?
    if (x*tx+y*ty > 0) : # Increasing r
      if x > 0 : 
        n1 = n_mods_z_range( z, z1_max[1], z_strip_A )
      else :
        n1 = n_mods_z_range( z, z1_max[1], z_strip_C )
    else :
      if x > 0 :
        # From A side to C side
        n1 =  n_mods_z_range( z, z1_min[0], z_strip_A )
        n1 += n_mods_z_range( z1_min[1], z1_max[1], z_strip_C )   
      else :
        # From C side to A side
        n1 =  n_mods_z_range( z, z1_min[0], z_strip_C )
        n1 += n_mods_z_range( z1_min[1], z1_max[1], z_strip_A )
  else :
    # Simplest case, we start inside r_min, we can only cross once ahead of us.
    # Simplify by ignoring tracks which cross the A/C side
    if (x+tx*(z1_min[1]-z)) > 0 :
      n1 = n_mods_z_range( z1_min[1], z1_max[1], z_strip_A )
    else :
      n1 = n_mods_z_range( z1_min[1], z1_max[1], z_strip_C )
  return n1			
  
def trackInVELOAcceptance( swimPoint, parent, trackparent, track, minhits) :
  # For non VELO tracks, this part is always true
  if not track.hasVelo() : 
    return 1
  #
  x  = trackparent.endVertex().position().x() - swimPoint*parent.slopes().Unit().X()
  y  = trackparent.endVertex().position().y() - swimPoint*parent.slopes().Unit().Y()
  z  = trackparent.endVertex().position().z() - swimPoint*parent.slopes().Unit().Z() 
  tx = track.slopes().x()
  ty = track.slopes().y()
  if nHitsOnTrackInVELOAcceptance(x,y,z,tx,ty) >= minhits :
    return 1
  else :
    return 0
