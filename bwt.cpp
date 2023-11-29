#include <vector>
#include <algorithm>
#include <cstring>
#include <memory>
#include <string>
#include <climits>

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