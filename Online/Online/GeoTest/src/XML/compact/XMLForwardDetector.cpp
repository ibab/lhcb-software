#define _USE_MATH_DEFINES
#include "XML/compact/XMLForwardDetector.h"
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

Subdetector& ForwardDetector::fromCompact(LCDD& lcdd, const SensitiveDetector& sens)  {
  Document    doc        = lcdd.document();
  Component   compact    = handle();
  Tag_t       det_name   = name();
  Dimension   dim        = dimensions();
  Material    air        = lcdd.material(Tag_Air);
  int         id         = compact.id();
  Volume      motherVol  = lcdd.pickMotherVolume(*this);
  Rotation    reflectRot = lcdd.rotation(Tag_reflect_rot);
  bool        reflect    = compact.reflect();
  Component   beam(compact.child(Tag_beampipe));

  double      rmax       = dim.outer_r();
  double      rmin       = dim.inner_r();
  double      zinner     = dim.inner_z();
  double      outgoingR  = beam.outgoing_r();
  double      incomingR  = beam.incoming_r();
  double      xangle     = beam.crossing_angle();
  double      xangleHalf = xangle / 2;
  double      thickness  = Layering(compact).totalThickness();
  double      zpos       = zinner + (thickness / 2);
  double      zouter     = zinner + thickness;
  // Beampipe position in envelope.
  double beamPosX        = tan(xangleHalf) * zpos;

  // Detector envelope solid. 
  Tube envelopeTube(doc,det_name + "_tube");
  envelopeTube.setDimensions(rmin,rmax,thickness);
  lcdd.add(envelopeTube);
  // Incoming beampipe solid.
  Tube beamInTube(doc,det_name + "_beampipe_incoming_tube");
  beamInTube.setDimensions(0.,outgoingR,thickness * 2);
  lcdd.add(beamInTube);
  // Outgoing beampipe solid.
  Tube beamOutTube(doc,det_name + "_beampipe_outgoing_tube");
  beamOutTube.setDimensions(0,incomingR,thickness * 2);
  lcdd.add(beamOutTube);
  // First envelope bool subtraction of incoming beampipe.
  SubtractionSolid envelopeSubtraction1(doc,det_name + "_subtraction1_tube");
  envelopeSubtraction1.setFirstSolid(envelopeTube);
  envelopeSubtraction1.setSecondSolid(beamInTube);
  lcdd.add(envelopeSubtraction1);
  // Position of incoming beampipe.
  Position beamInPos(doc,det_name + "_subtraction1_tube_pos");
  beamInPos.setX(beamPosX);
  envelopeSubtraction1.setPosition(beamInPos);
  lcdd.add(beamInPos);
  
  /** Rotation of incoming beampipe. */
  Rotation beamInRot(doc,det_name + "_subtraction1_tube_rot");
  beamInRot.setY(xangleHalf);
  envelopeSubtraction1.setRotation(beamInRot);
  lcdd.add(beamInRot);

  // Second envelope bool subtracion of outgoing beampipe.
  SubtractionSolid envelopeSubtraction2(doc,det_name + "_subtraction2_tube");
  envelopeSubtraction2.setFirstSolid(envelopeSubtraction1).setSecondSolid(beamOutTube);
  lcdd.add(envelopeSubtraction2);
  
  // Position of outgoing beampipe.
  Position beamOutPos(doc,det_name + "_subtraction2_tube_pos");
  beamOutPos.setX(-beamPosX);
  // Rotation of outgoing beampipe.
  Rotation beamOutRot(doc,det_name + "_subtraction2_tube_rot");
  beamOutRot.setY(-xangleHalf);

  envelopeSubtraction2.setPosition(beamOutPos).setRotation(beamOutRot);
  lcdd.add(beamOutPos);
  lcdd.add(beamOutRot);
  
  // Final envelope bool volume.
  Volume envelopeVol(doc,det_name + "_envelope_volume", envelopeSubtraction2, air);

  // Process each layer element.
  double layerPosZ   = -thickness / 2;
  double layerDisplZ = 0;
  for(Collection_t c(compact,Tag_layer); c; ++c)  {
    Component layer = c;
    Layering layering(layer);
    double layerThickness = layering.singleLayerThickness(layer);
    // Create tube envelope for this layer, which can be reused in bool definition
    // in the repeat loop below.
    Tube layerTube(doc, det_name + "_layer_tube");
    layerTube.setDimensions(rmin,rmax,layerThickness);
    lcdd.add(layerTube);

    for(int i=0, m=0, repeat=layer.repeat(); i<repeat; ++i, m=0)  {
      Tag_t  layer_nam = det_name + _toString(i,"_layer%d");
      // Increment to new layer position.
      layerDisplZ += layerThickness / 2;
      layerPosZ   += layerThickness / 2;
      // First layer subtraction solid.
      SubtractionSolid layerSubtraction1(doc,layer_nam + "_subtraction1");
      layerSubtraction1.setFirstSolid(layerTube).setSecondSolid(beamInTube);
      lcdd.add(layerSubtraction1);

      Position layerSubtraction1Pos(doc,layer_nam + "_subtraction1_pos");
      double layerGlobalZ = zinner + layerDisplZ;
      double layerPosX    = tan(xangleHalf) * layerGlobalZ;
      layerSubtraction1Pos.setX(layerPosX);
      lcdd.add(layerSubtraction1Pos);

      layerSubtraction1.setPosition(layerSubtraction1Pos);
      layerSubtraction1.setRotation(beamInRot);
      // Second layer subtraction solid.
      SubtractionSolid layerSubtraction2(doc,layer_nam + "_subtraction2");
      layerSubtraction2.setFirstSolid(layerSubtraction1).setSecondSolid(beamOutTube);
      lcdd.add(layerSubtraction2);
      
      Position layerSubtraction2Pos(doc,layer_nam + "_subtraction2_pos");
      layerSubtraction2Pos.setX(-layerPosX);
      lcdd.add(layerSubtraction2Pos);
      layerSubtraction2.setPosition(layerSubtraction2Pos).setRotation(beamOutRot);
      
      // Layer LV.
      Volume layerVol(doc,layer_nam + "_volume", layerSubtraction2, air);
      
      // Slice loop.
      int sliceCount = 0;
      double slicePosZ = -layerThickness / 2;
      double sliceDisplZ = 0;
      for(Collection_t l(layer,Tag_slice); l; ++l, ++m)  {
        Component slice = l;
        Tag_t slice_nam = layer_nam + _toString(sliceCount,"_slice%d");
        /** Get slice parameters. */
        double sliceThickness = slice.thickness();
        Material slice_mat(lcdd.material(slice.material()));

        // Go to mid of this slice.
        sliceDisplZ += sliceThickness / 2;
        slicePosZ   += sliceThickness / 2;

        // Slice's basic tube.
        Tube sliceTube(doc, slice_nam + "_tube");
        sliceTube.setDimensions(rmin,rmax,sliceThickness);
        lcdd.add(sliceTube);

        // First slice subtraction solid.
        SubtractionSolid sliceSubtraction1(doc,slice_nam + "_subtraction1");
        sliceSubtraction1.setFirstSolid(sliceTube).setSecondSolid(beamInTube);
        lcdd.add(sliceSubtraction1);
        
        Position sliceSubtraction1Pos(doc,slice_nam + "_subtraction1_pos");
        double sliceGlobalZ = zinner + (layerDisplZ - layerThickness / 2) + sliceDisplZ;
        double slicePosX    = tan(xangleHalf) * sliceGlobalZ;
        sliceSubtraction1Pos.setX(slicePosX);
        lcdd.add(sliceSubtraction1Pos);
        
        sliceSubtraction1.setPosition(sliceSubtraction1Pos).setRotation(beamInRot);
 
        // Second slice subtraction solid.
        SubtractionSolid sliceSubtraction2(doc,slice_nam + "_subtraction2");
        sliceSubtraction2.setFirstSolid(sliceSubtraction1).setSecondSolid(beamOutTube);
        lcdd.add(sliceSubtraction2);
       
        Position sliceSubtraction2Pos(doc,slice_nam + "_subtraction2_pos");
        sliceSubtraction2Pos.setX(-slicePosX);
        lcdd.add(sliceSubtraction2Pos);
        
        sliceSubtraction2.setPosition(sliceSubtraction2Pos).setRotation(beamOutRot);
        
        // Slice LV.
        Volume sliceVol(doc,slice_nam + "_volume");
        sliceVol.setMaterial(slice_mat);
        lcdd.add(sliceSubtraction2);
        
        if ( slice.isSensitive() )        {
          sliceVol.setSensitiveDetector(sens);
        }

        setAttributes(lcdd, compact, sliceVol);
        lcdd.add(sliceVol);
        
        // Slice PV.
        PhysVol slicePV(doc,sliceVol);
        slicePV.setZ(slicePosZ);
        layerVol.addPhysVol(slicePV);
        // Start of next slice.
        sliceDisplZ += sliceThickness / 2;
        slicePosZ   += sliceThickness / 2;
        ++sliceCount;
      }
      setVisAttributes(lcdd, compact, layerVol);
      lcdd.add(layerVol);

      // Layer PV.
      PhysVol layerPV(doc,layerVol);
      layerPV.setZ(layerPosZ);
      layerPV.addPhysVolID(Tag_layer, i);
      envelopeVol.addPhysVol(layerPV);
      // Increment to start of next layer.
      layerDisplZ += layerThickness / 2;
      layerPosZ   += layerThickness / 2;
    }
  }
  setVisAttributes(lcdd, compact, envelopeVol);
  
  lcdd.add(envelopeVol);                // Add envelope LV.
  PhysVol envelopePV(doc,envelopeVol);  // Add envelope PV.
  envelopePV.setZ(zpos);
  envelopePV.addPhysVolID(Tag_system, id);
  envelopePV.addPhysVolID(Tag_barrel, 1);
  motherVol.addPhysVol(envelopePV);
  
  // Reflect it.
  if ( reflect )  {
    PhysVol envelopePV2(doc,envelopeVol);
    envelopePV2.setZ(-zpos).setRotation(reflectRot);
    envelopePV2.addPhysVolID(Tag_system, id);
    envelopePV2.addPhysVolID(Tag_barrel, 2);
    motherVol.addPhysVol(envelopePV2);
  }
  return *this;
}
