# TwobitParser
TwobitParser is a cpp library for accessing content of 2bit files. It also offers extra command-line output functions.

# fasta vs 2bit
- FASTA file is a text-based file for representing either nucleotide sequences or amino acid (protein) sequences.Each record in fasta file format, has a header followed by the sequence, nucleotide or amino acids from the next line. Fasta file can store sigle or multiple sequences and has no upper limit for storage. Data from a fasta file is fetched sequentially but random access can be achived using an index file of the same fasta file.
File extensions for fasta file are '.fasta', '.fna', '.ffn', '.faa', '.frn' and '.fa'.

- 2Bit file is a serialized file format which stores only nucleotide sequences. Highly efficient and compact, uses ~4 fold less memory for storage as compared to fasta format, Since each nucleotide is represente by 2 bits instead of 8 bits in ASCII-based text format. 2bit file can store upto 4GB of total genomic size.

More detailed information about structure of 2bit file format is avialable at UCSC website "http://genome.ucsc.edu/FAQ/FAQformat.html#format7"
File extensions for a 2bit file is '.2bit'.
	
### Advantage in term of storage and accessibility  
| Parameter           | Fasta     | 2bit     |
| :------------------ | :-------- | :------- |
| **Storage memory**  | 51.8 MB   | 13.2 MB  |
| **Access time**     | 3 sec     | 1 sec    |

# Usage
Usage example is also provide in file called '''example.cpp'''.

Example is also provide in example.cpp file 
  #### Including header
    #include "twobitparser.cpp"
  
  #### Creating an object
    string filename = "example.2bit";
    TwoBitReader tbr_obj(filename);

  #### Accessing list of sequencesIDs along with sequence length
    map<string,size_t> *sequenceMap;
    sequenceMap = tbr_obj.showSequenceIds();
   
    // for viewing content of map
    for (auto m : *sequenceMap){
        cout<<m.first<<"\t"<<m.second<<endl;
    }
  
  #### Fetch a sequence or sub-sequence
    // using sequence Id
    char *sequence = nullptr;
    sequence = tbr_obj.extractSequence("SampleChr1");
    cout << sequence << endl; 
    free(sequence);
  
    // using sequence id with start and end
    sequence = tbr_obj.extractSequence("SampleChr1",10,100);
    cout << sequence << endl;
    free(sequence);
  
    // using a Bed file
    string bedfile = "example.bed";
    
    // gives output in fasta format
    tbr_obj.extractSequenceFromBedFile(bedfile,0);
                      or
    // gives output in bed format
    tbr_obj.extractSequenceFromBedFile(str_bedfile,1);


   #### Fetch basic statistics of the sequence
     map<char,size_t> m = *(tbr_obj.bases(sequence));
     for(auto mm : m){
        cout<<mm.first<<"\t"<<mm.second<<endl;
     }
  
   #### Fetch Hard masked informations/N block informations
    // Number of N's in complete file
    cout << "total N's in 2bit file: " << tbr_obj.countNs() << endl;
    
    // Number of N's in a sequence
    string str_seqId = "SampleChr1";
    cout << "total N's in sequence " << str_seqId << " : " << tbr_obj.countNs(str_seqId) << endl;
    
    // all N Blocks in the 2bit file
    std::map<string,std::map<int,int>> NMap = tbr_obj.NMap;
  
    
   #### Fetch Soft masked informations
    // Total number of nucleotide in soft masked region
    cout << "total softmasked nucleotide in 2bit file: " << tbr_obj.countSoftMasked()<<endl;
  
    // Number of nucleotides in soft masked region in a sequence
    cout<<"total softmasked nucleotide in fasta file: "<<tbr_obj.countSoftMasked("SampleChr1")<<endl;
  
    // all soft masked region in 2bit file
    std::map<string,std::map<int,int>> maskMap = tbr_obj.maskMap;
  
_Functions with command line output_
   #### Converting sequence from 2bit to fasta
    string outputfile = "exampleOutput.fasta";
    tbr_obj.twoBitToFasta(outputfile);
  
   #### Soft masking sequences using coordinates from bed file
    string bedFile = "example.bed";
    tbr_obj.softMaskSequencesUsingBedFile(bedFile);
  
   #### Hard masking sequences using coordinates from bed file
    string bedFile = "example.bed";
    tbr_obj.hardMaskSequencesUsingBedFile(bedFile);

# License
MIT License

Copyright (c) 2022 AbhijeetKaran

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
