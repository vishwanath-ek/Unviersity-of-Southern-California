#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

string inputBoard[8][8];// Stores the input
int inputDepth;// The Depth till which the moves are expanded

struct Board// Structure of each state
{
    string currentBoard[8][8];
    string player;
    int parenti;
    int parentj;
    int childi;
    int childj;
    int value;
    int depth;
};

vector<Board> generateSuccesor(Board);

Board configureInput()// This function takes the inputBoard matrix, which is the input and converts it into the BOARD format
{
    // Store input into a Temporary Board structure
    Board tempBoard;
    for(int i=0;i<8;i++)
     for(int j=0;j<8;j++)
      tempBoard.currentBoard[i][j]=inputBoard[i][j];

// All statements below are initializations
    tempBoard.player="A";
    tempBoard.parenti=0;
    tempBoard.parentj=0;
    tempBoard.childi=0;
    tempBoard.childj=0;
    tempBoard.value=0;
    tempBoard.depth=-1;

    return tempBoard;
}

void printBoard(Board x)// To print the board configuration
{
  for(int i=0;i<8;i++)
  {
    for(int j=0;j<8;j++)
      cout<<x.currentBoard[i][j]<<" ";
    cout<<"\n";
  }
}

Board utility(Board myBoard) // Calculates utility of the Board i.e., Number of pieces of player A - Number of pieces of player B
{
    int numberOfPiecesA=0;
    int numberOfPiecesB=0;

    for(int i=0;i<8;i++)
     for(int j=0;j<8;j++)
     {
         if(myBoard.currentBoard[i][j]=="o" || myBoard.currentBoard[i][j]=="k")
       {
         numberOfPiecesA++;
         if(myBoard.currentBoard[i][j]=="k")
         numberOfPiecesA++;
       }

         if(myBoard.currentBoard[i][j]=="*" || myBoard.currentBoard[i][j]=="K")
         {
             numberOfPiecesB++;
             if(myBoard.currentBoard[i][j]=="K")
             numberOfPiecesB++;
         }
     }

     if(numberOfPiecesB==0)// If player A wins value of Board should be +Infinity, i.e., +65535 in this case
     numberOfPiecesA=65535;

     if(numberOfPiecesA==0)// If player B wins value of Board should be -Infinity, i.e., -65535 in this case
     numberOfPiecesB=65535;

    myBoard.value=(numberOfPiecesA-numberOfPiecesB);// Utility definition as per the question

    return myBoard;
}

Board checkForConversion(Board myBoard)// Checks whether either player's piece has reached the opposie end and converts that into a KING
{
     for(int j=0;j<8;j++)
      if(myBoard.currentBoard[0][j]=="o")
       myBoard.currentBoard[0][j]="k";

     for(int j=0;j<8;j++)
      if(myBoard.currentBoard[7][j]=="*")
       myBoard.currentBoard[7][j]="K";

    return myBoard;
}

