#include "filesys.h"

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

Filesys::Filesys(string diskname, int numberofblocks, int blocksize):Sdisk(diskname, numberofblocks, blocksize)
{
    blocksize = getblocksize();
    numberofblocks = getnumberofblocks();

    this->rootsize = blocksize / 12;
    this->fatsize = ((numberofblocks * 6) / blocksize) + 1;

    string buffer;
    int error = getblock(1, buffer);

    if (error = 0)
    {
        cout << "Corrupted Sdisk" << endl;
        return;
    }

    if (buffer[0] == '#')
    {
        buildfs();
        fssynch();
    }
    else
    {
        readfs();
    }
}

int Filesys::fsclose()
{
    return 1;
}

int Filesys::newfile(string file)
{
    for (int i = 0; i < filename.size(); i++)
    {
        if (filename[i] == file)
        {
            cout << "file exist" << endl;
            return 0;
        }
    }
    
    for (int i = 0; i < filename.size(); i++)
    {
        if (filename[i] == "$$$$$")
        {
            filename[i] = file;
            firstblock[i] = 0;
            fssynch();
            return 1;
        }
    }

    return 0;
}

int Filesys::rmfile(string file)
{
    int b = getfirstblock(file);

    if (b != 0)
    {
        return 0;
    }

    for (int i = 0; i < filename.size(); i++)
    {
        if (filename[i] == file)
        {
            filename[i] = "$$$$$";
        }
    }
    fssynch();

    return 1;
}

int Filesys::getfirstblock(string file)
{
    for (int i = 0; i < filename.size(); i++)
    {
        if (filename[i] == file)
        return firstblock[i];
    }

    return -1;
}

int Filesys::addblock(string file, string block)
{
    int allocate = fat[0];

    if (allocate == 0)
    {
        cout << "no space in root" << endl;
        return 0;
    }

    int b = getfirstblock(file);

    if (b == -1)
    {
        cout << "file doesn't exist" << endl;
        return 0;
    }
    
    fat[0] = fat[allocate];
    fat[allocate] = 0;

    if (b == 0)
    {
        for (int i = 0; i < filename.size(); i++)
        {
            if (filename[i] == file)
            {
                firstblock[i] = allocate;
            }
        }
    }
    else
    {
        while (fat[b] != 0)
        {
            b = fat[b];
        }

        fat[b] = allocate;
    }
    putblock(allocate, block);
    fssynch();
    return allocate;
}

int Filesys::delblock(string file, int blocknumber)
{
    int deallocate = blocknumber;
    if (!checkfileblock(file, blocknumber))
    {
        return 0;
    }

    int b = getfirstblock(file);

    if (b == blocknumber)
    {
        for (int i = 0; i < filename.size(); i++)
        {
            if (filename[i] == file)
            {
                firstblock[i] = fat[blocknumber];
                break;
            }
        }
    }
    else
    {
        while (fat[b] != deallocate)
        {
            b = fat[b];
        }

        fat[b] = fat[deallocate];
    }

    fat[blocknumber] = fat[0];
    fat[0] = blocknumber;
    fssynch();
    return 1;
}

bool Filesys::checkfileblock(string file, int block)
{
    int blockid = getfirstblock(file);
    if (blockid <= 0)
    {
        return false;
    }
    
    while (block != 0)
    {
        if (blockid == block)
        {
            return true;
        }
        else
        {
            blockid = fat[blockid];
        }
    }
    return false;
}

int Filesys::readblock(string file, int blocknumber, string& buffer)
{
    if (checkfileblock(file, blocknumber))
    {
        getblock(blocknumber, buffer);
        return 1;
    }
    else
    {
        return 0;
    }
}

int Filesys::writeblock(string file, int blocknumber, string buffer)
{
    if (checkfileblock(file, blocknumber))
    {
        putblock(blocknumber, buffer);
        return 1;
    }
    else
    {
        return 0;
    }
}

int Filesys::nextblock(string file, int blocknumber)
{
    if (checkfileblock(file, blocknumber))
    {
        return fat[blocknumber];
    }
    else
    {
        return -1;
    }
}

int Filesys::buildfs() // builds the file system
{
    for (int i = 0; i < rootsize; i++)
    {
        filename.push_back("$$$$$");
        firstblock.push_back(0);
    }

    fat.push_back(2 + fatsize);
    fat.push_back(-1);

    for (int i = 0; i < fatsize; i++)
    {
        fat.push_back(-1);
    }

    for (int i = fatsize + 2; i < getnumberofblocks(); i++)
    {
        fat.push_back(i + 1);
    }

    fat[fat.size() - 1] = 0;
    
    return 1; 
}

int Filesys::readfs() // reads the file system
{
    string buffer;
    getblock(1, buffer);
    istringstream instream1;
    instream1.str(buffer);

    for (int i = 0; i < rootsize; i++)
    {
        string s;
        int b;

        instream1 >> s >> b;
        filename.push_back(s);
        firstblock.push_back(b);
    }

    string buffer2;
    string b;

    for (int i = 0; i < fatsize; i++)
    {
        getblock(2+i, b);
        buffer2 += b;
    }

    istringstream instream2;
    instream2.str(buffer2);

    for (int i = 0; i < getnumberofblocks(); i++)
    {
        int n;
        instream2 >> n;
        fat.push_back(n);
    }
    return 1;
}

int Filesys::fssynch() // writes the FAT and ROOT to the sdisk
{
    ostringstream ostream1;

    for (int i = 0; i < filename.size(); i++)
    {
        ostream1 << filename[i] << " " << firstblock[i] << " ";
    }
    string buffer1 = ostream1.str();
    vector<string> blocks1 = block(buffer1, getblocksize());
    int error = putblock(1, blocks1[0]);

    ostringstream ostream2;

    for (int i = 0; i < fat.size(); i++)
    {
        ostream2 << fat[i] << " ";
    }
    string buffer2 = ostream2.str();
    vector<string> blocks2 = block(buffer2, getblocksize());

    for (int i = 0; i < blocks2.size(); i++)
    {
        putblock(2+i, blocks2[i]);
    }
    
    return 1;
}

vector<string> Filesys::ls()
{ 
    vector<string> flist;
    for (int i = 0; i < filename.size(); i++)
        {
        if (filename[i] != "XXXXX")
            {
                flist.push_back(filename[i]);
            }
        }
    return flist;
}