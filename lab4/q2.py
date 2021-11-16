# To run this file, type: python q2.py number_of_nodes file_containing_edges
# This directory contains input.txt which can be passed as the last argument to test the code.
# to use input.txt, type: python q2.py 5 input.txt

import sys
from collections import defaultdict
from random import randint


class Graph:
    """
    This class represents a graph and it models the network topology.
    It is an undirected graph with no self-loops.
    """

    def __init__(self, n_nodes):
        self.n_nodes = n_nodes
        self.graph = defaultdict(lambda: [])
        self.routing_table = [[float('inf') for _ in range(n_nodes + 1)] for _ in range(n_nodes + 1)]
        self.hop_node = [[-1 for _ in range(n_nodes + 1)] for _ in range(n_nodes + 1)]
        for i in range(1, n_nodes + 1):
            for j in range(1, n_nodes + 1):
                if i == j:
                    self.routing_table[i][j] = 0  # no self-loops
                    self.hop_node[i][j] = i  # same src and dest, so no hop node

    def add_edge(self, u, v, c):
        self.graph[u].append((v, c))  # add u->v with cost c
        self.graph[v].append((u, c))  # undirected graph, so add v->u with cost c
        self.routing_table[u][v] = c  # initialize the routing table with the cost of the edge.
        self.routing_table[v][u] = c
        self.hop_node[u][v] = v
        self.hop_node[v][u] = u

    def dv_routing_step(self, u):
        updated = False
        for neighbour, dist in self.graph[u]:  # for each neighbour of u
            for i in range(1, self.n_nodes + 1):  # for each node in the network
                # Here two conditions are to be satisfied:
                # 1. The edge (i, neighbour) exists in the graph.
                # 2. Update the distance between current node and u if there is a
                #    shorter path passing through the neighbour of u.
                if self.routing_table[neighbour][i] != float('inf') and \
                        self.routing_table[u][i] > dist + self.routing_table[neighbour][i]:
                    self.hop_node[u][i] = neighbour
                    self.routing_table[u][i] = dist + self.routing_table[neighbour][i]
                    updated = True
        # if there was atleast one update, we haven't converged yet.
        return updated

    def print_routing_table(self, node):
        """
        Pretty prints the local routing table.
        Idea take from: https://stackoverflow.com/a/31820892/13721388
        """
        costs = self.routing_table[node]
        hop_node = [hop[node] for hop in self.hop_node]
        head_fmt_str = "|" + " " * 2 + "{:>5}" + " " * 3
        head_fmt_str *= 3
        head_fmt_str += "|"
        s = "+" + "-" * 10
        s *= 2
        s += "+" + "-" * 19
        s += "+"
        fmt_str = "|{:>5}" + " " * 5
        fmt_str *= 2
        fmt_str += "|" + " " * 5 + "{:>5}" + " " * 9
        fmt_str += "|"
        print(s)
        print(head_fmt_str.format("Dest", "Cost", "First Hop Node"))
        print(s)
        for i in range(1, n_nodes + 1):
            print(fmt_str.format(i, costs[i], hop_node[i]))
        print(s)


if __name__ == '__main__':
    n_nodes = int(sys.argv[1])  # number of nodes is the first argument
    input_file = sys.argv[2]  # input file is the second argument
    graph = Graph(n_nodes)  # create a graph with n_nodes nodes
    with open(input_file) as f:
        edges = f.read().splitlines()  # read all the edges
    for edge in edges:
        u, v, c = map(int, edge.split())
        graph.add_edge(u, v, c)  # add undirected edge u-v with cost c
    converged = set()
    while len(converged) != n_nodes:
        cur_node = randint(1, n_nodes)  # pick a random node which simulates random updates.
        updated = graph.dv_routing_step(cur_node)  # update the routing table.
        if not updated:
            converged.add(cur_node)
        else:
            converged = set()  # if there was atleast one update, we haven't converged yet.
    for i in range(1, n_nodes + 1):
        print(f"\nThe local routing table for node {i}:")
        graph.print_routing_table(i)
