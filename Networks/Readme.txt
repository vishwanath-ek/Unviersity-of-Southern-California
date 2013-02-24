Name : Vishwanath Eswarakrishnan
USC-ID : 9712382632


******************* WHAT I HAVE DONE IN THE PROJECT ********************

		---- ALL CODING IS DONE IN C++ language ----

The assignment consists of 5 files 'warehouse.cpp', 'store1.cpp', 'store2.cpp', 'store3.cpp' and 'store4.cpp'. Together they perform the following functions :

Each store has a set of inventories present in the store that is the number of cameras, laptops and printers ( represented 'StoreNumber.txt' file ). Every store reads this data and sends it to the warehouse. The warehouse inturn waits for all the four stores, for their data and after received, calculates the sum of all these and stores it ( fork() implemented in warehouse ). It checks for any negative sum and if present , it makes it positive and others it makes the sum zero and all these quantities are represented together as the truck vector. Thus, the truck vector consists of the summations of corresponding inventories present in each store. 

This is the END OF PHASE 1.

In PHASE 2-PART I, the truck vector is sent to store 1, which updates the inventories it has and also updates the truck vector (i.e., compares each item it has in iventories and takes item from the truck, if available and also puts items into the truck if in excess. Here takes away implies subtracts from truck and puts item meaning it adds). After this, the updated truck vector is sent to store 2 and store 2 respectively performs the same as above and sends the updated truck vector to store 3 and which inturn does the same and passes to store 4. Store 4 also repeats the same and later, sends the updated truck vector to store 1 which marks the start of Part 2 of Phase 2.


In PHASE 2-PART II, the store 4 sends the updated truck vector to store 1, and the above phase 2-part I repeats all over except , after the truck vector reaches store 4, the inventories is updated and so is the truck vector and the truck vector is sent back to the warehouse. The warehouse receives it.

This marks the END OF PHASE 2.


********************** Explanation of coded files *************************

There are a total of 6 files including the makefile viz. 'warehouse.cpp', 'store1.cpp', 'store2.cpp', 'store3.cpp'and 'store4.cpp'. The funcitons of each of these are explained as follows :



- warehouse.cpp :
  
It accepts all the inventories ( a structure object used for representing inventories ) that each store has using Stream socket. Each store is entertained by a child process created by 'fork()' in the warehouse program. After all the data has been received and summed up, the warehouse sents a truck vector ( again a structure object ) which consists of the updated sum of inventories to store 1. This is done using a Datagram socket. (READ PHASE 1-PART I ABOVE, FOR MORE EXPLANATION ON UPDATION).
The warehouse then waits till it receives data from store 4. 

FOR MORE DETAILED EXPLANATION, REFER COMMENTS IN file 'warehouse.cpp'.




-store1.cpp :

It sends its inventory information to the warehouse by using a structure object, through a Stream socket.
 
It accepts the truck vector from the warehouse using a Datagram socket and updates its inventories, also the truck vector. The updated truck vector is sent to store 2 and it waits till it receives data from store 4 after which it again forwards the truck vector to store 2 and exits.

The updation process is done by checking, value of each inventory category in store with the corresponding inventory value in the truck vector. If the value of current inventory in store is greater than zero, it adds all to the corresponding truck vector category value , thus making the inventory of store zero. If the value of current inventory in store is less than zero, it checks the truck vector for a corresponding category value to be greater than zero. If it is greater than zero then it updates the truck vector to be the difference of current inventory category value of store from the truck vector category value, else it does nothing. The difference can be zero but not less than zero.

FOR MORE DETAILED EXPLANATION, REFER COMMENTS IN file 'store1.cpp'.



-store2.cpp :

It sends its inventory information to the warehouse by using a structure object, through a Stream socket.

It accepts the truck vector from store 1 using a Datagram socket and then updates the the truck vector and its inventories as in store1.cpp . It sends the updated truck vector to store 3 after which it waits for store 1 again to send data. After receiving this data it exits.

FOR MORE DETAILED EXPLANATION, REFER COMMENTS IN file 'store2.cpp'.   
  


-store3.cpp :

It sends its inventory information to the warehouse by using a structure object, through a Stream socket.

It accepts the truck vector from store 2 using a Datagram socket and then updates the the truck vector and its inventories as in store1.cpp . It sends the updated truck vector to store 4 after which it waits for store 2 again to send data. After receiving this data it exits.

FOR MORE DETAILED EXPLANATION, REFER COMMENTS IN file 'store3.cpp'.



-store4.cpp :

It sends its inventory information to the warehouse by using a structure object, through a Stream socket.

It accepts the truck vector from store 3 using a Datagram socket and then updates the the truck vector and its inventories as in store1.cpp . It sends the updated truck vector to store 1 after which it waits for store 3 again to send data. After receiving this data, it send this data to the warehouse, and exits.

FOR MORE DETAILED EXPLANATION, REFER COMMENTS IN file 'store4.cpp'.



-Makefile.mak :

This file contains the code which compiles all the '.cpp' files after typing 'make' and hitting return in the terminal. Each line in this file consists of the statement for compilation and a set of dependencies, which should be fulfilled first. There is an intermediate byte code output with '.o' extension.
'make clean' is done in order to remove all the files generated from make. 




************************* INSTRUCTIONS FOR TA *********************************

---IMPORTANT------> THE FILES "MUST BE" EXECUTED IN THIS ORDER : warehouse.cpp ---then---> store1.cpp -> store2.cpp -> store3.cpp -> store4.cpp 

*NOTE :

The input files are i.e., Store-1.txt, Store-2.txt, Store-3.txt and Store-4.txt. These have the data in the format below -

(BEGIN)
Cameras X
Laptops Y
Printers Z
(END)									

where : 'X','Y' and 'Z' are integers.



In NUNKI, please follow the instructions below :

1.Go to the directory where the files are placed using the change directory (cd) command (note that all files ought to be placed in the same directory).


*2.Type to rename file 'Makefile.mak' to 'Makefile' as follows:

  	 mv Makefile.mak Makefile
   
   then press return to execute the above statement.           

3.Type 'make' and press return.

4.Execute each file by typing the following and hitting return one after the other, in this order, in separate windows :

  ./warehouse
  
  ./store1
   
  ./store2
 
  ./store3

  ./store4

5. After the output is seen, please make sure to type 'make clean' and press return to clean all files generated by make command.   
    




**************** FORMAT OF MESSAGES EXCHANGED ******************

All the messages exhanged are object of structure called 'Details' in the Code.

This structure consists of following integer members : numberOfCameras, numberOfLaptops and numberOfPrinters.

Everytime a message which is an object of the above structure is exchanged. And thus, at each step only one object is exchanged.
Before sending the pointer to object is type casted to character pointer format i.e., (char *)&pointerToStructureObject.
And while receiving also an object of structure is created, received in any unknown format i.e., (void *)&pointerToTheBuffer.



************* CONDITIONS IN WHICH THE PROJECT MAY FAIL *************

-> The project may fail is the port numbers used are not free at that point of time.

-> The project may fail if any of the above instructions are not followed.

NOTE : 'setsockopt' function not used and reusage of adresses not handled.



************* REUSED CODE ***************


NO CODE REUSED FROM BEEJ's. 
DYNAMIC ADDRESS ALLOCATION IN CLIENT : REUSED FROM www.stackoverflow.com


----->END OF FILE