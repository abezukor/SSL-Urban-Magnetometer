import sys
import csv
import os

#Function return true if all elements of values are can be floats, otherwise it returns false
def isfloats(values):
    for value in values:
        try:
            float(value)
        except ValueError:
            return False
    return True

try:
    data = ""
    
    fileDirPath = sys.argv[1].split("/") #Get the path and split by /
    fileNames = [int(name) for name in os.listdir(sys.argv[1])] #Get the names of all files in that directory
    fileNames.sort()#Sort those names
    fileNames = [str(name) for name in fileNames]#Put back to string
    fileNamesWPath = ["/".join(fileDirPath[:len(fileDirPath)-1])+"/"+name for name in fileNames] #list of names w/ path
    
    #Read all the data
    for fileName in fileNamesWPath:
        with open(fileName, 'r') as file:
            data += file.read()
    

    csvlines = [[idx] + val.split(" ") for idx,val in enumerate(data.split("\n"))] #Split by line and add numeric Value
    
    #Clean the Data
    csvlines= [l for l in csvlines if "-nan" not in l] #no nans
    csvlines= [l for l in csvlines if len(l)==6]#All length 6
    csvlines= [l for l in csvlines if isfloats(l[3:])]#Things that should be floats are

    #Write new CSV File
    with open(sys.argv[2],'w', newline="") as csvFile:
        csvWriter = csv.writer(csvFile,delimiter=",")
        csvWriter.writerow(["Num", "Device", "SensorNum", "VectX","VectY","VectZ"])#Write header row
        csvWriter.writerows(csvlines) #Write Data
    
except Exception as e:
    print('''There has been an error. The syntax for using this program is 
    \"python3 makeCSV.py [PATH TO FOLDER CONTAINING DATA FILES] [PATH TO OUTPUT CSV LOCATION]\"
    '''.replace("\n",""))
    print("The error was:", e)
