#include "Queue.h"
#include <string>
#include <iostream>
using namespace std;

Queue::Queue()
{

	this->numOfNodes=0;
	this->head=0;
	this->last=0;
}

Queue::Queue(int numOfNodes)
{
		this->numOfNodes=numOfNodes;
		this->head=0;
		this->last=0;

}

void Queue::setNumOfNodes(int num)
{
		this->numOfNodes=num;
}

void Queue::setHead(Node * node)
{

		this->head=node;
}

void Queue::setLast(Node * node)
{

		this->last=node;
}

int Queue::getNumOfNodes()
{
		return this->numOfNodes;
}

Node * Queue::getHead()
{
	return this->head;


}

Node * Queue::getLast()
{
	return this->last;

}

bool Queue::isEmpty()
{
	if((this->numOfNodes)<=0)
	{
		return true;

	}
	else
	{
		return false;
	}
}

Node * Queue::ifLinkExists(string link)
{

	Node * ptr=this->getHead();

	while(ptr!=0)
	{
		if(ptr->getLink()==link)
		{

			break;
		}

		ptr=ptr->getNext();

	}

	return ptr;
}


Node *  Queue::enqueue(Node * node)
{

	if(this->isEmpty())
	{//空队列

		this->setHead(node);
		this->setLast(node);
	}
	else
	{
		this->getLast()->setNext(node);
		this->setLast(node);
	}

	this->setNumOfNodes((this->getNumOfNodes())+1);

}

Node *  Queue::dequeue()
{

	Node *node=this->getHead();

	if(node!=this->getLast())
	{
		this->setHead(node->getNext());
		node->setNext(0);
	}
	else
	{
		this->setHead(node->getNext());
		this->setLast(node->getNext());

	}

	this->setNumOfNodes((this->getNumOfNodes())-1);

	return node;
}


void Queue::clearList()
{


    while(!this->isEmpty())
    {

        this->dequeue();


    }

}

