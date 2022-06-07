#include <bits/stdc++.h>
#include<string.h>
#include<iostream>
#include<stdio.h>
#include<fstream>
#include<bitset>
#include<vector>
#include<map>

#define bits32 unsigned int
#define bits64 unsigned long long
#define UBYTE char
#define LARGEBITSET std::bitset<5000000000>

using namespace std;

class Header{
    public:
        bits32 signature;
        bits32 version;
        bits32 seqCount;
        bits32 reserved;
};

class BitIndex{
    public:
        UBYTE nameSize;
        char *name;	
        bits32 offset;
        size_t sequenceSize;
};

class Data{
    public:
        bits32 size;
        bits32 nBlockCount;
        bits32 *nStarts;
        bits32 *nSizes;
        bits32 maskBlockCount;
        bits32 *maskStarts;
        bits32 *maskSizes;
        bits32 reserved;
};

class TwoBitReader{
    public:
        string str_filename;
        Header header;
        fstream f;
        std::map<string,BitIndex*> seqBitMap;
        std::map<string,std::map<int,int>> NMap;
        std::map<string,std::map<int,int>> maskMap;

        TwoBitReader(string str_filen){
            str_filename = str_filen;
            f.open(str_filename,ios::in|ios::binary);            
            readHeader(f,header);
            readIndex(f,header,seqBitMap);
            readNBlocks();
        }

        ~TwoBitReader(){};

        // below will be private functions
    inline void readHeader(fstream &f,Header &h);
    inline void readIndex(fstream &f,Header &header,std::map<string,BitIndex*> &seqBitMap);
    void readData(string &str_filename,BitIndex *bi,LARGEBITSET *bitArray,LARGEBITSET *bitNs,size_t &szt_seqSize);
    void readNBlocks();

        map<string,size_t>* showSequenceIds();
        
        // void bitToNuc(size_t &b1,size_t &b2);
        char bitToNucM(size_t &b1,size_t &b2);
        void tokenizer(string &line,string &str_seqID,string &str_start,string &str_end);

        char* extractSequence(string seqID);
        char* extractSequence(const string &str_sequenceName,const size_t &szt_start,const size_t &szt_end);
        
        //QUESTIONABLE FUNCTION ,OUTPUT ON CONSOLE
        void extractSequenceFromBedFile(const string &str_bedFileName,const size_t mode);

        size_t countNs();
        size_t countNs(string str_seqName);
        
        map<char,size_t>* bases(char *sequence);
};


map<char,size_t>* TwoBitReader::bases(char *sequence){
    char *ptr = sequence;
    map<char,size_t> *temp_map = new map<char,size_t>();
    temp_map->insert({'A',0});
    temp_map->insert({'C',0});
    temp_map->insert({'T',0});
    temp_map->insert({'G',0});
    temp_map->insert({'N',0});

    for(;*ptr != '\0';ptr++){
        switch(*ptr){
            case 'A': { 
                (*temp_map)['A'] += 1;
                break;
            }
            case 'a': { 
                (*temp_map)['A'] += 1;
                break;
            }
            case 'C': { 
                (*temp_map)['C'] += 1;
                break;
            }
            case 'c': { 
                (*temp_map)['C'] += 1;
                break;
            }case 'T': { 
                (*temp_map)['T'] += 1;
                break;
            }
            case 't': { 
                (*temp_map)['T'] += 1;
                break;
            }case 'G': { 
                (*temp_map)['G'] += 1;
                break;
            }
            case 'g': { 
                (*temp_map)['G'] += 1;
                break;
            }case 'N': { 
                (*temp_map)['N'] += 1;
                break;
            }
            case 'n': { 
                (*temp_map)['N'] += 1;
                break;
            } 
        }    
    }
    return temp_map;
}



// sections for utils functions
map<string,size_t>* TwoBitReader::showSequenceIds(){
    map<string,size_t> *temp_map = new map<string,size_t>();
    for (auto i : seqBitMap){   
        string str_seqName = i.first;
        BitIndex *bi = i.second;    
        temp_map->insert({str_seqName,bi->sequenceSize});
    }
    return temp_map;
}

void TwoBitReader::tokenizer(string &line,string &str_seqID,string &str_start,string &str_end){
    stringstream check1(line);
    string token;
    int h = 0;
    while(getline(check1, token,'\t'))
    {  
        switch(h){
            case 0: { str_seqID = token; break; }
            case 1: { str_start = token; break; }
            case 2: { str_end = token; break; }
            case 3: { return; }
        }
        h++;
    }
}


char TwoBitReader::bitToNucM(size_t &b1,size_t &b2){ //N table to tally and make the right nucleotide
    switch(b1+b2){
        case 0: { 
            return 'T'; break; 
        } 
        case 2: {
            return 'G'; break;
        }
        case 1: {
            switch(b1){
                case 0: {
                    return 'C'; break;
                }
                case 1: {
                    return 'A'; break; 
                }
            }
            break;
        }
    }
    return 'N';
}

