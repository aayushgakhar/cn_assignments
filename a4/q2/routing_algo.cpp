#include "node.h"
#include <iostream>
#include <unordered_map>
#include <queue>

using namespace std;

void printRT(vector<RoutingNode *> nd)
{
  /*Print routing table entries*/
  for (int i = 0; i < nd.size(); i++)
  {
    nd[i]->printTable();
  }
}

void routingAlgo(vector<RoutingNode *> nd)
{

  for (RoutingNode *node : nd)
  {
    node->initialize();
    node->sendMsg();
  }

  for (RoutingNode *node : nd)
  {
    node->djikstra(nd, node);
  }

  /*Print routing table entries after routing algo converges */
  printf("Printing the routing tables after the convergence \n");
  printRT(nd);
}

void RoutingNode::recvMsg(RouteMsg *msg)
{
  // your code here

  // Traverse the routing table in the message.
  // Check if entries present in the message table is closer than already present
  // entries.
  // Update entries.

  if (map.find(msg->from_name) == map.end())
  {
    return;
  }

  map[msg->from_name] = msg->neighbours;

  for (int i = 0; i < getinterfaces().size(); ++i)
  {
    if (getinterfaces()[i].first.getConnectedIp() == msg->from)
    {
      continue;
    }
    RouteMsg nmsg;
    nmsg.from = msg->from;
    nmsg.neighbours = neighbours;
    nmsg.from_name = msg->from_name;
    nmsg.recvip = getinterfaces()[i].first.getConnectedIp();
    ((RoutingNode *)getinterfaces()[i].second)->recvMsg(&nmsg);
    djikstra();
  }
}

int minDistance(int dist[], int visited[], int n)
{
  int min = INT_MAX, min_index;

  for (int v = 0; v < n; v++)
    if (visited[v] == 0 && dist[v] <= min)
      min = dist[v], min_index = v;

  return min_index;
}

string get_ip(RoutingNode *p, RoutingNode *d, int c)
{
  vector<pair<NetInterface, Node *>> interfaces = p->getinterfaces();
  for (int i = 0; i < interfaces.size(); ++i)
  {
    if (interfaces[i].second->getName() == d->getName())
    {
      pair<string, string> ip;
      if (c == 1)
        return interfaces[i].first.getip();
      else
        return interfaces[i].first.getConnectedIp();
    }
  }
  return "";
}

int getnexthop(vector<RoutingNode *> nd, int i, int parent[])
{
  while (parent[parent[i]] != -1)
  {
    i = parent[i];
  }
  return i;
}

void RoutingNode::djikstra(vector<RoutingNode *> nd, RoutingNode *source)
{
  int n = 0 + nd.size();
  int dist[nd.size()];
  int visited[nd.size()];
  int parent[nd.size()];
  int **graph = new int *[n];
  for (int i = 0; i < n; ++i)
  {
    graph[i] = new int[n];
    dist[i] = INT_MAX;
    visited[i] = 0;
    parent[i] = -1;
  }
  for (int i = 0; i < nd.size(); ++i)
  {
    for (auto ne : nd[i]->neighbours)
    {
      graph[i][ne.first.getid()] = ne.second;
    }
  }
  dist[source->getid()] = 0;
  for (int count = 0; count < nd.size() - 1; count++)
  {
    int u = minDistance(dist, visited, nd.size());
    visited[u] = 1;
    for (int i = 0; i < nd[u]->neighbours.size(); i++)
    {
      int v = nd[u]->neighbours[i].first.getid();
      if (!visited[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v])
      {
        dist[v] = dist[u] + graph[u][v];
        parent[v] = u;
      }
    }
  }
  mytbl.tbl.clear();
  for (int i = 0; i < n; i++)
  {
    if (parent[i] == -1)
    {
      for (auto itf2 : nd[i]->ips)
      {
        source->addTblEntry(itf2, itf2, itf2, dist[i]);
      }
      continue;
    }
    int nh = getnexthop(nd, i, parent);
    string nhs = get_ip(source, nd[nh], 2);
    string src = get_ip(source, nd[nh], 1);
    for (auto itf2 : nd[i]->ips)
    {
      source->addTblEntry(itf2, nhs, src, dist[i]);
    }
  }
}

void RoutingNode::djikstra()
{
  unordered_map<string, vector<pair<Node, int>>> nd = map;
  int n = 0 + nd.size();
  int dist[nd.size()];
  int visited[nd.size()];
  int parent[nd.size()];
  int **graph = new int *[n];
  for (int i = 0; i < n; ++i)
  {
    graph[i] = new int[n];
    dist[i] = INT_MAX;
    visited[i] = 0;
    parent[i] = -1;
  }
  for (int i = 0; i < nd.size(); ++i)
  {
    for (auto ne : neighbours)
    {
      graph[i][ne.first.getid()] = ne.second;
    }
  }
  dist[getid()] = 0;
  for (int count = 0; count < nd.size() - 1; count++)
  {
    // int u = minDistance(dist, visited, nd.size());
    // visited[u] = 1;
    string u = "a";
  }
  for (int i = 0; i < n; i++)
  {
    if (parent[i] == -1)
      continue;
  }
}
