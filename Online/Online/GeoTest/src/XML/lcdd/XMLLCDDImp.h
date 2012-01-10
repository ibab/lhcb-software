#ifndef DETDESC_XMLLCDDIMP_H
#define DETDESC_XMLLCDDIMP_H
#include "XML/lcdd/XMLLCDD.h"
#include "XML/lcdd/XMLObjects.h"
#include "XML/lcdd/XMLVolumes.h"
#include "XML/lcdd/XMLShapes.h"
#include "XML/DocumentHandler.h"

// C++ include files
#include <map>

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  /*
  *   XML namespace declaration
  */
  namespace XML  {

    class LCDDImp : public LCDD, public DocumentHandler  {
    public:
      typedef std::map<std::string,Handle_t> ReadoutMap;

      xercesc::DOMDocument *m_doc;
      ReadoutMap    m_readouts;
      Handle_t      m_root;
      Header        m_header;
      Handle_t      m_idDict;
      Handle_t      m_limits;
      Handle_t      m_regions;
      Handle_t      m_detectors;
      Handle_t      m_display;
      Handle_t      m_fields;

      // GDML fields
      Handle_t      m_gdml;
      Handle_t      m_define;
      Handle_t      m_structure;
      Handle_t      m_materials;
      Handle_t      m_solids;


      Handle_t      m_worldVol;
      Handle_t      m_trackingVol;
      Handle_t      m_setup;

      void convertMaterials(const std::string& uri);
      void convertMaterials(Handle_t doc_element);

      LCDDImp();
      void fromCompact(Handle_t doc_element);
      void fromCompact(const std::string& fname);

      virtual Document create();
      virtual void addStdMaterials();
      virtual void endDocument();

      virtual RefElement getRefChild(Handle_t e, const XMLCh* name, bool throw_if_not=true)  const;
      virtual RefElement getRefChild(Handle_t e, const XMLCh* tag, const XMLCh* name, bool throw_if_not=true)  const;
      virtual RefElement pickMotherVolume(const Subdetector& sd) const;

      virtual RefElement limitSet(const XMLCh* name)  const
      {  return getRefChild(m_limits,name);                           }  
      virtual RefElement visAttributes(const XMLCh* name) const
      {  return getRefChild(m_display,name,false);                     }  
      virtual RefElement material(const XMLCh* name)  const
      {  return getRefChild(m_materials,name);                         }
      virtual RefElement region(const XMLCh* name)  const
      {  return getRefChild(m_regions,name);                           }
      virtual RefElement idSpec(const XMLCh* name)  const
      {  return getRefChild(m_idDict,name);                            }
      virtual RefElement volume(const XMLCh* name)  const
      {  return getRefChild(m_structure,name);                         }
      virtual RefElement define(const XMLCh* tag, const XMLCh* name) const
      {  return getRefChild(m_define,tag,name);                        }
      virtual RefElement rotation(const XMLCh* name) const 
      {  return getRefChild(m_define,Tag_rotation,name);               }
      virtual RefElement position(const XMLCh* name) const 
      {  return getRefChild(m_define,Tag_position,name);               }
      virtual RefElement solid(const XMLCh* name) const 
      {  return getRefChild(solids(),name);                            }
      virtual RefElement constant(const XMLCh* name) const 
      {  return getRefChild(m_define,Tag_constant,name);               }
      virtual RefElement readout(const XMLCh* name)  const; 

      virtual Document document() const                { return Document(m_doc);        }
      virtual Handle_t header()  const                 { return m_header;               }
      virtual Handle_t constants() const               { return m_define;               }
      virtual Handle_t structure()  const              { return m_structure;            }
      virtual Handle_t solids()  const                 { return m_solids;               }
      virtual Handle_t limitset()  const               { return m_limits;               }
      virtual Handle_t regions() const                 { return m_regions;              }
      virtual Handle_t materials()  const              { return m_materials;            }
      virtual Handle_t worldVolume() const             { return m_worldVol;             }
      virtual Handle_t trackingVolume() const          { return m_trackingVol;          }
#define __R  return *this
      virtual LCDD& add(const Constant& constant)      { constants().append(constant);__R;}
      virtual LCDD& add(const Solid& solid)            { solids().append(solid);      __R;}
      virtual LCDD& add(const Volume& vol)             { structure().append(vol);     __R;}
      virtual LCDD& add(const Material& mat)           { materials().append(mat);     __R;}
      virtual LCDD& add(const Position& pos)           { constants().append(pos);     __R;}
      virtual LCDD& add(const Rotation& rot)           { constants().append(rot);     __R;}
      virtual LCDD& add(const LimitSet& limset)        { limitset().append(limset);   __R;}
      virtual LCDD& add(const Region& region)          { regions().append(region);    __R;}
      virtual LCDD& add(const VisAttr& attr)           { m_display.append(attr);      __R;}


      virtual LCDD& addConstant(const RefElement& element)      { m_define.append(element);    __R;}
      virtual LCDD& addSolid(const RefElement& solid)           { solids().append(solid);      __R;}
      virtual LCDD& addVolume(const RefElement& vol)            { structure().append(vol);     __R;}
      virtual LCDD& addMaterial(const RefElement& element)      { m_materials.append(element); __R;}
      virtual LCDD& addVisAttribute(const RefElement& element)  { m_display.append(element);   __R;}
      virtual LCDD& addSensitiveDetector(const RefElement& e)   { m_detectors.append(e);       __R;}
      virtual LCDD& addLimitSet(const RefElement& limset)       { limitset().append(limset);   __R;}
      virtual LCDD& addRegion(const RefElement& region)         { regions().append(region);    __R;}
      virtual LCDD& addIDSpec(const RefElement& element)        { m_idDict.append(element);    __R;}
      virtual LCDD& addReadout(const RefElement& readout);
#undef __R
    };
  }
}         /* End namespace DetDesc   */
#endif    /* DETDESC_XMLLCDDIMP_H    */
