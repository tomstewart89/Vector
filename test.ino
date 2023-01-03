/*
 *  Test ) = {};tax supported by vector.h
 *  
 *  This file is part of Simple vector library for Arduino ESP boards: https://github.com/BojanJurca/Cplusplus-vectors-for-Arduino
 *  
 *  Bojan Jurca, December 25, 2022
 *  
 */


#include "vector.h"


void setup () {
  Serial.begin (115200);

  // vector constructors
  // -------------------

    // definition of a vector
    vector<int> vectA;
    // error checking: there is no reason why this would not succeed

    // vector constructor with brace enclosed initializer list
    vector<String> vectB = { "one", "two", "tree" };
    // error checking: if there is not enough heap memory, created vector is empty: vectB.empty() or vectB.size () == 0
    
    // vector constructor with defined increment number of elements to reduce frequent resizes
    vector<char> vectC (10);
    // error checking: there is no reason why this would not succeed since the vector is not incrementing its storage just yet

    // vector initialized with copy-constructor
    vector<String> vectD = vectB;
    // error checking: if there is not enough heap memory, created vector is empty: vectD.empty () or vectD.size () == 0


  // vector assignment
  // -----------------

    // vector assignment with brace enclosed initializer list
    vectD = { "five", "six" };
    // error checking: if there is not enough heap memory, vector is empty: vectD.empty () or vectD.size () == 0
    vectD = {};
    // error checking: there is no reason why this would not succeed

    // vector assignement from another vector
    vectD = vectB;
    // error checking: if there is not enough heap memory vector is empty: vectD.empty () or vectD.size () == 0
    //                 but if vectB is empty too then the assigment always succeeds


  // vector comparison
  // -----------------

    Serial.println ( (vectD == vectB) ? "vectors are equal" : "vectors are different");


  // iterating through vector elements
  // ---------------------------------

    vectC = { 'a', 'b', 'c' };

    // with [] operator
    for (int i = 0; i < vectD.size (); i++)
      Serial.println (vectD [i]);

    // with "at" member function
    for (int i = 0; i < vectD.size (); i++)
      Serial.println (vectD.at (i));

    // for each, with an iterator
    for (auto element: vectC)
      Serial.println (element); 


  // inserting new elements in the vector
  // ------------------------------------

    // push_back
    for (int i = 0; i < 10; i++)
      if (vectA.push_back (i) == false)
        ; // <- error handling here
    // alternativelly you can reserve space for 10 elements in advance (vectA.reserve (10)) and than push_back will succeed (at least for simple vector types)

    // push_front - this is not supported by standard C++ vectors
    if (vectA.push_front (-1) == false)
      ; // <- error handling here

    // insert
    if (vectA.insert (3, -3) == false)
      ; // <- error handling here


  // deleting elements from the vector
  // ---------------------------------

    // pop_back
    vectA.pop_back ();
    // error checking: there is no need to check although pop_back would return false if vector is already empty

    // pop_front - this is not supported by standard C++ vectors
    vectA.pop_front ();
    // error checking: there is no need to check although pop_front would return false if vector is already empty

    // erase
    vectA.erase (vectA.size () - 1);
    // error checking: there is no need to check although erase would return false if element doesn't exist


  // find an element in the vector
  // -----------------------------

  vector<String> vectF = {"one", "two", "tree", "four", "five"};
  Serial.printf ("Element two found on position %i\n", vectF.find ("two"));
  vectF.pop_front ();
  Serial.printf ("Element two found on position %i\n", vectF.find ("two"));


  // performance of vectors 
  // ----------------------

    // for_each compared to for each iterator and [] operator

    vector<int> vectE (20000);
    for (int i = 0; i < 20000; i++)
      vectE.push_back (i);

    unsigned long sum = 0;
    unsigned long startMillis = millis ();
    for (auto element: vectE)
      sum += element;
    unsigned long endMillis = millis ();
    Serial.println (sum);
    Serial.printf ("Looping through %i elements with for each iterator took %lu ms\n", vectE.size (), endMillis - startMillis); // 1 ms

    sum = 0;
    startMillis = millis ();
    for (int i = 0; i < vectE.size (); i++)
      sum += vectE [i];
    endMillis = millis ();
    Serial.println (sum);
    Serial.printf ("Looping through %i elements with [] operator took %lu ms\n", vectE.size (), endMillis - startMillis); // 1 ms

    // circular queue implemented with C++ vector compared to circular queue implemented with C array
    
    vectE.clear ();
    vectE.reserve (17);
   
    startMillis = millis ();
    for (long i = 0; i < 1000000; i++) {
      vectE.push_back (i);                     // add 1 element to the end
      if (vectE.size () > 16) vectE.erase (0); // delete 1 element from the beginning
    }
    endMillis = millis ();
    Serial.printf ("Inserting 1.000.000 elements in circular queue implemented with vector took %lu ms, circular queue content:\n", endMillis - startMillis); // 461 ms
    for (byte i = 0; i < vectE.size (); i++)
      Serial.printf ("   %i\n", vectE [i]);

    int queue [16];
    char b = 0;
    char e = -1;
    startMillis = millis ();
    for (long i = 0; i < 1000000; i++) {
      e = (e + 1) & 0b00001111; // 15
      queue [e] = i;
      if (e == b) b = (b + 1) & 0b00001111; // 15
    }
    endMillis = millis ();
    Serial.printf ("Inserting 1.000.000 elements in circular queue implemented with array of integers took %lu ms, circular queue content:\n", endMillis - startMillis); // 46 ms
    byte i = (b - 1) & 0b00001111;
    do {
      i = (i + 1) & 0b00001111; // 15
      Serial.printf ("   %i\n", queue [i]);
    } while (i != e);

  
  // storage
  // -------

  vector<unsigned long> vectG (100);
  while (true) {
    if (!vectG.push_back ( 2022 )) {
      Serial.printf ("Out of memory - no more than %i elements could be placed into vector\n", vectG.size ());
      break;
    }
  }
  
}


void loop () { 
  
}
