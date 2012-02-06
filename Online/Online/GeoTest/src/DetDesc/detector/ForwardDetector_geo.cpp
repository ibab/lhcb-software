// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetDesc/Layering.h"
#include "DetDesc/detector/DetFactoryHelper.h"
#include "DetDesc/detector/CompactDetectors.h"

using namespace std;
using namespace DetDesc;
using namespace DetDesc::Geometry;

namespace DetDesc {  namespace Geometry {
  
  template <> Ref_t DetElementFactory<ForwardDetector>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
    xml_det_t   x_det      = e;
    xml_dim_t   dim        = x_det.dimensions();
    bool        reflect    = xml_comp_t(x_det).reflect();
    xml_comp_t  beam       = x_det.child(XML::Tag_beampipe);
    string      det_name   = x_det.nameStr();
    string      det_type   = x_det.typeStr();
    int         id         = x_det.id();
    Layering    layering(x_det);

    Material    air        = lcdd.material(_X(Air));
    DetElement  sdet       (lcdd,det_name,det_type,x_det.id());

    Volume      motherVol  = lcdd.pickMotherVolume(sdet);

    double      rmax       = dim.outer_r();
    double      rmin       = dim.inner_r();
    double      zinner     = dim.inner_z();
    double      outgoingR  = beam.outgoing_r();
    double      incomingR  = beam.incoming_r();
    double      xangle     = beam.crossing_angle();
    double      xangleHalf = xangle / 2;
    double      thickness  = layering.totalThickness();
    double      zpos       = zinner + (thickness / 2);
    //double      zouter     = zinner + thickness;
    // Beampipe position in envelope.
    double      beamPosX   = std::tan(xangleHalf) * zpos;

    // Detector envelope solid. 
    Tube envelopeTube(lcdd,det_name+"_tube",rmin,rmax,thickness);

    // First envelope bool subtracion of outgoing beampipe.
    // Incoming beampipe solid.
    Tube beamInTube(lcdd,det_name + "_beampipe_incoming_tube",0,outgoingR,thickness * 2);
    // Position of incoming beampipe.
    Position beamInPos(lcdd,det_name + "_subtraction1_tube_pos",beamPosX,0,0);
    /// Rotation of incoming beampipe.
    Rotation beamInRot(lcdd,det_name + "_subtraction1_tube_rot",0,xangleHalf,0);

    // Second envelope bool subtracion of outgoing beampipe.
    // Outgoing beampipe solid.
    Tube     beamOutTube(lcdd,det_name + "_beampipe_outgoing_tube",0,incomingR,thickness * 2);
    // Position of outgoing beampipe.
    Position beamOutPos(lcdd,det_name + "_subtraction2_tube_pos",-beamPosX,0,0);
    // Rotation of outgoing beampipe.
    Rotation beamOutRot(lcdd,det_name + "_subtraction2_tube_rot",0,-xangleHalf,0);

    // First envelope bool subtraction of incoming beampipe.
    SubtractionSolid envelopeSubtraction1(lcdd,det_name+"_subtraction1_tube",
					  envelopeTube,beamInTube,beamInPos,beamInRot);
    SubtractionSolid envelopeSubtraction2(lcdd,det_name+"_subtraction2_tube",
					  envelopeSubtraction1,beamOutTube,beamInPos,beamOutRot);

    // Final envelope bool volume.
    Volume envelopeVol(lcdd,det_name + "_envelope_volume", envelopeSubtraction2, air);

