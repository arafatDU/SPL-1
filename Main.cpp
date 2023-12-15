#include<bits/stdc++.h>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;


void compressedAllFiles( vector<string> filePaths);
void decompressedAllFiles( vector<string> filePaths);
void huffCompression(string inpPath, string folderPath);
void huffDecompression(string inpPath, string folderPath);
void bwtCompress(string inpPath, string folderPath);
void bwtDecompress(string inpPath, string folderPath);



unsigned char check=0b10000000;

struct translation{
    translation *zero=NULL,*one=NULL;
    unsigned char character;
};

struct ersel{
    ersel *left,*right;
    long int number;
    unsigned char character;
    string bit;
};

bool erselcompare0(ersel a,ersel b){
    return a.number<b.number;
}


void write_from_uChar(unsigned char,unsigned char*,int,FILE*);
void huffcom(string filePath, string compressFolderPath);

void str_without_compress(char*);
unsigned char process_8_bits_NUMBER(unsigned char*,int,FILE*);
void process_n_bits_TO_STRING(unsigned char*,int,int*,FILE*,translation*,unsigned char);
void burn_tree(translation*);
void huffdecom(string compressedfile, string decomFolderPath);




class FileAccessor
{
public:
	static std::shared_ptr<std::vector<char>> GetSymbVectPtr(const std::string& path);

	static void WriteSymbVectToFile(const std::shared_ptr<std::vector<char>>& inpVect, const std::string& path);

	static long int GetFileSize(const std::string& path);
};

std::shared_ptr<std::vector<char>> FileAccessor::GetSymbVectPtr(const std::string& path)
{
	std::shared_ptr<std::vector<char>> symbVectorPtr = std::shared_ptr<std::vector<char>>(new std::vector<char>);
	std::ifstream file(path, std::ios::binary);
	if (file.is_open() && !file.fail())
	{
		char t;
		while (!file.eof()) {
			file.read((char*)&t, sizeof(char));
			symbVectorPtr->push_back(t);
		}
	}
	else
	{
		return nullptr;
	}
	file.close();
	symbVectorPtr->pop_back();
	return symbVectorPtr;
};

void FileAccessor::WriteSymbVectToFile(const std::shared_ptr<std::vector<char>>& inpVect, const std::string& path) {
	std::ofstream file;
	file.open(path, std::ios::binary);
	if (file.is_open() && !file.fail())
	{
		unsigned int from = 0;
		char* buff = new char[UCHAR_MAX];
		while (from < inpVect->size())
		{
			if (from + UCHAR_MAX < inpVect->size())
			{
				for (int i = 0; i < UCHAR_MAX; ++i)
				{
					buff[i] = inpVect->at(from + i);
				}
				file.write(buff, sizeof(char)*UCHAR_MAX);
				from += UCHAR_MAX;
			}
			else {
				for (int i = 0; i < inpVect->size() - from; ++i)
				{
					buff[i] = inpVect->at(from + i);
				}
				file.write(buff, sizeof(char)*(inpVect->size() - from));
				from = inpVect->size();
			}
		}
	}
	else {
		//error
	}
	file.close();
}

long FileAccessor::GetFileSize(const std::string & path)
{
	std::ifstream file(path, std::ios::binary);
	file.seekg(0, std::ios::end);
	long int file_size = file.tellg();
	return file_size;
}




class HuffLeaf {
public:
	char value;
	HuffLeaf* left;
	HuffLeaf* right;
	HuffLeaf* parent;
	bool isLeaf;
	unsigned long int freq;
public:
	HuffLeaf(char value_t, HuffLeaf* parent_t, int freq_t);

	HuffLeaf(HuffLeaf* right_t, HuffLeaf* left_t);

	~HuffLeaf();

	std::string GetCode(char inp);
};

class Huffman {
private:
	static std::map<char, unsigned int> GetFreqVect(std::shared_ptr<std::vector<char>> inpVect);

	static char pack_byte(bool bits[8]);

	static HuffLeaf* CreateTree(std::map<char, unsigned int> freqVect);

public:

	static void encode(const std::shared_ptr<std::vector<char>>& inpVect, const std::string& path);

	static std::shared_ptr<std::vector<char>> decode(const std::string& path);
};






HuffLeaf::HuffLeaf(char value_t, HuffLeaf* parent_t, int freq_t)
{
	value = value_t;
	parent = parent_t;
	freq = freq_t;
	isLeaf = true;
	right = nullptr;
	left = nullptr;
}

HuffLeaf::HuffLeaf(HuffLeaf* right_t, HuffLeaf* left_t) {
	right = right_t;
	left = left_t;
	freq = right_t->freq + left_t->freq;
	value = '\0';
	isLeaf = false;
	parent = nullptr;
}

HuffLeaf::~HuffLeaf()
{
	delete left;
	delete right;
}

std::string HuffLeaf::GetCode(char inp)
{
	if (left->isLeaf)
	{
		if (left->value == inp)
			return "1";
	}
	else
	{
		std::string v = left->GetCode(inp);
		if (v[0] != 'n')
			return "1" + v;
	}

	if (right->isLeaf)
	{
		if (right->value == inp)
			return "0";
	}
	else
	{
		std::string v = right->GetCode(inp);
		if (v[0] != 'n')
			return "0" + v;
	}
	return "n";
}

std::map<char, unsigned int> Huffman::GetFreqVect(std::shared_ptr<std::vector<char>> inpVect)
{
	std::map<char, unsigned int> res;
	for (auto c = inpVect->begin(); c < inpVect->end(); ++c)
	{
		auto iter = res.find(*c);
		if (iter == res.end())
		{
			res[*c] = 1;
		}
		else
		{
			if (res[*c] < INT32_MAX) res[*c]++;
		}
	}
	return res;
}

HuffLeaf* Huffman::CreateTree(std::map<char, unsigned int> freqVect)
{
	std::vector<HuffLeaf*> huffVect;
	for (auto f = freqVect.begin(); f != freqVect.end(); ++f)
	{
		huffVect.push_back(new HuffLeaf(f->first, nullptr, f->second));
	}

	while (huffVect.size() > 1)
	{
		std::sort(huffVect.begin(), huffVect.end(), [](const HuffLeaf* lhs, const HuffLeaf* rhs) {
			return lhs->freq > rhs->freq; });
		HuffLeaf* rightLeaf = huffVect.back();
		huffVect.pop_back();
		HuffLeaf* leftLeaf = huffVect.back();
		huffVect.pop_back();
		HuffLeaf* fuzed = new HuffLeaf(rightLeaf, leftLeaf);
		rightLeaf->parent = fuzed;
		leftLeaf->parent = fuzed;
		huffVect.push_back(fuzed);
	}

	return huffVect.at(0);
}

char Huffman::pack_byte(bool bits[8]) {
	char res(0);
	for (unsigned i(8); i--;)
	{
		res <<= 1;
		res |= char(bits[i]);
	}
	return res;
}

