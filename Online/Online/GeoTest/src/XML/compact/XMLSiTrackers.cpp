#define _USE_MATH_DEFINES
#include "XML/compact/XMLSiTrackers.h"
#include "XML/lcdd/XMLShapes.h"
#include "XML/lcdd/XMLVolumes.h"
#include "XML/lcdd/XMLObjects.h"
#include "XML/lcdd/XMLLCDD.h"

// C+_+ include files
#include <cmath>
#include <vector>
#include <map>

using namespace std;
using namespace DetDesc::XML;

#define _X(x)  Strng_t(#x)

Subdetector& SiTrackerBarrel::fromCompact(LCDD& lcdd, const SensitiveDetector& sens)   {
  Document    doc      = lcdd.document();
  Tag_t       det_name = name();
  Component   compact  = handle();
  Material    air      = lcdd.material(Tag_Air);
  Tube        envelope   (doc,det_name+"_envelope");
  Volume      envelopeVol(doc,det_name+"_envelope_volume",envelope,air);
  Volume      motherVol = lcdd.pickMotherVolume(*this);
  int         id = this->id();

  map<string, Volume> modules;
  for(Collection_t c(compact,Tag_module); c; ++c) {
    Volume vol(buildModule(lcdd,compact,c,sens));
    modules.insert(make_pair(_toString(RefElement(c).name()),vol));
  }

  for(Collection_t c(compact,Tag_layer); c; ++c)  {
    Component layer        = c;
    int       mod_id       = layer.id();
    Tag_t     mod_nam      = layer.module();
    Tag_t     layer_name   = det_name + _toString(mod_id,"_layer%d");
    Volume    mod_envelope = modules[mod_nam];
    Component barrel_env (layer.child(_X(barrel_envelope)));
    Tube      layer_tube (doc, layer_name+"_tube");
    Volume    layer_vol  (doc, layer_name, layer_tube, air);
    Element   rphi_layout(layer.child(_X(rphi_layout)));
    Element   z_layout = layer.child(_X(z_layout));
    double    ir       = barrel_env.inner_r();
    double    var_or   = barrel_env.outer_r();
    double    oz       = barrel_env.z_length();
    int       nphi     = rphi_layout.attr<int>(_X(nphi));
    double    phi0     = rphi_layout.attr<double>(_X(phi0));
    double    phi_tilt = rphi_layout.attr<double>(_X(phi_tilt));
    double    rc       = rphi_layout.attr<double>(_X(rc));
    double    rphi_dr  = rphi_layout.hasAttr(_X(dr)) ? rphi_layout.attr<double>(_X(dr)) : 0.0;
    double    phi_incr = (2.*M_PI)/nphi;
    double    phic     = phi0;
    double    z0       = z_layout.attr<double>(_X(z0));
    double    nz       = z_layout.attr<double>(_X(nz));
    double    z_dr     = z_layout.attr<double>(_X(dr));
    double    z_incr   = (2.*z0)/(nz-1);
    double    module_z = -z0;
    int       module   = 0;

    layer_tube.setDimensions(ir,var_or,oz);
    lcdd.add(layer_tube);
    for(int ii=0; ii<nphi; ++ii)  {
      double dx = z_dr * cos(phic + phi_tilt);
      double dy = z_dr * sin(phic + phi_tilt);
      double  x = rc * cos(phic);
      double  y = rc * sin(phic);
      for (int j=0; j<nz; ++j)   {
        Tag_t mod_place_name = layer_name + _toString(ii,"_phi%d") + _toString(j,"_z%d");
        double z    = module_z;
        double rotx = M_PI/2.;
        double roty = -((M_PI/2.) - phic - phi_tilt);
        double rotz = 0;
        Position pos(doc,mod_place_name+"_position",x,y,z);
        Rotation rot(doc,mod_place_name+"_rotation",rotx, roty, rotz);
        lcdd.add(pos).add(rot);

        PhysVol pv(doc,mod_envelope);
        pv.setPosition(pos).setRotation(rot).addPhysVolID(_X(module),module);
        ++module;
        layer_vol.addPhysVol(pv);
        x  += dx;
        y  += dy;
        dx *= -1;
        dy *= -1;
        module_z += z_incr;
      }
      phic    += phi_incr;
      rc      += rphi_dr;
      rphi_dr *= -1;
      module_z = -z0;
    }
    setAttributes(lcdd, layer, layer_vol);
    lcdd.add(layer_vol);
    PhysVol layer_pv(doc, layer_vol);
    layer_pv.addPhysVolID(Attr_system,id);
    layer_pv.addPhysVolID(Attr_barrel,0);
    motherVol.addPhysVol(layer_pv);
  }
  return *this;
}


