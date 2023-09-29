#include <bits/stdc++.h> 

#define EMPTY_STRING ""
#include <dirent.h>
#include <vector>
using namespace std;
vector<string> files;
string folderpath="Folder/";
unordered_map <string,pair<int,string> > filesizes;

// A Tree node
struct Node
{
    char ch;
    int freq;
    Node *left, *right;
};

// Function to allocate a new tree node
Node* getNode(char ch, int freq, Node* left, Node* right)
{
    Node* node = new Node();

    node->ch = ch;
    node->freq = freq;
    node->left = left;
    node->right = right;

    return node;
}

// Comparison object to be used to order the heap
struct comp
{
    bool operator()(Node* l, Node* r)
    {
        // highest priority item has lowest frequency
        return l->freq > r->freq;
    }
};

// Utility function to check if Huffman Tree contains only a single node
bool isLeaf(Node* root)
{
    return root->left == nullptr && root->right == nullptr;
}

// Traverse the Huffman Tree and store Huffman Codes in a map.
void encode(Node* root, string str, unordered_map<char, string> &huffmanCode)
{
    if (root == nullptr)
    {
        return;
    }

    // found a leaf node
    if (isLeaf(root))
    {
        huffmanCode[root->ch] = (str != EMPTY_STRING) ? str : "1";
    }

    encode(root->left, str + "0", huffmanCode);
    encode(root->right, str + "1", huffmanCode);
}
int value_of_char( char c )
{
  return (unsigned char)c;
}
string decimalToBinary(char letter)
{
        cout<<"\n"<<letter<<endl;
        unsigned int n =  value_of_char(letter);;
        cout<< n<<endl;
    //finding the binary form of the number and
    //coneverting it to string.
    string s = bitset<64> (n).to_string();

    //Finding the first occurance of "1"
    //to strip off the leading zeroes.
    const auto loc1 = s.find('1');

    if(loc1 != string::npos)
        return s.substr(loc1);

    return "0";
}
string ASCIIToBinary(string str)
{
    int n = str.length();
    char char_array[n + 1];
    string bin="";
    string binary_string="";
    char zero='0';
    strcpy(char_array, str.c_str());

    for (int i = 0; i < n; i++)
    {
        //int * r=ee(char_array[i]);
        bin= decimalToBinary(char_array[i]);
                cout<<bin<<"\n";
        int n = 8-bin.size();
        if(n!=0){
                bin.insert(0, n,zero);
        }
        binary_string.append(bin);
    }
    //string bin=ASCIIToBinary(s);
    cout << binary_string << endl;

   return binary_string;

}
string BinaryToASCII(string binaryString)
{
    string text = EMPTY_STRING;
    stringstream sstream(binaryString);
    while (sstream.good())
    {
        bitset<8> bits;
        sstream >> bits;
        text += char(bits.to_ulong());
    }
    return text;

}
// Builds Huffman Tree and decode given input text
void buildHuffmanTree(string text)
{

    // base case: empty string
    if (text == EMPTY_STRING)
    {
        return;
    }

    // count frequency of appearance of each character
    // and store it in a map
    unordered_map<char, int> freq;
    for (char ch: text)
    {
        freq[ch]++;
    }

    // Create a priority queue to store live nodes of
    // Huffman tree;
    priority_queue<Node*, vector<Node*>, comp> pq;

    // Create a leaf node for each character and add it
    // to the priority queue.
    for (auto pair: freq)
    {
        pq.push(getNode(pair.first, pair.second, nullptr, nullptr));
    }

    // do till there is more than one node in the queue
    while (pq.size() != 1)
    {
        // Remove the two nodes of highest priority
        // (lowest frequency) from the queue

        Node *left = pq.top();
        pq.pop();
        Node *right = pq.top();
        pq.pop();

        // Create a new internal node with these two nodes
        // as children and with frequency equal to the sum
        // of the two nodes' frequencies. Add the new node
        // to the priority queue.

        int sum = left->freq + right->freq;
        pq.push(getNode('\0', sum, left, right));
    }

    // root stores pointer to root of Huffman Tree
    Node* root = pq.top();

    // Traverse the Huffman Tree and store Huffman Codes
    // in a map. Also print them

    unordered_map<char, string> huffmanCode;
    encode(root, EMPTY_STRING, huffmanCode);



    int size_compressed=0;
    int size_uncompressed =text.size()*8;
    for (auto pair: huffmanCode)
    {
        size_compressed=size_compressed+freq[pair.first]*(pair.second.size());
        cout<<freq[pair.first]<<"*"<<pair.second.size()<<endl;
    }
    cout<<"size of compressed file: "<<size_compressed<<endl;
    cout<<"size of decompressed file: "<<size_uncompressed<<endl;


    cout << "Huffman Codes are:\n" << '\n';
    for (auto pair: huffmanCode)
    {
        cout << pair.first << " " << bitset<8>(pair.first).to_string() << " "<< pair.second << '\n';

    }

    cout << "\nOriginal string is:\n" << text << '\n';
    // Print encoded string
    string binaryString;
    for (char ch: text)
    {
        binaryString += huffmanCode[ch];
    }
    cout << "\nEncoded string is:\n" << binaryString << endl;
    int n=binaryString.size();
    ofstream MyWriteFile("compressed.txt");
    int num_zeros=0;
    if(n%8!=0)
    {
        num_zeros=8-(n%8);
        cout<<num_zeros<<endl;

        for(int i=0; i<num_zeros; i++)
        {
            binaryString.append("0");
        }
    }

    MyWriteFile <<"padding"<<'\n'<< num_zeros<<'\n';
    MyWriteFile <<"ASCII"<<'\n'<< BinaryToASCII(binaryString) <<"\n";
    cout<<"ASCII"<<'\n'<< BinaryToASCII(binaryString) <<"\n";

    MyWriteFile<< "huffmanCode"<<'\n';
    string first;
    for (auto pair: huffmanCode)
    {
        if(pair.first==' ')
            first="space";
        else if (pair.first=='\n')
            first="newline";
        else
            first=pair.first;
        MyWriteFile << first << " " << pair.second << '\n';

    }
    MyWriteFile.close();
}

