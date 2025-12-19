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

if __name__ == "__main__":
    FILEPATH = os.path.join("src", "logs", "flow.txt")

    with open(FILEPATH, "w") as file:
        flow_samples = np.random.normal(MEAN_FLOW, STD_FLOW, NUM_SAMPLES)
        for sample in flow_samples:
            file.write(f"{sample},\n")