size_t TwoBitReader::countNs(){
    size_t totalNs = 0;
    for(auto f : NMap){
        for(auto s : f.second){
            totalNs += s.second;
        }
    }
    return totalNs;
}

size_t TwoBitReader::countNs(string str_seqName){
    size_t totalNs = 0;
    for(auto f : NMap[str_seqName]){
        totalNs += f.second;
    }
    return totalNs;
}

// SEction for all the reading information
inline void TwoBitReader::readHeader(fstream &rf,Header &h){
    rf.read(reinterpret_cast<char *>(&h.signature),sizeof(h.signature));
    rf.read(reinterpret_cast<char *>(&h.version),sizeof(h.version));
    rf.read(reinterpret_cast<char *>(&h.seqCount),sizeof(h.seqCount));
    rf.read(reinterpret_cast<char *>(&h.reserved),sizeof(h.reserved));
}

void getSequenceLength(fstream &rf,std::map<string,BitIndex*> &seqBitMap){
    for (auto m : seqBitMap){
        BitIndex *bi = m.second;
        rf.seekg(bi->offset,ios::beg);
        bits32 sequenceSize = 0;
        rf.read(reinterpret_cast<char *>(&sequenceSize),sizeof(sequenceSize));  
        bi->sequenceSize = sequenceSize;
    }
}

inline void TwoBitReader::readIndex(fstream &rf,Header &header,std::map<string,BitIndex*> &seqBitMap){
    for(int i=0;i<header.seqCount;i++){
        
        BitIndex *bi = new BitIndex();
        
        rf.read(reinterpret_cast<char *>(&bi->nameSize),sizeof(bi->nameSize));
        char n[int(bi->nameSize)];
        bi->name = (char*)malloc(sizeof(char)*int(bi->nameSize));
        bi->name[int(bi->nameSize)+1] = '\0';
        rf.read(reinterpret_cast<char *>(&n),int(bi->nameSize));
        for(int i=0;i<bi->nameSize;i++){
            bi->name[i] = n[i];
        }
    
        rf.read(reinterpret_cast<char *>(&bi->offset),sizeof(bi->offset));
        seqBitMap.insert({bi->name,bi});
    }
    getSequenceLength(rf,seqBitMap);
}


void TwoBitReader::readNBlocks(){
    fstream rf(str_filename,ios::in|ios::binary);
    Data d;
        
    for (auto p : seqBitMap)
    {
        BitIndex *bi = p.second;
        rf.seekg(bi->offset,ios::beg);

        // NMap.insert({{bi->name,new map(pair<int,int>({0,0}))}});

        rf.read(reinterpret_cast<char *>(&d.size),sizeof(d.size));
        rf.read(reinterpret_cast<char *>(&d.nBlockCount),sizeof(d.nBlockCount));

        // N info
        d.nStarts = (bits32*)malloc(sizeof(bits32)*d.nBlockCount);
        map<int,int> n;

        for(int i=0;i<d.nBlockCount;i++){
            rf.read(reinterpret_cast<char *>(&d.nStarts[i]),sizeof(d.nStarts[i]));
        }
        d.nSizes = (bits32*)malloc(sizeof(bits32)*d.nBlockCount);
        for(int i=0;i<d.nBlockCount;i++){
            rf.read(reinterpret_cast<char *>(&d.nSizes[i]),sizeof(d.nSizes[i]));
            n.insert({d.nStarts[i],d.nSizes[i]});
        }   
        
        rf.read(reinterpret_cast<char *>(&d.maskBlockCount),sizeof(d.maskBlockCount));
        
        // mask info
        d.maskStarts = (bits32*)malloc(sizeof(bits32)*d.maskBlockCount);
        map<int,int> m;
        for(int i=0;i<d.maskBlockCount;i++){
            rf.read(reinterpret_cast<char *>(&d.maskStarts[i]),sizeof(d.maskStarts[i]));
        }

        d.maskSizes = (bits32*)malloc(sizeof(bits32)*d.maskBlockCount);
        for(int i=0;i<d.maskBlockCount;i++){
            rf.read(reinterpret_cast<char *>(&d.maskSizes[i]),sizeof(d.maskSizes[i]));
            m.insert({d.maskStarts[i],d.maskSizes[i]});
        }
        
        NMap.insert({{bi->name,std::move(n)}});
        maskMap.insert({bi->name,std::move(m)});
    }
}