void Huffman::encode(const std::shared_ptr<std::vector<char>>& inpVect, const std::string& path) {
	std::ofstream out;
	out.open(path, std::ios::binary);
	if (out.is_open()) {

		unsigned int symbCount = inpVect->size();
		out.write((char*)&symbCount, sizeof(unsigned int));

		std::map<char, unsigned int> freqVect = GetFreqVect(inpVect);
		int freqSize = freqVect.size();

		out.write((char*)&freqSize, sizeof(int));
		for (auto f = freqVect.begin(); f != freqVect.end(); ++f)
		{
			out.write((char*)&f->first, sizeof(char));
			out.write((char*)&f->second, sizeof(int));
		}

		std::shared_ptr<HuffLeaf> TreeRoot = std::shared_ptr<HuffLeaf>(CreateTree(freqVect));
		std::map<char, std::string> encMap;

		std::vector<bool> codedBools;
		std::string codeS;
		for (int i = 0; i < inpVect->size(); ++i)
		{
			if (!encMap.count(inpVect->at(i)))
				encMap[inpVect->at(i)] = TreeRoot->GetCode(inpVect->at(i));
			codeS = encMap[inpVect->at(i)];
			for (int j = 0; j < codeS.size(); ++j)
				codedBools.push_back(codeS[j] == '1');
			if (codedBools.size() >= 8)
			{
				char s(0);
				bool bits[8];
				for (int i = 0; i < 8; ++i) {
					bits[i] = codedBools.front();
					codedBools.erase(codedBools.begin());
				}
				s = pack_byte(bits);
				out.write((char*)&s, sizeof(char));
			}
		}
		if (codedBools.size() > 0)
		{
			while (codedBools.size() < 8)
				codedBools.push_back(false);
			char s(0);
			bool bits[8];
			for (int i = 0; i < 8; ++i) {
				bits[i] = codedBools.front();
				codedBools.erase(codedBools.begin());
			}
			s = pack_byte(bits);
			out.write((char*)&s, sizeof(char));
		}
		out.close();
	}
	else
	{
		//error
		return;
	}
}

std::shared_ptr<std::vector<char> > Huffman::decode(const std::string & path) {
	std::ifstream fin;
	fin.open(path, std::ios::binary);
	if (fin.is_open())
	{
		unsigned int symbCount;
		fin.read((char*)&symbCount, sizeof(unsigned int));
		int freqVectSize;
		std::map<char, unsigned int> freqVect;
		fin.read((char*)&freqVectSize, sizeof(int));
		unsigned int freq;
		char val;
		for (unsigned int i = 0; i < freqVectSize; ++i)
		{
			fin.read((char*)&val, sizeof(char));
			fin.read((char*)&freq, sizeof(int));
			freqVect[val] = freq;
		}

		std::shared_ptr<HuffLeaf> TreeRoot = std::shared_ptr<HuffLeaf>(CreateTree(freqVect));
		HuffLeaf* currLeaf = TreeRoot.get();

		std::shared_ptr<std::vector<char>> outVect = std::shared_ptr<std::vector<char>>(new std::vector<char>);
		std::deque<bool> boolsToDecode;

		while (!fin.eof())
		{
			char s;
			fin.read((char*)&s, sizeof(char));

			for (int i = 0; i < 8; ++i)
				boolsToDecode.push_back(s & (1 << i));

			while (boolsToDecode.size() > 0)
			{
				if (currLeaf->isLeaf) {
					outVect->push_back(currLeaf->value);
					currLeaf = TreeRoot.get();
				}
				if (boolsToDecode.front())
					currLeaf = currLeaf->left;
				else
					currLeaf = currLeaf->right;
				boolsToDecode.pop_front();
			}
			if (currLeaf->isLeaf) {
				outVect->push_back(currLeaf->value);
				currLeaf = TreeRoot.get();
			}
		}

		while (outVect->size() > symbCount)
			outVect->pop_back();

		fin.close();
		return outVect;
	}
	else
	{
		fin.close();
		//error
        return nullptr;
	}
}





class BWT
{
private:
    static const unsigned char MaxBlockSize = UCHAR_MAX - 1;

    static std::vector<char> encodeBlock(std::vector<char> inpVect);

    static std::vector<char> decodeBlock(std::vector<char> inpVect);

public:
    static std::shared_ptr<std::vector<char>> encode(const std::shared_ptr<std::vector<char>> &inpVect);

    static std::shared_ptr<std::vector<char>> decode(const std::shared_ptr<std::vector<char>> &inpVect);
};
std::vector<char> BWT::encodeBlock(std::vector<char> inpVect)
{
    int blockSize = inpVect.size();
    std::vector<char *> transMat;

    for (int i = 0; i < blockSize; ++i)
    {
        transMat.push_back(new char[blockSize + 1]);
    }

    for (int i = 0; i < blockSize; ++i)
    {
        for (int j = 0; j < blockSize; ++j)
        {
            transMat.at(i)[j] = inpVect.at((i + j) % blockSize);
        }
    }
    for (int i = 0; i < blockSize; ++i)
    {
        transMat.at(i)[blockSize] = '\0';
    }

    std::sort(transMat.begin(), transMat.end(), [](const char *lhs, const char *rhs)
              { return strcmp(lhs, rhs) < 0; });

    char *inpChar = new char[blockSize + 1];
    for (int i = 0; i < blockSize; ++i)
    {
        inpChar[i] = inpVect.at(i);
    }
    inpChar[blockSize] = '\0';
    unsigned char inpIndex = 0;
    while (strcmp(inpChar, transMat[inpIndex]) != 0)
        inpIndex++;

    std::vector<char> result;
    result.push_back(inpIndex);
    for (int i = 0; i < blockSize; ++i)
    {
        result.push_back(transMat.at(i)[blockSize - 1]);
    }

    for (int i = 0; i < blockSize; ++i)
    {
        delete[] transMat.at(i);
    }

    return result;
}

std::vector<char> BWT::decodeBlock(std::vector<char> inpVect)
{
    unsigned char lineNumber = inpVect.front();
    inpVect.erase(inpVect.begin());
    int blockSize = inpVect.size();
    std::vector<char *> transMat;

    for (int i = 0; i < blockSize; ++i)
    {
        transMat.push_back(new char[blockSize + 1]);
    }

    for (int i = 0; i < blockSize; ++i)
    {
        for (int j = 0; j < blockSize; ++j)
        {
            transMat.at(i)[j] = '0';
        }
    }

    for (int i = 0; i < blockSize; ++i)
    {
        transMat.at(i)[blockSize] = '\0';
    }

    for (int j = blockSize - 1; j >= 0; --j)
    {
        for (int i = 0; i < blockSize; ++i)
        {
            transMat.at(i)[j] = inpVect.at(i);
        }
        std::sort(transMat.begin(), transMat.end(), [](const char *lhs, const char *rhs)
                  { return strcmp(lhs, rhs) < 0; });
    }

    std::vector<char> result;
    for (int i = 0; i < blockSize; ++i)
    {
        if (lineNumber >= blockSize)
            return result;
        result.push_back(transMat.at(lineNumber)[i]);
    }

    for (int i = 0; i < blockSize; ++i)
    {
        delete[] transMat.at(i);
    }

    return result;
}

std::shared_ptr<std::vector<char>> BWT::encode(const std::shared_ptr<std::vector<char>> &inpVect)
{
    unsigned int inpVectsize = inpVect->size();
    std::vector<char> *vect = new std::vector<char>;
    std::shared_ptr<std::vector<char>> result = std::shared_ptr<std::vector<char>>(vect);
    unsigned int from = 0;
    unsigned int to = 0;
    while (from < inpVectsize)
    {
        if (inpVectsize - from >= MaxBlockSize)
        {
            to = from + MaxBlockSize;
        }
        else
        {
            to = inpVectsize;
        }
        std::vector<char> inpblock;
        std::vector<char> outblock;
        for (unsigned int j = from; j < to; ++j)
            inpblock.push_back(inpVect->at(j));
        outblock = encodeBlock(inpblock);
        for (auto el = outblock.begin(); el < outblock.end(); ++el)
            result->push_back(*el);
        from = to;
    }
    return result;
}

