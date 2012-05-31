import os
import importUtils, genClassDicts

#================================================================================
class genAssocDicts(importUtils.importUtils,genClassDicts.genClassDicts):
#--------------------------------------------------------------------------------
  def __init__(self,godRoot,dictOutput,srcOutput):
    importUtils.importUtils.__init__(self)
    genClassDicts.genClassDicts.__init__(self,godRoot,dictOutput,srcOutput)
#--------------------------------------------------------------------------------
  def genPackageDict(self,godPackage):
    if godPackage.has_key('assoc'):
      assocNum = 0
      for assoc in godPackage['assoc']:
        assocAtt = assoc['attrs']
        wgt = ''
        wgtT = ''
        if assocAtt['weight'] != 'NONE' :
          wgt = 'Weighted'
          wgtT = ','+assocAtt['weight']
        assocT = 'Relation%s%s<%s,%s%s>' % (wgt, assocAtt['type'], assocAtt['from'], assocAtt['to'], wgtT )
        self.sDictInstances = self.conc(self.sDictInstances, '%s m_assoc_%d;' % (assocT, assocNum))
        self.sClassSelections = self.conc(self.sClassSelections, '<class name="%s"/>' % assocT)
        assocNum += 1
    self.sIncludes = self.genIncludes().split('\n')
    self.sIncludes.append('#include "Relations/Relation1D.h"')
    self.sIncludes.append('#include "Relations/Relation2D.h"')
    self.sIncludes.append('#include "Relations/RelationWeighted1D.h"')
    self.sIncludes.append('#include "Relations/RelationWeighted2D.h"')
