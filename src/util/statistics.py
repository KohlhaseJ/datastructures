import numpy as np
import matplotlib.pyplot as plt

insFC = np.loadtxt('../out/FCFS_insertion.txt', dtype = float, delimiter=',')
datFC = np.loadtxt('../out/FCFS_search.txt', dtype = float, delimiter=',')
insLC = np.loadtxt('../out/LCFS_insertion.txt', dtype = float, delimiter=',')
datLC = np.loadtxt('../out/LCFS_search.txt', dtype = float, delimiter=',')
insRH = np.loadtxt('../out/RobinHood_insertion.txt', dtype = float, delimiter=',')
datRH = np.loadtxt('../out/RobinHood_search.txt', dtype = float, delimiter=',')
trials = np.unique(datFC[:,0])
loadFactors = np.unique(datFC[:,1])

x=[]
insy1min=[]
insy2min=[]
insy3min=[]
insy1max=[]
insy2max=[]
insy3max=[]
insy1avg=[]
insy2avg=[]
insy3avg=[]
y1min=[]
y2min=[]
y3min=[]
y1max=[]
y2max=[]
y3max=[]
y1avg=[]
y2avg=[]
y3avg=[]

#datFC = datFC[np.where(datFC[:,0]==0)[0],:]
#datLC = datLC[np.where(datFC[:,0]==0)[0],:]
#datRH = datRH[np.where(datFC[:,0]==0)[0],:]

for loadFactor in loadFactors:
    ins1 = insFC[np.where(insFC[:,1] == loadFactor)[0],2]
    prob1 = datFC[np.where(datFC[:,1] == loadFactor)[0],2]
    ins2 = insLC[np.where(insLC[:,1] == loadFactor)[0],2]
    prob2 = datLC[np.where(datLC[:,1] == loadFactor)[0],2]
    ins3 = insRH[np.where(insRH[:,1] == loadFactor)[0],2]
    prob3 = datRH[np.where(datRH[:,1] == loadFactor)[0],2]

    x.append(loadFactor)

    insy1min.append(np.min(ins1))
    insy2min.append(np.min(ins2))
    insy3min.append(np.min(ins3))
    insy1max.append(np.max(ins1))
    insy2max.append(np.max(ins2))
    insy3max.append(np.max(ins3))
    insy1avg.append(np.mean(ins1))
    insy2avg.append(np.mean(ins2))
    insy3avg.append(np.mean(ins3))

    y1min.append(np.min(prob1))
    y2min.append(np.min(prob2))
    y3min.append(np.min(prob3))
    y1max.append(np.max(prob1))
    y2max.append(np.max(prob2))
    y3max.append(np.max(prob3))
    y1avg.append(np.mean(prob1))
    y2avg.append(np.mean(prob2))
    y3avg.append(np.mean(prob3))
# X insFCmin insFCmax insFCavg insLCmin insLCmax insLCavg insRHmin insRHmax insRHavg FCmin FCmax FCavg LCmin LCmax LCavg RHmin RHmax RHavg
output = np.asarray([x,insy1min,insy1max,insy1avg,insy2min,insy2max,insy2avg,insy3min,insy3max,insy3avg,y1min,y1max,y1avg,y2min,y2max,y2avg,y3min,y3max,y3avg]).T
np.savetxt('../out/stats.txt',output,fmt='%.2f')

plt.plot(x,y1max,label='FC')
plt.plot(x,y2max,label='LC')
plt.plot(x,y3max,label='RH')
plt.legend()
plt.show()