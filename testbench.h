#ifndef TESTBENCH_H
#define TESTBENCH_H

/** This function is used to test the linked list. It's source should not be
  modified for any reason.
  
  This function will read the file /sd/tests/dll.test from the SD card, and run
  the test trace contained therein. This trace consist of single lines of
  commands corresponding to linked list operations, according to the following 
  format:
  
    insertHead       ih <num>
    insertEnd        it <num>
    insertAfter      ia <num>
    insertBefore     ib <num>
    deleteBackward   db <expect>
    deleteForward    df <expect>
    getHead          h  <expect>
    getTail          t  <expect>
    getCurrent       c  <expect>
    getNext          n  <expect>
    getPrevious      p  <expect>
    getSize          s  <expect>
    Check list       e  <expect> <expect> ...
    Reset            r

  <num> indicates the number to insert. <expect> indicates the expected return
  value of the function.

  Check list (e) walks the list from head to tail using getHead() and getNext(),
  verifying each expected value along the way. It also checks that the list has
  the correct number of items as indicated in the command. **This will move the
  current pointer**.
  
  Reset (r) calls destroyList() to delete the current list, and reinitializes
  a new list by calling create_dlinkedlist().

  Additionally, these special instructions are available:
    
    Comment                         % The rest of this line is ignored
    insertAfter (expect fail)       iaf <num>
    insertBefore (expect fail)      ibf <num>
    
  Consider this example trace:

    % Sample trace
    ih 1
    ih 2
    ih 3
    iaf 42          % Attempt to insert 42; current pointer is invalid
    e  3 2 1
    
  This trace begins by inserting new head node repeatedly, building up the list:
    (1) -> (2, 1) -> (3, 2, 1)
  Since no command to set the current pointer has ever been used, the current
  pointer is in its initial state - invalid. Therefore, an attempt to use
  insertAfter(42) in this scenario should fail. This is tested using (iaf 42).
  Finally, the list is verified with the Check List command. All commands on
  this trace would succeed, and the final state of the list is (3, 2, 1).
  
  Output is both printed to the serial terminal (via USB) and saved to "outfile"
  on the SD card.
*/
void test_dlinkedlist(const char* tracefile, const char* outfile);

#endif