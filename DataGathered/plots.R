#How to run: source("DataGathered/plots.R")
#Take in data
inData <- read.csv(file="DataGathered/NightData.csv",row.names=NULL)
#Remove nan lines. Should not be nessassary
inData <- inData[complete.cases(inData), ]
#Make a plot with X Vect
plot(inData$Num,inData$VectX, col='red',xlab="Number",ylab="Vector Value",
ylim=c(min(c(min(inData$VectX),min(inData$VectY),min(inData$VectZ))),
max(c(max(inData$VectX),max(inData$VectY),max(inData$VectZ)))))

points(y=inData$VectY,x=inData$Num,col='green')#Add Y vect
points(y=inData$VectZ,x=inData$Num,col='blue')#Add Z Vect

legend(1, 95, legend=c("X Vector","Y Vector","Z Vector"), col=c("red", "green","blue"), lty=1:2, cex=0.8)#Add legend