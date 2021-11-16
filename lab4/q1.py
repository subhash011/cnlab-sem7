# To run this file, type: python q1.py number_of_nodes source_node file_containing_edges
# This directory contains input.txt which can be passed as the last argument to test the code.
# to use input.txt, type: python q1.py 5 1 input.txt

import heapq
import sys
from collections import defaultdict as dict


class Graph:
    """
    This class represents a graph and it models the network topology.
    It is an undirected graph with no self-loops.
    """

    def __init__(self, n_nodes):
        self.n_nodes = n_nodes
        self.graph = [[float('inf') for _ in range(n_nodes + 1)] for _ in range(n_nodes + 1)]
        self.parent = [-1 for _ in range(n_nodes + 1)]
        for i in range(1, n_nodes + 1):
            for j in range(1, n_nodes + 1):
                if i == j:
                    self.graph[i][j] = 0  # no self-loops

    def add_edge(self, u, v, c):
        self.graph[u][v] = c  # add u->v with cost c
        self.graph[v][u] = c  # undirected graph, so add v->u with cost c

    def find_min_dist(self, dist, sptSet):
        """
        It returns the min distance node from the source node which has
        not yet been added to the shortest path set.
        """
        min_dist = float('inf')
        min_node = None
        for node, d in dist.items():
            if not sptSet[node] and d < min_dist:
                min_dist = d
                min_node = node
        return min_node

    def dijkstra(self, source):
        dist = dict(lambda: float('inf'))
        # distance of source node from itself is 0
        dist[source] = 0
        # In python, a priority queue with a custom comparator is implemented as a heapq
        # with a tuple (priority, key) as the elements.
        queue = [(0, source)]
        while queue:
            # pop the node with the lowest cost
            cost, node = heapq.heappop(queue)
            # if the cost is greater than the distance, then we can skip this node
            if cost > dist[node]:
                continue
            itr = enumerate(self.graph[node])
            next(itr)
            for neighbor, cost in itr:
                # if the neighbour has a shorter path through the current node, we update the distance
                # and add the neighbor to the priority queue.
                if dist[neighbor] > dist[node] + cost:
                    dist[neighbor] = dist[node] + cost
                    self.parent[neighbor] = node
                    heapq.heappush(queue, (dist[neighbor], neighbor))
        return dist

    def get_path(self, node):
        """
        It returns the path from the source node to the given node.
        """
        path = []
        while node != -1:
            path.append(node)
            node = self.parent[node]
        path.reverse()
        path = [str(i) for i in path]
        return '->'.join(path)

    def print_distances(self, source, dist):
        """
        It pretty prints the distances and path from the source node to every other node.
        It prints 'inf' if a node is unreachable from the source node.
        Idea take from: https://stackoverflow.com/a/31820892/13721388
        """
        print(f"\nCost and path to all nodes from the node {source}.\n")
        paths = []
        maxlen = float('-inf')
        for i in range(1, self.n_nodes + 1):
            path = self.get_path(i)
            paths.append(path)
            maxlen = max(maxlen, len(path))
        fmt_str = "|{:>5}" + " " * 5
        fmt_str *= 2
        fmt_str += "|"
        head_fmt_str = "|" + " " * 2 + "{:>5}" + " " * 3
        head_fmt_str *= 2
        spaces = (maxlen - 4) // 2
        head_fmt_str += "| " + (" " * spaces) + "{}" + (" " * spaces) + "  |"
        s = "+" + "-" * 10
        s *= 2
        s += "+" + "-" * (maxlen + 2)
        s += "+"
        print(s)
        print(head_fmt_str.format("Dest", "Cost", "Path"))
        print(s)
        for i in sorted(dist.keys()):
            print(fmt_str.format(i, dist[i]), end="")
            path = self.get_path(i)
            print(" " + path + " " * (maxlen - len(path)) + " |")
        print(s)
        print("\n")


if __name__ == '__main__':
    n_nodes = int(sys.argv[1])  # number of nodes is the first argument
    source = int(sys.argv[2])  # source node is the second argument
    input_file = sys.argv[3]  # input file is the third argument
    graph = Graph(n_nodes)  # create a graph with n_nodes nodes
    with open(input_file, 'r') as f:
        edges = f.read().splitlines()  # read all the edges from the input file
    for edge in edges:
        u, v, c = map(int, edge.split())
        graph.add_edge(u, v, c)  # add undirected edge u-v with cost c
    d = graph.dijkstra(source)  # find the shortest path from source to all other nodes
    graph.print_distances(source, d)  # print the shortest path from source to all other nodes
