import matplotlib.pyplot as plt

# max_par = [455507, 509800, 589489, 633433, 786030, 1074112, 1514533, 2649454]
# max = [70364, 97535, 174015, 383088, 706967, 1421388, 2560130, 5537886]
# max_par = [434238, 488550, 434757, 451127, 543919, 501475, 551005, 477098]
# max = [4631, 5681, 6798, 13977, 21125, 51090, 67636, 169717]

x = [256, 512, 1024, 2048, 4096, 8192, 16384, 32768]

#plot max vs max_par in dots united by lines
plt.plot(x, max_par, 'ro-', label='max_par')
plt.plot(x, max, 'bo-', label='max')
plt.legend()
plt.xlabel('Words per list')
plt.ylabel('Time (nano seconds)')
plt.yscale('log')
plt.title('Max vs Max_par (letra a)')
plt.show()


