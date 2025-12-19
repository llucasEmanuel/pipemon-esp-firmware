import numpy as np
import math
import os

# em mm
EXT_DIAMETER = 101.6
INTERNAL_THICKNESS = 1.8
INT_DIAMETER = (EXT_DIAMETER - 2*INTERNAL_THICKNESS) * 1e-2

# em dm^2
INT_AREA = ((INT_DIAMETER/2)**2) * math.pi

# m/s
FLUID_SPEED = 1

# dm^3/s = l/s
FLOW = INT_AREA * (FLUID_SPEED * 10)

# valores usados na distribuição normal
MEAN_FLOW = FLOW
STD_FLOW = 0.05

# número de registros salvos no arquivo
NUM_SAMPLES = 50

LEAK_BEGIN = NUM_SAMPLES//3

if __name__ == "__main__":
    FILEPATH = os.path.join("src", "logs", "leak.txt")

    curr_flow = MEAN_FLOW

    with open(FILEPATH, "w") as file:
        for i in range(NUM_SAMPLES):

            if i >= LEAK_BEGIN:
                decay_factor = 0.02 * curr_flow
                curr_flow -= decay_factor

            flow_sample = np.random.normal(curr_flow, STD_FLOW, 1)
            flow = flow_sample.item()
            flow = 0 if flow < 0 else flow
            file.write(f"{flow},\n")
