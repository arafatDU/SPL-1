#include<bits/stdc++.h>
using namespace std;



class Huffman
{
	private:
	/*** Node structure ***/
	class node
	{
		public:
		char data;
		node * left,* right;
	
		node(char item)   // node constructor
		{
			data = item;
			left = right = 0;
		}
	};
	typedef node * nodePointer;
	public:
		Huffman();
		void buildDecodingTree(ifstream & codeIn);
		void insert(char ch, string code);
		void printTree(ostream & out, nodePointer root, int indent);
		void displayDecodingTree(ostream & out);
		private:
		nodePointer root;
	};
inline Huffman::Huffman()
{
	root = new node('*');
}
inline void Huffman::displayDecodingTree(ostream & out)
{
	printTree(out, root, 0);
}
//======================================================================================
void Huffman::buildDecodingTree(ifstream & codeIn)
{
	char ch;
	string code;
	for (;;)
	{
		if ( codeIn.eof() ) return;
		codeIn >> ch >> code;
			cout<<"\n"<<ch<<" = "<<code;
			if(ch == '_')
			{
				ch = ' ';
			}
			insert(ch, code);
	}
}


void Huffman::insert(char ch, string code)          //traverses each character from file from its huffman code      
{
	Huffman::nodePointer p = root;
	for(int i = 0; i < code.length(); i++)
	{
		switch (code[i])
		{
		case '0' :
				if (p->left == 0) // create node along path
				p->left = new Huffman::node('*');
				p = p->left;
				break;
		case '1' :
				if (p->right == 0) // create node along path
				p->right = new Huffman::node('*');
				p = p->right;
				break;
		default:
			cerr << "*** Illegal character in code ***\n";
			exit(1);
		}
	}
	p->data = ch;
}

	
void Huffman::printTree(ostream & out, Huffman::nodePointer root,int indent)  //To print huffman tree
{
	if (root != 0)
	{
		printTree(out, root->right, indent + 4);
		out << setw(indent) << " " << root->data << endl;
		printTree(out, root->left, indent + 4);
	}
}



int main()
{
	int total_bits_written;
	int current_bits = 0;
	char data;
	char filename[32];
	cout<<"\n\t====================================";
	cout<<"\n\t\t Text File Decompressor\n";
	cout<<"\t====================================";
	cout << "\n\nEnter name of code file: ";
	cin >> filename;
    ifstream codestream(filename);
	if (!codestream.is_open())
	{
		cout <<"Error !!! Cannot open code file.\n";
		exit(1);
	}
	Huffman h;
	h.buildDecodingTree(codestream);
	cout << "\nHere is the Huffman decoding tree:\n";
	h.displayDecodingTree(cout);
	cout << endl;

    return 0;
}