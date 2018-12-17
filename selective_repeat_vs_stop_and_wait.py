#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt

with open("selective_repeat_vs_stop_and_wait.txt") as f:
        data = f.read()

data = list(filter('' or None, data.split('\n'))) # fastest
#size, selective_repeat, stop_and_wait, loss_prob

sizes = []
selective_repeat = []
stop_and_wait = []
loss_prob = []

for reading in data:
    a, b, c, d = reading.split(', ')
    a = a.strip()
    b = b.strip()
    c = c.strip()
    d = d.strip()

    sizes.append(a)
    selective_repeat.append(b)
    stop_and_wait.append(c)
    loss_prob.append(d)



fig, ax = plt.subplots()
ax.plot(sizes, selective_repeat)
ax.plot(sizes, stop_and_wait)
ax.set_title('Selective repeat performance vs stop and wait performance')

ax.set_xlabel('File size in bytes')
ax.set_ylabel('time in milliseconds')

ax.legend(["selective_repeat", "stop_and_wait"])

for i, txt in enumerate(loss_prob):
        ax.annotate(txt, (sizes[i], selective_repeat[i]))
        ax.annotate(txt, (sizes[i], stop_and_wait[i]))

plt.show()