std::shared_ptr<std::vector<char>> BWT::decode(const std::shared_ptr<std::vector<char>> &inpVect)
{
    unsigned int inpVectsize = inpVect->size();
    std::vector<char> *vect = new std::vector<char>;
    std::shared_ptr<std::vector<char>> result = std::shared_ptr<std::vector<char>>(vect);
    unsigned int from = 0;
    unsigned int to = 0;
    while (from < inpVectsize)
    {
        if (inpVectsize - from >= MaxBlockSize + 1)
        {
            to = from + MaxBlockSize + 1;
        }
        else
        {
            to = inpVectsize;
        }
        std::vector<char> inpblock;
        std::vector<char> outblock;
        for (unsigned int j = from; j < to; ++j)
            inpblock.push_back(inpVect->at(j));
        outblock = decodeBlock(inpblock);
        for (auto el = outblock.begin(); el < outblock.end(); ++el)
            result->push_back(*el);
        from = to;
    }
    return result;
}







const int WINDOW_SIZE = 4096;
const int DECOMP_BUFF_SIZE = 72;

class Llist
{
public:
    Llist();
    int pos;
    Llist *next;
};

Llist::Llist()
{
    pos = -1;
    next = 0;
}

class HashTable
{
public:
    HashTable();
    ~HashTable();
    void AddNode(const char *buff, int p);
    void RemoveNode(const char *buff, int p);
    Llist *SearchTable(const char *buff, int p);

private:
    Llist *Table[256][256];
};

HashTable::HashTable()
{
    // Allocate hash table for storing character indices
    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            Table[i][j] = new Llist;
        }
    }
}

HashTable::~HashTable()
{
    // Delete all nodes in the hash table
    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            Llist *temp = Table[i][j];
            Llist *old = temp;
            while (temp->next != 0)
            {
                temp = temp->next;
                delete old;
                old = temp;
            }

            delete temp;
        }
    }
}

void HashTable::AddNode(const char *buff, int p)
{
    // Traverse the linked list to the end
    Llist *temp = Table[(unsigned)buff[p]][(unsigned)buff[p + 1]];
    while (temp->next != 0)
    {
        temp = temp->next;
    }

    // Add a node at the end of the linked list
    temp->pos = p;
    temp->next = new Llist;
}
void HashTable::RemoveNode(const char *buff, int p)
{
    // Remove a node in the hash table
    Llist *temp = Table[(unsigned)buff[p]][(unsigned)buff[p + 1]];
    Llist *old = temp;
    while (temp->pos != (int)p)
    {
        old = temp;
        temp = temp->next;
    }
    while (old == temp)
    {
        temp = temp->next;
        old->pos = temp->pos;
        old->next = temp->next;
    }

    delete temp;
}
Llist *HashTable::SearchTable(const char *buff, int p)
{
    // Look up match in table
    Llist *find = Table[(unsigned)buff[p]][(unsigned)buff[p + 1]];
    return find;
}

// Forward declaraction of main functions
void lz77Compress( const string &fn, const string &outputfolder);
int find_match(HashTable *hash, const char *buff, int &winstart, int &current, const int size);
void lz77Decompress( const std::string &filename, const std::string &outputfolder);

class BitBuffer
{
public:
    BitBuffer(const string &filename);
    ~BitBuffer();
    void addOne();
    void addZero();
    void addByte(char byte);
    void addBytes(int location, int len);
    void writeFooter();

private:
    char bitbuff[8];
    int len;
    ofstream fout;
    void flushBuff();
};

BitBuffer::BitBuffer(const string &filename)
{
    len = 0;
    fout.open(filename, ios::out | ios::binary);
}

BitBuffer::~BitBuffer()
{
    flushBuff();
    writeFooter();
    flushBuff();
    fout.close();
}
void BitBuffer::addOne()
{
    bitbuff[len] = 49;
    len++;
    if (len == 8)
    {
        flushBuff();
        len = 0;
    }
}
void BitBuffer::addZero()
{
    bitbuff[len] = 48;
    len++;
    if (len == 8)
    {
        flushBuff();
        len = 0;
    }
}
void BitBuffer::addByte(char byte)
{
    for (int i = 0; i < 8; i++)
    {
        if (byte & 128)
        {
            addOne();
        }
        else
        {
            addZero();
        }

        byte <<= 1;
    }
}
void BitBuffer::addBytes(int location, int length)
{
    for (int i = 0; i < 12; i++)
    {
        if (location & 2048)
        {
            addOne();
        }
        else
        {
            addZero();
        }

        location <<= 1;
    }
    for (int i = 0; i < 5; i++)
    {
        if (length & 16)
        {
            addOne();
        }
        else
        {
            addZero();
        }
        length <<= 1;
    }
}
void BitBuffer::flushBuff()
{
    if (len == 8)
    {
        char temp = 0;
        for (int j = 0; j < 8; j++)
        {
            if (bitbuff[j] == 49)
            {
                temp = temp | 1;
            }
            else
            {
                temp = temp | 0;
            }
            if (j < 7)
            {
                temp <<= 1;
            }
        }
        fout.write(&temp, 1);
    }
}

void BitBuffer::writeFooter()
{
    char temp = 0;
    int length = len;
    for (int i = 0; i < length; i++)
    {
        if (bitbuff[i] == 49)
        {
            temp = temp | 1;
        }
        else
        {
            temp = temp | 0;
        }

        temp <<= 1;
    }
    len = 0;

    for (int i = 0; i < 8 - length; i++)
    {
        temp = temp | 0;
        if (i < 8 - length - 1)
        {
            temp <<= 1;
        }
    }
    addByte(temp);
    addByte(8 - length + 1);
}








class FileManager
{
    private:
        string file;
        DIR* dirHandle;
        struct dirent* dirRent;

    public:
        FileManager();
        ~FileManager();

        // GET-Methods
        string getFile();

        // SET-Methods
        void setFile(string file);

        // Class-Methods
        bool checkIsFile(string pth);
        vector<string> ruleOutFolders(vector<string> contentList);
        vector<string> getFileList(vector<string> contentList);
};

FileManager::FileManager()
{
    file = "./"; // Set a default string for initialization, will change in use
}

FileManager::~FileManager()
{
}

string FileManager::getFile()
{
    return file;
}

void FileManager::setFile(string file)
{
    this->file = file;
}

bool FileManager::checkIsFile(string pth)
{
    struct stat fileInfo;
    if (stat(pth.c_str(), &fileInfo) == 0)
    {
        return S_ISREG(fileInfo.st_mode); // It's a regular file
    }
    return false; // Not a regular file
}

vector<string> FileManager::ruleOutFolders(vector<string> contentList)
{
    int x = contentList.size();
    vector<string> ruleOut;

    for (int i = 0; i < x; i++)
    {
        if (checkIsFile(contentList.at(i)))
        {
            ruleOut.push_back(contentList.at(i));
        }
    }
    return ruleOut; // Output Vector with Files
}

vector<string> FileManager::getFileList(vector<string> contentList)
{
    int x = contentList.size();
    vector<string> fileList;

    for (int i = 0; i < x; i++)
    {
        if (checkIsFile(contentList.at(i)))
        {
            fileList.push_back(contentList.at(i));
        }
    }
    return fileList; // Output Vector with Files
}

class DirectoryManager
{
    private:
        string dirPath;
        DIR* dirHandle;
        struct dirent* dirRent;

    public:
        DirectoryManager();
        ~DirectoryManager();

        // GET-Methods
        string getDirPath();

        // SET-Methods
        void setDirPath(string dirPath);

        // Class-Methods
        bool checkIsDirectory(string directory);
        vector<string> getDirectoryList();
        vector<string> getFullDirectoryList();
        vector<string> sortOutFiles(vector<string> folderList);
};

DirectoryManager::DirectoryManager()
{
    dirPath = "./"; // set the current folder you are in as default
}

DirectoryManager::~DirectoryManager()
{
}

string DirectoryManager::getDirPath()
{
    return dirPath;
}

void DirectoryManager::setDirPath(string dirPath)
{
    this->dirPath = dirPath;
}

