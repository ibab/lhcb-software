// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetDesc/detector/DetFactoryHelper.h"
#include "DetDesc/detector/CompactDetectors.h"
#include "DetDesc/Layering.h"

using namespace std;
using namespace DetDesc;
using namespace DetDesc::Geometry;

namespace DetDesc { namespace Geometry {
  
  static void placeStaves(LCDD&         lcdd, 
			  const string& detName, 
			  double        rmin, 
			  int           numsides, 
			  double        total_thickness, 
			  Volume        envelopeVolume, 
			  double        innerAngle, 
			  Volume        sectVolume)
  {
    double innerRotation    = innerAngle;
    double offsetRotation   = -innerRotation / 2;
    double sectCenterRadius = rmin + total_thickness / 2;
    double rotX =  M_PI / 2;
    double rotY = -offsetRotation;
    double posX = -sectCenterRadius  * std::sin(rotY);
    double posY =  sectCenterRadius  * std::cos(rotY);
    string nam =   sectVolume.name();
    //numsides = 2;
    for (int module = 0; module < numsides; ++module)  {
      Position position(lcdd, detName + _toString(module,"_stave_module%d_position"),-posX,-posY,0);
      Rotation rotation(lcdd, detName + _toString(module,"_stave_module%d_rotation"),rotX,rotY,0);

      PhysVol sectPhysVol(sectVolume);
      envelopeVolume.addPhysVol(sectPhysVol,position,rotation);
      sectPhysVol.addPhysVolID(_X(stave), 0);
      sectPhysVol.addPhysVolID(_X(module),module);

      rotY -=  innerRotation;
      posX  = -sectCenterRadius * std::sin(rotY);
      posY  =  sectCenterRadius * std::cos(rotY);
    }
  }

  template <> Ref_t DetElementFactory<PolyhedraBarrelCalorimeter2>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
    xml_det_t   x_det     = e;
    xml_comp_t  staves    = x_det.child(_X(staves));
    xml_dim_t   dim       = x_det.dimensions();

    string      det_name  = x_det.nameStr();
    string      det_type  = x_det.typeStr();
    Material    air       = lcdd.material(_X(Air));
    Material    gapMat    = air;
    Layering    layering(x_det);
    double      totalThickness = layering.totalThickness();
    int         totalRepeat = 0;
    int         totalSlices = 0;
    double      gap         = xml_dim_t(x_det).gap();
    int         numSides    = dim.numsides();
    double      detZ        = dim.z();
    double      rmin        = dim.rmin();
    double      zrot        = M_PI/numSides;
    Rotation    rot(lcdd,det_name+"_rotation",0,0,zrot);
    Position    pos_identity = lcdd.position("identity_pos");
    DetElement  sdet(lcdd,det_name,det_type,x_det.id());
    Volume      motherVol = lcdd.pickMotherVolume(sdet);

    cout << det_name << "  Gap:" << gap << endl;

    for(xml_coll_t c(x_det,_X(layer)); c; ++c)  {
      xml_comp_t x_layer = c;
      int repeat = x_layer.repeat();
      totalRepeat += repeat;
      totalSlices += x_layer.numChildren(_X(slice));
    }

    PolyhedraRegular polyhedra(lcdd,det_name+"_polyhedra",numSides,rmin,rmin+totalThickness,detZ);
    Volume           envelopeVol(lcdd,det_name+"_envelope",polyhedra,air);

    // Add the subdetector envelope to the structure.
    double innerAngle     = 2*M_PI/numSides;
    double halfInnerAngle = innerAngle/2;
    double tan_inner      = std::tan(halfInnerAngle) * 2;
    double innerFaceLen   = rmin * tan_inner;
    double outerFaceLen   = (rmin+totalThickness) * tan_inner;
    double staveThickness = totalThickness;

    Trapezoid staveTrdOuter(lcdd,det_name+"_stave_trapezoid_outer",
			    innerFaceLen/2,outerFaceLen/2,detZ/2,detZ/2,staveThickness/2);
    Volume staveOuterVol(lcdd,det_name+"_stave_outer",staveTrdOuter,air);

    Trapezoid staveTrdInner(lcdd,det_name+"_stave_trapezoid_inner",
			    innerFaceLen/2-gap,outerFaceLen/2-gap,detZ/2,detZ/2,staveThickness/2);
    Volume staveInnerVol(lcdd,det_name+"_stave_inner",staveTrdInner,air);

    double layerOuterAngle = (M_PI-innerAngle)/2;
    double layerInnerAngle = (M_PI/2 - layerOuterAngle);
    double layer_pos_z = -(staveThickness / 2);                        
    double layer_dim_x = innerFaceLen/2 - gap * 2;
    int layer_number = 0;

