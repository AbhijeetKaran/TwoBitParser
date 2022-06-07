#include"twobitparser.cpp"
#include<map>

int main(int argc, char** argv)
{
    string str_filename = argv[1];
    TwoBitReader tbr_obj(str_filename);

    // all the chromosomes id  with their respective lengths
    // tbr_obj.showSequences();  change name from showsequence to showSequenceId returns a map
    map<string,size_t> *sequenceMap;
    sequenceMap = tbr_obj.showSequenceIds();
    for (auto m : *sequenceMap){
        cout<<m.first<<"\t"<<m.second<<endl;
    }


    //below two functions are overloaded
    /* tbr_obj.extractSequence(); 
        Name changed to extractSequence("sequenceName") returns a char array of string
        it also needs to be destroyed to avoid meamory leaks
    */
    char *sequence = nullptr;
    sequence = tbr_obj.extractSequence("SampleChr1");
    cout<<sequence<<endl;

    map<char,size_t> m = *(tbr_obj.bases(sequence));
    for(auto mm : m){
        cout<<mm.first<<"\t"<<mm.second<<endl;
    }
    free(sequence);

    /*  tbr_obj.extractSequence("SampleChr1",10,100); 
        returns a sequence of seqid given length
    */
    sequence = tbr_obj.extractSequence("SampleChr1",10,100);
    cout<<sequence<<endl;
    free(sequence);

    /*  No change in the function name or function but has two mode 
        mode '0' : prints the sequences in fasta format  
        mode '1' : prints row with 4th column as sequence 
    */
    cout<<"from bed file"<<endl;
    string str_bedfile = "formask.bed";
    tbr_obj.extractSequenceFromBedFile(str_bedfile,0);


    /*  
        count the number of Ns in full file,
        or in a particular sequence
    */
    cout<<"total N's in fasta file: "<<tbr_obj.countNs()<<endl;
    cout<<"total N's in fasta file: "<<tbr_obj.countNs("SampleChr1")<<endl;


    /*contain info of all Ns and masked regions*/
    std::map<string,std::map<int,int>> NMap = tbr_obj.NMap;
    std::map<string,std::map<int,int>> maskMap = tbr_obj.maskMap;

    return 0;
}