bool DirectoryManager::checkIsDirectory(string directory)
{
    struct stat fileInfo;
    if (stat(directory.c_str(), &fileInfo) == 0)
    {
        return S_ISDIR(fileInfo.st_mode); // It's a directory
    }
    return false; // Not a directory
}

vector<string> DirectoryManager::getDirectoryList()
{
    vector<string> folderList;
    dirHandle = opendir(getDirPath().c_str());
    if (dirHandle)
    {
        while ((dirRent = readdir(dirHandle)) != NULL)
        {
            string rootPath = getDirPath() + dirRent->d_name;
            if (checkIsDirectory(rootPath))
            {
                folderList.push_back(rootPath);
            }
        }
        closedir(dirHandle); // Close the directory
    }
    return folderList;
}

vector<string> DirectoryManager::getFullDirectoryList()
{
    vector<string> folderList;
    dirHandle = opendir(getDirPath().c_str());
    if (dirHandle)
    {
        while ((dirRent = readdir(dirHandle)) != NULL)
        {
            string rootPath = getDirPath() + dirRent->d_name;
            folderList.push_back(rootPath);
        }
        closedir(dirHandle); // Close the directory
    }
    return folderList;
}

vector<string> DirectoryManager::sortOutFiles(vector<string> folderList)
{
    int x = folderList.size();
    vector<string> ruleOut;

    for (int i = 0; i < x; i++)
    {
        if (checkIsDirectory(folderList.at(i)))
        {
            ruleOut.push_back(folderList.at(i));
        }
    }
    return ruleOut; // Output Vector without Files
}

class FolderMGMT
{
    public:
        DirectoryManager* dirMGMT;
        FileManager* fileMGMT;

    public:
        FolderMGMT();
        ~FolderMGMT();

        //Class-Functions
        void outputFolderList();
        void outputFileList();
        string getDirectoryPath();
        vector<string> getFilePathList();
};

FolderMGMT::FolderMGMT()
{
    dirMGMT = new DirectoryManager;
    fileMGMT = new FileManager;
}

FolderMGMT::~FolderMGMT()
{
    delete (dirMGMT);
    delete (fileMGMT);
}



void FolderMGMT::outputFolderList()
{
    cout << "Here is all folders:" << endl;
    vector<string> list = dirMGMT->getDirectoryList();
    for (const string& entry : list)
    {
        cout << entry << endl;
    }
}

void FolderMGMT::outputFileList()
{
    cout << "Here is all files:" << endl;
    vector<string> list = fileMGMT->getFileList(dirMGMT->getFullDirectoryList());

    for (const string& filePth : list)
    {
        cout << filePth;

        // Get the file size
        struct stat fileStat;
        if(stat(filePth.c_str(), &fileStat) == 0)
        {
            // Display the file size in bytes
            cout<<"  ("<<fileStat.st_size << " bytes)";
        }
        cout<<endl;
    }
}

vector<string> FolderMGMT::getFilePathList()
{
    vector<string> list = fileMGMT->getFileList(dirMGMT->getFullDirectoryList());
    vector<string> filePaths; //store the file paths

    for (const string& filePth : list)
    {
        filePaths.push_back(filePth);
    }

    return filePaths;
}







void compressFolderUsingBWT( vector<string> filePaths)
{
    size_t commonDirPos = filePaths[0].find_last_of('/');
    string commonDirectory = filePaths[0].substr(0, commonDirPos);
    string decompressFolderPath = commonDirectory +  "/bwtComFolder";


    if (mkdir(decompressFolderPath.c_str(), 0777) == 0)
    {
        cout << "\nCreated folder: " << decompressFolderPath << endl;

        for (const string& filePath : filePaths)
        {
            bwtCompress(filePath, decompressFolderPath);      
        }
    }
    else
    {
        cout << "Failed to create folder: " << decompressFolderPath << endl;
    }
}


void compressFolderUsingLZ77( vector<string> filePaths)
{
    size_t commonDirPos = filePaths[0].find_last_of('/');
    string commonDirectory = filePaths[0].substr(0, commonDirPos);
    string decompressFolderPath = commonDirectory +  "/lzComFolder";


    if (mkdir(decompressFolderPath.c_str(), 0777) == 0)
    {
        cout << "\nCreated folder: " << decompressFolderPath << endl;

        for (const string& filePath : filePaths)
        {
            lz77Compress(filePath, decompressFolderPath);      
        }
    }
    else
    {
        cout << "Failed to create folder: " << decompressFolderPath << endl;
    }
}



void compressFolderUsingHuffman( vector<string> filePaths)
{
    size_t commonDirPos = filePaths[0].find_last_of('/');
    string commonDirectory = filePaths[0].substr(0, commonDirPos);
    string decompressFolderPath = commonDirectory +  "/huffComFolder";


    if (mkdir(decompressFolderPath.c_str(), 0777) == 0)
    {
        cout << "\nCreated folder: " << decompressFolderPath << endl;

        for (const string& filePath : filePaths)
        {
            //huffcom(filePath, decompressFolderPath);
            //lz77Compress(filePath, decompressFolderPath);
            huffCompression(filePath, decompressFolderPath);
        }
    }
    else
    {
        cout << "Failed to create folder: " << decompressFolderPath << endl;
    }
}







void compressedAllFiles( vector<string> filePaths)
{
    size_t commonDirPos = filePaths[0].find_last_of('/');
    string commonDirectory = filePaths[0].substr(0, commonDirPos);
    string decompressFolderPath = commonDirectory +  "/comFolder";
    string comparisonFile = commonDirectory + "/comparison.txt";


    long int size;
    long int hsize;
    long int lsize;
    long int bsize;

    std::ofstream compareFile(comparisonFile);


    if (mkdir(decompressFolderPath.c_str(), 0777) == 0)
    {
        cout << "\nCreated folder: " << decompressFolderPath << endl;

        for (const string& filePath : filePaths)
        {
            size_t lastSlash = filePath.find_last_of('/');
            string rawName = filePath.substr(lastSlash+1);

            size = FileAccessor::GetFileSize(filePath);

            huffcom(filePath, decompressFolderPath);
            //huffCompression(filePath, decompressFolderPath);
            string hOutPath = decompressFolderPath + "/" + rawName + ".huff";
            hsize = FileAccessor::GetFileSize(hOutPath);

            lz77Compress(filePath, decompressFolderPath);
            string lzOutPath = decompressFolderPath + "/" + rawName + ".lzss";
            lsize = FileAccessor::GetFileSize(lzOutPath);

            if(lsize < hsize)
            {
                if (std::remove(hOutPath.c_str()) == 0) {
                    std::cout << "File deleted successfully: " << hOutPath << std::endl;
                } else {
                std::cerr << "Error: Could not delete the file: " << hOutPath << std::endl;
                    
                }
            }else{
                if (std::remove(lzOutPath.c_str()) == 0) {
                    std::cout << "File deleted successfully: " << lzOutPath << std::endl;
                } else {
                std::cerr << "Error: Could not delete the file: " << lzOutPath << std::endl;
                    
                }
            }


            bsize = hsize;
            if(size < 6000000)
            {
                bwtCompress(filePath, decompressFolderPath);
                string bOutPath = decompressFolderPath + "/" + rawName + ".bwt";
                bsize = FileAccessor::GetFileSize(bOutPath);

                if(bsize < lsize)
                {
                    if (std::remove(lzOutPath.c_str()) == 0) {
                        std::cout << "File deleted successfully: " << lzOutPath << std::endl;
                    } else {
                        std::cerr << "Error: Could not delete the file: " << lzOutPath << std::endl;
                    
                    }   
                }else{
                    if (std::remove(bOutPath.c_str()) == 0) {
                        std::cout << "File deleted successfully: " << bOutPath << std::endl;
                    } else {
                        std::cerr << "Error: Could not delete the file: " << bOutPath << std::endl;
                    
                    }
                }
            }

            cout<<"Size: "<<size<<"   Huffman: "<<hsize<<"   LZ77: "<<lsize<<"   BWT: "<< bsize <<endl;
        
            if (compareFile.is_open()) 
            {
                compareFile << "-----------------------------------------------" <<endl;
                compareFile<<"FilePath: "<<filePath<<endl;
                compareFile << "-----------------------------------------------" <<endl<<endl;
                
                compareFile<<"Original File Size: "<<size<<" Bytes"<<endl;
                compareFile<<"Huffman Compressed File Size: "<<hsize<<" Bytes"<<endl;
                compareFile<<"LZ77 Compressed File Size: "<<lsize<<" Bytes"<<endl;
                compareFile<<"BWT Compressed File Size: "<<bsize<<" Bytes"<<endl<<endl;
                compareFile<<"Compression Ratio:-"<<endl;
                compareFile<<"Huffman Coding Algorithm = "<<100*(float)hsize/size<<endl;
                compareFile<<"LZ77 Algorithm = "<<100*(float)lsize/size<<endl;
                compareFile<<"BWT Algorithm = "<<100*(float)bsize/size<<endl<<endl<<endl<<endl;
            } else {
                std::cerr << "Error: Could not open the file for writing." << std::endl;
            }

        }
        compareFile.close();
    }
    else
    {
        cout << "Failed to create folder: " << decompressFolderPath << endl;
    }
}