void decode1()
{
    cout << "\nDecoded string is:\n";
    std::ifstream MyWriteFile("compressed.txt");
    string line;
    string ASCII_Code="";
    int padding=0;
    string key;
    bool asciibool=false;
    string value;
    unordered_map<string, string> huffmanCode;
    while(getline(MyWriteFile, line))
    {
        if (line == "padding")
        {
            getline(MyWriteFile, line);
            stringstream number(line);
            number >> padding;
        }
        else if (line == "huffmanCode")
        {
            while (MyWriteFile >> key >> value)
            {
                cout << "Read: " << key << " " << value << endl;
                if(key=="space"){
                    key=" ";
                }
                else if(key=="newline")
                {
                    key="\n";
                }
                huffmanCode[key]=value;
            }
        }
        else if (line == "ASCII"||asciibool==true)
        {
            asciibool=true;
            getline(MyWriteFile, line);
            ASCII_Code.append(line);
            cout << line<<'\n';
        }


    }
    MyWriteFile.close();
    ASCII_Code.resize (ASCII_Code.size()-1);
    string encoded=ASCIIToBinary(ASCII_Code);
    cout<<  "\nencoded string before is:\n" <<encoded<<endl;
    if (encoded.size () > 0)
        encoded.resize (encoded.size () - padding);
    cout<<  "\nencoded string after is:\n" <<encoded<<endl;
    string trial;
    string decoded;
    for (std::string::size_type i = 0; i < encoded.size(); i++)
    {
        {
            trial.push_back(encoded[i]);
            bool bResult = false;
            auto it = huffmanCode.begin();
            while(it != huffmanCode.end())
            {
                // Check if value of this entry matches with given value
                if(it->second == trial)
                {
                    decoded.append(it->first);
                    trial.clear();
                }
                // Go to next entry in map
                it++;
            }
        }
    }
    cout<<  "\decoded string is:\n" <<decoded<<endl;

}
vector<string> openn(string path )
{
    //cout <<"in folder"<<endl;
    DIR*    dir;
    dirent* pdir;
    fstream my_file;
    string s1,s3,filepath;
    string allstrings;
    dir = opendir(path.c_str());

    while (pdir = readdir(dir))
    {
        s3= pdir->d_name;
        filepath= folderpath + s3;
        files.push_back(filepath);
        if( (strcmp(pdir->d_name,".")==0 )|| (strcmp(pdir->d_name,"..")==0))
            files.pop_back();


        for (auto i=0; i<files.size(); i++)
        {
            my_file.open( files[i] );
            while (getline(my_file,s1))
            {
                //filesizes.insert(make_pair(s1,s1.length()));
                filesizes.insert(make_pair(s1,make_pair(s1.length(),pdir->d_name)));
                if (files.size()==3)
                    allstrings.append(s1);
            }

            my_file.close();

        }
    }
    cout<<"All strings appended=" <<allstrings<<endl;

    for (auto i = filesizes.begin(); i != filesizes.end(); i++)
    {
        cout << i->first << " = (" << i->second.first << "," <<i->second.second <<")"<<endl;

    }
    buildHuffmanTree(allstrings);
    return files;
}


// Huffman coding algorithm
int main()
{
    //string text = "Huffman coding is a data compression algorithm.";
    char choice;

    do
    {
        cout << "A) Compress file\nB) Decompress file \nC) Compress folder \nD) Decompress folder \nQ) Quit" << endl;
        cin >> choice;
        if (choice == 'a' || choice == 'A')
        {
            cout << "Compress file" << endl;
            std::ifstream inFile;
            inFile.open("decompressed.txt"); //open the input file

            std::stringstream strStream;
            strStream << inFile.rdbuf(); //read the file
            std::string str = strStream.str(); //str holds the content of the fil
            buildHuffmanTree(str);
        }
        else if (choice == 'b' || choice == 'B')
        {
            cout << "Decompress file" << endl;
            decode1();
        }
        else if (choice =='c'|| choice== 'C')
        {
            vector<string> f;
            f = openn(folderpath);

        }
        else if (choice =='d'|| choice== 'D')
        {

        }
    }
    while (choice != 'q' && choice != 'Q');
    cout << "Exit" << endl;
    system("PAUSE");

    return 0;
}