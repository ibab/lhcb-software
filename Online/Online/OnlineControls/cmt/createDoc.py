import os, pydoc
if __name__=="__main__":
  try:
    os.mkdir('../html')
  except:
    pass
  os.chdir('../html')
  pydoc.writedocs('../python')