vector<Board> getForwardJumps(Board myBoard,int i,int j)// Gets all the possible FORWARD JUMPS possible for Player A as well as Player B, as 'K' can perform forward Jumps
{
    vector<Board> tempVector;

    // ALL THE IF STATEMENTS BELOW ARE CASES for the piece positions
    if(i==0 || i==1)
     {
       myBoard=checkForConversion(myBoard);
      return tempVector;
     }

//Cases for Player A's piece below

    if(myBoard.currentBoard[i][j]=="o" || myBoard.currentBoard[i][j]=="k")
    {
        if(j==0||j==1)
            if((myBoard.currentBoard[i-1][j+1]=="*" || myBoard.currentBoard[i-1][j+1]=="K") && myBoard.currentBoard[i-2][j+2]==".")
             {
                 myBoard.currentBoard[i-1][j+1]=".";
                 myBoard.currentBoard[i-2][j+2]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i-2;myBoard.childj=j+2;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
                 return tempVector;
             }
             else
                return tempVector;


        if(j==7 || j==6)
            if((myBoard.currentBoard[i-1][j-1]=="*" || myBoard.currentBoard[i-1][j-1]=="K") && myBoard.currentBoard[i-2][j-2]==".")
             {
                 myBoard.currentBoard[i-1][j-1]=".";
                 myBoard.currentBoard[i-2][j-2]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i-2;myBoard.childj=j-2;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
                return tempVector;
             }
            else
                return tempVector;



        if((myBoard.currentBoard[i-1][j+1]=="*" || myBoard.currentBoard[i-1][j+1]=="K") && myBoard.currentBoard[i-2][j+2]==".")
             {
                 Board temp=myBoard;
                 temp.currentBoard[i-1][j+1]=".";
                 temp.currentBoard[i-2][j+2]=myBoard.currentBoard[i][j];
                 temp.currentBoard[i][j]=".";
                 temp.childi=i-2;temp.childj=j+2;
                 temp.parenti=i;temp.parentj=j;
                 temp=checkForConversion(temp);
                 tempVector.push_back(temp);
             }


        if((myBoard.currentBoard[i-1][j-1]=="*" || myBoard.currentBoard[i-1][j-1]=="K") && myBoard.currentBoard[i-2][j-2]==".")
             {
                 myBoard.currentBoard[i-1][j-1]=".";
                 myBoard.currentBoard[i-2][j-2]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i-2;myBoard.childj=j-2;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
             }
        return tempVector;
    }

     // Cases for Player B's piece i.e., 'K' below

        if(j==0||j==1)
            if((myBoard.currentBoard[i-1][j+1]=="o" || myBoard.currentBoard[i-1][j+1]=="k") && myBoard.currentBoard[i-2][j+2]==".")
             {
                 myBoard.currentBoard[i-1][j+1]=".";
                 myBoard.currentBoard[i-2][j+2]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i-2;myBoard.childj=j+2;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
                return tempVector;
             }
             else
                return tempVector;



        if(j==7 || j==6)
            if((myBoard.currentBoard[i-1][j-1]=="o" || myBoard.currentBoard[i-1][j-1]=="k") && myBoard.currentBoard[i-2][j-2]==".")
             {
                 myBoard.currentBoard[i-1][j-1]=".";
                 myBoard.currentBoard[i-2][j-2]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i-2;myBoard.childj=j-2;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
                 return tempVector;
             }
            else
                return tempVector;



        if((myBoard.currentBoard[i-1][j+1]=="o" || myBoard.currentBoard[i-1][j+1]=="k") && myBoard.currentBoard[i-2][j+2]==".")
             {
                 Board temp=myBoard;
                 temp.currentBoard[i-1][j+1]=".";
                 temp.currentBoard[i-2][j+2]=myBoard.currentBoard[i][j];
                 temp.currentBoard[i][j]=".";
                 temp.childi=i-2;temp.childj=j+2;
                 temp.parenti=i;temp.parentj=j;
                 temp=checkForConversion(temp);
                 tempVector.push_back(temp);
             }


        if((myBoard.currentBoard[i-1][j-1]=="o" || myBoard.currentBoard[i-1][j-1]=="k") && myBoard.currentBoard[i-2][j-2]==".")
             {
                 myBoard.currentBoard[i-1][j-1]=".";
                 myBoard.currentBoard[i-2][j-2]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i-2;myBoard.childj=j-2;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
             }
        return tempVector;
}

