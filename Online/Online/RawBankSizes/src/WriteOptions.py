f=open("/tmp/banks.in","r")
g=open("../options/RawSize_HistParams.opts","r+")
banks = f.readlines()
opts = g.readlines()
opts.append("shit")
newbanks = []
f.close()
#print banks
#print opts
print "========= Running WriteOptions.py...."
#print (len(banks), len(opts))
for i in range(len(banks)):
  bank = banks[i].split("\n")
  banks[i] = bank[0]
  bank = banks[i].split(" ")
  banks[i] = bank[0];
  for j in range(len(opts)):
    if opts[j].find("//")<0:
      ii=opts[j].find(bank[0])
      if ii>=0:
        break
  if j == len(opts)-1:
    print("No options found for bank ",bank[0]," adding index to newbanks")
    newbanks.append(i)
opts.remove("shit") #len(opts)-1)
#print newbanks

for i in range(len(newbanks)):
  print "Adding Default Options for "+banks[newbanks[i]]
  opts.append("RawBankSizes."+banks[newbanks[i]]+"=(100,0.0,10000.0,0,1024,\"Unknown\");"+"\r\n")
#print(opts)
opts.sort()
#print(opts)
g.seek(0)
g.writelines(opts)
g.close()
