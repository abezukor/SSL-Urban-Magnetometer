import sys

def isInt(i):
    try:
        int(i)
    except:
        return False
    return True
def has3(i):
    sep = i.split(" ")
    try:
        float(sep[1])
        float(sep[2])
        float(sep[3])
    except:
        return False
    return True
isConsecutive = lambda i, j: i+1==j or (i==99 and j == 0)

if(len(sys.argv)>=2):
    with open(sys.argv[1], 'r') as file:
        data = file.read().replace("\n","")
    colons = data.split(":/0 ")

    has3float = [d for d in colons if has3(d)]
    print(len(has3float)/len(colons), " Have 3 arguments")

    nums = [(int(d[:2]),d) for d in colons if isInt(d[:2])]
    cons = [(isConsecutive(nums[i][0], nums[i+1][0]),i,nums[i][1]) for i in range(0,len(nums)-1)]
    isCons = [i[0] for i in cons if i[0]]
    print(len(isCons)/len(cons), " are consecutive")

else:
    print("Put file names in arguments.")