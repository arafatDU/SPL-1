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





class RLE {
public:
	static void encode(const std::shared_ptr<std::vector<char>>& inpVect, const std::string& path);

	static std::shared_ptr<std::vector<char>> decode(const std::string& path);
};



void RLE::encode(const std::shared_ptr<std::vector<char> >& inpVect, const std::string& path)
{
	std::ofstream file;
	file.open(path, std::ios::binary);
	if (file.is_open() && !file.fail())
	{
		if (inpVect)
		{
			std::shared_ptr<std::vector<char>> outVect = std::shared_ptr<std::vector<char>>(new std::vector<char>);
			unsigned int currind = 0;
			char currchar;
			char times = 0;
			while (currind < inpVect->size()) {
				currchar = inpVect->at(currind);
				currind++;
				if (currind == inpVect->size())
				{
					outVect->push_back(times);
					outVect->push_back(currchar);
					times = 0;
				}
				else if (currchar == inpVect->at(currind) && times < SCHAR_MAX - 1) {
					times++;
				}
				else {
					outVect->push_back(times);
					outVect->push_back(currchar);
					times = 0;
				}
			}

			char serLen = 0;
			std::deque<char> buff;
			unsigned int ind = 0;
			while (ind < outVect->size()) {
				if (serLen == SCHAR_MIN + 1)
				{
					file.write((char*)&serLen, sizeof(char));
					while (buff.size() > 0) {
						file.write((char*)&(buff.front()), sizeof(char));
						buff.pop_front();
					}
					serLen = 0;
				}

				if (outVect->at(ind) == 0) {
					serLen--;
					buff.push_back(outVect->at(ind + 1));
				}
				else {
					if (serLen != 0) {
						file.write((char*)&serLen, sizeof(char));
						while (buff.size() > 0) {
							file.write((char*)&(buff.front()), sizeof(char));
							buff.pop_front();
						}
						serLen = 0;
					}
					file.write((char*)&(outVect->at(ind)), sizeof(char));
					file.write((char*)&(outVect->at(ind + 1)), sizeof(char));
				}
				ind += 2;
			}
			if (serLen != 0) {
				file.write((char*)&serLen, sizeof(char));
				while (buff.size() > 0) {
					file.write((char*)&(buff.front()), sizeof(char));
					buff.pop_front();
				}
			}

		}
		else {
			//error, file not opened
		}
	}
}

std::shared_ptr<std::vector<char> > RLE::decode(const std::string& path)
{
	std::shared_ptr<std::vector<char>> inpVect = FileAccessor::GetSymbVectPtr(path);
	std::shared_ptr<std::vector<char>> outVect = std::shared_ptr<std::vector<char>>(new std::vector<char>);
	if (inpVect)
	{
		char symb;
		char count;
		unsigned int currind = 0;
		while (currind < inpVect->size() - 1)
		{
			count = inpVect->at(currind);
			currind++;
			if (count >= 0) {
				symb = inpVect->at(currind);
				currind++;
				for (char i = 0; i <= count; ++i) {
					outVect->push_back(symb);
				}
			}
			else
			{
				for (char i = 0; i > count; --i)
				{
					symb = inpVect->at(currind);
					currind++;
					outVect->push_back(symb);
				}
			}
		}
	}
	else
	{
		//error, failed to open
		return nullptr;
	}
	return outVect;
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


void rleCompress(string inpPath, string folderPath)
{
    std::shared_ptr<std::vector<char>> inpData = FileAccessor::GetSymbVectPtr(inpPath);
	if (!inpData) return;

    size_t lastSlash = inpPath.find_last_of('/');
    string rawName = inpPath.substr(lastSlash+1);
    string outPath = folderPath + "/" + rawName + ".rle";

    RLE::encode(inpData, outPath);

}



void rleDecompress(string inpPath, string folderPath)
{
    std::shared_ptr<std::vector<char>> outData = RLE::decode(inpPath);

    size_t lastSlash = inpPath.find_last_of('/');
    string rawName = inpPath.substr(lastSlash+1);
    size_t lastDot = rawName.find_last_of('.');
    string name = rawName.substr(0, lastDot);
    string outPath = folderPath + "/New-r-" + name;

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

    RLE::encode(BWTData, outPath);

}
void bwtDecompress(string inpPath, string folderPath)
{
	cout<<"Huffman encode continue..."<<endl;
	std::shared_ptr<std::vector<char>> Data = RLE::decode(inpPath);

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






	//cout<< "CR: " << FileAccessor::GetFileSize(inpPath) <<endl;
    //rleCompress(inpPath, outFolder);
    rleDecompress(inpPath, outFolder);



    return 0;
}