#include <iostream>
#include <dirent.h>
#include <fstream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <cctype>
#include <algorithm>
#include <cstring>
#include <stdio.h>
#include <map>
#include <iterator>
using namespace std;

class Node {
public:
    string word = "";
    int position = 0;
    int docid = 0;
    Node* next = NULL;
};

map<string, int> document;

void remove_duplicates(std::vector<int> &v)
{
    auto end = v.end();
    for (auto it = v.begin(); it != end; ++it) {
        end = std::remove(it + 1, end, *it);
    }

    v.erase(end, v.end());
}

Node* read_clean_data(vector <string> stop_words){
    struct dirent *dir;

    DIR *dr = opendir("/home/mohammad/Desktop/data/");

    if (dr == NULL) {
        printf("Could not open current directory");
        return 0;
    }

    Node* head = NULL;
    Node* tail = NULL;
    int docid = 1;

    fstream fout;
    fout.open("Docid.csv", ios::out | ios::app);


    while ((dir = readdir(dr))) {
        if ( !strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..") )
        {
            // do nothing (straight logic)
        }
        else {

            fstream file;
            string word, t, q, filename;
            string path = "/home/mohammad/Desktop/data/";
            string file_name = path + dir->d_name;
            fout << file_name << ", ";
            filename = file_name;
            file.open(filename.c_str());

            document.insert(pair<string, int>(filename,docid));

            int position = 0;

            while (file >> word) {
                //clean text from punctuation.------------------------------------------
                for (int i = 0, len = word.size(); i < len; i++) {
                    if (ispunct(word[i])) {
                        word.erase(i--, 1);
                        len = word.size();
                    }
                }
                //-----------------------------------------------------------------------

                //check if is it a stop words.-------------------------------------------
                int flag = 0;
                for(int i = 0; i < stop_words.size(); i++) {
                    transform(stop_words[i].begin(), stop_words[i].end(), stop_words[i].begin(),[](unsigned char c) { return std::tolower(c); });
                    if (word == stop_words[i]) {
                        flag = 1;
                        break;
                    }
                }
                    //-----------------------------------------------------

                    if(flag == 0) {
                        Node *next_node = new Node();
                        if (head == NULL) {
                            Node *first = new Node();
                            first->docid = docid;
                            first->position = position;
                            position++;
                            transform(word.begin(), word.end(), word.begin(),[](unsigned char c){ return std::tolower(c); });
                            first->word = word;
                            first->next = next_node;
                            tail = first->next;
                            head = first;
                            next_node->next = NULL;
                        } else {
                            tail->next = next_node;
                            tail->word = word;
                            tail->position = position;
                            tail->docid = docid;
                            tail = next_node;
                            next_node->next = NULL;
                        }
                    }
                position++;
            }
            fout << docid << "\n";
            docid++;
        }
    }
    closedir(dr);

    return head;
}

vector <string> read_stop_words(){
    fstream file;
    string stop_words_path = "/home/mohammad/Desktop/data.txt";
    file.open(stop_words_path.c_str());
    string word;
    vector <string> stop_words;
    while (file >> word) {
        stop_words.push_back(word);
    }
    return stop_words;
}



vector<string> take_phrase(vector<string> stop_words){
    string phrase;
    cout<<"Please enter phrase to search: "<<endl;
    getline(cin, phrase);


    //split phrase to words
    vector <string> words;
    string word;
    for ( char ch : phrase){
        if (ch == ' '){

            for (int i = 0, len = word.size(); i < len; i++) {
                if (ispunct(word[i])) {
                    word.erase(i--, 1);
                    len = word.size();
                }
            }

                words.push_back(word);
                word = "";
        }

        else
            word = word + ch;
    }
    words.push_back(word);
    return words;
}


vector<string> query_phrase(Node* head, vector<string> phrase){
    Node* pointer = head;
    vector<int> exist;

    while(pointer->next != NULL) {
            if (pointer->word == phrase[0] && pointer->next->word == phrase[1] && pointer->docid == pointer->next->docid){
                exist.push_back(pointer->docid);
                pointer = pointer->next;
            }
            else{
                pointer = pointer->next;
            }
    }

    remove_duplicates(exist);
    map<string, int>::iterator itr;
    vector<string> paths;
    for (itr = document.begin(); itr != document.end(); ++itr) {
        for(int i : exist){
            if(itr->second == i)
                paths.push_back(itr->first);
        }
    }
        return paths;
}


int main(void) {
    vector <string> stop_words = read_stop_words();
    Node* head_inverted_index = read_clean_data(stop_words);
    vector<string> phrase = take_phrase(stop_words);
    vector<string> result = query_phrase(head_inverted_index, phrase);

    for(string i : result)
        cout<<i<<endl;



return 0;
}