void decompressedAllFiles( vector<string> filePaths)
{
    size_t commonDirPos = filePaths[0].find_last_of('/');
    string Directory = filePaths[0].substr(0, commonDirPos);
    string commonDirectory = filePaths[0].substr(0, Directory.find_last_of('/'));
    string decompressFolderPath = commonDirectory +  "/decomFolder";



    if (mkdir(decompressFolderPath.c_str(), 0777) == 0)
    {
        cout << "\nCreated folder: " << decompressFolderPath << endl;

        for (const string& filePath : filePaths)
        {
            //huffdecom(filePath, decompressFolderPath);
            //lz77Decompress(filePath, decompressFolderPath);
            //huffDecompression(filePath, decompressFolderPath);

            size_t lastDot = filePath.find_last_of('.');
            string compressMode = filePath.substr(lastDot+1);

            if(compressMode == "huff")
            {
                huffDecompression(filePath, decompressFolderPath);
            }else if(compressMode == "lzss")
            {
                lz77Decompress(filePath, decompressFolderPath);
            }else if(compressMode == "bwt")
            {
                cout<<"Loading..."<<endl;
            }else{
                cout<<"Couldn't find compress mode."<<endl;
            }
        }
    }
    else
    {
        cout << "Failed to create folder: " << decompressFolderPath << endl;
    }
}



int main() {
    int choice = 0;
    string path = "";
    bool running = true;

    // Creating an instance of FolderMGMT
    FolderMGMT* fmgmt = new FolderMGMT;

    while (running) {
        cout << endl << endl << endl << endl << endl << endl;
        cout << "\n########################################\n"
             << "\t\tFolderMGMT\n\n"
             << "Path:\t\t[" << fmgmt->dirMGMT->getDirPath() << "]" << endl
             << "\n########################################\n"
             << "1 - change path" << endl
             << "2 - output folders in path" << endl
             << "3 - output files in path" << endl
             << "4 - Compress Your Folder" << endl
             << "5 - Compress Folder using Huffman" << endl
             << "6 - Compress Folder using LZ77" << endl
             << "7 - Compress Folder using BWT" << endl
             << "8 - Decompress Your Folder" << endl
             << "9 - exit" << endl;

        cout << "Enter: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "\nPlease enter a path: ";
                cin >> path;
                fmgmt->dirMGMT->setDirPath(path);
                break;
            case 2:
                cout << "\nOutput of [" << fmgmt->dirMGMT->getDirPath() << "]" << endl;
                fmgmt->outputFolderList();
                cin.get();
                break;
            case 3:
                cout << "\nOutput of [" << fmgmt->dirMGMT->getDirPath() << "]" << endl;
                fmgmt->outputFileList();
                cin.get();
                break;
            case 4:
                cout << "\n---------------------------------------------------"
                     << "\nCompression of [" << fmgmt->dirMGMT->getDirPath() << "]\n"
                     << "---------------------------------------------------" << endl;
                compressedAllFiles(fmgmt->getFilePathList());
                cin.get();
                break;
            case 5:
                cout << "\n---------------------------------------------------"
                     << "\nCompression of [" << fmgmt->dirMGMT->getDirPath() << "]\n"
                     << "---------------------------------------------------" << endl;
                compressFolderUsingHuffman(fmgmt->getFilePathList());
                cin.get();
                break;
            case 6:
                cout << "\n---------------------------------------------------"
                     << "\nCompression of [" << fmgmt->dirMGMT->getDirPath() << "]\n"
                     << "---------------------------------------------------" << endl;
                compressFolderUsingLZ77(fmgmt->getFilePathList());
                cin.get();
                break;
            case 7:
                cout << "\n---------------------------------------------------"
                     << "\nCompression of [" << fmgmt->dirMGMT->getDirPath() << "]\n"
                     << "---------------------------------------------------" << endl;
                compressFolderUsingBWT(fmgmt->getFilePathList());
                cin.get();
                break;
            case 8:
                {string folderName;
                cout<<"Enter folder name: ";
                cin>>folderName;
                string newPath = path + folderName + "/";
                fmgmt->dirMGMT->setDirPath(newPath);

                cout << "\n---------------------------------------------------"
                     << "\nDecompression of [" << fmgmt->dirMGMT->getDirPath() << "]\n"
                     << "---------------------------------------------------" << endl;
                decompressedAllFiles(fmgmt->getFilePathList());
                cin.get();
                fmgmt->dirMGMT->setDirPath(path);
                break;}
            case 9:
                cout << "Exiting FolderMGMT" << endl;
                running = false;
                break;
            default:
                cout << "wrong entry" << endl;
                break;
        }
    }

    delete fmgmt;

    return 0;
}





void huffCompression(string inpPath, string folderPath)
{
    cout<<inpPath<<endl;
    cout<<folderPath<<endl;
    std::shared_ptr<std::vector<char>> inpData = FileAccessor::GetSymbVectPtr(inpPath);
	if (!inpData) return;

    size_t lastSlash = inpPath.find_last_of('/');
    string rawName = inpPath.substr(lastSlash+1);
    string outPath = folderPath + "/" + rawName + ".huff";

    Huffman::encode(inpData, outPath);

}

void huffDecompression(string inpPath, string folderPath)
{
    std::shared_ptr<std::vector<char>> outData = Huffman::decode(inpPath);

    size_t lastSlash = inpPath.find_last_of('/');
    string rawName = inpPath.substr(lastSlash+1);
    size_t lastDot = rawName.find_last_of('.');
    string name = rawName.substr(0, lastDot);
    string outPath = folderPath + "/New-h-" + name;

    FileAccessor::WriteSymbVectToFile(outData, outPath);

}




void bwtCompress(string inpPath, string folderPath)
{
    std::shared_ptr<std::vector<char>> inpData = FileAccessor::GetSymbVectPtr(inpPath);
	if (!inpData) return;
	std::shared_ptr<std::vector<char>> BWTData;


	BWTData = BWT::encode(inpData);



    size_t lastSlash = inpPath.find_last_of('/');
    string rawName = inpPath.substr(lastSlash+1);
    string outPath = folderPath + "/" + rawName + ".bwt";

    Huffman::encode(BWTData, outPath);

}