Volume SiTrackerBarrel::buildModule(LCDD& lcdd, 
                                    Component detector, 
                                    Component node,
                                    const SensitiveDetector& sens)   
{
  Document       doc = lcdd.document();
  Material       air = lcdd.material(Tag_Air);
  Tag_t     det_name = detector.name();
  string module_name = _toString(node.name());
  Volume mod_vol;
  int sensor_number = 0;
  for(Collection_t i(detector,_X(module)); i; ++i)  {
    Component mod_element = i;
    string nam = _toString(mod_element.name());
    if ( nam == module_name )  {
      Component mod_envelope = mod_element.child(_X(module_envelope));
      Tag_t  mod_name   = det_name + "_" + mod_element.name();
      double mod_length = mod_envelope.length();
      double mod_width  = mod_envelope.width();
      double mod_thick  = mod_envelope.thickness();
      Box mod_box(doc, mod_name+"_box", mod_width, mod_length, mod_thick);

      mod_vol = Volume(doc,mod_name,mod_box,air);
      lcdd.add(mod_box);
      int ncomponents = 0;
      for(Collection_t j(mod_element,_X(module_components)); j; ++j, ++ncomponents)  {
        Component c = j;
        Position  c_pos    = c.child(Tag_position, false);
        Rotation  c_rot    = c.child(Tag_rotation, false);
        Tag_t     c_name   = mod_name + _toString(ncomponents,"_component%d");
        double    c_length = c.length();
        double    c_width  = c.width();
        double    c_thick  = c.thickness();
        Material  c_mat    = lcdd.material(c.material());
        Box       c_box(doc,c_name+"_box",c_width,c_length,c_thick);
        Volume    c_vol(doc,c_name,c_box,c_mat);
        PhysVol   c_physVol(doc,c_vol);

        lcdd.add(c_box).add(c_vol);
        if ( c_pos )  {
          Position pos(doc,c_name+"_position");
          pos.fromXML(c_pos);
          lcdd.add(pos);
          c_physVol.setPosition(pos);
        }
        if ( c_rot )  {
          Rotation rot(doc,c_name+"_rotation");
          rot.fromXML(c_pos);
          lcdd.add(rot);
          c_physVol.setRotation(rot);
        }
        if ( c.isSensitive() )  {
          c_vol.setSensitiveDetector(sens);
          c_physVol.addPhysVolID(Tag_sensor,sensor_number);
        }
        setAttributes(lcdd, c, c_vol);
        mod_vol.addPhysVol(c_physVol);
      }
      setVisAttributes(lcdd, mod_element, mod_vol);
    }
  }
  check(!mod_vol,"Failed to find module "+module_name);
  lcdd.add(mod_vol);
  return mod_vol;
}

