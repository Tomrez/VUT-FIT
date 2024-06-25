# Zde napište váš kód
import numpy as np
import soundfile as sf
import matplotlib.pyplot as plt
from scipy import signal

ekg_signal, fs = sf.read('xrezni33.wav')

#! UKOL 1

# Select a 5-second segment
start_time = 0  # Choose the start time in seconds
segment_duration = 5  # Choose the duration of the segment in seconds
end_time = start_time + segment_duration
start_sample = int(start_time * fs)
end_sample = int(end_time * fs)
selected_segment = ekg_signal[start_sample:end_sample]

# Create time values for plotting
time_values = np.linspace(start_time, end_time, len(selected_segment))

# Display the 5-second segment in the time domain
plt.figure(figsize=(10, 4))
plt.plot(time_values, selected_segment, label="EKG Signal")
plt.title("5-sekundový úsek EKG")
plt.xlabel("Čas(v sekundách)")
plt.ylabel("Amplituda")
plt.legend()
plt.grid(True)
plt.show()

#! UKOL 2

# Select a 5-second segment
start_time = 0 # Choose the start time in seconds
segment_duration = 5  # Choose the duration of the segment in seconds
end_time = start_time + segment_duration
start_sample = int(start_time * fs)
end_sample = int(end_time * fs)
selected_segment = ekg_signal[start_sample:end_sample]

# Calculate the spectrum using FFT
frequencies = np.fft.fftfreq(len(selected_segment), 1/fs)
magnitude_spectrum = np.abs(np.fft.fft(selected_segment))

# Display the spectrum
plt.figure(figsize=(10, 4))
plt.plot(frequencies, magnitude_spectrum, label="Spectrum")
plt.title("Frequency Spectrum of EKG Signal")
plt.xlabel("Frequency (Hz)")
plt.ylabel("Magnitude")
plt.xlim(0, fs/2)  # Display only positive frequencies
plt.grid(True)
plt.show()


#! UKOL 3

# Select a 5-second segment
start_time = 0  # Choose the start time in seconds
segment_duration = 5  # Choose the duration of the segment in seconds
end_time = start_time + segment_duration
start_sample = int(start_time * fs)
end_sample = int(end_time * fs)
selected_segment = ekg_signal[start_sample:end_sample]

# Resample to 100 Hz with antialiasing filter
target_fs = 100
nyquist = 0.5 * fs
normal_cutoff = 0.4 * nyquist
b, a = signal.butter(4, normal_cutoff/nyquist, btype='low', analog=False)
resampled_segment = signal.resample(selected_segment, int(len(selected_segment) * target_fs / fs))

# Create time values for plotting
time_values_resampled = np.linspace(start_time, end_time, len(resampled_segment))

# Display the resampled segment
plt.figure(figsize=(10, 4))
plt.plot(time_values_resampled, resampled_segment, label="Resampled EKG Signal")
plt.title("Resampled EKG Signal at 100 Hz")
plt.xlabel("Time (seconds)")
plt.ylabel("Amplitude")
plt.legend()
plt.grid(True)
plt.show()
