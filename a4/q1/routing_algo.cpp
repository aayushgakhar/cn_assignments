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


unordered_map<string, vector<RoutingEntry>> graph;
// vector<pair<string, routingtbl>> edges;

void routingAlgo(vector<RoutingNode *> nd)
{

  bool saturation = false;

  int g[nd.size()][nd.size()];

  int ic = 0;

  for(int i = 0; i < nd.size(); ++i)
  {
    for(RoutingNode *node : nd)
    for(int j = 0; j < nd.size(); ++j)
    {
      g[i][j] = 0;
    }
  }

  

  for (RoutingNode *node : nd)
  {
    node->initialFlooding();
    // cout << "Initial Flooding" << endl;
    node->sendMsg();
    
    ic += node->getInterfaceCount();
  }


  for (RoutingNode *node : nd)
  {
    node->djikstra(graph);
  }

  // for (int i = 1; i < nd.size(); ++i)
  // {
  //   for (RoutingNode *node : nd)
  //   {
  //     node->sendMsg();
  //   }
  // }

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

  routingtbl *recvRoutingTable = msg->mytbl;
  int c = 0;
  // for (auto it : *(msg->umap))
  // {
  //   if(umap.find(it.first) == umap.end()){
  //     umap[it.first] = it.second;
  //     c++;
  //   }
  // }
  // cout << c << endl;
  // if (c == 0)
  // {
  //     return;
  // }
  struct routingtbl ntbl;
  for (int i = 0; i < recvRoutingTable->tbl.size(); ++i)
  {
    ntbl.tbl.push_back(recvRoutingTable->tbl[i]);
  }
  graph[msg->from] = ntbl.tbl;
  // cout<<graph.size()<<endl;
  // for(NetInterface iface: interfaces){
  //   if(iface.getip() == msg->from){
  //     return;
  //   }
  // }

  // RoutingEntry newEntry;
  // newEntry.dstip = msg->from;
  // newEntry.nexthop = msg->from;
  // newEntry.ip_interface = msg->recvip;
  // newEntry.cost = 1;
  // mytbl.tbl.push_back(newEntry);

  return;

  for (RoutingEntry entry : recvRoutingTable->tbl)
  {
    // Check routing entry

    bool entryExists = false;
    for (int i = 0; i < mytbl.tbl.size(); ++i)
    {
      RoutingEntry myEntry = mytbl.tbl[i];
      // printf("i=%d, nodeRT.cost=%d, DV.cost=%d\n",i, myEntry.cost, entry.cost );
      if (myEntry.dstip == entry.dstip)
      {
        entryExists = true;
        // update existing entry
        if (myEntry.cost > entry.cost + 1)
        {
          myEntry.cost = entry.cost + 1;
          myEntry.nexthop = msg->from;
          mytbl.tbl[i] = myEntry;
        }
      }
    }
    if (!entryExists)
    {
      // add the new entry
      RoutingEntry newEntry;
      newEntry.dstip = entry.dstip;
      newEntry.nexthop = msg->from;
      newEntry.ip_interface = msg->recvip;
      newEntry.cost = entry.cost + 1;
      mytbl.tbl.push_back(newEntry);
    }
  }
}
