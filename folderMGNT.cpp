#include<bits/stdc++.h>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;
void compressedAllFiles( vector<string> filePaths);


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


void compressedAllFiles( vector<string> filePaths)
{
    size_t commonDirPos = filePaths[0].find_last_of('/');
    string commonDirectory = filePaths[0].substr(0, commonDirPos);
    string decompressFolderPath = commonDirectory +  "/decompressFolder";


    // Read the files using the file paths
    for (const string& filePath : filePaths)
    {
        ifstream file(filePath);
        if (file.is_open())
        {
            cout << "\nContents of " << filePath.substr(commonDirPos+1) << ":" << endl;
            string line;
            while (getline(file, line))
            {
                cout << line << endl;
            }
            file.close();
        }
        else
        {
            cout << "Unable to open file: " << filePath << endl;
        }
    }


    if (mkdir(decompressFolderPath.c_str(), 0777) == 0)
    {
        cout << "\nCreated folder: " << decompressFolderPath << endl;

        // Create a new file named "newFile.txt" within the "decompressFolder" directory
        string newFilePath = decompressFolderPath + "/newFile.txt";

        string newFileContent = "This is the content of the new file.";
        ofstream newFile(newFilePath);
        if (newFile.is_open())
        {
            newFile << newFileContent;
            newFile.close();
            cout << "Created file: " << newFilePath << endl;
        }
        else
        {
            cout << "Failed to create file: " << newFilePath << endl;
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
             << "4 - Compressed all the files" << endl
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
