#define _USE_MATH_DEFINES
#include "XML/compact/XMLCalorimeter.h"
#include "XML/lcdd/XMLShapes.h"
#include "XML/lcdd/XMLVolumes.h"
#include "XML/lcdd/XMLObjects.h"
#include "XML/lcdd/XMLLCDD.h"
#include "DetDesc/Layering.h"

// C+_+ include files
#include <cmath>

using namespace std;
using namespace DetDesc;
using namespace DetDesc::XML;

Subdetector& CylindricalBarrelCalorimeter::fromCompact(LCDD& lcdd, const SensitiveDetector& sens)  {
  Document    doc      = lcdd.document();
  Component   compact  = handle();
  Tag_t       det_name = name();
  Dimension   dim      = dimensions();
  Material    air      = lcdd.material(Tag_Air);
  Tube        envelope   (doc,det_name+"_envelope");
  Volume      envelopeVol(doc,det_name+"_envelope_volume",envelope,air);
  double      z    = dim.outer_z();
  double      rmin = dim.inner_r();
  double      r    = rmin;
  int         n    = 0;

  envelope.setDimensions(rmin,r,2.*z);
  for(Collection_t c(compact,Tag_layer); c; ++c)  {
    Component layer = c;
    for(int i=0, m=0, repeat=layer.repeat(); i<repeat; ++i, m=0)  {
      Tag_t  layer_name = det_name + _toString(n,"_layer%d");
      Tube   layer_tub(doc,layer_name);
      Volume layer_vol(doc,layer_name+"_volume",layer_tub,air);
      double rlayer = r;
      for(Collection_t l(layer,Tag_slice); l; ++l, ++m)  {
        Component slice = l;
        Material  slice_mat  = lcdd.material(slice.material());
        Tag_t     slice_name = layer_name + _toString(m,"slice%d");
        Tube      slice_tube(doc,slice_name);
        Volume    slice_vol (doc,slice_name+"_volume",slice_tube,slice_mat);
        double    router = r + slice.thickness();

        if ( slice.isSensitive() ) slice_vol.setSensitiveDetector(sens);
        slice_tube.setDimensions(r,router,z * 2);
        r = router;
        lcdd.add(slice_tube).add(slice_vol);
        // Set vis attributes of slice
        setAttributes(lcdd, slice, slice_vol);            
        layer_vol.addPhysVol(PhysVol(doc,slice_vol));
      }
      setVisAttributes(lcdd, compact, layer_vol);
      layer_tub.setDimensions(rlayer,r,z * 2);

      PhysVol layer_physvol(doc,layer_vol);
      layer_physvol.addPhysVolID(Attr_layer,n);
      envelopeVol.addPhysVol(layer_physvol);
      lcdd.add(layer_vol).add(layer_tub);
      ++n;
    }
  }
  PhysVol physvol(doc,envelopeVol);
  physvol.addPhysVolID(Attr_system,id())
         .addPhysVolID(Attr_barrel,0);
  envelopeVol.addPhysVol(physvol);
  Volume(lcdd.pickMotherVolume(*this)).addPhysVol(physvol);
  lcdd.add(envelope).add(envelopeVol);
  setAttributes(lcdd, compact, envelopeVol);
  return *this;
}

