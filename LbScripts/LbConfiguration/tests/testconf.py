from LbConfiguration import Project

if __name__ == '__main__':
    print Project.Gauss.CMTExtraTags()
    print Project.Gauss.FullSize()
    print Project.getProject("gaudi").FullSize()
    print Project.Gaudi
    print Project.Gauss
    print Project.getProject("toto").FullSize()

