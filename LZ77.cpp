#include <bits/stdc++.h>
using namespace std;

const int WINDOW_SIZE = 4096;

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
void Compress(const char *buff, const int size, const string &fn);
int find_match(HashTable *hash, const char *buff, int &winstart, int &current, const int size);

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

int main()
{
    string filename;
    string mode;

    mode = "compress";
    filename = "testfile.txt";

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
        throw runtime_error("Could not open input file '" + filename + "'");
    }

    if (mode.compare("compress") == 0)
    {
        // Send message to user of the mode we chose
        cout << "Compressing '" + filename + "'" << endl;
        Compress(data.get(), size, filename);
    }

    return 0;
}

void Compress(const char *memblock, const int size, const string &fn)
{
    const string filename = fn + ".comp";
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
            bits.addByte(memblock[current - 1]);
            i++;
        }
    }
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
