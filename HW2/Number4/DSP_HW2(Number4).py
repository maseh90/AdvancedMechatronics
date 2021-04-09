import csv

t = [] # column 0
data1 = [] # column 1

#EXTRACTS THE RAW DATA FROM THE CSV FILE
with open('sigD.csv',newline='') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        data1.append(float(row[1])) # second column

#SETS UP THE FFT TO BE PLOTTED (ACTS ON THE RAW DATA)
import matplotlib.pyplot as plt # for plotting
import numpy as np # for sine function
number_data_points = len(data1) # number of data points (signal)
total_time_samples = t[-1] # ending time data point
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

#PLOTS THE RAW DATA IN ONE SUBPLOT, THE FFT IN THE OTHER SUBPLOT
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t,y,'k') # plots the raw data
ax1.set_title('Unprocessed Signal Data (sigD)')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax2.loglog(frq,abs(Y),'k') # plotting the fft
ax2.set_title('FFT - Unprocessed Signal Data (sigD)')
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()
