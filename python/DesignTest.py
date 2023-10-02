import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as sig

fs = 44100
f0 = 1000
Q = 1
gain = 10


def design_peak_equalizer(fs, f0, gain, Q):
    A  =  pow(10,gain/40)
    w0 = 2*np.pi*f0/fs
    alpha = np.sin(w0)/(2*Q)
    b = np.zeros(3)
    a = np.zeros(3)

    b[0] =   1 + alpha*A
    b[1] =  -2*np.cos(w0)
    b[2] =   1 - alpha*A
    a[0] =   1 + alpha/A
    a[1] =  -2*np.cos(w0)
    a[2] =   1 - alpha/A

    b = b/a[0]
    a = a/a[0]

    return b,a

b,a = design_peak_equalizer(fs,f0,gain,Q)
print(b)
print(a)

w,h = sig.freqz(b,a,fs = fs)

fig,ax = plt.subplots()

ax.plot(w,20*np.log10(np.abs(h)))

plt.show()






