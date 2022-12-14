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
    node->sendMsg();
  }

  for (RoutingNode *node : nd)
  {
    node->routing_table(nd, node);
  }

  /*Print routing table entries after routing algo converges */
  printf("Printing the routing tables after the convergence \n");
  printRT(nd);
}

void RoutingNode::recvMsg(RouteMsg *msg)
{
  if (map.find(msg->from_name) == map.end())
  {
    return;
  }

  map[msg->from_name] = msg->neighbours;
  map_ip[msg->from_name] = msg->ips;
  
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
  }
  djikstra();
}

