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

using namespace std;

struct slot
{
    map<string, string> macromap;
    list<slot> slotlist; //original name, right?
    list<slot*> currentlist;
    list<slot*> currentobj;
    string code;
    void slotset(){
        currentlist.push_back(this);
        //currentobj.push_back(this);
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
    void inset_obj(string word);
};
slot slot::prev_obj(){
    list<slot*>::iterator refspot = currentlist.back()->currentobj.begin();
    advance(refspot, currentlist.back()->currentobj.size() - 2);
    return **refspot;
}
void slot::inset_obj(string word)
{
    slot tempslot;
    tempslot.set_code(word);
    currentlist.back()->slotlist.push_back(tempslot);
    currentlist.back()->slotlist.back().slotset();
    currentlist.back()->currentobj.push_back(&currentlist.back()->slotlist.back());
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
    currentlist.back()->slotlist.back().slotset();
    currentlist.back()->currentobj.push_back(&currentlist.back()->slotlist.back());
}
void slot::wordeval(string word, string& tempcode)
{
    #define LASTOBJ currentlist.back()->currentobj
    #define LASTSLOT currentlist.back()->slotlist
    //yay macros
    if(!macromap[word].empty()){
        if(first_word(tempcode).compare("enclose") == 0){
            tempcode.erase(0, first_whitespace(tempcode));
            if(tempcode.empty())
            tempcode = "\"" + macromap[word] + "\"";
            else
            tempcode = "\"" + macromap[word] + "\"" + tempcode;
        }else if(first_word(tempcode).compare("ignoremac") == 0){
            if(LASTOBJ.back()->code.compare("1")){
                if(tempcode.empty())
                tempcode = macromap[word];
                else
                tempcode = macromap[word]+ " " + tempcode;
            }
        }else{
            if(tempcode.empty())
                tempcode = macromap[word];
            else
                tempcode = macromap[word]+ " " + tempcode;
        }
        //that was an ordeal...
    }else{
        macromap.erase(macromap.find(word));

        if(word.compare("print") == 0){
            cout << LASTOBJ.back()->code;
        }else if(word.compare("copy") == 0){
           LASTOBJ.push_back(LASTOBJ.back());
        }else if (word.compare("eval") == 0){
            LASTOBJ.back()->eval();
        }else if (word.compare("compare") == 0){
            int compared = prev_obj().code.compare(LASTOBJ.back()->code);
            slot tempslot;
            tempslot.set_code("" + compared);
            currentlist.back()->slotlist.push_back(tempslot);
            currentlist.back()->slotlist.back().slotset();
            LASTOBJ.push_back(&currentlist.back()->slotlist.back());
        }else if (word.compare("(") == 0){
            slot tempslot;
            LASTSLOT.push_back(tempslot);
            LASTSLOT.back().slotset();
            currentlist.push_back(&LASTSLOT.back());
        }else if (word.compare("<<(") == 0){
            currentlist.push_back(LASTOBJ.back());
        }else if (word.compare(">>(") == 0){
            slot tempslot;
            LASTSLOT.push_back(tempslot);
            LASTSLOT.back().slotset();
            LASTSLOT.back().currentobj.push_back(LASTOBJ.back());
            currentlist.push_back(&LASTSLOT.back());
        }else if (word.compare(")") == 0){
            if(currentlist.size() == 0){
                cout << "\nError, Too many )\'s?";
                exit (EXIT_FAILURE);
            }
            slot* tempslot = currentlist.back();
            currentlist.pop_back();
            //LASTSLOT.push_back(tempslot);
            LASTOBJ.push_back(tempslot);
        }else if (word.compare("cdr") == 0){
            slot tempslot = *LASTOBJ.back();
            LASTSLOT.push_back(tempslot);
            LASTSLOT.back().slotset();
            LASTOBJ.push_back(&LASTSLOT.back());
            LASTOBJ.back()->currentobj.pop_front();
        }else if (word.compare("car") == 0){
            slot tempslot = *LASTOBJ.back()->currentobj.front();
            LASTSLOT.push_back(tempslot);
            LASTSLOT.back().slotset();
            LASTOBJ.push_back(&LASTSLOT.back());
        }else if (word.compare("reverse") == 0){
            slot tempslot = *LASTOBJ.back();
            LASTSLOT.push_back(tempslot);
            LASTSLOT.back().slotset();
            LASTSLOT.back().currentobj.reverse();
            LASTOBJ.push_back(&LASTSLOT.back());
        }else if (word.compare("pop_back") == 0){
            LASTOBJ.pop_back();
        }else if (word.compare("to_end") == 0){
            currentlist.back()->slotlist.push_back(slotlist.back());
            LASTOBJ.push_back(&currentlist.back()->slotlist.back());
        }else if (word.compare("pop_prev") == 0){
            list<slot*>::iterator refspot = LASTOBJ.begin();
            advance(refspot, LASTOBJ.size() - 2);
            LASTOBJ.erase(refspot);
        }else if (word.compare("delete") == 0){
            list<slot>::iterator spot = slotlist.end();
            slot* lastslot = LASTOBJ.back();
            while(&*spot != lastslot){
                spot--;
            }
            for(list<slot*>::iterator refspot = LASTOBJ.end(); refspot != LASTOBJ.begin(); --refspot){
                if(*refspot == lastslot){
                    LASTOBJ.erase(refspot);
                }
            }
            slotlist.erase(spot);
        }else if (word.compare("delprev") == 0){
           list<slot>::iterator spot = slotlist.end();
           list<slot*>::iterator prevspot = LASTOBJ.begin();
           advance(prevspot, LASTOBJ.size() - 2);
            slot* lastslot = *prevspot;
            while(&*spot != lastslot){
                spot--;
            }
            for(list<slot*>::iterator refspot = LASTOBJ.end(); refspot != LASTOBJ.begin(); --refspot){
                if(*refspot == lastslot){
                    LASTOBJ.erase(refspot);
                }
            }
            slotlist.erase(spot);
        }else if (word.compare("clean") == 0){
            //backref = currentobj.back();
            //for(int i)
            //list<slot>::iterator spot = slotlist.end();
            //for(int i = currentobj.size(); i > 1; --i ){
            //  if(&*spot != backref)
        //         slotlist.erase(spot);
        // }
        }else if (word.compare("this") == 0){
            currentlist.back()->slotlist.push_back(*this);
            LASTOBJ.push_back(&currentlist.back()->slotlist.back());
        }else if (word.compare("defmacro") == 0){
            //tempcode = macromap[currentobj.back()->code] + " " + tempcode;
            string macrostring = LASTOBJ.back()->code;
            string macrostring2 = macrostring;
            size_t macrosize = last_whitespace(macrostring);
            if(macrosize == string::npos)
                macromap[macrostring] = macrostring;
            else
                macromap[macrostring.erase(0, macrosize + 1)] = macrostring2.erase(macrosize, macrostring2.length());
        }else if (word.compare("dprevmacro") == 0){
            string macromeaning = prev_obj().code;
            string macroname = LASTOBJ.back()->code;
            macromap[macroname] = macromeaning;
            //Massive math block i means long int, d is double
        }else if (word.compare("+d") == 0){
            double double_contents = strtod(LASTOBJ.back()->code.c_str(), NULL) + strtod(prev_obj().code.c_str(), NULL);
            inset_obj(to_string(double_contents));
        }else if (word.compare("+i") == 0){
            long int long_contents = strtol(LASTOBJ.back()->code.c_str(), NULL, 0) + strtol(prev_obj().code.c_str(), NULL, 0);
            inset_obj(to_string(long_contents));
        }else if (word.compare("-d") == 0){
            double double_contents = strtod(LASTOBJ.back()->code.c_str(), NULL) - strtod(prev_obj().code.c_str(), NULL);
            inset_obj(to_string(double_contents));
        }else if (word.compare("-i") == 0){
            long int long_contents = strtol(LASTOBJ.back()->code.c_str(), NULL, 0) - strtol(prev_obj().code.c_str(), NULL, 0);
            inset_obj(to_string(long_contents));
        }else if (word.compare("*d") == 0){
            double double_contents = strtod(LASTOBJ.back()->code.c_str(), NULL) * strtod(prev_obj().code.c_str(), NULL);
            inset_obj(to_string(double_contents));
        }else if (word.compare("*i") == 0){
            long int long_contents = strtol(LASTOBJ.back()->code.c_str(), NULL, 0) * strtol(prev_obj().code.c_str(), NULL, 0);
            inset_obj(to_string(long_contents));
        }else if (word.compare("/d") == 0){
            double double_contents = strtod(LASTOBJ.back()->code.c_str(), NULL) / strtod(prev_obj().code.c_str(), NULL);
            inset_obj(to_string(double_contents));
        }else if (word.compare("/i") == 0){
            long int long_contents = strtol(LASTOBJ.back()->code.c_str(), NULL, 0) / strtol(prev_obj().code.c_str(), NULL, 0);
            inset_obj(to_string(long_contents));
        //Math block close
        }else if (word.compare("!") == 0){
            if(LASTOBJ.back()->code.compare("0") == 0){
                LASTOBJ.back()->code = "1";
            }else{
                LASTOBJ.back()->code = "0";
            }
        }else if (word.compare(">") == 0){
           if(atof(LASTOBJ.back()->code.c_str())>atof(prev_obj().code.c_str())){
                inset_obj("1");
            }else{
                inset_obj("0");
            }
        }else if (word.compare("<") == 0){
           if(atof(LASTOBJ.back()->code.c_str())<atof(prev_obj().code.c_str())){
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
            //if string
            if(firstchar == '\"'){
                 string_maker(tempcode, firstchar);
            }else{
                code_maker(tempcode, to_end);
        }
    }
}



int main()
{
    double d = strtol("90.3", NULL, 0);
    //cout << d;
    slot a;
    a.slotset();
    a.set_code(" 3.0 2.0 2.0 /i print");
    a.eval();
   // cout << a.slotlist.size();
    //cout << a.slotlist.size();

    //cout <<     a.slotlist.size();
    //a.set_code("\"a print\" eval");
    /*
    slot* b = &a;
    slot c;
    c.set_code("\"a print\" eval");
    b->slotlist.push_back(c);
    b->slotlist.back().eval();
*/
}
