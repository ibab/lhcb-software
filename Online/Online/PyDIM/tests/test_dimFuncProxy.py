import pydim, time

counter = 2
def myDummyFunc():
    global counter 
    counter += 1
    if counter%3 == 1:
         return (1, {1:(1,2,3)}, 'CUCU')
    elif counter%3 == 2:
         return (1, {12312 : ('fdsa',1,2,3), 'fdsa': 434432243})
    else:
        raise Exception('Unimplemented')

if __name__=='__main__':
    pydim.PyDimRpcProxy((myDummyFunc,))
    pydim.dis_start_serving()
    while True:
         time.sleep(1)