    sdet.setVolume(envelopeVol).setEnvelope(polyhedra);
    // Set envelope volume attributes.
    sdet.setAttributes(lcdd, envelopeVol,
		       x_det.attr<string>(_A(region)),
		       x_det.attr<string>(_A(limits)),
		       x_det.visStr());

    for(xml_coll_t c(x_det,_X(layer)); c; ++c)  {
      xml_comp_t   x_layer_element = c;
      int          repeat = x_layer_element.repeat();     // Get number of times to repeat this layer.
      const Layer* lay    = layering.layer(layer_number); // Get the layer from the layering engine.
      // Loop over repeats for this layer.
      for (int j = 0; j < repeat; j++)    {                
	string layer_name = det_name+_toString(layer_number,"_stave_layer%d");
	double layer_thickness = lay->thickness();
	DetElement  layer(lcdd,layer_name,det_name+"/Layer",x_det.id());

	// Layer position in Z within the stave.
	layer_pos_z += layer_thickness / 2;
	// Position of layer.
	Position layer_position(lcdd,layer_name+"_position", 0, 0, layer_pos_z);
	// Layer box.
	Box layer_box(lcdd,layer_name+"_box", layer_dim_x, detZ/2, layer_thickness);
	// Layer volume. 
	Volume layer_volume(lcdd,layer_name,layer_box,air);

	// Create the slices (sublayers) within the layer.
	double slice_pos_z = -(layer_thickness / 2);
	int slice_number = 0;
	for(xml_coll_t k(x_layer_element,_X(slice)); k; ++k)  {
	  xml_comp_t x_slice = k;
	  string   slice_name      = layer_name + _toString(slice_number,"_slice%d");
	  double   slice_thickness = x_slice.thickness();
	  Material slice_material  = lcdd.material(x_slice.materialStr());
	  DetElement slice(lcdd,slice_name,det_name+"/Layer/Slice",x_det.id());

	  slice_pos_z += slice_thickness / 2;
	  // Slice Position.
	  Position slice_position(lcdd,slice_name+"_position",0,0,slice_pos_z);
	  // Slice box. 
	  Box slice_box(lcdd,slice_name+"_box",layer_dim_x,detZ/2,slice_thickness);

	  // Slice volume.
	  Volume slice_volume(lcdd,slice_name,slice_box,slice_material);
	  if ( x_slice.isSensitive() ) slice_volume.setSensitiveDetector(sens);
	  // Set region, limitset, and vis.
	  slice.setAttributes(lcdd, slice_volume,
			      x_slice.attr<string>(_A(region)),
			      x_slice.attr<string>(_A(limits)),
			      x_slice.visStr());
	  // slice PhysVol
	  PhysVol slice_physvol(slice_volume);
	  slice_physvol.addPhysVolID(_X(slice),slice_number);
	  layer_volume.addPhysVol(slice_physvol,slice_position);

	  layer.add(slice);
	  // Increment Z position for next slice.
	  slice_pos_z += slice_thickness / 2;
	  // Increment slice number.
	  ++slice_number;             
	}
	// Set region, limitset, and vis.
	layer.setAttributes(lcdd, layer_volume,
			    x_layer_element.attr<string>(_A(region)),
			    x_layer_element.attr<string>(_A(limits)),
			    x_layer_element.visStr());

	// Layer physical volume.
	PhysVol layer_physvol(layer_volume);
	layer_physvol.addPhysVolID(_X(layer),layer_number);
	staveInnerVol.addPhysVol(layer_physvol,layer_position);

	sdet.add(layer);

	// Increment the layer X dimension.
	layer_dim_x += layer_thickness * std::tan(layerInnerAngle);// * 2;
	// Increment the layer Z position.
	layer_pos_z += layer_thickness / 2;
	// Increment the layer number.
	++layer_number;         
      }
    }

    // Make stave inner physical volume.
    PhysVol staveInnerPhysVol(staveInnerVol);
    // Add stave inner physical volume to outer stave volume.
    staveOuterVol.addPhysVol(staveInnerPhysVol,pos_identity);

    // Set the vis attributes of the outer stave section.
    sdet.setVisAttributes(lcdd,staves.visStr(),staveOuterVol);

    // Place the staves.
    placeStaves(lcdd,det_name,rmin,numSides,totalThickness,envelopeVol,innerAngle,staveOuterVol);

    PhysVol envelopePhysVol(envelopeVol);
    envelopePhysVol.addPhysVolID(_X(system), sdet.id());
    envelopePhysVol.addPhysVolID(_X(barrel), 0);
    motherVol.addPhysVol(envelopePhysVol,pos_identity,rot);
    return sdet;
  }
}}
DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,PolyhedraBarrelCalorimeter2);
