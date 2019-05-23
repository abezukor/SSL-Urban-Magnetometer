import sys

if(len(sys.argv)>=3):
    with open(sys.argv[1], 'r') as file:
        data = file.read().replace('\n', '').replace(':', '\n :').replace(" : Log \n :\n", "")
    goodData = [d for d in data.split('\n') if d[:5] == ' :/0 ']
    formatted = open(sys.argv[2],'w')
    formatted.write("\n".join(goodData))
    formatted.close()
    print("Done")
else:
    print("Put file names in arguments.")