import numpy as np
import matplotlib.pyplot as plt
from enum import Enum
from random import random


class AlohaState(Enum):
    """
    Enum for the states of a node in the network.
    """
    IDLE = 1  # Node is idle
    TRANSMIT = 2  # Node is transmitting
    RE_TRANSMIT = 3  # Node has sufferred collision and needs to be retransmitted


def simulate_slotted_aloha(n_slots, n_nodes, frame_gen_prob):
    """
    A simultor for the slotted ALOHA protocol.

    Parameters:
    -----------
        n_slots: int -> number of slots to simulate
        n_nodes: int -> number of nodes in the network
        states: list of NodeState -> the states of the nodes
        frame_gen_prob: float -> the probability of a node generating a frame.

    Returns:
    --------
        success: int -> number of frames successfully sent
        collissions: int -> number of slots which had collissions
        idle: int -> number of slots which were idle
    """
    # initialize states for all nodes
    states = [AlohaState.IDLE] * n_nodes
    transmitting_status = []
    for _ in range(n_slots):
        transmitting_node = -1
        for node in range(n_nodes):
            # if a node can generate a frame, set it to transmitting
            if random() <= frame_gen_prob:
                states[node] = AlohaState.TRANSMIT
                transmitting_node = node

        n_transmit = states.count(AlohaState.TRANSMIT)
        # if no nodes to transmit, the channel stays idle
        if n_transmit == 0:
            transmitting_status.append(-2)
        # if exactly one node has a frame to transmit, send and set
        # the state of the node to idle so that it can produce more frames
        elif n_transmit == 1:
            transmitting_status.append(transmitting_node)
            states[transmitting_node] = AlohaState.IDLE
        # If more than one node have frames to transmit, all will send, which results
        # in collission so set all those nodes to RE_TRANSMIT, this will eventually in
        # the future (random time) get to send a frame.
        else:
            transmitting_status.append(-1)
            states = [AlohaState.RE_TRANSMIT if state == AlohaState.TRANSMIT else state for state in states]

    success = 0
    collission = 0
    idle = 0
    for status in transmitting_status:
        if status == -1:
            collission += 1
        elif status == -2:
            idle += 1
        else:
            success += 1
    return success, collission, idle


if __name__ == '__main__':
    NUMBER_SLOTS = 1000
    NUMBER_NODES = 100
    # generate random lambdas in the range given
    lamdas = list(np.random.uniform(low=0, high=1, size=(49,)))
    lamdas.append(1)
    lamdas.sort()
    succ_avgs = []
    collission_avgs = []
    idle_avgs = []
    print("Started simulation for slotted ALOHA")
    # for each lambda simulate ALOHA and store the average results in the above lists
    for l in lamdas:
        success, collission, idle = simulate_slotted_aloha(NUMBER_SLOTS, NUMBER_NODES, frame_gen_prob=l / NUMBER_NODES)
        succ_avgs.append(success / NUMBER_SLOTS)
        collission_avgs.append(collission / NUMBER_SLOTS)
        idle_avgs.append(idle / NUMBER_SLOTS)
    succ_avgs = np.array(succ_avgs)
    collission_avgs = np.array(collission_avgs)
    idle_avgs = np.array(idle_avgs)
    lamdas = np.array(lamdas)
    plt.plot(lamdas, succ_avgs, "-r", label="simulation")
    plt.plot(lamdas, lamdas * np.exp(-1 * lamdas), "-b", label="theoretical")
    plt.xlabel("Lambda")
    plt.ylabel("Throughput")
    plt.legend(loc="upper left")
    plt.show()
