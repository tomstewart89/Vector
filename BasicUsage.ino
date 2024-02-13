#include <WiFi.h>

#include "vector.hpp"


void setup () {
    Serial.begin (115200);

    // constructor of vector from brace enclosed initializer list
    vector<int> v1 = { 400, 300, 600, 500 };
    vector<String> vs = { "one", "two", "tree" };

    // copy-constructor
    vector<int> v2 = v1;

    // vector assignment
    vector<int> v3;
    v3 = v2;

    // vector comparison
    Serial.println ( v1 == v3 ? "v1 and v3 are equal" : "v1 and v3 are different" );

    // insert new elements into vector
    v3.push_back (700);
    v3.push_front (100);  // please note that push_front not a STL C++ vector member function
    v3.insert (2, 200);   // insert element at selected position
        
    // find an element in the vector
    int position = v3.find (100);
    if (position >= 0)
        Serial.println ("100 found in v3 at position " + String (position));
    else
        Serial.println ("100 not found in v3");

    // delete element from the vector
    v3.erase (position);  // delete element at selected position
    v3.pop_back ();
    v3.pop_front ();      // please note that push_front not a STL C++ vector member function
    
    // scan vector elements with their position index:
    Serial.println ("--- v3 = ---");
    for (int i = 0; i < v3.size (); i++)
        Serial.println (v3 [i]);
    // or with an iterator:
    Serial.println ("--- v3 = ---");
    for (auto e: v3)
        Serial.println (e);

    // sort vector elements:
    v3.sort ();
    Serial.println ("--- sorted v3 = ---");
    for (auto e: v3)
        Serial.println (e);

    // checking error for each function call:
    int e = v3.push_back (900);
    if (e)
        Serial.printf ("push_back error: %s\n", v3.errorCodeText (e));
    else
        Serial.println ("push_back succeeded");
        
    // or checking errors of multiple operations:
    for (int i = 1000; i < 1100; i++)
        v3.push_back (i);
    if (v3.lastErrorCode) {
        Serial.printf ("100 push_backs error: %s\n",  v3.errorCodeText (v3.lastErrorCode));
        v3.clearLastErrorCode (); // clear lastErrorCode before next operations
    } else
        Serial.println ("100 push_backs succeeded");

    // handle possible problems of vector of complex data types like Strings
    vector<int> vi = { 1, 2, 3 };
    if (vi.lastErrorCode) {
        Serial.printf ("failed to initialize the vector\n"); // What can go wrong? Controller may not have enough free memory to put all the elements into the vector.
    } else {
        int f = vi.find (2);
        if (f < 0) Serial.printf ("element not found\n"); // Well, the element 2 will in this case always be found at position 1, since vector initialization succeeded.
        else Serial.printf ("element found at position %i\n", f);
    }
        
    vector<String> vstring = { "one", "two", "tree" };
    if (vstring.lastErrorCode != vstring.OK) {
        Serial.printf ("failed to initialize the vector\n"); // What can go wrong? Controller may not have enough free memory to put all the elements into the vector.
    } else {
        int f = vstring.find ("two");
        switch (f) {
          case vstring.NOT_FOUND:   Serial.printf ("element not found\n"); // The element "two" should be found at possition 1 if there is no other error
                                    break;
          case vstring.BAD_ALLOC:   Serial.printf ("find failed, that doesn't mean that the element is not there\n"); // Creation of find parameter failed due to lack of memory so find couldn't even start searching
                                    break;
          default:                  Serial.printf ("element found at position %i\n", f);
                                    break;
        }
    }

    // sort vector of Strings
    vstring.sort ();
    Serial.println ("--- sorted vstring = ---");
    for (auto e: vstring)
        Serial.println (e);

    // find min (max) element of the vector
    Serial.println ("--- min_element, max_element ---");
    auto minElement = min_element (v3);
    if (minElement) // check if min element is found (if v3 is not empty)
        Serial.printf ("min element of v3 = %i\n", *minElement);
    auto maxElement = max_element (v3);
    if (maxElement) // check if max element is found (if v3 is not empty)
        Serial.printf ("max element of v3 = %i\n", *maxElement);
}

void loop () {

}
