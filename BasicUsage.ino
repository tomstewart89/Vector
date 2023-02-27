#include <WiFi.h>

#include "vector.h"

void setup () {
    Serial.begin (115200);

    // constructor of vector from brace enclosed initializer list
    vector<int> v1 = { 400, 300, 600, 500 };

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

    // checking error of each function call:
    vector<int>::errorCode e = v3.push_back (900);
    if (e == vector<int>::OK)
        Serial.println ("push_back succeeded");
    else
        Serial.println ("push_back error " + String (e));
    // or checking errors of multiple operations:
    for (int i = 1000; i < 1100; i++)
        v3.push_back (i);
    if (v3.lastErrorCode == vector<int>::OK)
        Serial.println ("100 push_backs succeeded");
    else {
        Serial.println ("100 push_backs error " + String (v3.lastErrorCode));
        v3.clearLastErrorCode (); // clear lastErrorCode before next operations
    }
}

void loop () {

}