Subdetector& CylindricalEndcapCalorimeter::fromCompact(LCDD& lcdd, const SensitiveDetector& sens)  {
  Document    doc      = lcdd.document();
  Tag_t       det_name = name();
  Handle_t    compact  = handle();
  Dimension   dim      = dimensions();
  Material    air      = lcdd.material(Tag_Air);
  Tube        envelope   (doc,det_name+"_envelope");
  Volume      envelopeVol(doc,det_name+"_envelope_volume",envelope,air);
  Volume      motherVol = lcdd.pickMotherVolume(*this);
  bool        reflect   = dim.reflect();
  double      zmin      = dim.inner_z();
  double      rmin      = dim.inner_r();
  double      rmax      = dim.outer_r();
  double      totWidth  = Layering(compact).totalThickness();
  double      z    = zmin;
  int         n    = 0;

  for(Collection_t c(compact,Tag_layer); c; ++c)  {
    Component layer = c;
    double layerWidth = 0;
    for(Collection_t l(layer,Tag_slice); l; ++l)
      layerWidth += Component(l).thickness();
    for(int i=0, m=0, repeat=layer.repeat(); i<repeat; ++i, m=0)  {
      double zlayer = z;
      Tag_t   layer_name = det_name + _toString(n,"_layer%d");
      Tube    layer_tub(doc,layer_name);
      Volume  layer_vol(doc,layer_name+"_volume",layer_tub,air);

      for(Collection_t l(layer,Tag_slice); l; ++l, ++m)  {
        Component  slice = l;
        double     w = slice.thickness();
        Tag_t      slice_name = layer_name + _toString(m,"slice%d");
        Material   slice_mat  = lcdd.material(slice.material());
        Tube       slice_tube(doc,slice_name);
        Volume     slice_vol (doc,slice_name+"_volume", slice_tube, slice_mat);

        if ( slice.isSensitive() ) slice_vol.setSensitiveDetector(sens);
        slice_tube.setDimensions(rmin,rmax,w);

        lcdd.add(slice_tube).add(slice_vol);
        setAttributes(lcdd, slice, slice_vol);
        PhysVol slice_physvol(doc,slice_vol);
        slice_physvol.setZ(z-zlayer-layerWidth/2.+w/2.);
        layer_vol.addPhysVol(slice_physvol);

        z += w;
      }
      setVisAttributes(lcdd,layer,layer_vol);
      layer_tub.setDimensions(rmin,rmax,layerWidth);
      PhysVol layer_phys(doc,layer_vol);
      layer_phys.setZ(zlayer-zmin-totWidth/2.+layerWidth/2.);
      layer_phys.addPhysVolID(Attr_layer,n);
      envelopeVol.addPhysVol(layer_phys);
      lcdd.add(layer_vol).add(layer_tub);
      ++n;
    }
  }
  envelope.setDimensions(rmin,rmax,totWidth,M_PI);

  PhysVol layer_physvol(doc,envelopeVol);
  layer_physvol.setZ(zmin+totWidth/2.);
  layer_physvol.addPhysVolID(Attr_system,id());
  layer_physvol.addPhysVolID(Attr_barrel,1);
  motherVol.addPhysVol(layer_physvol);

  if ( reflect )   {
    Rotation rot = lcdd.rotation(Tag_reflect_rot);
    PhysVol r_layer_physvol(doc,envelopeVol);
    r_layer_physvol.setZ(-zmin-totWidth/2.);
    r_layer_physvol.setRotation(rot);
    r_layer_physvol.addPhysVolID(Attr_system,id());
    r_layer_physvol.addPhysVolID(Attr_barrel,2);
    motherVol.addPhysVol(r_layer_physvol);
  }

  setAttributes(lcdd, compact, envelopeVol);
  lcdd.add(envelopeVol);
  return *this;
}

