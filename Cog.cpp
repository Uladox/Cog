/*#This file is part of Cog
#
#	Copyright 2014 Uladox
#
#    Cog is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    Cog is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Cog.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <list>
#include <stdlib.h>
#include <iterator>
#include <map>
#include <fstream>
#include <math.h>
#include <utility>

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

string& slot::macrokeeper::operator[](string value){
        bool found = false;
        string* result;
        list<pair<string, slot> >::iterator iter = macroslist.begin();
        if(macroslist.size() == 0){
            slot tempkeeper;
            pair<string, slot> tempair = make_pair(value, tempkeeper);
            macroslist.push_back(tempair);
            result = &macroslist.back().second.code;
            found = true;
        }
        while(found != true){
            if(iter->first == value){
                result = &iter->second.code;
                //.inside;
                found = true;
            }else if(iter == macroslist.end()){
                slot tempkeeper;
                macroslist.push_back(make_pair(value,tempkeeper));
                result = &macroslist.back().second.code;
                found = true;
            }else{
                advance(iter, 1);
            }
        }
        return *result;
    }

//returns macrkeep, not string
slot::macrokeeper& slot::macrokeeper::macl(string value){
    bool found = false;
    slot::macrokeeper* result;
    list<pair<string, slot> >::iterator iter = macroslist.begin();
    if(macroslist.size() == 0){
        slot tempkeeper;
        pair<string, slot> tempair = make_pair(value, tempkeeper);
        macroslist.push_back(tempair);
        result = &macroslist.back().second.macromap;
        found = true;
    }
    while(found != true){
        if(iter->first == value){
            result = &iter->second.macromap;
            //.inside;
            found = true;
        }else if(iter == macroslist.end()){
            slot tempkeeper;
            macroslist.push_back(make_pair(value,tempkeeper));
            result = &macroslist.back().second.macromap;
            found = true;
        }else{
            advance(iter, 1);
        }
    }
    return *result;
}
slot& slot::macrokeeper::get_slot(string value){
    bool found = false;
    slot* result;
    list<pair<string, slot> >::iterator iter = macroslist.begin();
    if(macroslist.size() == 0){
        slot tempkeeper;
        pair<string, slot> tempair = make_pair(value, tempkeeper);
        macroslist.push_back(tempair);
        result = &macroslist.back().second;
        found = true;
    }
    while(found != true){
        if(iter->first == value){
            result = &iter->second;
            //.inside;
            found = true;
        }else if(iter == macroslist.end()){
            slot tempkeeper;
            macroslist.push_back(make_pair(value,tempkeeper));
            result = &macroslist.back().second;
            found = true;
        }else{
            advance(iter, 1);
        }
    }
    return *result;
}
// |  I could use auto here, but I would rather annoy you
// v  I am so eval() Wahahahaha!!!
list<pair<string, slot> >::iterator slot::macrokeeper::find(string value){
    list<pair<string, slot> >::iterator iter = macroslist.begin();
    while((iter->first != value) && (iter != macroslist.cend()))
        advance(iter, 1);
    return iter;
}
void slot::macrokeeper::erase(list<pair<string,slot>>::iterator erasable){
    macroslist.erase(erasable);
}
slot slot::prev_obj(){
    list<slot>::iterator refspot = currentlist.back()->slotlist.begin();
    advance(refspot, currentlist.back()->slotlist.size() - 2);
    return *refspot;
}
slot slot::slot_last(int pos){
    list<slot>::iterator refspot = slotlist.begin();
    advance(refspot, slotlist.size() - pos);
    return *refspot;
}
slot slot::neg_slotlast(int pos){
    list<slot>::iterator refspot = slotlist.begin();
    advance(refspot, pos);
    return *refspot;
}
void slot::inset_obj(string word)
{
    slot tempslot;
    tempslot.set_code(word);
    currentlist.back()->slotlist.push_back(tempslot);
}
string stradjust(string rawstring)
{
    size_t found = rawstring.find("\\");
    while(found != string::npos){
        rawstring.insert(found, "\\");
        found = rawstring.find("\\", found+2);
    }
    found = rawstring.find("\"");
    while(found != string::npos){
        rawstring.insert(found, "\\");
        found = rawstring.find("\"", found+2);
    }
    return rawstring;
}
string slot::object_code()
{
    string objcode;

    int obs = slotlist.size();
    for(int i = 0; i < obs; ++i){
            objcode += "\"" + stradjust(neg_slotlast(i).code) + "\" ";
            if(neg_slotlast(i).slotlist.size() != 0){
            objcode += "<<( " + neg_slotlast(i).object_code() + ") ";
        }
    }
    return objcode;
}
string slot::get_macs()
{
    string macs;
    macs += "( ";
    for(list<pair<string, slot>>::iterator maciter = macromap.macroslist.begin(); maciter != macromap.macroslist.end(); ++maciter){
        macs += "\"" + stradjust(maciter->second.code) + " " + stradjust(maciter->first) + "\" defmacro ";
    }
    macs += ") dc";
    return macs;
}
size_t first_whitespace(string tempcode)
{
    size_t tempspace = tempcode.find(" ");
    size_t temptab = tempcode.find("\t");
    size_t tempnewln = tempcode.find("\n");
    //finds closest whitespace
    size_t ultimatesize = (tempspace < tempnewln ? tempspace : tempnewln);
    ultimatesize = (ultimatesize < temptab ? ultimatesize : temptab);
    return ultimatesize;
}

size_t last_whitespace(string tempcode)
{
    size_t tempspace = tempcode.rfind(" ");
    size_t temptab = tempcode.rfind("\t");
    size_t tempnewln = tempcode.rfind("\n");
    //finds closest whitespace
    size_t ultimatesize = (tempspace < tempnewln ? tempspace : tempnewln);
    ultimatesize = (ultimatesize < temptab ? ultimatesize : temptab);
    return ultimatesize;
}
string first_word(string& tempcode)
{
        char firstchar = tempcode[0];
        while(firstchar == ' '||firstchar == '\t'||firstchar == '\n'){
            tempcode.erase(0, 1);
            firstchar = tempcode[0];
        }
        size_t ultimatesize = first_whitespace(tempcode);
        string evenmoretempcode = tempcode;
        //if not at end of string, cut off part and eval, else eval
        if(ultimatesize != string::npos){
            evenmoretempcode.erase(ultimatesize, tempcode.length());
        }else{
            return evenmoretempcode;
        }
}

//determines what to do with each section of parsed string
//how to understand: test each statement
//do not hope to understand this mess
//only I can know its true power
//even though it is very simple
//it is a lot of code
void slot::stringeval(string word)
{
    slot tempslot;
    tempslot.set_code(word);
    currentlist.back()->slotlist.push_back(tempslot);
}
/*
bool macrohandler(list<slot> currobj, list<slot*> currlist, string word, string& tempcode)
{

    bool finished;
    while(currlist.size() != 0 && finished == false){
        slot* tempslot = currlist.back();
        if(tempslot->topmacros){
            while(currlist.size() > 1)
                currlist.pop_back();
        }

        else if(first_word(tempcode) == "`"){
            currlist.front()->inset_obj(tempslot->macromap[word]);
            tempcode.erase(0, first_whitespace(tempcode));
            if(tempslot->macromap[word].empty())
                tempslot->macromap.erase(tempslot->macromap.find(word));
            finished = true;
        }
        //yay macros!
        else if(!tempslot->macromap[word].empty()){
            if(first_word(tempcode) == "enclosemac"){
                tempcode.erase(0, first_whitespace(tempcode));
                if(tempcode.empty())
                    tempcode = "\"" + tempslot->macromap[word] + "\"";
                else
                    tempcode = "\"" + tempslot->macromap[word] + "\"" + tempcode;
            }else if(first_word(tempcode) == "ignoremac"){
                tempcode.erase(0, first_whitespace(tempcode));
                if(currobj.back().code == "0"){
                    if(tempcode.empty())
                        tempcode = tempslot->macromap[word];
                    else
                        tempcode = tempslot->macromap[word]+ " " + tempcode;
                }
            }else{
                if(tempcode.empty())
                    tempcode = tempslot->macromap[word];
                else
                    tempcode = tempslot->macromap[word]+ " " + tempcode;
            }
            finished = true;
        }
        currlist.pop_back();
    }
    return finished;
}
*/
void slot::wordeval(string word, string& tempcode){
#define LASTSLOT currentlist.back()->slotlist

        if(first_word(tempcode) == "\'"){
            inset_obj(word);
            tempcode.erase(0, first_whitespace(tempcode));
        }
        else
        //if(macrohandler(slotlist, currentlist, word, tempcode)){
//
  //      }
        //else
        if(word == "print"){
            cout << LASTSLOT.back().code;
        }else if(word == "cin"){
            string input;
            cin >> input;
            inset_obj(input);
        }else if(word == "cinline"){
            string input;
            getline(cin, input);
            inset_obj(input);
        }else if(word == "strtofile"){
            string input;
            string line;
            ifstream file (LASTSLOT.back().code);
            if(file.is_open()){
                while(getline(file, line)){
                    input += line + "\n";
                }
                file.close();
            }
            input.erase(input.length() -1, input.length());
            LASTSLOT.back().code = input;
        }else if (word == "write"){
            ofstream file (LASTSLOT.back().code);
            if (file.is_open()){
                file << prev_obj().code;
                file.close();
                }
            else cout << "Unable to open file";
        }else if(word == "import"){
            string input;
            string line;
            ifstream file (LASTSLOT.back().code);
            if(file.is_open()){
                while(getline(file, line)){
                    input += line + "\n";
                }
                file.close();
            }
            input.erase(input.length() -1, input.length());
            LASTSLOT.pop_back();
            tempcode = input + " " + tempcode;
        }else if(word == "copy"){
            LASTSLOT.push_back(LASTSLOT.back());
        }else if (word == "eval"){
            LASTSLOT.back().eval();
        }else if (word == "compare"){
            int compared = prev_obj().code.compare(LASTSLOT.back().code);
            inset_obj("" + compared);
        }else if (word == "("){
            slot tempslot;
            LASTSLOT.push_back(tempslot);
            currentlist.push_back(&LASTSLOT.back());
        }else if (word == "<<("){
            currentlist.push_back(&LASTSLOT.back());
        }else if (word == ">>("){
            slot tempslot;
            LASTSLOT.push_back(tempslot);
            LASTSLOT.back().slotlist.push_back(prev_obj());
            currentlist.push_back(&LASTSLOT.back());
        }else if (word == ")"){
            if(currentlist.size() == 0){
                cout << "\nError, Too many )\'s?";
                exit (EXIT_FAILURE);
            }
            //slot* tempslot = currentlist.back();
            currentlist.pop_back();
            //LASTSLOT.push_back(*tempslot);
        }else if (word == "cdr"){
            slot tempslot = LASTSLOT.back();
            LASTSLOT.push_back(tempslot);
            LASTSLOT.back().slotlist.pop_front();
        }else if (word == "car"){
            slot tempslot = LASTSLOT.back().slotlist.front();
            LASTSLOT.push_back(tempslot);
        }else if (word == "reverse"){
            slot tempslot = LASTSLOT.back();
            LASTSLOT.push_back(tempslot);
            LASTSLOT.back().slotlist.reverse();
        }else if (word == "uproot"){
            list<slot>::iterator spot = LASTSLOT.back().slotlist.begin();
            size_t currlistsize = LASTSLOT.back().slotlist.size();
            for(int i =  0; i < currlistsize; ++i){
                LASTSLOT.push_back(*spot);
                LASTSLOT.push_back(LASTSLOT.back());
                advance(spot, 1);
            }
        }else if (word == "dc"){
            LASTSLOT.pop_back();
        }else if (word == "dp"){
            list<slot>::iterator refspot = LASTSLOT.begin();
            advance(refspot, LASTSLOT.size() - 2);
            LASTSLOT.erase(refspot);
        }else if (word == "clean"){
            LASTSLOT.clear();
        }else if(word == "die"){
            macromap["die"] = "1";
        }else if (word == "this"){
            LASTSLOT.push_back(*this);
        }else if (word == "tempcode"){
            inset_obj(tempcode);
        }else if (word == "cur"){
            list<slot*> templist = currentlist;
            templist.pop_back();
            slot tempslot = *(templist.back());
            int before = strtol(LASTSLOT.back().code.c_str(), NULL, 0);
            for(int i = 0; i < before; i++)
                tempslot.slotlist.pop_back();
            tempslot = tempslot.slotlist.back();
            LASTSLOT.push_back(tempslot);
        }else if (word == "defmacro"){
            //tempcode = macromap[currentobj.back()->code] + " " + tempcode;
            string macrostring = LASTSLOT.back().code;
            string macrostring2 = macrostring;
            size_t macrosize = last_whitespace(macrostring);
            if(macrosize == string::npos)
                macromap[macrostring] = macrostring;
            else
                macromap[macrostring.erase(0, macrosize + 1)] = macrostring2.erase(macrosize, macrostring2.length());
        }else if(word == "machere"){
            string macrostring = LASTSLOT.back().code;
            string macrostring2 = macrostring;
            size_t macrosize = last_whitespace(macrostring);
            if(macrosize == string::npos)
                LASTSLOT.back().macromap[macrostring] = macrostring;
            else
                LASTSLOT.back().macromap[macrostring.erase(0, macrosize + 1)] = macrostring2.erase(macrosize, macrostring2.length());
        }else if (word == "."){
            currentlist.back()->macromap.get_slot(LASTSLOT.back().code) = prev_obj();
        }else if (word == "?"){
            //macromap.
            list<slot>::iterator tempiter = LASTSLOT.back().slotlist.begin();
            /*
            slot returnedslot = currentlist.back()->macromap.get_slot((*tempiter).code);
            int itsize = LASTSLOT.back().slotlist.size();
            //advance(tempiter, 1);
            cout << itsize;
            for(int i = 0; i < itsize; ++i){
                    cerr << (*tempiter).code;
            advance(tempiter, 1);
                returnedslot = returnedslot.macromap.get_slot((*tempiter).code);
            }
            LASTSLOT.push_back(returnedslot);*/
            slot returnedslot = currentlist.back()->macromap.get_slot((*tempiter).code);
            advance(tempiter, 1);
            int ad = LASTSLOT.back().slotlist.size();
            for (int i = 1; i < ad; ++i){
                cerr << (*tempiter).code;
                returnedslot = returnedslot.macromap.get_slot((*tempiter).code);
                advance(tempiter, 1);
            }
            LASTSLOT.push_back(returnedslot);
        }else if (word == "dprevmacro"){
            string macromeaning = prev_obj().code;
            string macroname = LASTSLOT.back().code;
            macromap[macroname] = macromeaning;
            //Massive math block i means long int, d is double
        }else if (word == "deflistmacro"){
            string macro;
            list<slot>::iterator spot = LASTSLOT.back().slotlist.begin();
            int lastsize = LASTSLOT.back().slotlist.size();
            for(int i = 0; i < lastsize - 1; ++i){
                macro += " " + (*spot).code;
                advance(spot, 1);
            }
            macromap[(*spot).code] = macro;
            //cout << (*spot).code;
        }else if (word == "macify"){
            tempcode = LASTSLOT.back().code + " " + tempcode;
            list<slot>::iterator refspot = LASTSLOT.begin();
            advance(refspot, LASTSLOT.size() - 2);
            LASTSLOT.erase(refspot);
        }else if (word == "get_macs"){
            inset_obj(LASTSLOT.back().get_macs());
        }else if (word == "to_str"){
            inset_obj(object_code());
        }else if(word == "strequal"){
            inset_obj(to_string(LASTSLOT.back().code == prev_obj().code));
        }else if (word == "stradd"){
            inset_obj(prev_obj().code + LASTSLOT.back().code);
        }else if (word == "strerase"){
            list<slot>::iterator spot = LASTSLOT.begin();
            advance(spot, LASTSLOT.size()-3);
            slot tempslot = *spot;
            inset_obj(tempslot.code.erase(strtol(prev_obj().code.c_str(), NULL, 0),strtol(LASTSLOT.back().code.c_str(), NULL, 0)));
        }else if(word == "strfind"){
            inset_obj(to_string(prev_obj().code.find(LASTSLOT.back().code)));
        }else if(word == "strfindprev"){
            list<slot>::iterator spot = LASTSLOT.begin();
            advance(spot, LASTSLOT.size()- 3);
            slot tempslot = *spot;
            inset_obj(to_string(tempslot.code.find(LASTSLOT.back().code)));
        }else if (word == "strnpos"){
            inset_obj(to_string(string::npos));
        }else if (word == "+d"){
            double double_contents = strtod(LASTSLOT.back().code.c_str(), NULL) + strtod(prev_obj().code.c_str(), NULL);
            inset_obj(to_string(double_contents));
        }else if (word == "+i"){
            long int long_contents = strtol(LASTSLOT.back().code.c_str(), NULL, 0) + strtol(prev_obj().code.c_str(), NULL, 0);
            inset_obj(to_string(long_contents));
        }else if (word == "-d"){
            double double_contents = strtod(LASTSLOT.back().code.c_str(), NULL) - strtod(prev_obj().code.c_str(), NULL);
            inset_obj(to_string(double_contents));
        }else if (word == "-i"){
            long int long_contents = strtol(LASTSLOT.back().code.c_str(), NULL, 0) - strtol(prev_obj().code.c_str(), NULL, 0);
            inset_obj(to_string(long_contents));
        }else if (word == "*d"){
            double double_contents = strtod(LASTSLOT.back().code.c_str(), NULL) * strtod(prev_obj().code.c_str(), NULL);
            inset_obj(to_string(double_contents));
        }else if (word == "*i"){
            long int long_contents = strtol(LASTSLOT.back().code.c_str(), NULL, 0) * strtol(prev_obj().code.c_str(), NULL, 0);
            inset_obj(to_string(long_contents));
        }else if (word == "/d"){
            double double_contents = strtod(LASTSLOT.back().code.c_str(), NULL) / strtod(prev_obj().code.c_str(), NULL);
            inset_obj(to_string(double_contents));
        }else if (word == "/i"){
            long int long_contents = strtol(LASTSLOT.back().code.c_str(), NULL, 0) / strtol(prev_obj().code.c_str(), NULL, 0);
            inset_obj(to_string(long_contents));
        }else if (word == "%d"){
            double double_contents = fmod(strtod(LASTSLOT.back().code.c_str(), NULL), strtod(prev_obj().code.c_str(), NULL));
            inset_obj(to_string(double_contents));
        }else if (word == "%i"){
            long int long_contents = strtol(LASTSLOT.back().code.c_str(), NULL, 0) % strtol(prev_obj().code.c_str(), NULL, 0);
            inset_obj(to_string(long_contents));
        }else if (word == "sqrtd"){
            double double_contents = sqrt(strtod(LASTSLOT.back().code.c_str(), NULL));
            inset_obj(to_string(double_contents));
        }else if (word == "sqrti"){
            long int long_contents = sqrt(strtol(LASTSLOT.back().code.c_str(), NULL, 0));
            inset_obj(to_string(long_contents));
        }else if (word == "!"){
            if(LASTSLOT.back().code.compare("0") == 0){
                LASTSLOT.back().code = "1";
            }else{
                LASTSLOT.back().code = "0";
            }
        }else if (word == ">"){
           if(atof(LASTSLOT.back().code.c_str())>atof(prev_obj().code.c_str())){
                inset_obj("1");
            }else{
                inset_obj("0");
            }
        }else if (word == "<"){
           if(atof(LASTSLOT.back().code.c_str())<atof(prev_obj().code.c_str())){
                inset_obj("1");
            }else{
                inset_obj("0");
            }
        }else{
            //if it is not a statement, it must be a variable and a new
            //slot is created at current scope.
            inset_obj(word);
        }
    }


    //finds end of string including tabs as \t and new lines as \n etc.
    //also \" is not counted as end, but " is.
    //parses one char at a time.
    void slot::string_maker(string& tempcode, char& firstchar)
    {
        bool backslashed = false;
        bool to_end_string = true;
        string finalized_string;
        while(to_end_string){
            tempcode.erase(0, 1);
            firstchar = tempcode[0];
            if(backslashed == false){
                if(firstchar == '\\'){
                    backslashed = true;
                    } else if(firstchar == '\"'){
                        //this is the en of the string
                        //second unbackslashed "
                        tempcode.erase(0, 1);
                        stringeval(finalized_string);
                        to_end_string = false;
                    }else{
                        //adds char so if char is 'a' and
                        //string is "bc", the ouput is "bca"
                        finalized_string.append(1, firstchar);
                        }
                    }else if(backslashed == true){
                        backslashed = false;
                        if(firstchar == '\\'){
                            finalized_string += "\\";
                        } else if(firstchar == 'n'){
                            finalized_string += "\n";
                        } else if(firstchar == 't'){
                            finalized_string += "\t";
                        }else if(firstchar == '\"'){
                            finalized_string += "\"";
                        }

                    }

                }
            }

    //finds bit of string before next whitespace and sends it to wordeval
    void slot::code_maker(string& tempcode, bool& to_end)
    {

        size_t ultimatesize = first_whitespace(tempcode);
        //if not at end of string, cut off part and eval, else eval
        if(ultimatesize != string::npos){
            string evenmoretempcode = tempcode;
            evenmoretempcode.erase(ultimatesize, tempcode.length());
            tempcode.erase(0, evenmoretempcode.length()+1);
            wordeval(evenmoretempcode, tempcode);
        }else{
            string evenmoretempcode = tempcode;
            tempcode.clear();
            wordeval(evenmoretempcode, tempcode);
            if(tempcode.empty()){
                to_end = false;
            }
        }
    }



    void slot::eval()
    {
        string tempcode = code;
        bool to_end = true;
        while(to_end){
            char firstchar = tempcode[0];
            //whitespace ignorer loop (duh)
            while(firstchar == ' '||firstchar == '\t'||firstchar == '\n'){
                tempcode.erase(0, 1);
                firstchar = tempcode[0];
            }
            if(tempcode.empty()){
                to_end = false;
            }else if(firstchar == '\"'){
                 string_maker(tempcode, firstchar);
            }else{
                code_maker(tempcode, to_end);
        }
    }
}
