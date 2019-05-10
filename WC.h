
#ifndef WC_H
#define WC_H

class WC {
public:
    WC();
    char* convertToString();
    ~WC();
    void setTotalBytes(int );
    void setTotalLines(int ) ;
    void setTotalWord(int );
    void addByteCounter(int);
    void addLineCounter(int) ;
    void addWordCounter(int);


    int getTotalBytes() ;

    int getTotalLines() ;

    int getTotalWords() ;
    
    void print();

private:
    int totalBytes;
    int totalLines;
    int totalWords;
};

#endif /* WC_H */

