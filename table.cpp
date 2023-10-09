#include "table.h"
#include "block.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

Table::Table(string diskname,int numberofblocks, int blocksize, string flatfile, string indexfile):Filesys(diskname, numberofblocks, blocksize)
{
    this->flatfile = flatfile.substr(0,5);
    this->indexfile = indexfile.substr(0,5);
}

int Table::Build_Table(string input_file)
{
    ifstream infile;
    infile.open(input_file.c_str());
    int error = newfile(flatfile);
    error = newfile(indexfile);

    ostringstream ostream;

    string record, key;
    getline(infile, record);
    while (infile.good())
    {
        key = record.substr(0,5);
        vector<string>blocks = block(record, getblocksize());
        int b = addblock(flatfile, blocks[0]);

        ostream << key << " " << b << " ";
        getline(infile, record);
    }
    ostream << "kangaroo" << " " << 0 << " ";

    string buffer = ostream.str();

    vector<string>blocks2 = block(buffer, getblocksize());

    for(int i = 0; i < blocks2.size(); i++)
    {
        addblock(indexfile, blocks2[i]);
    }

    return 1; 
}

void Table::Search(string key)
{
    int b = IndexSearch(key);

    if (b == -1)
    {
        cout << "record not found" << endl;
        return;
    }
    else
    {
        string buffer;
        readblock(flatfile, b, buffer);
        cout << buffer << endl;
    }

    return;
}

int Table::IndexSearch(string key)
{
    string buffer, buffer2;
    int b = getfirstblock(indexfile);
    
    if (b == 0 || b == -1)
    {
        return -1;
    }

    while (b != 0)
    {
        readblock(indexfile, b, buffer);
        buffer2 += buffer;
        b = nextblock(indexfile, b);
    }

    istringstream istream;
    istream.str(buffer2);
    string ikey;
    int iblock;
    istream >> ikey >> iblock;
    
    while(ikey != "kangaroo")
    {
        if (ikey == key)
        {
            return iblock;
        }

        istream >> ikey >> iblock;
    }
    return -1;
}