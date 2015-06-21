#include "Node.h"
#include <string>
using namespace std;

Node::Node()
{
	this->id=0;
	this->next=0;
	this->link="";
	this->text="";

}

Node::Node(int id, string link,string text)
{
	this->id=id;
	this->next=0;
	this->link=link;
	this->text=text;

}

void Node::setId(int id)
{
		this->id=id;

}

void Node::setNext(Node *node)
{
		this->next=node;

}

void Node::setLink(string link)
{
		this->link=link;

}

void Node::setText(string text)
{

        this->text=text;

}


int Node::getId()
{
		return this->id;
}

Node * Node:: getNext()
{
		return this->next;

}

string Node::getLink()
{

	return this->link;
}

string Node::getText()
{
    return this->text;

}