vector<Board> getBackwardJumps(Board myBoard,int i,int j)// Gets all the possible BACKWARD JUMPS possible for Player B as well as Player A, as 'k' can perform backward Jumps
{
     vector<Board> tempVector;

     // ALL THE IF STATEMENTS BELOW ARE CASES for the piece positions
    if(i==6 || i==7)
     {
      myBoard=checkForConversion(myBoard);
      return tempVector;
     }

     //Cases for Player B's piece positions below

    if(myBoard.currentBoard[i][j]=="*" || myBoard.currentBoard[i][j]=="K")
    {

        if(j==0||j==1)
            if((myBoard.currentBoard[i+1][j+1]=="o" || myBoard.currentBoard[i+1][j+1]=="k") && myBoard.currentBoard[i+2][j+2]==".")
             {
                 myBoard.currentBoard[i+1][j+1]=".";
                 myBoard.currentBoard[i+2][j+2]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i+2;myBoard.childj=j+2;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
                 return tempVector;
             }
            else
                return tempVector;



        if(j==7 || j==6)
            if((myBoard.currentBoard[i+1][j-1]=="o" || myBoard.currentBoard[i+1][j-1]=="k") && myBoard.currentBoard[i+2][j-2]==".")
             {
                 myBoard.currentBoard[i+1][j-1]=".";
                 myBoard.currentBoard[i+2][j-2]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i+2;myBoard.childj=j-2;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                tempVector.push_back(myBoard);
                return tempVector;
             }
            else
                return tempVector;


        if((myBoard.currentBoard[i+1][j+1]=="o" || myBoard.currentBoard[i+1][j+1]=="k") && myBoard.currentBoard[i+2][j+2]==".")
             {
                 Board temp=myBoard;
                 temp.currentBoard[i+1][j+1]=".";
                 temp.currentBoard[i+2][j+2]=myBoard.currentBoard[i][j];
                 temp.currentBoard[i][j]=".";
                 temp.childi=i+2;temp.childj=j+2;
                 temp.parenti=i;temp.parentj=j;
                 temp=checkForConversion(temp);
                 tempVector.push_back(temp);
             }


        if((myBoard.currentBoard[i+1][j-1]=="o" || myBoard.currentBoard[i+1][j-1]=="k") && myBoard.currentBoard[i+2][j-2]==".")
             {
                 myBoard.currentBoard[i+1][j-1]=".";
                 myBoard.currentBoard[i+2][j-2]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i+2;myBoard.childj=j-2;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
             }
        return tempVector;
    }

        //Cases for Player A's piece i.e., 'k' positions below

        if(j==0||j==1)
            if((myBoard.currentBoard[i+1][j+1]=="*" || myBoard.currentBoard[i+1][j+1]=="K") && myBoard.currentBoard[i+2][j+2]==".")
             {
                 myBoard.currentBoard[i+1][j+1]=".";
                 myBoard.currentBoard[i+2][j+2]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i+2;myBoard.childj=j+2;
                 myBoard.parenti=i;myBoard.parentj=j;
                myBoard=checkForConversion(myBoard);
                tempVector.push_back(myBoard);
                return tempVector;
             }
            else
                return tempVector;


        if(j==7 || j==6)
            if((myBoard.currentBoard[i+1][j-1]=="*" || myBoard.currentBoard[i+1][j-1]=="K") && myBoard.currentBoard[i+2][j-2]==".")
             {
                 myBoard.currentBoard[i+1][j-1]=".";
                 myBoard.currentBoard[i+2][j-2]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i+2;myBoard.childj=j-2;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
                 return tempVector;
             }
            else
                return tempVector;



        if((myBoard.currentBoard[i+1][j+1]=="*" || myBoard.currentBoard[i+1][j+1]=="K") && myBoard.currentBoard[i+2][j+2]==".")
             {
                 Board temp=myBoard;
                 temp.currentBoard[i+1][j+1]=".";
                 temp.currentBoard[i+2][j+2]=myBoard.currentBoard[i][j];
                 temp.currentBoard[i][j]=".";
                 temp.childi=i+2;temp.childj=j+2;
                 temp.parenti=i;temp.parentj=j;
                 temp=checkForConversion(temp);
                 tempVector.push_back(temp);
             }


        if((myBoard.currentBoard[i+1][j-1]=="*" || myBoard.currentBoard[i+1][j-1]=="K") && myBoard.currentBoard[i+2][j-2]==".")
             {
                 myBoard.currentBoard[i+1][j-1]=".";
                 myBoard.currentBoard[i+2][j-2]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i+2;myBoard.childj=j-2;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
             }
        return tempVector;
}