    // Process each layer element.
    double layerPosZ   = -thickness / 2;
    double layerDisplZ = 0;
    for(xml_coll_t c(x_det,XML::Tag_layer); c; ++c)  {
      xml_comp_t x_layer = c;
      double layerThickness = layering.singleLayerThickness(x_layer);

      // Create tube envelope for this layer, which can be reused in bool definition
      // in the repeat loop below.
      Tube layerTube(lcdd, det_name + "_layer_tube",rmin,rmax,layerThickness);

      for(int i=0, m=0, repeat=x_layer.repeat(); i<repeat; ++i, m=0)  {
	string layer_nam = det_name + _toString(i,"_layer%d");
	// Increment to new layer position.
	layerDisplZ += layerThickness / 2;
	layerPosZ   += layerThickness / 2;

	// First layer subtraction solid.
	DetElement  layer(lcdd,layer_nam,"ForwardDetector/Layer",sdet.id());
	double      layerGlobalZ = zinner + layerDisplZ;
	double      layerPosX    = tan(xangleHalf) * layerGlobalZ;
	Position    layerSubtraction1Pos(lcdd,layer_nam + "_subtraction1_pos", layerPosX,0,0);
	Position    layerSubtraction2Pos(lcdd,layer_nam + "_subtraction2_pos",-layerPosX,0,0);

	SubtractionSolid layerSubtraction1(lcdd,layer_nam + "_subtraction1",
					   layerTube,beamInTube,layerSubtraction1Pos,beamInRot);
	// Second layer subtraction solid.
	SubtractionSolid layerSubtraction2(lcdd,layer_nam + "_subtraction2",
					   layerSubtraction1,beamOutTube,layerSubtraction2Pos,beamOutRot);
	// Layer LV.
	Volume layerVol(lcdd,layer_nam + "_volume", layerSubtraction2, air);
      
	// Slice loop.
	int sliceCount = 0;
	double slicePosZ = -layerThickness / 2;
	double sliceDisplZ = 0;
	for(xml_coll_t l(x_layer,XML::Tag_slice); l; ++l, ++m)  {
	  xml_comp_t x_slice = l;
	  string slice_nam = layer_nam + _toString(sliceCount,"_slice%d");
	  /** Get slice parameters. */
	  double sliceThickness = x_slice.thickness();
	  Material slice_mat = lcdd.material(x_slice.materialStr());

	  // Go to mid of this slice.
	  sliceDisplZ += sliceThickness / 2;
	  slicePosZ   += sliceThickness / 2;

	  // Slice's basic tube.
	  Tube sliceTube(lcdd, slice_nam + "_tube", rmin,rmax,sliceThickness);
	  DetElement slice(lcdd,slice_nam,"ForwardDetector/Layer/Slice",sdet.id());
	  double sliceGlobalZ = zinner + (layerDisplZ - layerThickness / 2) + sliceDisplZ;
	  double slicePosX    = tan(xangleHalf) * sliceGlobalZ;
	  Position sliceSubtraction1Pos(lcdd,slice_nam + "_subtraction1_pos",slicePosX,0,0);
	  Position sliceSubtraction2Pos(lcdd,slice_nam + "_subtraction2_pos",-slicePosX,0,0);

	  // First slice subtraction solid.
	  SubtractionSolid sliceSubtraction1(lcdd,slice_nam + "_subtraction1",
					     sliceTube,beamInTube,sliceSubtraction1Pos,beamInRot);
	  // Second slice subtraction solid.
	  SubtractionSolid sliceSubtraction2(lcdd,slice_nam + "_subtraction2",
					     sliceSubtraction1,beamOutTube,sliceSubtraction2Pos,beamOutRot); 
	  // Slice LV.
	  Volume sliceVol(lcdd,slice_nam + "_volume", sliceSubtraction2, slice_mat);
        
	  if ( x_slice.isSensitive() ) sliceVol.setSensitiveDetector(sens);
	  // Set attributes of slice
	  slice.setAttributes(lcdd, sliceVol, 
			      x_slice.attr<string>(_A(region)),
			      x_slice.attr<string>(_A(limits)),
			      x_slice.visStr());

	  // Slice PV.
	  PhysVol slicePV(sliceVol);
	  layerVol.addPhysVol(slicePV,Position(lcdd,slice_nam+"_pos",0,0,slicePosZ));
	  layer.add(slice);

	  // Start of next slice.
	  sliceDisplZ += sliceThickness / 2;
	  slicePosZ   += sliceThickness / 2;
	  ++sliceCount;
	}
	// Set attributes of slice
	layer.setAttributes(lcdd, layerVol, 
			    x_layer.attr<string>(_A(region)),
			    x_layer.attr<string>(_A(limits)),
			    x_layer.visStr());

	// Layer PV.
	PhysVol layerPV(layerVol);
	layerPV.addPhysVolID(_X(layer), i);
	envelopeVol.addPhysVol(layerPV,Position(lcdd,layer_nam+"_pos",0,0,layerPosZ));
	sdet.add(layer);

	// Increment to start of next layer.
	layerDisplZ += layerThickness / 2;
	layerPosZ   += layerThickness / 2;
      }
    }
    sdet.setVisAttributes(lcdd, x_det.visStr(), envelopeVol);
  
    PhysVol envelopePV(envelopeVol);  // Add envelope PV.
    envelopePV.addPhysVolID(_X(system), id);
    envelopePV.addPhysVolID(_X(barrel), 1);
    motherVol.addPhysVol(envelopePV,Position(lcdd,det_name+"_pos",0,0,zpos));
  
    // Reflect it.
    if ( reflect )  {
      Rotation reflectRot(lcdd.rotation(_X(reflect_rot)));
      PhysVol envelopePV2(envelopeVol);
      envelopePV2.addPhysVolID(_X(system), id);
      envelopePV2.addPhysVolID(_X(barrel), 2);
      motherVol.addPhysVol(envelopePV2,Position(lcdd,det_name+"_pos_reflect",0,0,-zpos),reflectRot);
    }
    return sdet;
  }
}}
DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,ForwardDetector);
