// Stringlist.h

/////////////////////////////////////////////////////////////////////////
//
// Student Info
// ------------
//
// Name : Simran Mann
// St.# : 301542808
// Email: sma308@sfu.ca
//
//
// Statement of Originality
// ------------------------
//
// All the code and comments below are my own original work. For any non-
// original work, I have provided citations in the comments with enough
// detail so that someone can see the exact source and extent of the
// borrowed work.
//
// In addition, I have not shared this work with anyone else, and I have
// not seen solutions from other students, tutors, websites, books,
// etc.
//
/////////////////////////////////////////////////////////////////////////

//
// Do not use any other #includes or #pragmas in this file.
//

#pragma once

#include <cassert>
#include <iostream>
#include <string>

using namespace std;

class Stringlist
{
    int cap;     // capacity
    string *arr; // array of strings
    int sz;      // size

    //
    // Helper function for throwing out_of_range exceptions.
    //
    void bounds_error(const string &s) const
    {
        throw out_of_range("Stringlist::" + s + " index out of bounds");
    }

    //
    // Helper function for checking index bounds.
    //
    void check_bounds(const string &s, int i) const
    {
        if (i < 0 || i >= sz)
        {
            bounds_error(s);
        }
    }

    //
    // Helper function for copying another array of strings.
    //
    void copy(const string *other)
    {
        for (int i = 0; i < sz; i++)
        {
            arr[i] = other[i];
        }
    }

    //
    // Helper function for checking capacity; doubles size of the underlying
    // array if necessary.
    //
    void check_capacity()
    {
        if (sz == cap)
        {
            cap *= 2;
            string *temp = new string[cap];
            for (int i = 0; i < sz; i++)
            {
                temp[i] = arr[i];
            }
            delete[] arr;
            arr = temp;
        }
    }

    struct Action // private struct that defines an undoable aciton
    {
        enum Actiontype
        {
            Insert,
            Remove,
            Set,
            Assign
        };
        Actiontype undoAction;
        // set default values for ind and value as they're not needed for every undo operation
        int ind = 0;
        string value = "";
        Stringlist *list = nullptr; // for undoing the assignment and remove_all operation
    };

    class LL // implement a doubly linked list inside the stringlist for the stack
    {
    private:
        struct Node
        {
            Action *act; // each node stores a ptr to an object of action type
            Node *next;
            Node *prev;
        };

        Node *head = nullptr;
        int len = 0;
        // removeFront_clear_version is a void private method used in the destructor
        // its different than the public removeFront method since it deletes
        // the memory allocated by the ptr to action object stored in each node
        void removeFront_clear_version()
        {
            if (is_empty())
            {
                return;
            }
            Node *to_remove = head;
            /*if there was an assignment operation done,
            need to delete the memory allocated by the new object
            of stringlist type when the action was pushed onto the struct*/
            if (head->act->undoAction == Action::Assign)
            {
                delete head->act->list;
            }
            delete head->act;

            if (head->next != nullptr)
            {
                head->next->prev = nullptr;
            }
            head = head->next;
            delete to_remove;
            len--;
        }

    public:
        LL() {}

        bool is_empty() const
        {
            return head == nullptr;
        }

        void insertFront(Action *new_action)
        {
            Node *new_node = new Node{new_action, head, nullptr};
            if (is_empty())
            {
                head = new_node;
            }
            else
            {
                head->prev = new_node;
                head = new_node;
            }
            len++;
        }

        Action *removeFront()
        {
            if (is_empty())
            {
                return nullptr;
            }
            Node *to_remove = head;
            Action *ret = head->act;
            if (head->next != nullptr)
            {
                head->next->prev = nullptr;
            }
            head = head->next;
            delete to_remove;
            len--;
            return ret;
        }

        void clear()
        {
            while (!is_empty())
            {
                removeFront_clear_version();
            }
        }
        int size()
        {
            return len;
        }
        ~LL()
        {
            clear();
        }
    };

    struct undoStack // implement a stack storing action objects using the LL structure
    {
        LL list;       // struct is an object of type a doubly linked list
        undoStack() {} // default constructor

        void push(Action *act)
        {
            list.insertFront(act); // insert the most recent action to the front of the linked list
        }

        Action *pop()
        {
            if (list.is_empty()) // if list is empty, there is nothing to pop
            {
                return nullptr;
            }
            // remove the most recent action stored at front of the list or "top" of the stack
            return list.removeFront();
        }

        bool is_empty()
        {
            return list.is_empty();
        }

        int length()
        {
            return list.size();
        }

        ~undoStack()
        {
            list.clear();
        }
    };

    undoStack stack; // initialize a stack inside the StringList class