vector<Board> getForwardMoves(Board myBoard,int i,int j)// Gets all forward Moves for Player A as well as Player B, 'K' can perform forward moves as well
{
    vector<Board> tempVector;
    // ALL THE IF STATEMENTS BELOW ARE CASES for the piece positions
    if(i==0)
     {
       myBoard=checkForConversion(myBoard);
      return tempVector;
     }
//Cases for Player A's piece
    if(myBoard.currentBoard[i][j]=="o" || myBoard.currentBoard[i][j]=="k")
    {
        if(j==0)
            if(myBoard.currentBoard[i-1][j+1]==".")
             {
                 myBoard.currentBoard[i-1][j+1]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i-1;myBoard.childj=j+1;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
                 return tempVector;
             }
             else
                return tempVector;


        if(j==7)
            if(myBoard.currentBoard[i-1][j-1]==".")
             {
                 myBoard.currentBoard[i-1][j-1]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i-1;myBoard.childj=j-1;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
                return tempVector;
             }
            else
                return tempVector;



        if(myBoard.currentBoard[i-1][j+1]==".")
             {
                 Board temp=myBoard;
                 temp.currentBoard[i-1][j+1]=myBoard.currentBoard[i][j];
                 temp.currentBoard[i][j]=".";
                 temp.childi=i-1;temp.childj=j+1;
                 temp.parenti=i;temp.parentj=j;
                 temp=checkForConversion(temp);
                 tempVector.push_back(temp);
             }


        if(myBoard.currentBoard[i-1][j-1]==".")
             {
                 myBoard.currentBoard[i-1][j-1]=myBoard.currentBoard[i][j];;
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i-1;myBoard.childj=j-1;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
             }
        return tempVector;
    }

//Cases for Player B's piece i.e.,'K' positions below
        if(j==0)
            if(myBoard.currentBoard[i-1][j+1]==".")
             {
                 myBoard.currentBoard[i-1][j+1]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i-1;myBoard.childj=j+1;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
                return tempVector;
             }
             else
                return tempVector;



        if(j==7)
            if(myBoard.currentBoard[i-1][j-1]==".")
             {
                 myBoard.currentBoard[i-1][j-1]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i-1;myBoard.childj=j-1;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);

                 return tempVector;
             }
            else
                return tempVector;



        if(myBoard.currentBoard[i-1][j+1]==".")
             {
                 Board temp=myBoard;
                 temp.currentBoard[i-1][j+1]=myBoard.currentBoard[i][j];
                 temp.currentBoard[i][j]=".";
                 temp.childi=i-1;temp.childj=j+1;
                 temp.parenti=i;temp.parentj=j;
                 temp=checkForConversion(temp);
                 tempVector.push_back(temp);
             }


        if(myBoard.currentBoard[i-1][j-1]==".")
             {
                 myBoard.currentBoard[i-1][j-1]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i-1;myBoard.childj=j-1;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
             }
        return tempVector;
}

vector<Board> getBackwardMoves(Board myBoard,int i,int j)// Gets all backward Moves for Player B as well as Player A, 'k' can perform backward moves as well
{
     vector<Board> tempVector;
     // ALL THE IF STATEMENTS BELOW ARE CASES for the piece positions
    if(i==7)
     {
         myBoard=checkForConversion(myBoard);
      return tempVector;
     }
   //Cases for Player B's piece positions below
    if(myBoard.currentBoard[i][j]=="*" || myBoard.currentBoard[i][j]=="K")
    {

        if(j==0)
            if(myBoard.currentBoard[i+1][j+1]==".")
             {
                 myBoard.currentBoard[i+1][j+1]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i+1;myBoard.childj=j+1;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
                 return tempVector;
             }
            else
                return tempVector;



        if(j==7)
            if(myBoard.currentBoard[i+1][j-1]==".")
             {
                 myBoard.currentBoard[i+1][j-1]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i+1;myBoard.childj=j-1;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
                return tempVector;
             }
            else
                return tempVector;


        if(myBoard.currentBoard[i+1][j+1]==".")
             {
                 Board temp=myBoard;
                 temp.currentBoard[i+1][j+1]=myBoard.currentBoard[i][j];
                 temp.currentBoard[i][j]=".";
                 temp.childi=i+1;temp.childj=j+1;
                 temp.parenti=i;temp.parentj=j;
                 temp=checkForConversion(temp);
                 tempVector.push_back(temp);
             }


        if(myBoard.currentBoard[i+1][j-1]==".")
             {
                 myBoard.currentBoard[i+1][j-1]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i+1;myBoard.childj=j-1;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
             }
        return tempVector;
    }

   //Cases for Player A's piece i.e., 'k' below
        if(j==0)
            if(myBoard.currentBoard[i+1][j+1]==".")
             {
                 myBoard.currentBoard[i+1][j+1]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i+1;myBoard.childj=j+1;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
                return tempVector;
             }
            else
                return tempVector;


        if(j==7)
            if(myBoard.currentBoard[i+1][j-1]==".")
             {
                 myBoard.currentBoard[i+1][j-1]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i+1;myBoard.childj=j-1;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
                 return tempVector;
             }
            else
                return tempVector;



        if(myBoard.currentBoard[i+1][j+1]==".")
             {
                 Board temp=myBoard;
                 temp.currentBoard[i+1][j+1]=myBoard.currentBoard[i][j];
                 temp.currentBoard[i][j]=".";
                 temp.childi=i+1;temp.childj=j+1;
                 temp.parenti=i;temp.parentj=j;
                 temp=checkForConversion(temp);
                 tempVector.push_back(temp);
             }


        if(myBoard.currentBoard[i+1][j-1]==".")
             {
                 myBoard.currentBoard[i+1][j-1]=myBoard.currentBoard[i][j];
                 myBoard.currentBoard[i][j]=".";
                 myBoard.childi=i+1;myBoard.childj=j-1;
                 myBoard.parenti=i;myBoard.parentj=j;
                 myBoard=checkForConversion(myBoard);
                 tempVector.push_back(myBoard);
             }

        return tempVector;
}


