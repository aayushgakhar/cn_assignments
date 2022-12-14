#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <limits.h>

using namespace std;

class Node;

/*
  Each row in the table will have these fields
  dstip:	Destination IP address
  nexthop: 	Next hop on the path to reach dstip
  ip_interface: nexthop is reachable via this interface (a node can have multiple interfaces)
  cost: 	cost of reaching dstip (number of hops)
*/
class RoutingEntry
{
public:
  string dstip, nexthop;
  string ip_interface;
  int cost;
};

/*
 * Class for specifying the sort order of Routing Table Entries
 * while printing the routing tables
 *
 */
class Comparator
{
public:
  bool operator()(const RoutingEntry &R1, const RoutingEntry &R2)
  {
    if (R1.cost == R2.cost)
    {
      return R1.dstip.compare(R2.dstip) < 0;
    }
    else if (R1.cost > R2.cost)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
};

/*
  This is the routing table
*/
struct routingtbl
{
  vector<RoutingEntry> tbl;
};

/*
  Message format to be sent by a sender
  from: 		Sender's ip
  mytbl: 		Senders routing table
  recvip:		Receiver's ip
*/
class RouteMsg
{
public:
  string from;              // I am sending this message, so it must be me i.e. if A is sending mesg to B then it is A's ip address
  string from_name;
  vector<pair<Node, int>> neighbours;
  vector<string> ips;
  string recvip;            // B ip address that will receive this message
};

/*
  Emulation of network interface. Since we do not have a wire class,
  we are showing the connection by the pair of IP's

  ip: 		Own ip
  connectedTo: 	An address to which above mentioned ip is connected via ethernet.
*/
class NetInterface
{
private:
  string ip;
  string connectedTo; // this node is connected to this ip
  int cost;

public:
  string getip()
  {
    return this->ip;
  }
  string getConnectedIp()
  {
    return this->connectedTo;
  }
  void setip(string ip)
  {
    this->ip = ip;
  }
  void setConnectedip(string ip)
  {
    this->connectedTo = ip;
  }
  void setcost(int cost)
  {
    this->cost = cost;
  }
  int getcost()
  {
    return this->cost;
  }
};

/*
  Struct of each node
  name: 	It is just a label for a node
  interfaces: 	List of network interfaces a node have
  Node* is part of each interface, it easily allows to send message to another node
  mytbl: 		Node's routing table
*/
class Node
{
private:
  string name;
  int id;
  vector<pair<NetInterface, Node *>> interfaces;

protected:
  struct routingtbl mytbl;
  virtual void recvMsg(RouteMsg *msg)
  {
    cout << "Base" << endl;
  }
  bool isMyInterface(string eth)
  {
    for (int i = 0; i < interfaces.size(); ++i)
    {
      if (interfaces[i].first.getip() == eth)
        return true;
    }
    return false;
  }

public:
  vector<pair<Node,int>> neighbours;
  vector<string> ips;
  unordered_map<string, vector<pair<Node, int>>> map;
  unordered_map<string, vector<string>> map_ip;
  void setName(string name)
  {
    this->name = name;
  }

  vector<pair<NetInterface, Node *>> getinterfaces(){
    return interfaces;
  }

    void setid(int id)
  {
    this->id = id;
  }

  int getid()
  {
    return this->id;
  }

  void addInterface(string ip, string connip, Node *nextHop, int cost)
  {
    NetInterface eth;
    ips.push_back(ip);
    eth.setip(ip);
    eth.setConnectedip(connip);
    eth.setcost(cost);
    interfaces.push_back({eth, nextHop});
  }

  void addTblEntry(string myip, int cost)
  {
    RoutingEntry entry;
    entry.dstip = myip;
    entry.nexthop = myip;
    entry.ip_interface = myip;
    entry.cost = cost;
    mytbl.tbl.push_back(entry);
  }
  void addTblEntry(string dstip, string nexthop, string ip_interface, int cost)
  {
    RoutingEntry entry;
    entry.dstip = dstip;
    entry.nexthop = nexthop;
    entry.ip_interface = ip_interface;
    entry.cost = cost;
    mytbl.tbl.push_back(entry);
  }

