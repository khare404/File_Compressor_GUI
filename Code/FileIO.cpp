#include "FileIO.h"

size_t FileIO::getFileSize(std::string path){
    stream.open(path, std::ios::binary | std::ios::ate|std::ios::in );
	if(!stream.is_open()) // stream.is_open() to check whether the file is open or not
        return 0;
    size_t fileSize = stream.tellg(); //tellg() function returns the current "get" position of the pointer in the stream
    stream.clear();
    stream.close();

    return fileSize;
}


bool FileIO::readFile(std::string path,char **bufferOut, size_t *sizeFile) {
	
    size_t size = getFileSize(path);
    if(size == 0)
        return false; // the file couldn't be opened.

    if(sizeFile != nullptr)
        *sizeFile = size;

    *bufferOut = new char[size];
	
    stream.open (path, std::ios::in | std::ios::binary);
    if(!stream.is_open())
        return false;

    if(*bufferOut != nullptr)
        if(!stream.read(*bufferOut,size)) //allows to read specified no. of bytes from the input stream & also strore them into buffer
        // here it will read upto the sizeof(bufferOut)
            return false;

    stream.clear();
    stream.close();
    return true;

}

bool FileIO::writeEncodedFile(std::string path, std::string fileExtension,std::deque<int> frequencies,std::deque<char> symbols,int padding) {


    stream.open(path + fileExtension, std::ios::binary | std::ios::out);
    //"std::ios::out" indicates that you intend to write data to the file
    if(!stream.is_open())
        return false; //error opening file


    unsigned long aux = frequencies.size();// size() function defined in string header file used to determine the size of a container
   stream.write(reinterpret_cast<char*>(&aux),sizeof(int)); // sizeof() used to determine the size in bytes of a data type or an object
   /*
   In this we are taking the address of the variable named 'aux' & then using 'reinterpret_cast' to convert it into pointer to char.
   <char*> indiactes it should change data into char
   &aux indicates that this data should be changed into char
   sizeof(int) specifies the number of bytes to write
   this function will write the binary representation of an 'int' variable aux to an output stream in character type
   this is a way to store raw binary data of an integer
   */
   
    for(unsigned long i = 0; i <frequencies.size();i++){
        stream.write(&(symbols.at(i)),sizeof(char));// .at() function to access element at a specific index
        stream.write(reinterpret_cast<char*>(&(frequencies.at(i))),sizeof(int));
    }

    //writing how many bits will not be considered at the end of the file (padding)
    stream.write(reinterpret_cast<char*>(&padding),sizeof(int));
    return true;

}
void FileIO::writeEncodedByte(const char bits[], unsigned long bufferSize){
	
	
    char byte[bufferSize];
    int j = 0;
    int x = 0;
    byte[0] = 0;

    for (int i = 0; i < 8*bufferSize; i++)
    {
        byte[j] |= (bits[i] & 1) << x++; 
		// (bits[i]&1) operation result will be 0 if the LSB of bits[i] is 0 & 1 if the LSB of bits[i] is 1 
        // then bit will be shifted to the left by 'x' positions
        if(x == 8){
            j++;
            x = 0;
            byte[j] = 0;
        }
    }
    stream.write(byte,bufferSize);

}
bool FileIO::readerHeader(std::string path,std::deque<int> *frequencies,std::deque<char> *symbols,int *padding){
    int size;
    int frequency;
    char symbol;

    stream.open(path, std::ios::binary | std::ios::in);
    if(!stream.is_open())
        return false;

    stream.read(reinterpret_cast<char*>(&size),sizeof(int));
    for(int i = 0; i < size; i ++){
        stream.read(&symbol,sizeof(char));
        symbols->push_back(symbol); 
		// push_back is used to add an element to the end of the container thus dynamically increasing the size of the container
        // '->' operator is used to access the member or member function of an object
        
		stream.read(reinterpret_cast<char*>(&frequency),sizeof(int));
        frequencies->push_back(frequency);
    }

    stream.read(reinterpret_cast<char*>(padding),sizeof(int));
    return true;

}

// reading symbols that has been added to the file by readerHeader member function
void FileIO::readSymbols(std::deque<char> *symbols) {
    //Constructing a vector of symbols to be decoded by program.
    char c;
    while(!stream.eof()){//'.eof()' used to check if the end-of-file or not
        stream.read(&c,sizeof(char));
        symbols->push_back(c);

    }
    symbols->pop_back(); 
    stream.close();

}



bool FileIO::writeDecodedFile(std::string path) {
    stream.open(path,std::ios::binary|std::ios::out);

    if(!stream.is_open())
        return false; //error opening file
}

void FileIO::writeDecodedByte(const char bits) {

    stream.put(bits); // to write a single character to the output stream, used to write in a text file or binary file

}

void FileIO::closeFile() {
    if(stream.is_open())
        stream.close();
}
