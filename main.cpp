#include <iostream>
#include <fstream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>

class Wrapper{
private:
    static std::exception_ptr threadException;
    std::ofstream file;
public:
    static std::exception_ptr& getThreadException(){return threadException;}
    Wrapper(const std::string& fileName = "example.txt"):file(fileName,std::ofstream::app)
    {
        if(!file.is_open()){
        throw std::runtime_error("unable to open file");
        }
    }
    ~Wrapper()
    {
        file.close();
    }
    std::ofstream& operator<<(const std::string message){
        file<<message;
        return file;
    }

};

std::exception_ptr Wrapper::threadException{nullptr};


void WriteToFile(const std::string& message){
    //handling exception in case if additional thread will throw it in background
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    try{
    Wrapper file;
    file << message << std::endl;
    }
    catch(...){
        Wrapper::getThreadException() = std::current_exception();
    }
    if(Wrapper::getThreadException()){
        try
        {
            std::rethrow_exception(Wrapper::getThreadException());
        }
        catch(const std::exception& exception)
        {
            std::cout<<"Thread error: " << exception.what() << '\n';
        }  
    }
}

int main(){
    std::thread t1([](){
        WriteToFile("text1");
    });
     std::thread t2([](){
        WriteToFile("text2");
    });
    t1.join();
    t2.join();
    return 0;
}