Subdetector& SiTrackerEndcap::fromCompact(LCDD& lcdd, const SensitiveDetector& sens)  {
  Component   compact  = handle();
  Document    doc      = lcdd.document();
  Tag_t       det_name = compact.name();
  int         id       = compact.id();
  Material    air      = lcdd.material(Tag_Air);
  VisAttr     vis      = lcdd.visAttributes(Attr_InvisibleWithDaughters);

  m_modParams.clear();
  check(!sens,"SiTrackerEndcap: Invalid handle to SensitiveDetector for:"+det_name);
  for(Collection_t i(compact,Tag_module); i; ++i)
    m_modParams.insert(make_pair(Component(i).name(),Dimension(i)));
  for(Collection_t i(compact,Tag_layer); i; ++i)  {
    Component layer(0);
    int      l_id     = layer.id();
    int      nwedges  = layer.attr<int>(_X(nwedges));
    Tag_t    l_name   = layer.name();
    LayerDim dim      = {id,layer.inner_r(),layer.outer_r(),layer.inner_z(),layer.thickness(),M_PI/nwedges};
    double   l_Z      = dim.inner_z + dim.thickness/2e0;
    Rotation l_rot(doc,l_name+"_rotation");
    Position l_pos(doc,l_name+"_position");
    Rotation r_rot(doc,l_name+"_rotation_reflect");
    Position r_pos(doc,l_name+"_position_negative",0e0,M_PI,-l_Z);
    double   phi, r = (dim.inner_r+dim.outer_r)/2e0;
    int j;

    l_pos.setZ(l_Z);
    lcdd.add(l_rot).add(l_pos).add(r_rot).add(r_pos);

    Tube     tube(doc, l_name+"_tube", dim.inner_r, dim.outer_r/cos(dim.deltaphi), dim.thickness/2e0);
    Volume   vol (doc, l_name, tube, air);
    Volume   wedge(makeWedge(lcdd, sens, compact, layer, dim));
    lcdd.add(tube).add(vol).add(wedge);

    if ( vis ) wedge.setVisAttributes(vis);
    for(j=0, phi=0; j<nwedges; ++j, phi += 2e0 * dim.deltaphi)  {
      Position p(doc, wedge.refName() + _toString(j,"_position%d"), r*cos(phi), r*sin(phi), 0e0);
      Rotation r(doc, wedge.refName() + _toString(j,"_rotation%d"), -M_PI/2e0, -M_PI/2e0 - phi, 0);
      PhysVol wedgePhysVol(doc,wedge, p, r);
      wedgePhysVol.addPhysVolID(Tag_wedge,1);
      vol.addPhysVol(wedgePhysVol);
      lcdd.add(p).add(r);
    }
    setVisAttributes(lcdd, layer, vol);
    lcdd.add(vol);

    PhysVol  l_physVol(doc, vol, l_pos, l_rot);
    l_physVol.addPhysVolID(Tag_system,id)
      .addPhysVolID(Tag_barrel,1)
      .addPhysVolID(Tag_layer,l_id);
    lcdd.trackingVolume().append(l_physVol);
    PhysVol  r_physVol(doc, vol, r_pos, r_rot);
    r_physVol.addPhysVolID(Tag_system,id)
      .addPhysVolID(Tag_barrel,2)
      .addPhysVolID(Tag_layer,l_id);    
    lcdd.trackingVolume().append(r_physVol);
  }
  return *this;
}

