#include <iostream>
#include "Cog.h"
#include <thread>
using namespace std;

struct Sobject{
    slot inslot;
    bool die;
    void run(){
        while(die != true){
            inslot.eval();
        }
    }
    void start(){
        thread beginrun(&Sobject::run, this);
        beginrun.join();
        //thread t1 (what);
        //t1.join();
    }

};

int main()
{
    Sobject a;
    a.inslot.set_code("a print");
    a.inslot.slotset();
    a.start();
}
