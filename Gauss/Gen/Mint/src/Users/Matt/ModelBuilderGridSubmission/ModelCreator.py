import os

class ModelCreator:
    constDecays = ['D0->a(1)(1260)+[D](->rho(770)0(->pi+,pi-),pi+),pi-',
                  'D0->a(1)(1260)+(->sigma10(->pi+,pi-),pi+),pi-',
                  'D0->rho(770)0(->pi+,pi-),rho(770)0(->pi+,pi-)',
                  'D0[P]->rho(770)0(->pi+,pi-),rho(770)0(->pi+,pi-)',
                  'D0[D]->rho(770)0(->pi+,pi-),rho(770)0(->pi+,pi-)']
    
    optionalDecays = ['FocusFlatteD0->NonResVbar0(->pi+,pi-),f(0)(980)0(->pi+,pi-)',
                      'D0->sigma10(->pi+,pi-),pi+,pi-']
    modelsUsed = []
    model = []
    fixedDecay = 'D0->a(1)(1260)+(->rho(770)0(->pi+,pi-),pi+),pi-'
    def __init__(self):
        for decay in self.constDecays:
            self.modelsUsed.append(decay)
#        for decay in self.optionalDecays:
#            self.modelsUsed.append(decay)
    
    def MakeModel(self):
        self.model.append('"'+self.fixedDecay+'_Amp"    1  30.1  0.1  0.0  0.0\n')
        self.model.append('"'+self.fixedDecay+'_Phase"    1  100  0.5  0.0  0.0\n\n')
        for model in self.modelsUsed:
            self.model.append('"'+model+'_Amp"    0  0.1  0.1  0.0  0.0\n')
            self.model.append('"'+model+'_Phase"  0  100  0.5  0.0  0.0\n')
            self.model.append('*\n')
        return model
    
    def Write(self,fileName):
        file = open(fileName,"w")
        
        #Write Fixed Decay
        file.writelines('"'+self.fixedDecay+'_Amp"    1  30.1  0.1  0.0  0.0\n')
        file.writelines('"'+self.fixedDecay+'_Phase"    1  100  0.5  0.0  0.0\n\n')

        for model in self.modelsUsed:
            file.writelines('"'+model+'_Amp"    0  0.1  0.1  0.0  0.0\n')
            file.writelines('"'+model+'_Phase"  0  100  0.5  0.0  0.0\n')
            file.writelines('*\n')
        file.close()
    
    def MakeInputFile(self,inputBaseFile,outputFile):
        self.MakeModel()
        file = open(inputBaseFile)
        outputlines=[]
        while 1:
            line = file.readline()# read file
            if not line:
                    break
            pass
            outputlines.append(line)
        file.close()
        
        for decay in self.model:
            outputlines.append(decay)
        
        outFile = open(outputFile,"w")
        outFile.writelines(outputlines)
        outFile.close()
        
        
"""
Example :
           
modelsForIt = open(modelTextFile,"w")
decs = ModelCreator()
optionalDecays = decs.optionalDecays
for j in range(0,len(optionalDecays)):
    myModel = ModelCreator()
    myModel.modelsUsed.append(optionalDecays[j])
    myModel.MakeInputFile("D_4pi_Base.txt", "D_4pi_"+str(j)+".txt")
    modelsForIt.writelines("D_4pi_"+str(j)+".txt\n")
modelsForIt.close()

"""
           