vector<Board> generateSuccessor(Board myBoard)// Calls different functions for generating the possible successor states
{
  vector<Board> successorBoard;// Stores all the successors generated
  vector<Board> tempVector;
  vector<Board> :: iterator x;
  successorBoard.clear();
  bool getForwardJumpsFlag=false;
  bool getBackwardJumpsFlag=false;

  if(myBoard.player=="A")
  {
      for(int i=0;i<8;i++)
       for(int j=0;j<8;j++)
        {
            tempVector.clear();
            if(myBoard.currentBoard[i][j]=="o" || myBoard.currentBoard[i][j]=="k")
            {
                tempVector=getForwardJumps(myBoard,i,j);// All successors are stored in tempVector after the call return
                for(x=tempVector.begin();x<tempVector.end();x++)// Each successor is stored inside successorBoard while iterating
                successorBoard.push_back(*x);
                if(!successorBoard.empty())// if successorBoard is not empty then set the flag true to identify whether there were any forwardJumps found
                 getForwardJumpsFlag=true;
            }

           tempVector.clear();
             if(myBoard.currentBoard[i][j]=="k")
            {
              tempVector=getBackwardJumps(myBoard,i,j);// All successors are stored in tempVector after the call return
              for(x=tempVector.begin();x<tempVector.end();x++)// Each successor is stored inside successorBoard while iterating
              successorBoard.push_back(*x);
              if(!successorBoard.empty())// if successorBoard is not empty then set the flag true to identify whether there were any forwardJumps found
               getBackwardJumpsFlag=true;
            }
        }

     if(successorBoard.empty() && (!getForwardJumpsFlag || !getBackwardJumpsFlag))// Check whether there were no forward jumps or backward jumps for Player A
    {
        for(int i=0;i<8;i++)
         for(int j=0;j<8;j++)
        {
          tempVector.clear();
            if(myBoard.currentBoard[i][j]=="o" || myBoard.currentBoard[i][j]=="k")// For player A's piece call getForwardMoves
            {
                tempVector=getForwardMoves(myBoard,i,j);
                for(x=tempVector.begin();x<tempVector.end();x++)
                successorBoard.push_back(*x);
            }

          tempVector.clear();
            if(myBoard.currentBoard[i][j]=="k")// calculate the backwardMoves if the piece is 'k'
            {
              tempVector=getBackwardMoves(myBoard,i,j);
              for(x=tempVector.begin();x<tempVector.end();x++)
              successorBoard.push_back(*x);
            }
        }
    }
}
 if(myBoard.player=="B")
  {
      for(int i=0;i<8;i++)
       for(int j=0;j<8;j++)
        {
            tempVector.clear();
            if(myBoard.currentBoard[i][j]=="*" || myBoard.currentBoard[i][j]=="K")// For player B's piece call getBackwardJumps
            {
                tempVector=getBackwardJumps(myBoard,i,j);
                for(x=tempVector.begin();x<tempVector.end();x++)
                successorBoard.push_back(*x);
                if(!successorBoard.empty())
                 getForwardJumpsFlag=true;
            }

            tempVector.clear();
            if(myBoard.currentBoard[i][j]=="K")// calculate the ForwardJumps if the piece is 'K'
            {
              tempVector=getForwardJumps(myBoard,i,j);
              for(x=tempVector.begin();x<tempVector.end();x++)
              successorBoard.push_back(*x);
              if(!successorBoard.empty())
                 getBackwardJumpsFlag=true;
            }
        }

     if(successorBoard.empty() && (!getForwardJumpsFlag || !getBackwardJumpsFlag))// Check whether there are no forward or backward Jumps for calculating moves
    {
        for(int i=0;i<8;i++)
         for(int j=0;j<8;j++)
        {
          tempVector.clear();
            if(myBoard.currentBoard[i][j]=="*" || myBoard.currentBoard[i][j]=="K")// calculate backwardMoves for player B pieces
            {
                tempVector=getBackwardMoves(myBoard,i,j);
                for(x=tempVector.begin();x<tempVector.end();x++)
                successorBoard.push_back(*x);
            }

          tempVector.clear();
            if(myBoard.currentBoard[i][j]=="K")// calculate ForwardMoves for 'K'
            {
              tempVector=getForwardMoves(myBoard,i,j);
              for(x=tempVector.begin();x<tempVector.end();x++)
              successorBoard.push_back(*x);
            }
        }
    }
  }

  vector <Board> :: iterator i;
  for(i=successorBoard.begin();i<successorBoard.end();i++)
   if((*i).player=="A")// Change the player from A to B in successors and vice-versa
    (*i).player="B";
   else
    (*i).player="A";

  return successorBoard;
}

