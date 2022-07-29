#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <time.h>
#include <map>

using namespace std;


enum Color {RED, BLACK};

struct Node
{
    /*
    We have node struct for tree
    with necessary properties
    */
    string publisher;
    int year;
    double naSales;
    double euSales;
    double otherSales;
	bool color;
	Node *left, *right, *parent;

    //We have a constructor to read line into node
    Node(vector<string> line){
    this->year = stoi(line[2]);
    this->publisher = line[3];
    this->naSales = stod(line[4]);
    this->euSales = stod(line[5]);
    this->otherSales = stod(line[6]);
    this->color = RED;
    left = right = parent = NULL;
    };

    //We have a copy constructor
    Node(Node* n){
        this->publisher = n->publisher;
        this->naSales = n->naSales;
        this->euSales = n->euSales;
        this->otherSales = n->otherSales;
        left = right = parent = NULL;
        this->color = RED;
    }

};

//Red-Black Tree Class
class RBT
{
private:
	Node *root;
    void swapColors(Node *, Node *);    //Swap two node's colors
	void leftRotate(Node *&, Node *&);  //leftRotate
	void rightRotate(Node *&, Node *&); //rightRotate
	void fix(Node *&, Node *&); //fixing the violation
public:
	// Constructor
	RBT() { root = NULL; }
	void insert(Node *);
	void getTree();
    Node* callBestNaSeller();
    Node* callBestEuSeller();
    Node* callBestOtherSeller();
    int size();
    void getRoot(){ cout << root->publisher << endl;}
};

//Helper function for getting the size of the tree
int getSizeHelper(Node* root) 
{ 
    if (root == NULL) 
        return 0; 
    else
        return(getSizeHelper(root->left) + 1 + getSizeHelper(root->right)); 
}
// A recursive helper function to print the tree in pre-order traversal
void getTreeHelper(Node *root, int indention )
{
	if (root == NULL){  //if root is null do nothing and go back
		return;
    }
    for (int i = 0; i < indention; i++){
            cout << "-";                    //print "-" with number of indention
        }
    string color = root->color ? "BLACK" : "RED";
	cout << "(" + color + ") "<<root->publisher << endl;    //first print the root
	getTreeHelper(root->left,indention + 1);                //then the left side
	getTreeHelper(root->right,indention + 1);               //and finally the right side
}

// A recursive helper function to calculate the best seller in Na
Node* bestNaSellerHelper(Node *root, Node *bestSeller){
    if (root == NULL)
        return bestSeller;
    if (bestSeller->naSales < root->naSales )
        bestSeller = root;
    bestSeller = bestNaSellerHelper(root->left,bestSeller);
    bestSeller = bestNaSellerHelper(root->right,bestSeller);
    return bestSeller;
}
// A recursive helper function to calculate the best seller in Eu
Node* bestEuSellerHelper(Node *root, Node *bestSeller){
    if (root == NULL)
        return bestSeller;
    if (bestSeller->euSales < root->euSales )
        bestSeller = root;
    bestSeller = bestNaSellerHelper(root->left,bestSeller);
    bestSeller = bestNaSellerHelper(root->right,bestSeller);
    return bestSeller;
}
// A recursive helper function to calculate the best seller in Others
Node* bestOtherSellerHelper(Node *root, Node *bestSeller){
    if (root == NULL)
        return bestSeller;
    if (bestSeller->otherSales < root->otherSales )
        bestSeller = root;
    bestSeller = bestOtherSellerHelper(root->left,bestSeller);
    bestSeller = bestOtherSellerHelper(root->right,bestSeller);
    return bestSeller;
}

//A recursive function to insert a node into tree
Node* insertToTree(Node* root, Node *node)
{

	//If the tree is empty, return the node
	if (root == NULL)
	    return node;

    //If the new node is alphabetically greater insert it into right
    if (node->publisher > root->publisher)
	{
		root->right = insertToTree(root->right, node); //connect it to right side
		root->right->parent = root;                     // and the parent of right is root now
	}
    //If the new node is alphabetically greater insert it into right
	else if (node->publisher < root->publisher)
	{
		root->left = insertToTree(root->left, node); //connect it to left side
		root->left->parent = root;                  // and the parent of left is root now
	}

    //If root and node are equal add sales to the old one
    else{                           
        root->naSales += node->naSales;
        root->euSales += node->euSales;
        root->otherSales += node->otherSales;
    }

	//return the root
	return root;
}

void RBT::leftRotate(Node *&root, Node *&node)
{
	Node *node_right = node->right;

	node->right = node_right->left;// new parents old right child is new left child of node
	if (node->right != NULL)
		node->right->parent = node; //new parents right child's new parent is now the node
	node_right->parent = node->parent;// setting node_right's parent nodes's old parent

	if (node->parent == NULL)
		root = node_right; //if node was null node_right is now root
	else if (node == node->parent->left)
		node->parent->left = node_right;  //if the node was its parent's left child
                                            //node_right is now that parent's right child
	else
		node->parent->right = node_right; //or left child
	node_right->left = node;    // node_right's new left child is node
	node->parent = node_right; // node's new parent is node_right
}