  void updateTblEntry(string dstip, int cost)
  {
    // to update the dstip hop count in the routing table (if dstip already exists)
    // new hop count will be equal to the cost
    for (int i = 0; i < mytbl.tbl.size(); i++)
    {
      RoutingEntry entry = mytbl.tbl[i];

      if (entry.dstip == dstip)
        mytbl.tbl[i].cost = cost;
    }

    // remove interfaces
    for (int i = 0; i < interfaces.size(); ++i)
    {
      // if the interface ip is matching with dstip then remove
      // the interface from the list
      if (interfaces[i].first.getConnectedIp() == dstip)
      {
        interfaces.erase(interfaces.begin() + i);
      }
    }
  }

  string getName()
  {
    return this->name;
  }

  struct routingtbl getTable()
  {
    return mytbl;
  }

  int getInterfaceCount()
  {
    int c = interfaces.size();
    return c;
  }

  void initialize()
  {
    for (int i = 0; i < interfaces.size(); ++i)
    {
      neighbours.push_back({*(interfaces[i].second),interfaces[i].first.getcost()});
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

  int getnexthop(int i, int parent[])
  {
    while (parent[parent[i]] != -1)
    {
      i = parent[i];
    }
    return i;
  }

  bool isconnected(string eth)
  {
    for (int i = 0; i < interfaces.size(); ++i)
    {
      if (interfaces[i].first.getConnectedIp() == eth)
        return true;
    }
    return false;
  }
  string connectedip(string eth)
  {
    for (int i = 0; i < interfaces.size(); ++i)
    {
      if (interfaces[i].first.getConnectedIp() == eth)
        return interfaces[i].first.getConnectedIp();
    }
    return "";
  }

  void printTable()
  {
    Comparator myobject;
    sort(mytbl.tbl.begin(), mytbl.tbl.end(), myobject);
    cout << this->getName() << ":" << endl;
    for (int i = 0; i < mytbl.tbl.size(); ++i)
    {
      cout << mytbl.tbl[i].dstip << " | " << mytbl.tbl[i].nexthop << " | " << mytbl.tbl[i].ip_interface << " | " << mytbl.tbl[i].cost << endl;
    }
  }

  void sendMsg()
  {
    initialize();
    map[this->getName()] = neighbours;
    for (int i = 0; i < interfaces.size(); ++i)
    {
      RouteMsg msg;
      msg.from = interfaces[i].first.getip();
      msg.from_name = this->getName();
      msg.neighbours = neighbours;
      msg.ips = ips;
      msg.recvip = interfaces[i].first.getConnectedIp();
      interfaces[i].second->recvMsg(&msg);
    }
  }
};

class RoutingNode : public Node
{
public:
  void recvMsg(RouteMsg *msg);
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
  
  string get_ip2(RoutingNode *p, vector<string> d, int c)
  {
    vector<pair<NetInterface, Node *>> interfaces = p->getinterfaces();
    for (int i = 0; i < d.size(); ++i)
    {
      if (isconnected(d[i]))
      {
        pair<string, string> ip;
        if (c == 1)
          return connectedip(d[i]);
        else return d[i];
      }
    }
    return "";
  }
  void routing_table(vector<RoutingNode *> nd, RoutingNode *source)
  {
    int n = nd.size();
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
      int nh = getnexthop(i, parent);
      string nhs = get_ip(source, nd[nh], 2);
      string src = get_ip(source, nd[nh], 1);
      for (auto itf2 : nd[i]->ips)
      {
        source->addTblEntry(itf2, nhs, src, dist[i]);
      }
    }
  }
  void djikstra()
  {
    vector<string> nd;
    for (auto ke : map)
    {
      nd.push_back(ke.first);
    }
    int n = nd.size();
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
      for (auto ne : map[nd[i]])
      {
        graph[i][ne.first.getid()] = ne.second;
      }
    }
    dist[getid()] = 0;
    for (int count = 0; count < nd.size() - 1; count++)
    {
      int u = minDistance(dist, visited, nd.size());
      visited[u] = 1;
      for (int i = 0; i < map[nd[u]].size(); i++)
      {
        int v = map[nd[u]][i].first.getid();
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
        for (auto itf2 : map_ip[nd[i]])
        {
          addTblEntry(itf2, itf2, itf2, dist[i]);
        }
        continue;
      }
      int nh = getnexthop(i, parent);
      string nhs = get_ip2(this, map_ip[nd[nh]], 2);
      string src = get_ip2(this, map_ip[nd[nh]], 1);

      for (auto itf2 : map_ip[nd[i]])
      {
        addTblEntry(itf2, nhs, src, dist[i]);
      }
    }
  }
};