Board maxCompare(Board x, Board y)// Compare the values and return the greatest
{
    if(x.value>y.value)
    return x;

   return y;
}

Board minCompare(Board x, Board y)// Compare the values and return the smallest
{
    if(x.value<y.value)
    return x;

   return y;
}

string printDepth(int depth)// Print Spaces corresponding to current depth
{
    string x="";
    for(int i=0;i<depth;i++)
      x=x+" ";
   return x;
}

Board maxValue(Board,int,int);

void alpha_beta_pruning(Board myBoard)// Function initiating the algorithm
{
   Board temp;
   temp=utility(myBoard);

   if(temp.value==65535)// Check whether the input Board is win-condition for A
    {
        cout<<"Heuristic value of the Board is "<<temp.value<<" , Player A wins the game !!!\n";
        return;
    }
    if(temp.value==-65535)// Check whether the input Board is a win-condition for B
    {
     cout<<"Heuristic value of the Board is "<<temp.value<<" , Player B wins the game !!!\n";
     return;
    }

   temp=maxValue(myBoard,-65535,65535);// Call to maxValue function as in the algorithm
   cout<<"\nExpansion Completed \n\nAnswer: Player A moves the piece at ("<<temp.parenti+1<<","<<temp.parentj+1<<") to ("<<temp.childi+1<<","<<temp.childj+1<<")"<<endl;
}

Board minValue(Board, int, int);