Volume SiTrackerEndcap::makeWedge(LCDD& lcdd, const SensitiveDetector& sens, Component compact, Component layer, const LayerDim& dim)  {
  Document doc(lcdd.document());
  Tag_t    modRef = layer.module();
  Material air    = lcdd.material(Tag_Air);
  Tag_t    sd_name= compact.name() + _toString(dim.id,"_layer%d");
  Tag_t    name   = sd_name + "_wedge";
  double   dz     = (dim.outer_r-dim.inner_r)/2e0;
  double   dx1    = dim.inner_r * tan(dim.deltaphi);
  double   dx2    = dim.outer_r * tan(dim.deltaphi);
  double   dy1    = dim.thickness/2e0;
  double   dy2    = dim.thickness/2e0;
  double   deltax = dx2 - dx1;
  double   slope  = deltax / (2*dz);

  // pull corners in by 0.1 microns to eliminate overlaps
  dx1 -= 0.0001;
  dx2 -= 0.0001;
  dz  -= 0.0001;

  Trapezoid wedgeTrd(doc, name+"_trapezoid", dx1, dx2, dy1, dy2, dz);
  Volume    wedgeVol(doc, name, wedgeTrd, air);
  Component module    = m_modParams[modRef];
  Dimension dimension = module.child(_X(module_parameters));
  if ( dimension )  {
    double  dy           = wedgeTrd.y1();
    double  r_size       = dimension.r_size();
    double  phi_size_max = dimension.phi_size_max();
    VisAttr vis          = lcdd.visAttributes(module.vis());
    
    vector<double> zcenters, zsizes, xsizes1, xsizes2;
    double zmin, zmax, zcurr = dz;
    while (zcurr - r_size > -dz)    {
      zmax = zcurr;
      zmin = zcurr-r_size;
      zcenters.push_back((zmin+zmax)/2);
      zsizes.push_back((zmax-zmin)/2);
      xsizes1.push_back(dx1 + slope*(zmin+dz));
      xsizes2.push_back(dx1 + slope*(zmax+dz));
      zcurr -= r_size;
    }
    zmax = zcurr;
    zmin = -dz;
    zcenters.push_back((zmin+zmax)/2);
    zsizes.push_back((zmax-zmin)/2);
    double xsize1 = dx1 + slope*(zmin+dz);
    double xsize2 = dx1 + slope*(zmax+dz);
    xsizes1.push_back(xsize1);
    xsizes2.push_back(xsize2);
    double zsize_last = 0.0;
    double xsize1_min = phi_size_max/2;
    double xsize_box = 0.0;
    int nboxes = 0;    
    int imodule = 0;

    for (int i=zcenters.size()-1; i >= 0; i--)    {
      if (zsizes[i] != zsize_last)        {
        zsize_last = zsizes[i];
        xsize1_min = phi_size_max/2;
        xsize_box = 0.0;
        nboxes = 0;
      }        
      int ntraps = (int)ceil(  2*(xsizes1[i] - nboxes*xsize_box) / phi_size_max );

      // Squares to fill extra space
      if (ntraps > 2)  {
        double delta_x = xsizes2[i] - xsizes1[i];
        xsize_box = delta_x*(int)floor(phi_size_max > delta_x ? phi_size_max/delta_x : delta_x/phi_size_max);
        if (xsize_box > 0.0)   {
          nboxes = (int)floor((xsizes1[i]-2*xsize1_min)/xsize_box);
        }
        ntraps = 2;
      }
      double xmin = -nboxes*xsize_box;
      double xmax = xmin+2*xsize_box;        
      for (int ibox = 0; ibox < nboxes; ibox++)    {
        Tag_t mod_name = sd_name + _toString(imodule,"_module%d");
        double xcenter = (xmin+xmax)/2;
        xmin += 2*xsize_box;
        xmax += 2*xsize_box;

        // FIXME: Multiply by 2 to conform to GDML convention of dividing inputs by 2.
        Box sliceBox(doc, mod_name + "_box");
        sliceBox.setDimensions(xsize_box*2,dy*2,zsizes[i]*2);
        lcdd.add(sliceBox);

        Volume mod_vol(doc, mod_name, sliceBox, air);
        // Make the box module.
        makeBoxModule(lcdd, sens, module, mod_vol);
        mod_vol.setVisAttributes(vis);
        lcdd.add(mod_vol);

        Position pos(doc, mod_name + "_position");
        pos.setX(xcenter).setZ(zcenters[i]);
        Rotation rot(doc,mod_name + "_rotation");
        lcdd.add(pos).add(rot);

        PhysVol pv(doc, mod_vol, pos, rot);
        pv.addPhysVolID(Tag_volume, imodule);
        wedgeVol.addPhysVol(pv);
        imodule++;
      }

      // Small symmetric trapezoids
      if ( ntraps == 1 )   {
        Tag_t     mod_name = sd_name + _toString(imodule,"_module%d");
        Trapezoid mod_trd(doc, mod_name+"_trapezoid", xsizes1[i], xsizes2[i], dy, dy, zsizes[i]);
        Volume    mod_vol(doc, mod_name, mod_trd, air);

        lcdd.add(mod_trd);
        makeTrdModule(lcdd, sens, module, mod_vol);
        mod_vol.setVisAttributes(vis);
        Position pos(doc, mod_name + "_position");
        pos.setZ(zcenters[i]);
        Rotation rot(doc, mod_name + "_rotation");
        lcdd.add(mod_vol).add(pos).add(rot);

        PhysVol pv(doc, mod_vol, pos, rot);
        pv.addPhysVolID(Tag_module, imodule);
        wedgeVol.addPhysVol(pv);
        imodule++;
      }

      // Split trapezoids
      if (ntraps == 2)      {
        double xoffset = xsize_box*nboxes;
        double xsize1  = (xsizes1[i]-xoffset)/ntraps;
        double xsize2  = (xsizes2[i]-xoffset)/ntraps;
        double xcenter = (xsize1+xsize2)/2 + xoffset;
        double theta   = abs(atan(slope/2));

        for (int ix = -1; ix <=1; ix=ix+2)        {
          Tag_t  mod_name = sd_name + _toString(imodule,"_module%d");
          Trap   mod_trap(doc, mod_name+"_trap",zsizes[i],theta*ix,0.0,dy,xsize1,xsize1,0.0,dy,xsize2,xsize2,0.0);
          Volume mod_vol (doc, mod_name, mod_trap, air);

          lcdd.add(mod_trap);
          makeTrapModule(lcdd, sens, module, mod_vol);
          mod_vol.setVisAttributes(vis);
          Position pos(doc, mod_name + "_position");
          pos.setX(ix*xcenter);
          pos.setZ(zcenters[i]);
          Rotation rot(doc, mod_name + "_rotation");
          lcdd.add(mod_vol).add(pos).add(rot);

          PhysVol pv(doc, mod_vol, pos, rot);
          pv.addPhysVolID(Tag_module, imodule);
          wedgeVol.addPhysVol(pv);
          imodule++;
        }
      }
    }
    return wedgeVol;
  }
  throw runtime_error("Module reference is missing for module:"+name);
}

