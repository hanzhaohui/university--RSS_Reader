#ifndef Node_H
#define Node_H
#include <string>
using namespace std;


class Node{

	private:

	int id;
	Node * next;
	string link;
	string text;

	public:

	Node();
	Node(int id,string link,string text);
	void setId(int id);
	void setNext(Node *node);
	void setLink(string link);
	void setText(string text);
	int getId();
	Node * getNext();
	string getLink();
	string getText();

};

#endif
