
logger.info  ( 'I am batch-script' ) 
logger.debug ( 'I am batch-script' ) 
print 'my Context:' , dir() 
h1  = ROOT.TH1D('h1','',10,0,10)
for i in h1 : h1[i] = VE(i,i)
h1 *= 10
h1.Draw()
canvas >> 'batch'
