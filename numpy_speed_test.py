#!/usr/bin/env python

import time
import numpy as np

test_data_size = (1 << 10)
test_macro_size = (1 << 14)

import numpy as np

d = np.random.rand(1024 * test_macro_size)
r = np.zeros(1024 * test_macro_size)

start = time.perf_counter()

r = np.sqrt(d)

stop = time.perf_counter()  # A few seconds later

print(stop-start)