void TwoBitReader::readData(string &str_filename,BitIndex *bi,LARGEBITSET *bitArray,LARGEBITSET *bitNs,size_t &szt_seqSize){
    fstream rf(str_filename,ios::in|ios::binary);
    Data d;

    rf.seekg(bi->offset,ios::beg);

    rf.read(reinterpret_cast<char *>(&d.size),sizeof(d.size));
    szt_seqSize = d.size;
    rf.read(reinterpret_cast<char *>(&d.nBlockCount),sizeof(d.nBlockCount));

    d.nStarts = (bits32*)malloc(sizeof(bits32)*d.nBlockCount);
    for(int i=0;i<d.nBlockCount;i++){
        rf.read(reinterpret_cast<char *>(&d.nStarts[i]),sizeof(d.nStarts[i]));
    }

    d.nSizes = (bits32*)malloc(sizeof(bits32)*d.nBlockCount);
    for(int i=0;i<d.nBlockCount;i++){
        rf.read(reinterpret_cast<char *>(&d.nSizes[i]),sizeof(d.nSizes[i]));
    }

    rf.read(reinterpret_cast<char *>(&d.maskBlockCount),sizeof(d.maskBlockCount));

    d.maskStarts = (bits32*)malloc(sizeof(bits32)*d.maskBlockCount);
    for(int i=0;i<d.maskBlockCount;i++){
        rf.read(reinterpret_cast<char *>(&d.maskStarts[i]),sizeof(d.maskStarts[i]));
    }

    d.maskSizes = (bits32*)malloc(sizeof(bits32)*d.maskBlockCount);
    for(int i=0;i<d.maskBlockCount;i++){
        rf.read(reinterpret_cast<char *>(&d.maskSizes[i]),sizeof(d.maskSizes[i]));
    }

    rf.read(reinterpret_cast<char *>(&d.reserved),sizeof(d.reserved));

    char c;
    size_t seqRun=0;
    unsigned long long int run = 0;
    int n=0;
    while(seqRun<d.size/4){
        rf.get(c);
        for (int i = 7; i >= 0; i--){
            bool b = ((c >> i) & 1);
            bitArray->set(run,b);
            run++;
        }
        seqRun++;
    }

    for(int i=0;i<d.nBlockCount;i++){
        unsigned long long int start = d.nStarts[i];
        unsigned long long int stop = start+d.nSizes[i];
        unsigned long long int cord=0;
        for (unsigned long long int j = start;j<=stop;j++){
            cord=j*2;
            bitNs[0].set(cord,1);
            bitNs[0].set(cord+1,1);
        }
    }
}


char* TwoBitReader::extractSequence(string seqID){
    BitIndex *bi = seqBitMap[seqID];

    LARGEBITSET *b_bitArray;
    LARGEBITSET *b_bitNs;
    
    b_bitArray = (LARGEBITSET *)malloc(sizeof(LARGEBITSET));
    b_bitNs = (LARGEBITSET *)malloc(sizeof(LARGEBITSET));
    
    readData(str_filename,bi,b_bitArray,b_bitNs,bi->sequenceSize);
    size_t seqSize = (bi->sequenceSize/2);

    char *sequence = (char*)malloc(seqSize*sizeof(char));
    sequence[seqSize+1] = '\0';

    for(int i=0,j=0;i<bi->sequenceSize;){
        size_t b1 = size_t(b_bitArray[0][i]);
        size_t b2 = size_t(b_bitArray[0][i+1]);
        sequence[j] = bitToNucM(b1,b2);
        i+=2;
        j++;
    }
    return sequence;
}

char* TwoBitReader::extractSequence(const string &str_sequenceName,const size_t &szt_start,const size_t &szt_end){
    BitIndex *bi = seqBitMap[str_sequenceName];

    LARGEBITSET *b_bitArray;
    LARGEBITSET *b_bitNs;
    
    b_bitArray = (LARGEBITSET *)malloc(sizeof(LARGEBITSET));
    b_bitNs = (LARGEBITSET *)malloc(sizeof(LARGEBITSET));
    
    readData(str_filename,bi,b_bitArray,b_bitNs,bi->sequenceSize);
    size_t seqSize = (bi->sequenceSize/2);

    size_t requiredSize = (szt_end-szt_start)+2;
    char *sequence = (char*)malloc(requiredSize*sizeof(char));
    sequence[requiredSize+1] = '\0';

    for(int i=szt_start*2,j=0;i<szt_end*2;){
        size_t b1 = size_t(b_bitArray[0][i]);
        size_t b2 = size_t(b_bitArray[0][i+1]);
        sequence[j] = bitToNucM(b1,b2);
        i+=2;
        j++;
    }

    return sequence;
}

void TwoBitReader::extractSequenceFromBedFile(const string &str_bedFileName,const size_t mode){
    ifstream bedfile;
    string line;
    bedfile.open(str_bedFileName);
    if(bedfile.is_open())
    {
        while (!bedfile.eof()){
            getline(bedfile,line);
            string str_seqID;
            string str_start;
            string str_end;
            tokenizer(line,str_seqID,str_start,str_end);
            size_t a = stoi(str_start);
            size_t b = stoi(str_end);
            char *sequence = extractSequence(str_seqID,a,b);
            switch(mode){
                case 0: {
                    cout<<'>'<<str_seqID<<endl;
                    cout<<sequence<<endl;
                    break;
                }
                case 1: {
                    cout<<str_seqID<<"\t"<<a<<"\t"<<b<<"\t";
                    cout<<sequence<<endl;
                    break;
                }
                default: {
                    cout<<"wrong input for last parameter"<<endl;
                    exit(0);
                }
            }
            free(sequence);
        }
    }
    bedfile.close();
}