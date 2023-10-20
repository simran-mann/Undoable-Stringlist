## Data Structures and Programming Assignment 2: An Undoable List

> In software, 'undo' erases our mistakes; in life, mistakes craft our story.
> Imagine the tales we'd lose with a real-life *Ctrl*+*Z*. -- *ChatGPT*

In this assignment, I was given a Stringlist class with operations as defined in the Stringlist.h file. 
My task is to add an undo feature to the string list class.

`Stringlist` has one unimplemented method:

```cpp
//
// Undoes the last operation that modified the list. Returns true if a
// change was undone.
//
// If there is nothing to undo, does nothing and returns false.
//
bool undo()
{
    cout << "Stringlist::undo: not yet implemented\n";
    return false;
}
```


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

