import csv

t = [] # column 0
data1 = [] # column 1
data2 = [] # column 2

#EXTRACTS THE RAW DATA FROM THE CSV FILE
with open('sigC.csv',newline='') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        data1.append(float(row[1])) # second column

#SETS UP THE FFT TO BE PLOTTED (ACTS ON THE RAW DATA)
import matplotlib.pyplot as plt # for plotting
import numpy as np # for sine function
number_data_points = len(data1) #number of data points (signal)
total_time_samples = t[-1] #ending time data point
sample_rate = number_data_points / total_time_samples #calculates the sample rate from these values
print(sample_rate)
Fs = sample_rate # sample rate
Ts = 1.0/Fs; # sampling interval
ts = np.arange(0,t[-1],Ts) # time vector
y = data1 # the data to make the fft from
n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

#PUTS THE MOVING AVERAGE IN A LIST TO BE PLOTTED
number_averaged = 50 # number of previous points to use for moving average
moving_averaged_data = [] # blank list to append the calculated moving average to
for data_index in range(len(data1) - number_averaged): # loops through raw data starting at a point where there are enough points to compute avg
    data_index = data_index + (number_averaged - 1) # can only start once enough data points have been secured
    current_sum = 0 # resets the current sum everytime the average has been computed
    for previous in range(number_averaged - 1): #loops through the previous points to compute the average
        current_sum = current_sum + data1[data_index - previous] #starts computing the sum to be divided by the total number of points
    average_value = current_sum / number_averaged # computes the current average
    moving_averaged_data.append(average_value) #appends the computed average to the list to be plotted

#DTERMINES THE TIMES THAT THE MOVING AVERAGE WAS CALCULATED AT
t_new = [] #blank list to hold times
for time in range(len(moving_averaged_data)): # only adds the times computed at
    t_new.append(t[time+(number_averaged-1)])
print(len(t_new))
print(len(moving_averaged_data))

#SETS UP THE FFT TO BE PLOTTED (ACTS ON THE MOVING AVERAGE DATA)
number_data_points2 = len(moving_averaged_data) #number of data points (signal)
total_time_samples2 = t_new[-1] #ending time data point
sample_rate2 = number_data_points2 / total_time_samples2 #calculates the sample rate from these values
Fz = sample_rate2 # sample rate
Tz = 1.0/Fz; # sampling interval
tz = np.arange(0,t_new[-1],Tz) # time vector
z = moving_averaged_data # the data to make the fft from
n2 = len(z) # length of the signal
k2 = np.arange(n2)
T2 = n2/Fz
frqz = k2/T2 # two sides frequency range
frqz = frqz[range(int(n2/2))] # one side frequency range
Z = np.fft.fft(z)/n2 # fft computing and normalization
Z = Z[range(int(n2/2))]

#PLOTS THE RAW DATA IN ONE SUBPLOT, THE MOVING AVERAGE IN THE OTHER
fig, (ax1, ax2) = plt.subplots(2, 1)
plt.title('Raw Signal (Black), MovingAvg Signal (Red) (N = 50) : Unprocessed Signal Data (sigC)')
ax1.plot(t,y,'k')
ax1.set_title('aw Sig')
ax1.plot(t_new,moving_averaged_data,'r')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')

#PLOTS THE FFT (RAW DATA) IN ONE SUBPLOT, THE FFT (MOVING AVERAGE) IN THE OTHER
ax2.loglog(frq,abs(Y),'k')
ax2.loglog(frqz,abs(Z),'r')
ax1.set_title('Raw Signal (Black), MovingAvg Signal (Red) (N = 50) : FFT (sigC)')
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()
