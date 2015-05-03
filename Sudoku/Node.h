class Node
{
	
public:
	Node();
	~Node();
	int i;
	int j;
	int explored_nums[9];
	Node *children[9];
	Node *parent;

	
	
};

