## Data Structures and Programming Assignment: An Undoable List

In this assingment, I implemented the operations defined in the Stringlist.h file which undo changes made to a list of strings. 

An example of how it works: 

#include "Stringlist.h"
#include <iostream>

using namespace std;

int main() {
    Stringlist lst;
    cout << lst << endl; // {}

    lst.insert_back("one");
    lst.insert_back("two");
    lst.insert_back("three");
    cout << lst << endl; // {"one", "two", "three"}

    lst.undo();
    cout << lst << endl; // {"one", "two"}

    lst.undo();
    cout << lst << endl; // {"one"}

    lst.undo();
    cout << lst << endl; // {}
}
```

