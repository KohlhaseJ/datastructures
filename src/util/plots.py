import numpy as np
import matplotlib.pyplot as plt

datFC = np.loadtxt('../out/FCFS_search.txt', dtype = float, delimiter=',')
datLC = np.loadtxt('../out/LCFS_search.txt', dtype = float, delimiter=',')
datRH = np.loadtxt('../out/RobinHood_search.txt', dtype = float, delimiter=',')

# function for setting the colors of the box plots pairs
def setBoxColors(bp):
    plt.setp(bp['boxes'][0], color='blue')
    plt.setp(bp['caps'][0], color='blue')
    plt.setp(bp['caps'][1], color='blue')
    plt.setp(bp['whiskers'][0], color='blue')
    plt.setp(bp['whiskers'][1], color='blue')

    plt.setp(bp['boxes'][1], color='red')
    plt.setp(bp['caps'][2], color='red')
    plt.setp(bp['caps'][3], color='red')
    plt.setp(bp['whiskers'][2], color='red')
    plt.setp(bp['whiskers'][3], color='red')


loadFactors = np.unique(datFC[:,1])
i = 1
xticks = []
xlabels = []
for loadFactor in [0.75, 0.8, 0.85, 0.9, 0.95]:

    dat = np.asarray([
            datLC[np.where(datLC[:,1] == loadFactor)[0],2],
            datRH[np.where(datRH[:,1] == loadFactor)[0],2]
    ]).T
    
    bp = plt.boxplot(dat, positions=[i,i+1], widths=0.7)
    setBoxColors(bp)
    xticks.append(i + .5)
    xlabels.append('{}'.format(loadFactor))
    i+=3

# temp lines
hB, = plt.plot([1,1],'b-')
hR, = plt.plot([1,1],'r-')
plt.legend((hB, hR),('LCFS', 'Robin Hood'))
hB.set_visible(False)
hR.set_visible(False)

plt.xticks(ticks=xticks, labels=xlabels)
plt.ylabel(r'Linear probing $S_n$')
plt.xlabel(r'Load factor $\alpha$')
plt.grid(axis='y')
plt.savefig('../out/ProbingBoxPlot.pdf')
exit()



loadFactors = np.unique(datFC[:,1])
for loadFactor in [0.75, 0.8, 0.85, 0.9, 0.95]:
    plt.clf()
    tst = np.asarray([
            datLC[np.where(datLC[:,1] == loadFactor)[0],2],
            datRH[np.where(datRH[:,1] == loadFactor)[0],2]
    ]).T

    plt.boxplot(tst, labels=['LCFS','Robin Hood'])
    plt.title('Load factor: {}'.format(loadFactor))
    plt.ylabel('Linear probing')
    plt.grid(axis='y')
    plt.savefig('../out/ProbingBoxPlot_{}.pdf'.format((int)(loadFactor*100)))