void SiTrackerEndcap::makeBoxModule(LCDD& lcdd, const SensitiveDetector& sens, Component module, Volume mod_vol)  {
  Document  doc      =  lcdd.document(); 
  Tag_t     mod_name =  mod_vol.name();
  Dimension mod_box  =  lcdd.solid(mod_vol.solidRef());
  double    moduleX  =  mod_box.x();
  double    posY     = -mod_box.y() / 2e0;
  double    moduleZ  =  mod_box.z();
  VisAttr   vis      =  lcdd.visAttributes(module.vis());

  // pull corners in by 0.5 microns to eliminate overlaps.
  moduleX -= 0.0005;
  moduleZ -= 0.0005;
  int componentNumber=0, sensor=0;
  for(Collection_t j(module,_X(module_components)); j; ++j, ++componentNumber)  {
    Component component = j;
    double    thickness = component.thickness();
    Material  material  = lcdd.material(component.material());
    Tag_t     com_name  = mod_name + _toString(componentNumber,"_component%d");
    Box       com_box(doc, com_name + "_box", moduleX, thickness, moduleZ);
    Volume    com_vol(doc, com_name, com_box, material);

    posY += thickness / 2;
    com_vol.setVisAttributes(vis);

    Position pos(doc, com_name + "_position", 0., posY, 0.);
    Rotation rot(doc, com_name + "_rotation");
    rot.setY(M_PI);
    lcdd.add(com_box).add(com_vol).add(pos).add(rot);

    PhysVol pv(doc, com_vol, pos, rot);
    pv.addPhysVolID(Tag_component, componentNumber);
    mod_vol.addPhysVol(pv);
    if ( component.isSensitive() )    {
      check(sensor > 1,"SiTrackerEndcap::makeBoxModule: "+com_name+" Max of 2 modules allowed!");
      com_vol.setSensitiveDetector(sens);
      pv.addPhysVolID(Tag_sensor, sensor);
      ++sensor;
    }
    com_vol.setVisAttributes(vis);
    posY += thickness / 2e0;
  }
}

