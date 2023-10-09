#include "sdisk.h"

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

Sdisk::Sdisk(string diskname, int numberofblocks, int blocksize)
{
    this->diskname = diskname;
    this->numberofblocks = numberofblocks;
    this-> blocksize = blocksize;

    ifstream infile;
    infile.open(diskname.c_str());

    if(infile.good())
    {
        cout << "checking disk" << endl;

        int count = 0;
        char c;

        infile.get(c);
        while(infile.good())
        {
            count++;
            infile.get(c);
        }

        if(count == numberofblocks * blocksize)
        {
            cout << "disk size is correct" << endl;
        }
        else
        {
            cout << "disk size is incorrect" << endl;
        }
        infile.close();
    }
    else
    {
        infile.close();
        ofstream ofile;
        ofile.open(diskname.c_str());
        cout << "creating sdisk" << endl;

        for (int i = 0; i < blocksize * numberofblocks; i++)
        {
            ofile.put('#');
        }
    }
    return;
}   

int Sdisk::getblocksize()
{
    return this->blocksize;
}

int Sdisk::getnumberofblocks()
{
    return this->numberofblocks;
}

int Sdisk::getblock(int blocknumber, string& buffer)
{
    if(blocknumber < 0 || blocknumber > numberofblocks)
    {
        return 0;
    }

    fstream iodisk;
    iodisk.open(diskname.c_str(), ios::in | ios::out);
    char c;
    buffer.clear();

    iodisk.seekg(blocknumber*blocksize);
    for(int i =0; i < blocksize; i++)
    {
        iodisk.get(c);
        buffer += c;
    }
    return 1;
}

int Sdisk::putblock(int blocknumber, string buffer)
{
    if(blocknumber < 0 || blocknumber > numberofblocks || buffer.length() != blocksize)
    {
        return 0;
    }

    fstream iofile;
    iofile.open(diskname.c_str(), ios::in | ios::out);
    
    iofile.seekp(blocknumber*blocksize);
    for(int i = 0; i < blocksize; i++)
    {
        iofile.put(buffer[i]);
    }
    return 1;
}