#include<bits/stdc++.h>


using namespace std;

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


void huffcom(string inpPath, string folderPath)
{
    std::shared_ptr<std::vector<char>> inpData = FileAccessor::GetSymbVectPtr(inpPath);
	if (!inpData) return;

    size_t lastSlash = inpPath.find_last_of('/');
    string rawName = inpPath.substr(lastSlash+1);
    string outPath = folderPath + "/" + rawName + ".huff";

    Huffman::encode(inpData, outPath);

}

// int  getHuffComR(string inpPath, string folderPath)
// {
//     std::shared_ptr<std::vector<char>> inpData = FileAccessor::GetSymbVectPtr(inpPath);
// 	if (!inpData) return -1;

// 	std::size_t fileSize = inpData->size();


// 	int fileSizeInt = static_cast<int>(fileSize);

//     return fileSizeInt;

// }


void huffdecom(string inpPath, string folderPath)
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




int main()
{
    string inpPath, outFolder;
    cout<<"Enter filepath: ";
    cin>>inpPath;
    cout<<"Enter output folderpath: ";
    cin>>outFolder;

    // std::shared_ptr<std::vector<char>> inpData = FileAccessor::GetSymbVectPtr(inpPath);
	// if (!inpData) return -1;
	// std::shared_ptr<std::vector<char>> outData;
	// std::shared_ptr<std::vector<char>> BWTData;


    //BWTData = BWT::encode(inpData);

    // cout << "BWT Data: ";
    // for (char c : *BWTData) {
    //     cout << c;
    // }
    // cout << endl <<endl<<endl;



    // outData = BWT::decode(BWTData);
    // cout << "Original Data: ";
    // for (char c : *outData) {
    //     cout << c;
    // }
    // cout << endl;


    //Huffman::encode(inpData, outPath);

    // outData = Huffman::decode(inpPath);
    
    // cout << "Original Data: ";
    // for (char c : *outData) {
    //     cout << c;
    // }
    // cout << endl;

    // FileAccessor::WriteSymbVectToFile(outData, outPath);

    //huffcom(inpPath, outFolder);
    //huffdecom(inpPath, outFolder);

	//bwtCompress(inpPath, outFolder);
	//bwtDecompress(inpPath, outFolder);


	cout<< "CR: " << FileAccessor::GetFileSize(inpPath) <<endl;



    return 0;
}