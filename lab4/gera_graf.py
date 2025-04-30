import pandas as pd
import matplotlib.pyplot as plt

threads = [1,2,4]

eficiencias_0 = [1,0.125, 0.055]
acelerações_0 = [1, 0.25, 0.22]
tempos_0 = [0.00025, 0.001, 0.0011]

eficiencias_1 = [1,0.2, 0.047]
acelerações_1 = [1, 0.41, 0.2]
tempos_1 = [0.28, 0.67, 1.45]

plt.plot(threads, tempos_0, label = "Tempo em s", linestyle=":")
plt.plot(threads, eficiencias_0, label = "Eficiência", linestyle="-")
plt.plot(threads, acelerações_0, label = "Aceleração", linestyle="--")
plt.legend()
plt.title("Métricas para N=10e3")
plt.xlabel("N. de threads")
plt.show()


plt.plot(threads, tempos_1, label = "Tempo em s", linestyle=":")
plt.plot(threads, eficiencias_1, label = "Eficiência", linestyle="-")
plt.plot(threads, acelerações_1, label = "Aceleração", linestyle="--")
plt.legend()
plt.title("Métricas para N=10e6")
plt.xlabel("N. de threads")
plt.show()
