#ifndef Queue_H
#define Queue_H
#include "Node.h"
#include <string>
using namespace std;
class Queue{

private:
	int numOfNodes;
	Node * head;
	Node * last;

public:
	Queue();
	Queue(int numOfNodes);
	void setNumOfNodes(int num);
	void setHead(Node * node);
	void setLast(Node * node);
	int getNumOfNodes();
	Node * getHead();
	Node * getLast();
	bool isEmpty();
	Node * ifLinkExists(string link);
	Node * enqueue(Node * node);
	Node * dequeue();
	void clearList();

};


#endif

