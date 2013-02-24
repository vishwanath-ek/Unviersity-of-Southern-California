NAME : Vishwanath EswaraKrishnan
ID : 9712382632
EMAIL : eswarakr@usc.edu


**********Description of the program structure, may be required to understand the program*************

IMPORTANT-> THE INPUT TO THE PROGRAM "MUST BE" FROM A FILE NAMED "input.txt"


1. The program is written in C++ language with extensive use of vector containers.

2. The input is read in the format as given in the question from "input.txt" and fed into a matrix named 'inputBoard' which    is of size 8x8.

3. The following functions are called one after the other when required :

   - configureInput(): This function is called after the input is read from the file and we need to transform the input into a structure that we use, called 'Board'. 
Each 'Board' structure consists of members : 
                       * String 2D matrix named 'currentBoard' which stores our board configuration.
                       * Player name i.e., A or B
                       * Depth of the state 
                       * Value of the state
                       * Current and Previous position of the states
  
   - printBoard(Board): Takes a 'Board' object as parameter and prints the contents of 'currentBoard' in Board passed as a parameter.

   - utility(Board): Takes a 'Board' object as a parameter and then returns the same 'Board' with value updated to number of pieces of player A - number of pieces of player B , in the parameter Board.   

   - checkForConversion(Board): Checks if the Board configuration has a 'o' or '*' in the last row and converts each into corresponding KING of each player.

   - getForwardJumps(Board,int,int): Takes parameters as a 'Board' and two integers which represent positions of the piece and calculates all possible jumps in the forward direction i.e., towards decreasing row number.Refer to the comments in the program for detailed explanation.

   - getBackwardJumps(Board,int,int): Takes parameters as a 'Board' and two integers which represent positions of the piece and calculates all possible jumps in the backward direction i.e., towards increasing row number.Refer to the comments in the program for detailed explanations.

   - getForwardMoves(Board,int,int): Takes parameters as a 'Board' and two integers which represent positions of the piece and calculates all possible moves in the forward direction i.e., towards decreasing row number.Refer to the comments in the program for detailed explanations.

   - getBackwardMoves(Board,int,int): Takes parameters as a 'Board' and two integers which represent positions of the piece and calculates all possible moves in the backward direction i.e., towards increasing row number.Refer to the comments in the program for detailed explanations.

   - generateSuccessor(Board): Takes a 'Board' as parameter and returns all possible successors that are possible from this state, by calling the functions mentioned above and returns a list of these successors. Please refer the comments in program for detailed explanation.

   - maxCompare(Board,Board): It returns the 'Board' with the maximum value , comparing those in parameters.    

   - minComapre(Board,Board): It retruns the 'Board' with the minimum value , comparing those in parameters.
  
   - printDepth(int): Takes the depth of a 'Board' as parameter and returns equivalent amount of spaces.
   
   - alpha_beta_pruning(Board): This functions main purpose is to initiate the call to rest of the Algorithm ,i.e., maxValue.
  
   - maxValue(Board): Takes parameter as a 'Board' and performs the functionality as in the algorithm.Please refer to comments for more explanation.

   - minValue(Board): Takes parameter as a 'Board' and performs the functionality as in the algorithm.Please refer to comments in the program for more explanation.

  

*********** Compiling the program, on ALUDRA ***************

1. Install putty and connect to "aludra.usc.edu" with port number - 22.Connection type is SFTP.

2. Enter the ID and password to login into the machine.

3. Upload yor file aludra using either 'FileZilla', 'notepad++' or by any other means.

4. Type the following command for compilation :  
	
	g++ -o aa AI.cpp

   'aa' -> not needed, you can write any other or leave it.  




*********** Executing the program, on ALUDRA ***************

1. Follow the connection and uploading steps from above.


2. If the temporary file name is 'aa' type -
   
 	 ./aa for execution of the program.


3. If nothing is written type -

 	./FILENAME



-------------->END OF FILE