void RBT::rightRotate(Node *&root, Node *&node)
{
	Node *node_left = node->left;

	node->left = node_left->right;
	if (node->left != NULL)
		node->left->parent = node;
	node_left->parent = node->parent;

	if (node->parent == NULL)
		root = node_left;
	else if (node == node->parent->left)
		node->parent->left = node_left;
	else
		node->parent->right = node_left;
	node_left->right = node;
	node->parent = node_left;
}

// This function fixes violations
// caused by BST insertion
void RBT::fix(Node *&root, Node *&node)
{
	Node *parent = NULL;
	Node *grandParent = NULL;

	while (node != root  && node->color == RED && (node->parent->color == RED)){
		parent = node->parent;
		grandParent = node->parent->parent;
		//If parent of node is left child
	    //of Grand-parent of node 
		if (parent == grandParent->left){
			Node *uncle = grandParent->right;
			//If the uncle of node is also red
			//Recoloring is required
			if (uncle != NULL && uncle->color == RED){
				grandParent->color = RED;
				parent->color = BLACK;
				uncle->color = BLACK;
				node = grandParent;
			}
			else{
				
				//If node is right child of its parent
				//Left rotation is required
				if (node == parent->right){
					node = parent;
					leftRotate(root, parent);
					parent = node->parent;
				}
				//If node is left child of its parent
				//Right rotation is required 
				rightRotate(root, grandParent);
				swapColors(parent, grandParent); //after rotation swap colors
				node = parent;
			}
		}
		//If parent of node is right child
		//of Grand parent of node
		else{
			Node *uncle = grandParent->left;
				//If The uncle of node is also red
				//Recoloring is required
			if ((uncle != NULL) && (uncle->color == RED)){
				grandParent->color = RED;
				uncle->color = BLACK;
				parent->color = BLACK;
				node = grandParent;
			}
			else{
				//If node is left child of its parent
				//Right rotation is required
				if (node == parent->left){
					node = parent;
					rightRotate(root, parent);
					parent = node->parent;
				}
				//If node is right child of its parent
				//Left rotation is required
				leftRotate(root, grandParent);
				swapColors(parent, grandParent); //swap colors
				node = parent;
			}
		}
	}
	root->color = BLACK; // root is black at the end
}

// Function to insert a new node with given data
void RBT::insert(Node *node)
{
	//keep the size to compare
    int temp = size();
    //insert it to tree
    root = insertToTree(root, node);
    if (temp != size()) // if size is increased means smt added
        fix(root, node);
}

// Function getTree in wanted order
void RBT::getTree()	 { getTreeHelper(root,0);}
int RBT::size() { return getSizeHelper(root); }

Node* RBT::callBestNaSeller() { 
    Node *node = new Node(root);
    return bestNaSellerHelper(root,node);
}
Node* RBT::callBestEuSeller() { 
    Node *node = new Node(root);
    return bestEuSellerHelper(root,node);
}
Node* RBT::callBestOtherSeller() { 
    Node *node = new Node(root);
    return bestOtherSellerHelper(root,node);
}
void RBT::swapColors(Node *n1,Node *n2){
    bool tmpColor = n1->color;
    n1->color = n2->color;
    n2->color = tmpColor;
}






void printBestSellers(RBT &rbTree, int year){
    if(year == 0)
        return;
    Node *bestNa = rbTree.callBestNaSeller();
    Node *bestEu = rbTree.callBestEuSeller();
    Node *bestOther = rbTree.callBestOtherSeller();
    cout << "End of the " << year << " Year" << endl;
    cout << "Best seller in North America: "<< bestNa->publisher << " - "
        << bestNa->naSales << " million" << endl;
    cout << "Best seller in Europe: "<< bestEu->publisher << " - "
        << bestEu->euSales << " million" << endl;
    cout << "Best seller rest of the World: "<< bestOther->publisher << " - "
        << bestOther->otherSales << " million" << endl;
}
int main(){
    RBT rbTree;
    string headers;
    string myLine, word;
    fstream file;
    bool decadeStarted = false; 
    int control = 0;
    int yearControl = 0;
    file.open("VideoGames.csv");
    if(file.is_open()) {
        getline(file,headers);

        while (getline(file,myLine)){
            if(control == 54)
                break;
            control++;
            vector<string> videoGameLine;
            istringstream ss(myLine);

            while (getline(ss,word,','))
            {
                videoGameLine.push_back(word);
            }
            Node* videoGame = new Node(videoGameLine);

            if(videoGame->year % 10 == 1 && videoGame->year != yearControl && videoGame->year != 1981){
                printBestSellers(rbTree,videoGame->year-1);
            }
            yearControl = videoGame->year;  
            rbTree.insert(videoGame);

        }
        printBestSellers(rbTree,yearControl);
        rbTree.getTree();
        //rbTree.getRoot();
        file.close();
        cout << sizeof(100) << endl;
        cout << sizeof("100") << endl;

    }
}