Board maxValue(Board myBoard,int alpha, int beta)
{
   Board tempResult=utility(myBoard);

   if(tempResult.value!=65535 && tempResult.value!=-65535)// Check for win-condition of either player
 {
   if(myBoard.depth==inputDepth)// Check for terminal States
   {
     Board temp = utility(myBoard);
     cout<<printDepth(inputDepth)<<"Depth "<<inputDepth<<": Heuristic value of the current board = "<<temp.value<<endl;
     return temp;
   }
    myBoard.depth++;

    myBoard.value=-65535;
    vector <Board> vectorOfBoards;

    if(myBoard.depth!=inputDepth)
    vectorOfBoards=generateSuccessor(myBoard);// Successors are generated and stored after the call in a vector
    else
     {
     Board temp = utility(myBoard);
     cout<<printDepth(inputDepth)<<"Depth "<<inputDepth<<": Heuristic value of the current board = "<<temp.value<<endl;
     return temp;
     }

    vector<Board> :: iterator i;
    Board tempBoard,v;
    v=myBoard;

    int tempCompare;
    bool prune=false;
    bool flagForPruningTest=false;
    bool printOnce=true;

    for(i=vectorOfBoards.begin();i<vectorOfBoards.end();i++)// for each successor a call to minValue is made
     {
         if(!prune)// Checking the flag condition for pruning
         {
            if((*i).depth!=inputDepth)
            cout<<printDepth((*i).depth)<<"Depth "<<(*i).depth<<": Player A moves the piece at ("<<(*i).parenti+1<<", "<<(*i).parentj+1<<") to ("<<(*i).childi+1<<", "<<(*i).childj+1<<")"<<endl;

            tempBoard=minValue(*i,alpha,beta);
            tempCompare=tempBoard.value;
            tempBoard=*i;
            tempBoard.value=tempCompare;
            v = maxCompare(v,tempBoard);
            alpha=max(alpha,v.value);// Update Alpha as maximum of values of successors
         }

        if(v.value>=beta)// Check prune condition
        {
         prune=true;
         if(flagForPruningTest)
          {
              if(printOnce)
                cout<<printDepth((*i).depth)<<"Depth "<<(*i).depth<<": Pruning Player A's moves: ";
              printOnce=false;
              cout<<" ("<<(*i).parenti+1<<", "<<(*i).parentj+1<<") to ("<<(*i).childi+1<<", "<<(*i).childj+1<<");";
              if(i==vectorOfBoards.end()-1)
          cout<<"Alpha = "<<alpha<<"; Beta = "<<beta<<endl;

          }
          flagForPruningTest=true;
         }
     }
    return v;
 }
   else
    return utility(myBoard);

}
Board minValue(Board myBoard,int alpha, int beta)// Similar to MaxValue except that this function is for Player B
{
  Board tempResult=utility(myBoard);

  if(tempResult.value!=65535 && tempResult.value!=-65535)
  {
    if(myBoard.depth==inputDepth)
   {
     Board temp = utility(myBoard);
     cout<<printDepth(inputDepth)<<"Depth "<<inputDepth<<": Heuristic value of the current board = "<<temp.value<<endl;
     return temp;
   }
    myBoard.depth++;

    myBoard.value=65535;
    vector <Board> vectorOfBoards;

    if(myBoard.depth!=inputDepth)
    vectorOfBoards=generateSuccessor(myBoard);
    else
     {
     Board temp = utility(myBoard);
     cout<<printDepth(inputDepth)<<"Depth "<<inputDepth<<": Heuristic value of the current board = "<<temp.value<<endl;
     return temp;
     }

    Board tempBoard,v;
    v=myBoard;
    int tempCompare;
    bool prune=false;
    bool flagForPruningTest=false;
    bool printOnce=true;
    bool printAlphaBetaValues=true;

    vector<Board> :: iterator i;

    for(i=vectorOfBoards.begin();i<vectorOfBoards.end();i++)
     {
         if(!prune)
        {
         if((*i).depth!=inputDepth)
         cout<<printDepth((*i).depth)<<"Depth "<<(*i).depth<<": Player B moves the piece at ("<<(*i).parenti+1<<", "<<(*i).parentj+1<<") to ("<<(*i).childi+1<<", "<<(*i).childj+1<<")"<<endl;

         tempBoard=maxValue(*i,alpha,beta);
         tempCompare=tempBoard.value;
         tempBoard=*i;
         tempBoard.value=tempCompare;
         v = minCompare(v,tempBoard);
         beta=min(beta,v.value);// Update beta to minimum of the successor values
        }
         if(v.value<=alpha)// Check pruning condition
         {
            prune=true;
            if(flagForPruningTest)
          {
              if(printOnce)
                cout<<printDepth((*i).depth)<<"Depth "<<(*i).depth<<": Pruning Player B's moves: ";
              printOnce=false;
              cout<<" ("<<(*i).parenti+1<<", "<<(*i).parentj+1<<") to ("<<(*i).childi+1<<", "<<(*i).childj+1<<");";

         if(i==vectorOfBoards.end()-1)
          cout<<"Alpha = "<<alpha<<"; Beta = "<<beta<<endl;
          }
          flagForPruningTest=true;
         }
     }
    return v;
  }
  else
    return utility(myBoard);
}

int main()
{
    // File Handling done below
    ifstream indata;
    string readLine;
    indata.open("input.txt");
    int counter=0;
    while(indata>>readLine)
    {
     counter++;
     if(counter != 9)
        for(int j=0;j<8;j++)
            inputBoard[counter-1][j]=readLine[j];
     else
       istringstream(readLine)>>inputDepth;
    }// END OF FILE HANDLING

    Board myBoard=configureInput();// Change the input configuration to the state as required by the program
    myBoard=checkForConversion(myBoard);// Check if either of the players Board has its piece in the last row and convert the piece to kings of respective players
    alpha_beta_pruning(myBoard);

    return 0;
}
