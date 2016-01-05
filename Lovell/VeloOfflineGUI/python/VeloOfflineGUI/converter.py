import pickle
f = open('channelMapper.txt', 'r')
i = 0
dict = {}
for line in f:
    if i>112: 
        i+=1
        continue
    if i<25: 
        i+=1
        continue
    sensor = int(line.split()[-2][0:-1])
    not_sensor = line.split()[-4][1:-2]
    dict[sensor] = not_sensor
    
    i += 1

pickle.dump(dict, open("sensor_mapping.p", "wb"))