Subdetector& PolyhedraBarrelCalorimeter2::fromCompact(LCDD& lcdd, const SensitiveDetector& sens)  {
  Document    doc       = lcdd.document();
  Component   compact   = handle();
  Tag_t       det_name  = name();
  Dimension   dim       = dimensions();
  Material    air       = lcdd.material(Tag_Air);
  Material    gapMat    = air;
  Element     staves    = compact.child(Tag_staves);
  Volume      motherVol = lcdd.pickMotherVolume(*this);
  Layering    layering(compact);
  double      totalThickness = layering.totalThickness();
  int         totalRepeat = 0;
  int         totalSlices = 0;
  double      gap         = compact.gap();
  int         numSides    = dim.numsides();
  double      detZ        = dim.z();
  double      rmin        = dim.rmin();
  double      r           = rmin;
  double      zrot        = M_PI/numSides;
  Rotation    rot(doc,det_name+"_rotation");

  rot.setZ(zrot);
  lcdd.add(rot);
  for(Collection_t c(compact,Tag_layer); c; ++c)  {
    Component layer = c;
    int repeat = layer.repeat();
    totalRepeat += repeat;
    totalSlices += layer.numChildren(Tag_slice);
  }

  PolyhedraRegular polyhedra(doc,det_name+"_polyhedra",numSides,rmin,rmin+totalThickness,detZ);
  Volume           envelopeVol(doc,det_name+"_envelope",polyhedra,air);
  PhysVol          envelopePhysVol(doc,envelopeVol);
  lcdd.add(polyhedra);
  envelopePhysVol.setRotation(rot);
  envelopePhysVol.addPhysVolID(Tag_system, id());
  envelopePhysVol.addPhysVolID(Tag_barrel, 0);
  motherVol.addPhysVol(envelopePhysVol);

  double halfInnerAngle = M_PI/numSides;
  double innerAngle     = 2e0*halfInnerAngle;
  double tan_inner      = tan(halfInnerAngle) * 2e0;
  double rmax           = rmin + totalThickness;
  double innerFaceLen   = rmin * tan_inner;
  double outerFaceLen   = rmax * tan_inner;
  double staveThickness = totalThickness;

  Trapezoid staveTrdOuter(doc,det_name+"_stave_trapezoid_outer");
  staveTrdOuter.setX1(innerFaceLen/2e0);
  staveTrdOuter.setX2(outerFaceLen/2e0);
  staveTrdOuter.setY1(detZ/2e0);
  staveTrdOuter.setY2(detZ/2e0);
  staveTrdOuter.setZ(staveThickness/2e0);
  lcdd.add(staveTrdOuter);
  Volume staveOuterVol(doc,det_name+"_stave_outer",staveTrdOuter,air);

  Trapezoid staveTrdInner(doc,det_name+"_stave_trapezoid_inner");
  staveTrdInner.setX1(innerFaceLen/2e0-gap);
  staveTrdInner.setX2(outerFaceLen/2e0-gap);
  staveTrdInner.setY1(detZ/2e0);
  staveTrdInner.setY2(detZ/2e0);
  staveTrdInner.setZ(staveThickness/2e0);
  lcdd.add(staveTrdInner);
  Volume staveInnerVol(doc,det_name+"_stave_inner",staveTrdInner,air);

  double layerOuterAngle = (M_PI-innerAngle)/2e0;
  double layerInnerAngle = (M_PI/2e0 - layerOuterAngle);
  double layer_position_z = -(staveThickness / 2);                        
  double layer_dim_x = innerFaceLen - gap * 2;
  int layer_number = 0;

  for(Collection_t c(compact,Tag_layer); c; ++c)  {
    Component    layer_element = c;
    int          repeat = layer_element.repeat();      // Get number of times to repeat this layer.
    const Layer* layer = layering.layer(layer_number); // Get the layer from the layering engine.

    // Loop over repeats for this layer.
    for (int j = 0; j < repeat; j++)    {                
      Tag_t layer_name = det_name + _toString(layer_number,"_stave_layer%d");
      double layer_thickness = layer->thickness();              
      int nslices = layer_element.numChildren(Tag_slices);

      // Layer position in Z within the stave.
      layer_position_z += layer_thickness / 2;
      // Position of layer.
      Position layer_position(doc,layer_name + "_position");
      layer_position.setZ(layer_position_z);
      lcdd.add(layer_position);
      // Layer box.
      Box layer_box(doc,layer_name + "_box", layer_dim_x, detZ, layer_thickness);
      lcdd.add(layer_box);
      // Layer volume. 
      Volume layer_volume(doc,layer_name,layer_box,air);

      // Create the slices (sublayers) within the layer.
      double slice_position_z = -(layer_thickness / 2);
      int slice_number = 0;
      for(Collection_t k(layer_element,Tag_slice); k; ++k)  {
        Component slice_element = k;
        Tag_t slice_name = layer_name + _toString(slice_number,"_slice%d");
        double slice_thickness = slice_element.thickness();
        Material slice_material = lcdd.material(slice_element.material());

        slice_position_z += slice_thickness / 2;
        // Slice Position.
        Position slice_position(doc,slice_name + "_position");
        slice_position.setZ(slice_position_z);
        lcdd.add(slice_position);
        // Slice box. 
        Box slice_box(doc,slice_name + "_box",layer_dim_x,detZ,slice_thickness);
        lcdd.add(slice_box);

        // Slice volume.
        Volume slice_volume(doc,slice_name,slice_box,slice_material);
        if ( slice_element.isSensitive() ) slice_volume.setSensitiveDetector(sens);
        lcdd.add(slice_volume);
        // Set region, limitset, and vis.
        setAttributes(lcdd, slice_element, slice_volume);
        // slice PhysVol
        PhysVol slice_physvol(doc,slice_volume);
        slice_physvol.setPosition(slice_position);
        slice_physvol.addPhysVolID(Tag_slice, slice_number);
        layer_volume.addPhysVol(slice_physvol);
        // Increment Z position for next slice.
        slice_position_z += slice_thickness / 2;
        // Increment slice number.
        ++slice_number;             
      }

      // Set region, limitset, and vis.
      setAttributes(lcdd, layer_element, layer_volume);
      // Add the layer logical volume to the structure.
      lcdd.add(layer_volume);
      // Layer physical volume.
      PhysVol layer_physvol(doc,layer_volume);
      layer_physvol.setPosition(layer_position);
      layer_physvol.addPhysVolID(Tag_layer, layer_number);
      staveInnerVol.addPhysVol(layer_physvol);
      // Increment the layer X dimension.
      layer_dim_x += layer_thickness * tan(layerInnerAngle) * 2;
      // Increment the layer Z position.
      layer_position_z += layer_thickness / 2;
      // Increment the layer number.
      ++layer_number;         
    }
  }
  // Make stave inner physical volume.
  PhysVol staveInnerPhysVol(doc,staveInnerVol);
  // Add stave inner physical volume to outer stave volume.
  staveOuterVol.addPhysVol(staveInnerPhysVol);
  // Set the vis attributes of the outer stave section.
  setVisAttributes(lcdd,staves,staveOuterVol);
  // Add the stave inner volume to the structure.
  lcdd.add(staveInnerVol);
  // Add the stave outer volume to the structure.
  lcdd.add(staveOuterVol);
  // Place the staves.
  placeStaves(lcdd,det_name,rmin,numSides,totalThickness,envelopeVol,innerAngle,staveOuterVol);
  // Set envelope volume attributes.
  setAttributes(lcdd,compact,envelopeVol);
  // Add the subdetector envelope to the structure.
  lcdd.add(envelopeVol);
  return *this;
}