void bwtDecompress(string inpPath, string folderPath)
{
	cout<<"Huffman encode continue..."<<endl;
	std::shared_ptr<std::vector<char>> Data = Huffman::decode(inpPath);

	cout<<"BWT continue..."<<endl;
    std::shared_ptr<std::vector<char>> outData = BWT::decode(Data);

    size_t lastSlash = inpPath.find_last_of('/');
    string rawName = inpPath.substr(lastSlash+1);
    size_t lastDot = rawName.find_last_of('.');
    string name = rawName.substr(0, lastDot);
    string outPath = folderPath + "/New-b-" + name;

	cout<<"Writting to folder..."<<endl;
    FileAccessor::WriteSymbVectToFile(outData, outPath);

}






void huffcom(string filePath, string decompressFolderPath){
    long int number[256];
    long int bits=0,total_bits=0;
    int letter_count=0;
    const char* file_name_cstr = filePath.c_str();
    
    for(long int *i=number;i<number+256;i++){                       
        *i=0;
    }
    
    string scompressed, OriginalFileName;

    size_t lastSlashPos = filePath.find_last_of('/');
    OriginalFileName = filePath.substr(lastSlashPos + 1);

    register FILE *original_fp=fopen(file_name_cstr,"rb"),*compressed_fp;
    if(NULL==original_fp){
        cout<<filePath<<" file does not exist"<<endl;
        return;
    }
    scompressed= decompressFolderPath + "/" + OriginalFileName;
    scompressed+=".huff";



    //--------------------1------------------------
    fseek(original_fp,0,SEEK_END);
    long int size=ftell(original_fp);
    rewind(original_fp);

    //--------------------2------------------------
    register unsigned char x;
    fread(&x,1,1,original_fp);
    for(long int i=0;i<size;i++){
        number[x]++;
        fread(&x,1,1,original_fp);
    }
    rewind(original_fp);

	for(long int *i=number;i<number+256;i++){                 
        	if(*i){
			letter_count++;
			}
    }
        // This code block counts number of times that all of the unique bytes is used on the first block
        // and stores that info in 'number' array
        // after that it checks the 'number' array and writes the number of unique byte count to 'letter_count' variable

    //--------------------3------------------------
    ersel array[letter_count*2-1];
    register ersel *e=array;
    for(long int *i=number;i<number+256;i++){                         
        	if(*i){
                e->right=NULL;
                e->left=NULL;
                e->number=*i;
                e->character=i-number;
                e++;
            }
    }
    sort(array,array+letter_count,erselcompare0);             
    
    //-------------------4------------------------- min1 and min2 represents nodes that has minimum weights
    ersel *min1=array,*min2=array+1,*current=array+letter_count,*notleaf=array+letter_count,*isleaf=array+2;            
    for(int i=0;i<letter_count-1;i++){                           
        current->number=min1->number+min2->number;
        current->left=min1;
        current->right=min2;
        min1->bit="1";
        min2->bit="0";     
        current++;
        
        if(isleaf>=array+letter_count){
            min1=notleaf;
            notleaf++;
        }
        else{
            if(isleaf->number<notleaf->number){
                min1=isleaf;
                isleaf++;
            }
            else{
                min1=notleaf;
                notleaf++;
            }
        }
        
        if(isleaf>=array+letter_count){
            min2=notleaf;
            notleaf++;
        }
        else if(notleaf>=current){
            min2=isleaf;
            isleaf++;
        }
        else{
            if(isleaf->number<notleaf->number){
                min2=isleaf;
                isleaf++;
            }
            else{
                min2=notleaf;
                notleaf++;
            }
        }
        
    }
    
    //-------------------5-------------------------
    for(e=array+letter_count*2-2;e>array-1;e--){
        if(e->left){
            e->left->bit=e->bit+e->left->bit;
        }
        if(e->right){
            e->right->bit=e->bit+e->right->bit;
        }
        
    }

    compressed_fp=fopen(&scompressed[0],"wb");

    //-------------writes first---------------
    {
        long int temp_size=size;
        unsigned char temp;
        for(int i=0;i<8;i++){
            temp=temp_size%256;
            fwrite(&temp,1,1,compressed_fp);
            temp_size/=256;
        }
        total_bits+=64;
    }

    //-----------writes second----------------
    fwrite(&letter_count,1,1,compressed_fp);
    total_bits+=8;
    //----------------------------------------
    

    //----writes third and forth---------------
    {
        cout<<"If you want a password write any number other then 0"<<endl
            <<"If you do not, write 0"<<endl;
        int check_password;
        //cin>>check_password;
        check_password = 0;
        if(check_password){
            string password;
            cout<<"Enter your password (Do not use whitespaces): ";
            cin>>password;
            int password_length=password.length();
            if(password_length==0){
                cout<<"You did not enter a password"<<endl<<"Process has been terminated"<<endl;
                fclose(compressed_fp);
                fclose(original_fp);
                remove(&scompressed[0]);
                return;
            }
            if(password_length>100){
                cout<<"Password cannot contain more then 100 characters"<<endl<<"Process has been terminated"<<endl;
                fclose(compressed_fp);
                fclose(original_fp);
                remove(&scompressed[0]);
                return;
            }
            unsigned char password_length_unsigned=password_length;
            fwrite(&password_length_unsigned,1,1,compressed_fp);
            fwrite(&password[0],1,password_length,compressed_fp);
            total_bits+=8+8*password_length;
        }
        else{
            fwrite(&check_password,1,1,compressed_fp);
            total_bits+=8;
        }
    }
        //Above code block puts password to compressed file
    //----------------------------------------

    //------------writes fifth----------------
    char *str_pointer;
    unsigned char current_byte,len,current_character;
    int current_bit_count=0;
    string str_arr[256];
    for(e=array;e<array+letter_count;e++){
        str_arr[(e->character)]=e->bit;
        len=e->bit.length();
        current_character=e->character;

        write_from_uChar(current_character,&current_byte,current_bit_count,compressed_fp);
        write_from_uChar(len,&current_byte,current_bit_count,compressed_fp);
        total_bits+=len+16;

        str_pointer=&e->bit[0];
        while(*str_pointer){
            if(current_bit_count==8){
                fwrite(&current_byte,1,1,compressed_fp);
                current_bit_count=0;
            }
            switch(*str_pointer){
                case '1':current_byte<<=1;current_byte|=1;current_bit_count++;break;
                case '0':current_byte<<=1;current_bit_count++;break;
                default:cout<<"An error has occurred"<<endl<<"Compression process aborted"<<endl;
                fclose(compressed_fp);
                fclose(original_fp);
                remove(&scompressed[0]);
                return;
            }
           str_pointer++;
        }
        
         bits+=len*e->number;
    }           
    total_bits+=bits;
    unsigned char bits_in_last_byte=total_bits%8;
    if(bits_in_last_byte){
        total_bits=(total_bits/8+1)*8;
        // from this point on total bits doesnt represent total bits but
        // instead it represents 8*number_of_bytes we are gonna use on our compressed file
    }
    /* Above loop of the code is doing 2 in this order
    1- It determines number of bits that we are gonna write to the compressed file
        (this number only represents number of bytes thats going to be translated it doesn't include translation script
    2-It writes the translation script into [name of the original].compressed file and the str_arr array */
    //----------------------------------------

    cout<<"The size of the ORIGINAL file is: "<<size<<" bytes"<<endl;
    cout<<"The size of the COMPRESSED file will be: "<<total_bits/8<<" bytes"<<endl;
    cout<<"Compressed file's size will be [%"<<100*((float)total_bits/8/size)<<"] of the original file"<<endl;
    if(total_bits/8>size){
        cout<<endl<<"COMPRESSED FILE'S SIZE WILL BE HIGHER THAN THE ORIGINAL"<<endl<<endl;
    }
    cout<<"If you wish to abort this process write 0 and press enter"<<endl
        <<"If you want to continue write any other number and press enter"<<endl;
    int check;
    //cin>>check;
    check = 1;
    if(!check){
        cout<<endl<<"Process has been aborted"<<endl;
        fclose(compressed_fp);
        fclose(original_fp);
        remove(&scompressed[0]);
        return;
    }

    //------------writes sixth----------------
    fread(&x,1,1,original_fp);
    for(long int i=0;i<bits;){
        str_pointer=&str_arr[x][0];
        while(*str_pointer){
            if(current_bit_count==8){
                fwrite(&current_byte,1,1,compressed_fp);
                current_bit_count=0;
            }
            switch(*str_pointer){
                case '1':i++;current_byte<<=1;current_byte|=1;current_bit_count++;break;
                case '0':i++;current_byte<<=1;current_bit_count++;break;
                default:cout<<"An error has occurred"<<endl<<"Process has been aborted";
                fclose(compressed_fp);
                fclose(original_fp);
                remove(&scompressed[0]);
                return;
            }
            str_pointer++;
        }
        fread(&x,1,1,original_fp);
    }
    
    if(current_bit_count==8){
        fwrite(&current_byte,1,1,compressed_fp);
    }
    else{
        current_byte<<=8-current_bit_count;
        fwrite(&current_byte,1,1,compressed_fp);
    }
    
    fclose(compressed_fp);
    fclose(original_fp);

    cout<<"Compression is complete"<<endl;   
}

