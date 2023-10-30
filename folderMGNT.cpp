#include<bits/stdc++.h>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

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