
#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt

with open("window_size_history.txt") as f:
        data = f.read()

data = list(filter('' or None, data.split('\n'))) # fastest
x = list(map(int, data))

plt.title('Selective repeat window size with loss probability = 0.01')
plt.yticks(np.arange(0, max(x)+1, 10.0))
plt.plot(x)
plt.show()