void write_from_uChar(unsigned char uChar,unsigned char *current_byte,int current_bit_count,FILE *fp_write){
    (*current_byte)<<=8-current_bit_count;
    (*current_byte)|=(uChar>>current_bit_count);
    fwrite(current_byte,1,1,fp_write);
    *current_byte=uChar;   
}



void huffdecom(string compressedfile, string decomFolderPath){
    int letter_count,password_length;
    register FILE *fp_compressed,*fp_new;
    char newfile[260]="New-";
    char newfilePath[360];
    char* file_name_str = new char[compressedfile.length() + 1];
    strcpy(file_name_str, compressedfile.c_str());

    


    size_t lastSlash = compressedfile.find_last_of('/');
    //string commonPath = compressedfile.substr(0, lastSlash+1);
    //cout<<commonPath<<endl;
    string filename = compressedfile.substr(lastSlash+1);
    
    size_t lastDot = filename.find_last_of('.');
    filename = filename.substr(0, lastDot);
    //cout<<filename<<endl;
    decomFolderPath = decomFolderPath + "/" + filename;
    strcpy(newfile+4,filename.c_str());
    strcpy(newfilePath, decomFolderPath.c_str());
    
    //cout<<newfilePath<<endl;

    // ifstream compressedFile(compressedfile, ios::binary);
    // if (!compressedFile) {
    //     cerr << "Error: Unable to open the compressed file." << endl;
    //     return;
    // }
    // cout<<"Successfully open"<<endl;



    // ofstream newFile(decomFolderPath, ios::binary);
    // if (!newFile) {
    //     cerr << "Error: Unable to create the new file." << endl;
    //     return;
    // }
    // cout<<"Successfully created file"<<endl;
    // return;


    

    fp_compressed=fopen(file_name_str,"rb");
    
    if (!fp_compressed) {
        printf("%s does not exist\n", file_name_str);
        return;
    }

    if(!fp_compressed){
        cout<<compressedfile<<" does not exist"<<endl;
        return;
    }



    //---------reads .first-----------
    long int size=0;
    {
        long int multiplier=1;
        unsigned char temp;
        for(int i=0;i<8;i++){
            fread(&temp,1,1,fp_compressed);
            size+=temp*multiplier;
            multiplier*=256;
        }
    }
        // Size was written to the compressed file from least significiant byte 
        // to the most significiant byte to make sure system's endianness
        // does not affect the process and that is why we are processing size information like this
    //-------------------------------
    


    //str_without_compress(file_name_str);
    //strcpy(newfile+4,filename.c_str());
    //---------reads .second-----------
    fread(&letter_count,1,1,fp_compressed);
    if(letter_count==0)letter_count=256;
    //-------------------------------


    
    //------------reads .third and .fourth--------------------
    fread(&password_length,1,1,fp_compressed);
    password_length = 0;
    if(password_length){
        char real_password[password_length+1],password_input[257];
        fread(real_password,1,password_length,fp_compressed);
        real_password[password_length]=0;
        cout<<"Enter password:";
        cin>>password_input;
        char *rp=real_password,*pi=password_input;
        for(;*rp&&*pi;rp++,pi++){
            if(*rp!=*pi){
                cout<<"Wrong password"<<endl;
                fclose(fp_compressed);
                return;
            }
        }
        if(*rp!=*pi){
            cout<<"Wrong password"<<endl;
            fclose(fp_compressed);
            return;
        }
        cout<<"Correct Password"<<endl;
    }
        // this code block reads and checks the password
    //----------------------------------------------------

    

    //----------------reads .fifth----------------------
        // and stores transformation info into translation tree for later use
    unsigned char current_byte=0,current_character;
    int current_bit_count=0,len;
    translation *root=(translation*)malloc(sizeof(translation));

    for(int i=0;i<letter_count;i++){
        current_character=process_8_bits_NUMBER(&current_byte,current_bit_count,fp_compressed);
        len=process_8_bits_NUMBER(&current_byte,current_bit_count,fp_compressed);
        if(len==0)len=256;
        process_n_bits_TO_STRING(&current_byte,len,&current_bit_count,fp_compressed,root,current_character);
    }
    //--------------------------------------------------


    
    //----------------reads .sixth----------------------
        // Translates .sixth from info that is stored in translation tree
        // than writes it to new file

    cout<<"Writing file path"<<newfilePath<<endl;
    
    fp_new=fopen(newfilePath,"wb");
    
    translation *node;

    

    for(long int i=0;i<size;i++){
        node=root;
        while(node->zero||node->one){
            if(current_bit_count==0){
                fread(&current_byte,1,1,fp_compressed);
                current_bit_count=8;
            }
            if(current_byte&check){
                node=node->one;
            }
            else{
                node=node->zero;
            }
            current_byte<<=1;           
            current_bit_count--;
        }
        fwrite(&(node->character),1,1,fp_new);
    }
    //--------------------------------------------------

    burn_tree(root);
    cout<<"Decompression is complete"<<endl;
}

void burn_tree(translation *node){          // this function is used for deallocating translation tree
    if(node->zero)burn_tree(node->zero);
    if(node->one)burn_tree(node->one);
    free(node);
}

// process_n_bits_TO_STRING function reads n successive bits from the compressed file
// and stores it in a leaf of the translation tree
// after creating that leaf and sometimes nodes that are binding that leaf to the tree
void process_n_bits_TO_STRING(unsigned char *current_byte,int n,int *current_bit_count,FILE *fp_read,translation *node,unsigned char uChar){
    for(int i=0;i<n;i++){
        if(*current_bit_count==0){
            fread(current_byte,1,1,fp_read);
            *current_bit_count=8;
        }

        switch((*current_byte)&check){
            case 0:
            if(!(node->zero))node->zero=(translation*)malloc(sizeof(translation));
            node=node->zero;
            break;
            case 128:
            if(!(node->one))node->one=(translation*)malloc(sizeof(translation));
            node=node->one;
            break;
        }
        (*current_byte)<<=1;
        (*current_bit_count)--;
    }
    node->character=uChar;
}