void SiTrackerEndcap::makeTrdModule(LCDD& lcdd, const SensitiveDetector& sens, Component module, Volume mod_vol)  {
  Tag_t     mod_name = mod_vol.name();
  Document  doc      = lcdd.document();
  Trapezoid trd      = lcdd.solid(mod_vol.solidRef());
  double x1 = trd.x1();
  double x2 = trd.x2();
  double y1 = trd.y1();
  double z  = trd.z();

  // pull corners in by 0.5 microns to eliminate overlaps.
  x1 -= 0.0005;
  x2 -= 0.0005;
  z  -= 0.0005;

  double posY = -y1;
  int com_id=0, sensor=0;
  for(Collection_t j(module,_X(module_components)); j; ++j, ++com_id)  {
    Component component = j;
    double    thickness = component.thickness();
    Material  material  = lcdd.material(component.material());
    Tag_t     com_name  = mod_name + _toString(com_id,"_component%d");
    Trapezoid slice_trd(doc, com_name + "_trd", x1, x2, thickness/2, thickness/2, z);
    Volume    slice_vol(doc, com_name, slice_trd, material);

    posY += thickness / 2;
    Position position(doc, com_name + "_position",0.,posY,0);
    Rotation rotation(doc, com_name + "_rotation");
    lcdd.add(slice_vol).add(slice_trd).add(position).add(rotation);

    PhysVol pv(doc, slice_vol, position, rotation);
    pv.addPhysVolID(Tag_component, com_id);
    mod_vol.addPhysVol(pv);
    if (component.isSensitive())    {
      check(sensor > 1,"SiTrackerEndcap::makeTrdModule: "+com_name+" Max of 2 modules allowed!");
      pv.addPhysVolID(Tag_sensor, sensor);
      slice_vol.setSensitiveDetector(sens);
      ++sensor;
    }
    // Set vis attributes of module component.
    slice_vol.setVisAttributes(lcdd.visAttributes(component.vis()));
    posY += thickness / 2;
  }
}

void SiTrackerEndcap::makeTrapModule(LCDD& lcdd, const SensitiveDetector& sens, Component module, Volume mod_vol)  {
  Document doc  = lcdd.document();
  Trap     trap = lcdd.solid(mod_vol.solidRef());
  Tag_t    mod_name = mod_vol.name();

  double a1 = trap.alpha1();
  double a2 = trap.alpha2();
  double x1 = trap.xHalfLength1();
  double x2 = trap.xHalfLength2();
  double x3 = trap.xHalfLength3();
  double x4 = trap.xHalfLength4();
  double y1 = trap.yHalfLength1();
  double z  = trap.zHalfLength();
  double theta = trap.theta();
  double phi   = trap.phi();

  // pull corners in by 0.5 microns to eliminate overlaps.
  x1 -= 0.0005;
  x2 -= 0.0005;
  x3 -= 0.0005;
  x4 -= 0.0005;
  z  -= 0.0005;

  double posY = -y1;
  int com_id=0, sensor=0;
  for(Collection_t j(module,_X(module_components)); j; ++j, ++com_id)  {
    Component component = j;
    double    thickness = component.thickness();
    Material  material  = lcdd.material(component.material());
    Tag_t     com_name  = mod_name + _toString(com_id,"_component%d");
    Trap      slice_trap(doc, com_name + "_trap", z, theta, phi, thickness/2, x1, x2, a1, thickness/2, x3, x4, a2);
    Volume    slice_vol (doc, com_name, slice_trap, material);

    posY += thickness / 2;
    Position pos(doc, com_name + "_position",0,posY,0);
    Rotation rot(doc, com_name + "_rotation");
    lcdd.add(slice_trap).add(slice_vol).add(pos).add(rot);

    PhysVol pv(doc, slice_vol, pos, rot);
    pv.addPhysVolID(Tag_component, com_id);
    mod_vol.addPhysVol(pv);
    // Set component sensitive detector.
    if (component.isSensitive())    {
      check(sensor > 1,"SiTrackerEndcap::makeTrapModule: "+com_name+" Max of 2 modules allowed!");
      //System.out.println(volume.getVolumeName() + " - " + this.sd.getRefName());
      slice_vol.setSensitiveDetector(sens);
      pv.addPhysVolID(Tag_sensor, sensor);
      ++sensor;
    }

    // Set vis attributes of module component.
    slice_vol.setVisAttributes(lcdd.visAttributes(component.vis()));
    posY += thickness / 2;
  }
}


