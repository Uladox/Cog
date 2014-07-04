#include <iostream>
#include "Cog.h"
#include <thread>
#include <list>
#include <stdlib.h>

using namespace std;

struct Sobject{
  slot inslot;
  bool die;
  thread sthread;
  void run();
  void end(){
    sthread.join();
    cout << "ended";
  }
  void start(){
    sthread = thread (&Sobject::run, this);
  }
};

list<Sobject*> killist;
void killem (){
  while (true) {
    if(killist.size() != 0){
      killist.front()->end();
      killist.pop_front();
     }
  }
}
thread killthread (killem);

void Sobject::run(){
    while(die != true){
      inslot.eval();
      if(inslot.macromap["die"] == "1")
	die = true;
    }
    killist.push_back(this);
  }

int main()
{
    Sobject a;
    a.inslot.set_code("a print  b print \"cat\" stradd print \"1 die\" defmacro");
    a.inslot.slotset();
    a.start();
    //hack, DO NOT USE IN REAL PROJECTS!!!!!!
    string n;
    cin >> n;
    exit(EXIT_SUCCESS);
    cerr << "\nYou have choosen to exit";
   
}
