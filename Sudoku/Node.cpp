#include "Node.h"
#include <stdlib.h>

Node::Node ()
{
	i = 0;
	j = 0;
	for(int k = 0;k<9;k++)
		explored_nums[k] = 0;

	parent = (Node*)malloc(sizeof(Node));
}


Node::~Node()
{

}