void PolyhedraBarrelCalorimeter2::placeStaves(LCDD& lcdd, 
                                              const Tag_t& detName, 
                                              double rmin, 
                                              int numsides, 
                                              double total_thickness, 
                                              Volume envelopeVolume, 
                                              double innerAngle, 
                                              Volume sectVolume)
{
  Document doc = lcdd.document();
  double innerRotation = innerAngle;
  double offsetRotation= -innerRotation / 2e0;
  double sectCenterRadius = rmin + total_thickness / 2e0;
  double rotY = -offsetRotation;
  double rotX = M_PI / 2e0;
  double posX = -sectCenterRadius * sin(rotY);
  double posY = sectCenterRadius * cos(rotY);

  for (int i = 0; i < numsides; i++)  {
    int moduleNumber = i;
    Position position(doc, detName + _toString(moduleNumber,"_stave0_module%d_position"));
    position.setX(posX).setY(posY);

    Rotation rotation(doc, detName + _toString(moduleNumber,"_stave0_module%d_rotation"));
    rotation.setX(rotX).setY(rotY);

    lcdd.add(position);
    lcdd.add(rotation);

    PhysVol sectPhysVol(doc,sectVolume);
    sectPhysVol.setPosition(position).setRotation(rotation);

    envelopeVolume.addPhysVol(sectPhysVol);
    sectPhysVol.addPhysVolID(Tag_stave, 0);
    sectPhysVol.addPhysVolID(Tag_module, moduleNumber);

    rotY -= innerRotation;
    posX  = -sectCenterRadius * sin(rotY);
    posY  = sectCenterRadius * cos(rotY);            
  }
}

Subdetector& PolyhedraEndcapCalorimeter2::fromCompact(LCDD& lcdd, const SensitiveDetector& sens)  {
  return *this;
}
Subdetector& EcalBarrel::fromCompact(LCDD& lcdd, const SensitiveDetector& sens)  {
  return *this;
}
