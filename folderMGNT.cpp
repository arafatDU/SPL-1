#include<bits/stdc++.h>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

void compressedAllFiles( vector<string> filePaths);
void write_from_uChar(unsigned char,unsigned char*,int,FILE*);
void huffcom(string original_file);



struct ersel{
    ersel *left,*right;
    long int number;
    unsigned char character;
    string bit;
};

bool erselcompare0(ersel a,ersel b){
    return a.number<b.number;
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


void compressedAllFiles( vector<string> filePaths)
{
    size_t commonDirPos = filePaths[0].find_last_of('/');
    string commonDirectory = filePaths[0].substr(0, commonDirPos);
    string decompressFolderPath = commonDirectory +  "/compressFolder";


    // Read the files using the file paths
    for (const string& filePath : filePaths)
    {
        huffcom(filePath);
    }


    // if (mkdir(decompressFolderPath.c_str(), 0777) == 0)
    // {
    //     cout << "\nCreated folder: " << decompressFolderPath << endl;

    //     // Create a new file named "newFile.txt" within the "decompressFolder" directory
    //     string newFilePath = decompressFolderPath + "/newFile.txt";

    //     string newFileContent = "This is the content of the new file.";
    //     ofstream newFile(newFilePath);
    //     if (newFile.is_open())
    //     {
    //         newFile << newFileContent;
    //         newFile.close();
    //         cout << "Created file: " << newFilePath << endl;
    //     }
    //     else
    //     {
    //         cout << "Failed to create file: " << newFilePath << endl;
    //     }
    // }
    // else
    // {
    //     cout << "Failed to create folder: " << decompressFolderPath << endl;
    // }
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


void huffcom(string original_file){
    long int number[256];
    long int bits=0,total_bits=0;
    int letter_count=0;
    const char* file_name_cstr = original_file.c_str();
    
    for(long int *i=number;i<number+256;i++){                       
        *i=0;
    }
    
    string scompressed;
    register FILE *original_fp=fopen(file_name_cstr,"rb"),*compressed_fp;
    if(NULL==original_fp){
        cout<<original_file<<" file does not exist"<<endl;
        return;
    }
    scompressed=original_file;
    scompressed+=".compressed";



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