Subdetector& SiTrackerEndcap2::fromCompact(LCDD& lcdd, const SensitiveDetector& sens)   {
  Document    doc      = lcdd.document();
  Component   compact  = handle();
  //int         sysID    = compact.id();
  Tag_t       det_name = compact.name();
  //bool        reflect  = compact.reflect();
  Material    air      = lcdd.material(Tag_Air);
  Material    vacuum   = lcdd.material(Tag_Vacuum);
  int         m_id=0, c_id=0, n_sensor=0;
  double      posY;

  for(Collection_t mi(compact,Tag_module); mi; ++mi, ++m_id)  {
    Component mod     = mi;
    Tag_t     m_nam   = mod.name();
    Tag_t     vol_nam = m_nam+"Volume";
    Component trd     = mod.child(Tag_trd);
  	double    x1      = trd.x1();
    double    x2      = trd.x2();
    double    z       = trd.z();
    double    y1, y2, total_thickness=0.;
    Collection_t ci(mod,Tag_module_component);

    /* Analyse these entries:
    <module name="SiVertexEndcapModule3">
        <trd x1="VertexEndcap_rmin3 * tan(pi/(VertexEndcapModules-0.1))" x2="VertexEndcap_rmax * sin(pi/(VertexEndcapModules-0.1))" z="(VertexEndcap_rmax - VertexEndcap_rmin3) / 2" />
        <module_component thickness="0.005*cm" material="Silicon" sensitive="true" />
        <module_component thickness="0.013*cm"   material="Carbon" />
    </module>
    */
    for(ci.reset(), total_thickness=0.0; ci; ++ci)
      total_thickness += Component(ci).thickness();

    y1 = y2 = total_thickness / 2;
    Trapezoid m_envelope(doc, m_nam+"Trd", x1, x2, y1, y2, z);
	  Volume    m_volume(doc, vol_nam, m_envelope, vacuum);

    lcdd.add(m_volume).add(m_envelope);
    m_volume.setVisAttributes(lcdd.visAttributes(mod.vis()));

    for(ci.reset(), n_sensor=0, c_id=0, posY=-y1; ci; ++ci, ++c_id)  {
      Component c       = ci;
      double    c_thick = c.thickness();
      Material  c_mat   = lcdd.material(c.material());
      Tag_t     c_name  = m_nam + _toString(c_id,"_component%d");
      Trapezoid trd(doc, c_name+"_trd", x1, x2, c_thick/2e0, c_thick/2e0, z);
      Volume    vol(doc, c_name, trd, c_mat);
      Position  pos(doc, c_name+"_position", 0e0, posY + c_thick/2e0, 0e0);
      Rotation  rot(doc, c_name+"_rotation");
      PhysVol   phv(doc, vol, pos, rot);

      phv.addPhysVolID(Tag_component,c_id);
      vol.addPhysVol(phv);
      lcdd.add(trd).add(vol).add(pos).add(rot);

      if ( c.isSensitive() ) {
        check(n_sensor > 1,"SiTrackerEndcap2::fromCompact: "+c_name+" Max of 2 modules allowed!");
        phv.addPhysVolID(Tag_sensor,c_id);
        vol.setSensitiveDetector(sens);
        ++n_sensor;
      }
      vol.setVisAttributes(lcdd.visAttributes(c.vis()));
      posY += c_thick;
    }
  }
  return *this;
}
