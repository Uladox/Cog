#ifndef COG_H_INCLUDED
#define COG_H_INCLUDED

#include <map>
#include <string>
#include <list>
using namespace std;

struct slot
{
  bool topmacros = true;
    struct macrokeeper
    {
        list<pair<string, slot> > macroslist;
        string& operator[](string value);
        list<pair<string, slot> >::iterator find(string value);
        void erase(list<pair<string, slot> >::iterator erasable);
        macrokeeper& macl(string value);
        slot& get_slot(string value);
    } macromap;

    list<slot> slotlist; //original name, right?
    list<slot*> currentlist;
    string code;
    void slotset(){
        currentlist.push_back(this);
    }
    void set_code(string newcode){
        code = newcode;
    }
    void code_maker(string& tempcode, bool& to_end);
    void string_maker(string& tempcode, char& firstchar);
    void eval();
    void wordeval(string word, string& tempcode);
    void stringeval(string word);
    slot prev_obj();
    slot slot_last(int pos);
    slot neg_slotlast(int pos);
    void inset_obj(string word);
    string object_code();
    string get_macs();
};


#endif // COG_H_INCLUDED
