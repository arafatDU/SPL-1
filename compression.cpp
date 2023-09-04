// C++ Program for Huffman Coding
#include<bits/stdc++.h>
using namespace std;

string str;
int q = 0;
int count3 = 0;
int count1 = 0;
string codes;
int num = 254;
char chh = char(num);

vector<pair<char,string> > charactermap;
vector<int> Huffcode;




struct MinHeapNode 
{
    //input characters
    int data;

    // frequency of the characters
    int freq;

    // left and right child
    MinHeapNode *left, *right;

    MinHeapNode(int data, int freq)
    {
        left = NULL;
        right = NULL;
        this->data = data;
        this->freq = freq;
    }

};

// comparision of two head nodes
// it will need in min heap
struct compare 
{
    bool operator()(MinHeapNode* l, MinHeapNode* r)
    {
        if(l->freq > r->freq) return true;
        else return false;
    }
};



void calculate_frequency(int frequency[],char filename[])
{
    char ch,ch1;
    int y;
    int f;
   int x;
    ifstream fin;
    fin.open(filename);
	if(!fin.is_open())
	{
		cout <<"Error !!! Cannot open Source file.\n";
		cin>>y;
		exit(1);
	}
    while(fin.get(ch))
    {
        
	if( ch >=32 && ch < 255)
	{
		charactermap.push_back(make_pair(ch,""));
		count3++;
	}
        y = int(ch);
        frequency[y]=frequency[y]+1;
    }
     ch1 = char(129);
    charactermap.push_back(make_pair(ch1,""));
    count3++;
    fin.close();
}


void SeperateCharacter(int frequency[],int actual_frequency[],int ASCII_values[])
{
	int k =0;
   	 int y=0;
   	 for(int j=32;j < 254;j++)   //For seperating occurring and non-occurring characters & to create actual 
   	 {			             // frequency array & ASCII_values array
		if(frequency[j] != 0)
        	{
          	  actual_frequency[k] = frequency[j];
           	 ASCII_values[y] = j;
           	 y++;
           	 k++;
           	 count1++;
		}
    	}

	actual_frequency[y] = 1;     // For adding pseudo-EOF in actual_frequency & ASCII_values arrays
	ASCII_values[k] = 129;
	count1++;
}



void printFrequencyTable(int actual_frequency[],int ASCII_values[])
{
	char ch;
	string str = "";
	str = str+"Space";
	cout<<"\n\tFrequency Table of Characters present in file\n\n";
	cout<<"\t===================================================\n";
	cout<<"\tCharacters\t\tFrequency\n";
	cout<<"\t===================================================\n";
	for(int i=0; i < count1; i++)
	{
		ch = char(ASCII_values[i]);
		if(ch == ' ')
			cout<<"\t    "<<str<<"\t\t  "<<actual_frequency[i]<<"\n";
		else	
			cout<<"\t    "<<ch<<"\t\t\t  "<<actual_frequency[i]<<"\n";
	}
}

void printCodes(struct MinHeapNode* root, string str)
{

    if (!root)
        return;

    if (root->data != chh)
    {
		char ch;
		ch = char(root->data);
        cout <<"\t    "<< ch << "\t\t\t  " << str << "\n";
	if(root->data == ' ')
	{
		codes = codes + "_" + " " + str + "�";
	}
	else
		codes = codes + ch + " " + str + "�";
	for(int i = 0;i < count3;i++)
	{

		if(charactermap[i].first == ch)
		{
			charactermap[i].second = str;
		}
  	 } 
     }
    printCodes(root->left, str + "0");
    printCodes(root->right, str + "1");
}



// The main function that builds a Huffman Tree and
// print codes by traversing the built Huffman Tree
priority_queue<MinHeapNode*, vector<MinHeapNode*>, compare> minHeap;
void HuffmanCodes(int data[], int freq[], int size)
{
    struct MinHeapNode *left, *right, *top;

    // Create a min heap & inserts all characters of data[]


    for (int i = 0; i < size; ++i)                                //Build Heap
        minHeap.push(new MinHeapNode(data[i], freq[i]));

    // Iterate while size of heap doesn't become 1
    while (minHeap.size() != 1) {

        // Extract the two minimum
        // freq items from min heap
        left = minHeap.top();
        minHeap.pop();

        right = minHeap.top();
        minHeap.pop();

        // Create a new internal node with frequency equal to the sum of the two nodes frequencies. Make the
        // two extracted node as left and right children of this new node. Add this node to the min heap '■' is a special value for internal nodes, not used
        top = new MinHeapNode(chh, left->freq + right->freq);

        top->left = left;
        top->right = right;
        minHeap.push(top);
    }

    // Print Huffman codes using
    // the Huffman tree built above
    printCodes(minHeap.top(),"");
}


// Driver program to test above functions
int main()
{
    int frequency[256];
	int ASCII_values[256];
	int actual_frequency[256];
	char ch;
	char filename[50];
	char cfilename[50];
	string decode;
	cout<<"\n\t====================================";
	cout<<"\n\t\t  Text File Compressor\n";
	cout<<"\t====================================";
	cout<<"\n\nEnter Name of File to Compress : ";
	cin>>filename;

    //To set initial values of arrays
    for(int i=0;i < 255;i++)   
  	{
     		ASCII_values[i] = 0;
    	 	actual_frequency[i] = 0;
    	 	frequency[i] = 0;
   	}

    //To calculate frequency of each character in file
    calculate_frequency(frequency,filename);  	//To calculate frequency of each character in file
	
   	SeperateCharacter(frequency,actual_frequency,ASCII_values);  //Make actual frequency array and ASCII values
	
	printFrequencyTable(actual_frequency,ASCII_values);  // Print frequency table
	cout<<"\nCharacter Table with Huffman codes \n";
	cout<<"\n\tCharacter\t\tHuffman Code\n";
	
   	HuffmanCodes(ASCII_values,actual_frequency,count1);  //Function to build Huffman tree
}
