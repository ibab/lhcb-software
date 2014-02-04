import os

TOP_LEVEL = ('run1', 'run2', 'run3', 'run4')
FD1 = 450
FD2 = ('file1', 'file2', 'file3')

def safe_make_folder(i):
    '''Makes a folder and its parent if not present'''
    try:
        os.makedirs(i)
    except:
        pass

def make_top_level():
    '''Makes folders with no subdirectories'''
    for i in TOP_LEVEL:
        safe_make_folder(i)

def make_folders():
    '''Makes folder and its subdirectories'''
    for j in (os.path.join('FDR', '{:03}_{:03}'.format(i, i + 49))
              for i in range(1,FD1, 50)):
        safe_make_folder(j)

def make_folders2():
    '''Makes codechef and its subdirectories'''
    for i in FD2:
        safe_make_folder(os.path.join('FDR2', i))

def main():
    make_top_level()
    make_folders()
    make_folders2()

if __name__ == "__main__":
    main()
