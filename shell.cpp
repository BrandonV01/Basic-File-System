#include "shell.h"
#include "block.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std;

Shell::Shell(string filename,  int numberofblocks, int blocksize):Filesys(filename, numberofblocks, blocksize)
{
    
}

int Shell::dir()
{
    vector<string> flist = ls();

    for (int i = 0; i < flist.size(); i++)
    {
        cout << flist[i] << endl;
    }
    return 1;
}

int Shell::add(string file,string buffer)
{
    int errorcode = newfile(file);

    if (errorcode == 0) 
    {
        /* file cannot be created*/ 
        return 0;
    }

    vector<string> blocks = block(buffer, getblocksize());

    for (int i = 0; i < blocks.size(); i++)
    {
        errorcode = addblock(file, blocks[i]);
    }
    return 1;
}

int Shell::del(string file)
{
    int b = getfirstblock(file);

    if (b == -1)
    {
        return 0;
    }

    if (b == 0)
    {
        return 1;
    }

    while (b != 0)
    {
        delblock(file,b);
        b = getfirstblock(file);
    }
    rmfile(file);
    
    return 1;
}

int Shell::type(string file)
{
    string buffer1, buffer2;
    
    int b = getfirstblock(file);

    if (b == -1)
    {
        /*no file*/
        return 0;
    }

    if (b == 0)
    {
        return 0;
    }

    while (b != 0)
    {
        readblock(file, b, buffer1);
        buffer2 += buffer1;
        b = nextblock(file, b);
    }

    cout << buffer2 << endl;
    return 1;
}

int Shell::copy(string file1, string file2)
{
    int b1 = getfirstblock(file1);
    if (b1 == -1)
    {
        return 0;
    }

    int b2 = getfirstblock(file2);
    if (b2 > -1)
    {
        return 0;
    }

    newfile(file2);

    string buffer;

    while (b1 != 0)
    {
        readblock(file1, b1, buffer);
        b1 = nextblock(file1, b1);
        addblock(file2, buffer);
    }

    return 1;
}