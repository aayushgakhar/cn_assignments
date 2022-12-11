#include "node.h"
#include <iostream>

using namespace std;

void printRT(vector<RoutingNode*> nd){
/*Print routing table entries*/
	for (int i = 0; i < nd.size(); i++) {
	  nd[i]->printTable();
	}
}



void routingAlgo(vector<RoutingNode*> nd){
 
  bool saturation=false;
 
  for(int i=1; i<nd.size(); ++i) {
    for (RoutingNode* node: nd){
      node->sendMsg();
    }
  }
 
  /*Print routing table entries after routing algo converges */
  printf("Printing the routing tables after the convergence \n");
  printRT(nd);
}

void RoutingNode::recvMsg(RouteMsg *msg) {
  //your code here
 
  // Traverse the routing table in the message.
  // Check if entries present in the message table is closer than already present 
  // entries.
  // Update entries.
 
  routingtbl *recvRoutingTable = msg->mytbl;
  for (RoutingEntry entry : recvRoutingTable->tbl) {
    // Check routing entry

    bool entryExists = false;
    for ( int i=0; i<mytbl.tbl.size(); ++i) {
      RoutingEntry myEntry = mytbl.tbl[i];
      //printf("i=%d, nodeRT.cost=%d, DV.cost=%d\n",i, myEntry.cost, entry.cost );
      if (myEntry.dstip==entry.dstip){
        entryExists=true;
        //update existing entry
        if (myEntry.cost>entry.cost+1) {
          myEntry.cost=entry.cost+1;
          myEntry.nexthop=msg->from;
          mytbl.tbl[i]=myEntry;
        }
      }
    }
    if (!entryExists) {
      // add the new entry
      RoutingEntry newEntry;
      newEntry.dstip = entry.dstip;
      newEntry.nexthop = msg->from;
      newEntry.ip_interface = msg->recvip;
      newEntry.cost = entry.cost+1;
      mytbl.tbl.push_back(newEntry);
    }
  }
 
}