// process_8_bits_NUMBER reads 8 successive bits from compressed file
// and returns it in unsigned char form
unsigned char process_8_bits_NUMBER(unsigned char *current_byte,int current_bit_count,FILE *fp_read){
    unsigned char val,temp_byte;
    fread(&temp_byte,1,1,fp_read);
    val=(*current_byte)|(temp_byte>>current_bit_count);
    *current_byte=temp_byte<<8-current_bit_count;
    return val;
}

void str_without_compress(char *p){
    char s[12]="desserpmoc.";
    char *p1=p,*p2=s;
    if(strlen(p)<12)return;
    for(;*p1;p1++);
    p1--;
    for(;*p2;p1--,p2++){
        if(*p1!=*p2){
            return;
        }
    }
    p1++;
    *p1=0;
}




int find_match(HashTable *hash, const char *buff, int &winstart, int &current, const int size)
{
    int max = 0;
    int maxloc = 0;
    int run;
    if (current == 0)
    {
        hash->AddNode(buff, current);
        (current)++;

        return -1;
    }
    else
    {
        Llist *search = hash->SearchTable(buff, current);
        while (search->pos != -1)
        {
            int k = 2;
            run = search->pos;
            while (buff[current + k] == buff[search->pos + k] && current + k < size)
            {
                k++;
                if (k > max)
                {
                    max = k;
                    maxloc = run;
                    if (max == 35)
                    {
                        break;
                    }
                }
            }

            if (max == 35)
            {
                break;
            }

            search = search->next;
        }

        if (max > 3)
        {
            for (int i = 0; i < max; i++)
            {
                hash->AddNode(buff, current);
                (current)++;
            }

            if (current - winstart > WINDOW_SIZE)
            {
                while (winstart != current - WINDOW_SIZE)
                {
                    hash->RemoveNode(buff, winstart);
                    (winstart)++;
                }
            }
            return current - max - maxloc;
        }
        else
        {
            hash->AddNode(buff, current);
            (current)++;
            if (current - winstart > WINDOW_SIZE)
            {
                while (winstart != current - WINDOW_SIZE)
                {
                    hash->RemoveNode(buff, winstart);
                    (winstart)++;
                }
            }
            return -1;
        }
    }
}

void lz77Compress( const string &fn, const string &outputfolder)
{
    int size;
        unique_ptr<char> data;
        ifstream fin(fn, ios::in | ios::binary | ios::ate);
        if (fin.is_open())
        {
            size = fin.tellg();
            data = unique_ptr<char>(new char[size]);
            fin.seekg(0, ios::beg);
            fin.read(data.get(), size);
            fin.close();
        }
        else
        {
            cout<<"Can't copen file: "<<fn<<endl;
        }

    const char *memblock = data.get();


    size_t lastSlash = fn.find_last_of('/');
    string name = fn.substr(lastSlash+1);


    const string filename = outputfolder + "/" + name + ".lzss";
    int winstart = 0;
    int current = 0;
    int temp;
    int check;
    BitBuffer bits(filename);
    HashTable hash;
    int i = 0;

    while (i < size)
    {
        temp = current;
        check = find_match(&hash, memblock, winstart, current, size);

        if (check >= 0)
        {
            bits.addOne();
            bits.addBytes(check, current - temp - 4);
            i += current - temp;
        }
        else
        {
            bits.addZero();
            bits.addByte(static_cast<unsigned char>(memblock[current - 1]));
            i++;
        }
    }


    //int csize = bits.getCompressedSize();
    //cout<<"Size: "<<size<<"  Csize: "<<csize<< "    Ratio:"<<(double)csize/size<<endl;
}


void lz77Decompress( const std::string &filename, const std::string &outputfolder)
{
    int size;
        unique_ptr<char> data;
        ifstream fin(filename, ios::in | ios::binary | ios::ate);
        if (fin.is_open())
        {
            size = fin.tellg();
            data = unique_ptr<char>(new char[size]);
            fin.seekg(0, ios::beg);
            fin.read(data.get(), size);
            fin.close();
        }
        else
        {
            cout<<"Can't copen file: "<<filename<<endl;
        }
    
    const char *memblock = data.get();



    char buff[DECOMP_BUFF_SIZE];
    int len = 0;
    int beg = 0;
    int end = 0;
    char cache[WINDOW_SIZE];
    int cacheend = 0;

    // Open output file
    size_t lastSlash = filename.find_last_of('/');
    //string common = filename.substr(0, lastSlash+1);
    string name = filename.substr(lastSlash+1);

    size_t lastDot = name.find_last_of('.');
    
    const std::string decomp_file = outputfolder + "/New-lz-" + name.substr(0, lastDot);
    std::ofstream fout(decomp_file, std::ios::out | std::ios::binary);

    for (int i = 0; i < size; i++)
    {
        char temp;
        temp = memblock[i];

        for (int j = 7; j >= 0; j--)
        {
            if (temp & 1)
            {
                buff[end + j] = '1';
            }
            else
            {
                buff[end + j] = '0';
            }

            temp >>= 1;
        }

        end += 8;
        if (end == DECOMP_BUFF_SIZE)
        {
            end = 0;
        }

        len += 8;
        if (len > DECOMP_BUFF_SIZE)
        {
            throw std::runtime_error("ERROR: Length overwrote data");
        }

        if (len + 8 >= DECOMP_BUFF_SIZE || i == size - 1)
        {
            while (len > 16)
            {
                if (buff[beg] == '0' && len >= 9)
                {
                    beg++;
                    if (beg == DECOMP_BUFF_SIZE)
                    {
                        beg = 0;
                    }

                    char byte = 0;
                    for (int j = 0; j < 8; j++)
                    {
                        if (buff[beg] == '0')
                        {
                            byte = byte | 0;
                        }
                        else
                        {
                            byte = byte | 1;
                        }

                        if (j < 7)
                        {
                            byte <<= 1;
                        }

                        beg++;

                        if (beg == DECOMP_BUFF_SIZE)
                        {
                            beg = 0;
                        }
                    }

                    if (fout.is_open())
                    {
                        fout.write(&byte, 1);
                        cache[cacheend] = byte;
                        cacheend++;
                        if (cacheend == WINDOW_SIZE)
                        {
                            cacheend = 0;
                        }
                    }

                    len -= 9;
                }
                else if (buff[beg] == '1' && len >= 18)
                {
                    beg++;
                    if (beg == DECOMP_BUFF_SIZE)
                    {
                        beg = 0;
                    }

                    int location = 0;
                    for (int j = 0; j < 12; j++)
                    {
                        if (buff[beg] == '0')
                        {
                            location = location | 0;
                        }
                        else
                        {
                            location = location | 1;
                        }

                        if (j < 11)
                        {
                            location <<= 1;
                        }

                        beg++;
                        if (beg == DECOMP_BUFF_SIZE)
                        {
                            beg = 0;
                        }
                    }

                    int length = 0;
                    for (int j = 0; j < 5; j++)
                    {
                        if (buff[beg] == '0')
                        {
                            length = length | 0;
                        }
                        else
                        {
                            length = length | 1;
                        }

                        if (j < 4)
                        {
                            length <<= 1;
                        }

                        beg++;

                        if (beg == DECOMP_BUFF_SIZE)
                        {
                            beg = 0;
                        }
                    }

                    length += 4;
                    len -= 18;
                    char byte = 0;

                    for (int j = 0; j < length; j++)
                    {
                        int tem = cacheend - location;
                        if (tem < 0)
                        {
                            tem += WINDOW_SIZE;
                        }

                        byte = cache[tem];

                        if (fout.is_open())
                        {
                            fout.write(&byte, 1);
                            cache[cacheend] = byte;
                            cacheend++;
                            if (cacheend == WINDOW_SIZE)
                            {
                                cacheend = 0;
                            }
                        }
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }
}
