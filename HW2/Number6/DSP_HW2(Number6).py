import csv

t = [] # column 0
data1 = [] # column 1
data2 = [] # column 2

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

#PUTS THE IIR-computed DATA IN A LIST TO BE PLOTTED
#   I used some internet resources to ensure that I was understanding the method
#   to calculate this filter correctly (as well as your videos).
new_average = [data1[0]] # new_average contains the iir filter generated data
for data_index in range(len(data1)-1): # loops through the data starting from the second data point
    data_index = data_index + 1 #starts from second data point
    A = 0.90 # weight A
    B = 0.10 # weight B
    new_average_temp = A * new_average[data_index - 1] + B * data1[data_index] # weighted average computation
    new_average.append(new_average_temp) # stores the temp as the new data point in the list
print(len(new_average))
print(len(t))

#SETS UP THE FFT TO BE PLOTTED (ACTS ON THE IIR DATA)
number_data_points2 = len(new_average) #number of data points (signal)
total_time_samples2 = t[-1] #ending time data point
sample_rate2 = number_data_points2 / total_time_samples2 #calculates the sample rate from these values
Fz = sample_rate2 # sample rate
Tz = 1.0/Fz; # sampling interval
tz = np.arange(0,t[-1],Tz) # time vector
z = new_average # the data to make the fft from
n2 = len(z) # length of the signal
k2 = np.arange(n2)
T2 = n2/Fz
frqz = k2/T2 # two sides frequency range
frqz = frqz[range(int(n2/2))] # one side frequency range
Z = np.fft.fft(z)/n2 # fft computing and normalization
Z = Z[range(int(n2/2))]

#PLOTS THE RAW DATA IN ONE SUBPLOT, THE IIR data IN THE OTHER
fig, (ax1, ax2) = plt.subplots(2, 1)
plt.title('Raw Signal (Black), IIR Data (Red) (A=0.90,B=0.10) : Unprocessed Signal Data (sigD)')
ax1.plot(t,y,'k')
ax1.set_title('aw Sig')
ax1.plot(t,new_average,'r')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')

#PLOTS THE FFT (RAW DATA) IN ONE SUBPLOT, THE FFT (IIR data) IN THE OTHER
ax2.loglog(frq,abs(Y),'k')
ax2.loglog(frqz,abs(Z),'r')
ax1.set_title('Raw Signal (Black), IIR Data (Red) (A=0.90,B=0.10) : FFT (sigD)')
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()