    // the following private methods are defiend to be used inside the undo function
    // they are similar to the insert,remove, set, and assignment methods however they do NOT
    // push a new object of action type as undoing operations cannot be undone

    void undo_set(int index, string value)
    {
        check_bounds("set", index);
        arr[index] = value; // set the value at the index back to what it intially was
    }

    void undo_assign(Stringlist &other)
    {
        delete[] arr;
        cap = other.cap;
        sz = other.sz;
        arr = new string[cap];
        copy(other.arr);
    }

    void undo_insert(int index) // removes the recently inserted string
    {
        check_bounds("remove_at", index); /// is this needed?

        for (int i = index; i < sz - 1; i++) // loop is never entered?
        {
            arr[i] = arr[i + 1];
        }
        sz--;
    }

    void undo_remove(int index, const string &s) // inserts the recently removed string
    {                                            // add the string back to the list
        if (index < 0 || index > sz)
        { // allows insert at end, i == sz
            bounds_error("insert_before");
        }
        check_capacity();

        for (int i = sz; i > index; i--)
        {
            arr[i] = arr[i - 1];
        }
        arr[index] = s;
        sz++;
    }

public:
    //
    // Default constructor: makes an empty StringList.
    //
    Stringlist()
        : cap(10), arr(new string[cap]), sz(0)
    {
    }

    //
    // Copy constructor: makes a copy of the given StringList.
    //
    // Does *not* copy the undo stack, or any undo information from other.
    //
    Stringlist(const Stringlist &other)
        : cap(other.cap), arr(new string[cap]), sz(other.sz)
    {
        copy(other.arr);
    }

    //
    // destructor
    //
    ~Stringlist()
    {
        delete[] arr;
    }

    //
    // Assignment operator: makes a copy of the given StringList.
    //
    // undoable
    //
    // For undoing, when assigning different lists, the undo stack is NOT
    // copied:
    //
    //    lst1 = lst2; // lst1 undo stack is updated to be able to undo the
    //                 // assignment; lst1 does not copy lst2's stack
    //                 //
    //                 // lst2 is not change in any way
    //
    // Self-assignment is when you assign a list to itself:
    //
    //    lst1 = lst1;
    //
    // In this case, nothing happens to lst1. Both its string data and undo
    // stack are left as-is.
    //

    Stringlist &operator=(const Stringlist &other)
    {
        if (this != &other)
        {
            Action *act = new Action;
            act->undoAction = Action::Assign;

            // save a copy of the current state of the list so the operation can be undone
            act->list = new Stringlist(*this);

            stack.push(act);

            delete[] arr; // delete the current state as it has been added to the stack

            cap = other.capacity();
            arr = new string[cap];
            sz = other.size();
            copy(other.arr);
        }
        return *this;
    }

    //
    // Returns the number of strings in the list.
    //
    int size() const { return sz; }

    //
    // Returns true if the list is empty, false otherwise.
    //
    bool empty() const { return size() == 0; }

    //
    // Returns the capacity of the list, i.e. the size of the underlying array.
    //
    int capacity() const { return cap; }

    //
    // Returns the string at the given index.
    //
    string get(int index) const
    {
        check_bounds("get", index);
        return arr[index];
    }

    //
    // Returns the index of the first occurrence of s in the list, or -1 if s is
    // not in the lst.
    //
    int index_of(const string &s) const
    {
        for (int i = 0; i < sz; i++)
        {
            if (arr[i] == s)
            {

                return i;
            }
        }
        return -1;
    }

    //
    // Returns true if s is in the list, false otherwise.
    //
    bool contains(const string &s) const
    {
        return index_of(s) != -1;
    }

    //
    // Returns a string representation of the list.
    //
    string to_string() const
    {
        string result = "{";
        for (int i = 0; i < size(); i++)
        {
            if (i > 0)
            {
                result += ", ";
            }
            result += "\"" + get(i) + "\"";
        }
        return result + "}";
    }

    //
    // Sets the string at the given index.
    //
    // undoable
    //
    void set(int index, string value) 
    {
        string old_val = arr[index];
        check_bounds("set", index); //check if index is valid 
        arr[index] = value;
        Action *act = new Action;
        act->undoAction = Action::Set;
        act->ind = index;
        act->value = old_val; // save the string that was replaced so the operation can be undone
        stack.push(act);
    }

    //
    // Insert s before index; if necessary, the capacity of the underlying array
    // is doubled.
    //
    // undoable
    //
    void insert_before(int index, const string &s) // inverse is remove at(index)
    {
        if (index < 0 || index > sz)
        { // allows insert at end, i == sz
            bounds_error("insert_before");
        }
        check_capacity();

        for (int i = sz; i > index; i--)
        {
            arr[i] = arr[i - 1];
        }
        arr[index] = s;
        sz++;
        Action *act = new Action;
        act->undoAction = Action::Insert;
        act->ind = index;
        act->value = s;
        stack.push(act);
    }

    //
    // Appends s to the end of the list; if necessary, the capacity of the
    // underlying array is doubled.
    //
    // undoable
    //
    void insert_back(const string &s) // inverse is remove at(index of last node)
    {
        insert_before(size(), s);
    }

    //
    // Inserts s at the front of the list; if necessary, the capacity of the
    // underlying array is doubled.
    //
    // undoable
    //
    void insert_front(const string &s) // inverse is remove_at(0)
    {
        insert_before(0, s);
    }

    //
    // Removes the string at the given index; doesn't change the capacity.
    //
    // undoable
    //
    void remove_at(int index) // inverse is insert before(index)
    {

        check_bounds("remove_at", index);

        string val = arr[index]; // save the current word before removing it so it can be undone

        for (int i = index; i < sz - 1; i++)
        {
            arr[i] = arr[i + 1];
        }
        sz--;
        Action *act = new Action;
        act->undoAction = Action::Remove;
        act->ind = index;
        act->value = val;
        stack.push(act);
    }

    //
    // Removes all strings from the list; doesn't change the capacity.
    //
    // undoable
    //
    // void undo_remove_all(){

    //}

    void remove_all() // inverse is insert end for each string
    {
        if (sz <= 0)
        {
            return;
        }
        /*create an action object storing the current list before
        removing all items,so if the action is popped from the stack,
        the undo_assign function will set the list back to its
        original state before calling remove_all();*/
        Action *act = new Action;
        act->undoAction = Action::Assign;
        act->list = new Stringlist(*this);
        stack.push(act);

        while (sz > 0)
        {
            // remove the item at the given index wihtout pushing an action onto the struct.
            // since the action type is assign rather than remove, we are not going to
            // call remove_at() since it pushes the inverse onto the stack one by one.
            undo_insert(sz - 1);
            // undo_insert will remove at the given index wihtout pushing new action
            // items onto the stack, so we can pop one single action from the stack
            // that reassings the entire list.
        }
    }

    //
    // Removes the first occurrence of s in the list, and returns true. If s is
    // nowhere in the list, nothing is removed and false is returned.
    //
    // undoable
    //

    bool remove_first(const string &s) // removes fisrt occurence of the string s in the list
    {
        int index = index_of(s);
        if (index == -1) // word is not in the list
        {
            return false;
        }
        // action struct is created in the remove_at function so this operation can be undone
        remove_at(index);
        return true;
    }

    //
    // Undoes the last operation that modified the list. Returns true if a
    // change was undone.
    //
    // If there is nothing to undo, does nothing and returns false.
    //

    void print_lst()
    {
        for (int i = 0; i < sz; i++)
        {
            cout << arr[i] << endl;
        }
    }

    bool undo()
    {
        // cout << "Stringlist::undo: not yet implemented\n";
        if (!stack.is_empty())
        {
            Action *undo = stack.pop();

            switch (undo->undoAction)
            {
            case (Action::Insert): // undoing an insert
                undo_insert(undo->ind);
                break;
            case (Action::Remove): // undoing a removal 
                undo_remove(undo->ind, undo->value);
                break;
            case (Action::Set): //undoing set
                undo_set(undo->ind, undo->value);
                break;
            case (Action::Assign): //undoing assignment or remove_all
            {
                // the list object should be valid if an assignment operation is being undone
                assert(undo->list != nullptr); 
                undo_assign(*(undo->list));
                delete undo->list;
                break;
            }
            }
            delete undo; // delete the object of type act
            return true;
        }
        return false;
    }

}; // class Stringlist

//
// Prints list to in the format {"a", "b", "c"}.
//
ostream &operator<<(ostream &os, const Stringlist &lst)
{
    return os << lst.to_string();
}

//
// Returns true if the two lists are equal, false otherwise.
//
// Does *not* consider any undo information when comparing two Stringlists. All
// that matters is that they have the same strings in the same order.
//
bool operator==(const Stringlist &a, const Stringlist &b)
{
    if (a.size() != b.size())
    {
        return false;
    }
    for (int i = 0; i < a.size(); i++)
    {
        if (a.get(i) != b.get(i))
        {
            return false;
        }
    }
    return true;
}

//
// Returns true if the two lists are not equal, false otherwise.
//
// Does *not* consider any undo information when comparing two Stringlists.
//
bool operator!=(const Stringlist &a, const Stringlist &b)
{
    return !(a == b);
}
