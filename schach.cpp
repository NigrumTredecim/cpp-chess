#include <iostream>
#include <fstream>
#include <cmath>
char input [5];
//input buffer may not be changed outside of "readinput()"
int inpstartrow;
int inpstartcol;
int inpendrow;
int inpendcol;
//piecedata buffer that can be built using "readpiecedata()" and will be constructed from the location buffer not the input buffer
int movingpieceid;
int movingpiececol;
int movingpiecetyp;
int targetpieceid;
int targetpiececol;
int targetpiecetyp;
int activeplayercol; //gets swapped around by main
int gamenotover;
int isfirstmove; //set for each piece in pieces [?] [3] by default gets removed by main
int enpassantpossible; //currently gets set by validatemove but should be moved to main as this may cause enpassant to become falsely possible due to a bug when checking for mate
int enpassantpossibleon;

int castlingqueensideallowedblack;
int castlingkingsideallowedblack;
int castlingqueensideallowedwhite;
int castlingkingsideallowedwhite;

int blackismate;
int whiteismate;

int i;
int movevalid;
//create array for the board
int board [8] [8] [2]; //were playing 3d chess now btw
//create and populate array for the pieces 
//the first int in the arrays is the colour and the second is the type
//1x is black
//2x is white
//x1 is pawn
//x2 is rook
//X3 is knight
//x4 is bishop
//x5 is queen
//x6 is king
//x7 is pawn promotion placeholder type
int pieces [33] [3] = {{0, 0, 0},
{1, 2, 1}, {1, 3, 1}, {1, 4, 1}, {1, 5, 1}, {1, 6, 1}, {1, 4, 1}, {1, 3, 1}, {1, 2, 1},
{1, 1, 1}, {1, 1, 1}, {1, 1, 1}, {1, 1, 1}, {1, 1, 1}, {1, 1, 1}, {1, 1, 1}, {1, 1, 1},
{2, 1, 1}, {2, 1, 1}, {2, 1, 1}, {2, 1, 1}, {2, 1, 1}, {2, 1, 1}, {2, 1, 1}, {2, 1, 1},
{2, 2, 1}, {2, 3, 1}, {2, 4, 1}, {2, 5, 1}, {2, 6, 1}, {2, 4, 1}, {2, 3, 1}, {2, 2, 1}};
void boardcopydimension(int sourcedimension, int targetdimension){
	int row = 0;
	int col = 0;
	while (row < 8){
		while (col < 8){
			board [row] [col] [targetdimension] = board [row] [col] [sourcedimension];
			++col;
		}
		++row;
		col = 0;
	}
	row = 0;
}
void dumplogicfieldtoshell(int dim){
	int horpos;
	int verpos;
	int num;
	horpos = 0;
	verpos = 0;
	num = 8;
	std::cout << "   A   B   C   D   E   F   G   H" << std::endl;
	while (verpos < 8){
		std::cout << num;
		--num;
		while (horpos < 8){
			std::cout << " ";
			if (2 == pieces [board [verpos] [horpos] [dim]] [0]){std::cout << "W";}
			if (1 == pieces [board [verpos] [horpos] [dim]] [0]){std::cout << "S";}
			if (0 == pieces [board [verpos] [horpos] [dim]] [0]){std::cout << "0";}
			std::cout << (pieces [board [verpos] [horpos] [dim]] [1]);
			std::cout << (pieces [board [verpos] [horpos] [dim]] [2]);
			horpos++;
		}
	std::cout << std::endl;
	horpos = 0;
	verpos++;
	}
	std::cout << "   A   B   C   D   E   F   G   H" << std::endl;
}
int readdebuginput(char inputchar){
	switch (inputchar){
		case 'A': return 0;
		case 'B': return 1;
		case 'C': return 2;
		case 'D': return 3;
		case 'E': return 4;
		case 'F': return 5;
		case 'G': return 6;
		case 'H': return 7;
		case 'a': return 0;
		case 'b': return 1;
		case 'c': return 2;
		case 'd': return 3;
		case 'e': return 4;
		case 'f': return 5;
		case 'g': return 6;
		case 'h': return 7;
		case '1': return 7;
		case '2': return 6;
		case '3': return 5;
		case '4': return 4;
		case '5': return 3;
		case '6': return 2;
		case '7': return 1;
		case '8': return 0;
	}
	return 8;
}
void debuginput(){ //read input from shell for debugging and testing
	std::cin >> input;
	inpstartcol = readdebuginput(input[0]);
	inpstartrow = readdebuginput(input[1]);
	inpendcol = readdebuginput(input[2]);
	inpendrow = readdebuginput(input[3]);
}
void readinput(){
	inpstartrow = 8; //set the coord variables to 8 to see invalid (missing) inputs easier
	inpstartcol = 8;
	inpendrow = 8;
	inpendcol = 8;
	while (inpstartrow == 8 || inpstartcol == 8 || inpendrow == 8 || inpendcol == 8){
		debuginput(); //comment this out when actual input has been implemented
	}
}
int getpieceid(int dim, int row, int col){return board [row] [col] [dim];}
int getpiececol(int id){return pieces [id] [0];}
int getpiecetype(int id){return pieces [id] [1];}
int getisfirstmove(int id){return pieces [id] [2];}
void readpiecedata(int dim, int startcol, int startrow, int endcol, int endrow){
	movingpieceid = board [startrow] [startcol] [0] ; //load the id of the piece to move
	movingpiececol = pieces [movingpieceid] [0] ; //copy the color of the selected piece into ram
	movingpiecetyp = pieces [movingpieceid] [1] ; //copy the type of the selected piece into ram
	isfirstmove = pieces [movingpieceid] [2] ; //copies the value that checks if the piece has been moved beforehand
	targetpieceid = board [endrow] [endcol] [dim] ; //read the target field
	targetpiececol = pieces [targetpieceid] [0] ; //copy the color of the target piece into ram
	targetpiecetyp = pieces [targetpieceid] [1] ; //copy the type of the target piece into ram
  }
void updatecastlingallowed(){
	castlingqueensideallowedblack = pieces [1] [2] && pieces [5] [2] ; //read the variable name
	castlingkingsideallowedblack = pieces [8] [2] && pieces [5] [2] ; //still the same
	castlingqueensideallowedwhite = pieces [25] [2] && pieces [29] [2] ; //nothing changed
	castlingkingsideallowedwhite = pieces [32] [2] && pieces [29] [2] ; //learn to read
  }
int validatemove(int dim, int startcol, int startrow, int endcol, int endrow, int activeplayercol){
	readpiecedata(1, startcol, startrow, endcol, endrow);
	if (movingpiececol != activeplayercol || movingpiececol == targetpiececol){ //rereads a new input if the old one picks an empty field, the oponents piece, or moves a piece on a field with a piece of the same colour
		std::cout << "what are u doing" << std::endl;
		return 0;
	}
	else { //executes the movement check if input validation doesnt fail
		if (movingpiecetyp == 0){
			std::cout << "how did this happen" << std::endl; //this should NEVER be reached as moving an empty field should already be filtered out
			return 0;
		}
		else if (movingpiecetyp == 1){ //pawn checks
			if (activeplayercol == 1){	//actions if black moves a pawn
				if (targetpiecetyp == 0){ //check if the target field is empty as this influences how the pawns can move
					if (0 == startcol - endcol){ //check if the move is straight
						if (startrow - endrow == -1){ //check if the move is one field straight
							return 1;
						}
						else if (isfirstmove == 1 && startrow - endrow == -2){ //check for double long opening pawn move
							if (pieces [board [endrow - 1] [startcol] [dim]] [0] == 0){	//check if the field you are jumping over is empty
								return 2;
							}
						} 
					}
					else if (enpassantpossible == 1 && board [startrow] [endcol] [dim] == enpassantpossibleon){ //check if the move is en passant
						return 3;
					}
				}
				else if (targetpiecetyp != 0){ //checks for when the target field isnt empty
					if (std::abs(startcol - endcol) == 1 && (startrow - endrow) == -1){ //checks if the move is one field diagonal
						return 1;
					}
				}
			}
			else if (activeplayercol == 2){ //actions if white moves a pawn
				if (targetpiecetyp == 0){ //check if the target field is empty as this influences how the pawns can move
					if (0 == startcol - endcol){ //check if the move is straight
						if (startrow - endrow == 1){ //check if the move is one field straight
							return 1;
						}
						else if (isfirstmove == 1 && startrow - endrow == 2){ //check for double long opening pawn move
							if (pieces [board [endrow + 1] [startcol] [dim]] [0] == 0){ //check if the field you are jumping over is empty
								return 2;
							} 
						} 
					}
					else if (enpassantpossible == 1 && board [startrow] [endcol] [dim] == enpassantpossibleon){ //check if the move is en passant
						return 3;
					} 
				}
				else if (targetpiecetyp != 0){ //checks for when the target field isnt empty
					if (std::abs(startcol - endcol) == 1 && (startrow - endrow) == 1){ //checks if the move is one field diagonal
						return 1;
					}
				} 
			}
		}
		else if (movingpiecetyp == 2){ //rook checks
			if (startrow == endrow){ //check if the rook move is a straight horizontal line
				if (startcol < endcol){ //moving right
					i = 0;
					while (startcol + 1 < endcol){
						++startcol;
						i = i + board [startrow] [startcol] [dim];
					}
					if (i == 0) {return 1;}
				}
				else if (startcol > endcol){ //moving left
					i = 0;
					while (startcol - 1 > endcol){
						--startcol;
						i = i + board [startrow] [startcol] [dim];
					}
					if (i == 0) {return 1;}
				}
			}
			else if (startcol == endcol){ //check if the move is a straight vertical line
				if (startrow < endrow){ //moving down
					i = 0;
					while (startrow + 1 < endrow){
						++startrow;
						i = i + board [startrow] [startcol] [dim];
					}
					if (i == 0) {return 1;}
				}
				else if (startrow > endrow){ //moving up
					i = 0;
					while (startrow - 1 > endrow){
						--startrow;
						i = i + board [startrow] [startcol] [dim];
					}
					if (i == 0) {return 1;}
				}
			} 
		}
		else if (movingpiecetyp == 3){ //knight checks
			if ((std::abs(startcol - endcol) == 1 && std::abs(startrow - endrow) == 2) || (std::abs(startcol - endcol) == 2 && std::abs(startrow - endrow) == 1)){
				return 1;
			}
		}
		else if (movingpiecetyp == 4){ //bishop checks
			if (std::abs(startrow - endrow) == std::abs(startcol - endcol)){ //check if the move is diagonal
				if (startcol < endcol && startrow > endrow){
					i = 0;
					while (startcol + 1 < endcol && startrow - 1 > endrow){
						++startcol;
						--startrow;
						i = i + board [startrow] [startcol] [dim];
					}
					if (i == 0) {return 1;}
				}
				else if (startcol > endcol && startrow > endrow){
					i = 0;
					while (startcol - 1 > endcol && startrow - 1 > endrow){
						--startcol;
						--startrow;
						i = i + board [startrow] [startcol] [dim];
					}
					if (i == 0) {return 1;}
				}
				else if (startcol < endcol && startrow < endrow){
					i = 0;
					while (startcol + 1 < endcol && startrow + 1 < endrow){
						++startcol;
						++startrow;
						i = i + board [startrow] [startcol] [dim];
					}
					if (i == 0) {return 1;}
				}
				else if (startcol > endcol && startrow < endrow){
					i = 0;
					while (startcol - 1 > endcol && startrow + 1 < endrow){
						--startcol;
						++startrow;
						i = i + board [startrow] [startcol] [dim];
					}
					if (i == 0) {return 1;}
				}
			}
		}
		else if (movingpiecetyp == 5){ //queen checks
			if (startrow == endrow){ //check if the queen move is a straight horizontal line
				if (startcol < endcol){ //moving right
					i = 0;
					while (startcol + 1 < endcol){
						++startcol;
						i = i + board [startrow] [startcol] [dim];
					}
					if (i == 0) {return 1;}
				}
				else if (startcol > endcol){ //moving left
					i = 0;
					while (startcol - 1 > endcol){
						--startcol;
						i = i + board [startrow] [startcol] [dim];
					}
					if (i == 0) {return 1;}
				}
			}
			else if (startcol == endcol){ //check if the move is a straight vertical line
				if (startrow < endrow){ //moving down
					i = 0;
					while (startrow + 1 < endrow){
						++startrow;
						i = i + board [startrow] [startcol] [dim];
					}
					if (i == 0) {return 1;}
				}
				else if (startrow > endrow){ //moving up
					i = 0;
					while (startrow - 1 > endrow){
						--startrow;
						i = i + board [startrow] [startcol] [dim];
					}
					if (i == 0) {return 1;}
				}
			}
			else if (std::abs(startrow - endrow) == std::abs(startcol - endcol)){ //check if the move is diagonal
				if (startcol < endcol && startrow > endrow){
					i = 0;
					while (startcol + 1 < endcol && startrow - 1 > endrow){
						++startcol;
						--startrow;
						i = i + board [startrow] [startcol] [dim];
					}
					if (i == 0) {return 1;}
				}
				else if (startcol > endcol && startrow > endrow){
					i = 0;
					while (startcol - 1 > endcol && startrow - 1 > endrow){
						--startcol;
						--startrow;
						i = i + board [startrow] [startcol] [dim];
					}
					if (i == 0) {return 1;}
				}
				else if (startcol < endcol && startrow < endrow){
					i = 0;
					while (startcol + 1 < endcol && startrow + 1 < endrow){
						++startcol;
						++startrow;
						i = i + board [startrow] [startcol] [dim];
					}
					if (i == 0) {return 1;}
				}
				else if (startcol > endcol && startrow < endrow){
					i = 0;
					while (startcol - 1 > endcol && startrow + 1 < endrow){
						--startcol;
						++startrow;
						i = i + board [startrow] [startcol] [dim];
					}
					if (i == 0) {return 1;}
				}
			}
		}
		else if (movingpiecetyp == 6){ //king checks
			if (isfirstmove == 1 && std::abs(startcol - endcol) == 2){  //check for castle (stage 1 still need to verify if the rook you want to castle with has already been moved)
				return 1;
			}
			else if (std::abs(startcol - endcol) <= 1 && std::abs(startrow - endrow) <= 1){ //check if the king move is not longer than 1 field
				return 1;
			} 
		}
		else if (movingpiecetyp == 7){ //pawn promotion checks
			
		}
		else { //catchall if no of the previous checks reacts
			return 0;
			std::cout << "what did you do?" << std::endl;
		}
	}
	return 0;
}
int checkformate(int dim, int kingcolour){
	int kingrow;
	int kingcol;
	int row;
	int col;
	int playerkingcheckcol;
	kingrow = 0;
	kingcol = 0;
	row = 0;
	col = 0;
	if (kingcolour == 1){playerkingcheckcol = 2;}
	if (kingcolour == 2){playerkingcheckcol = 1;}
	while (kingrow < 8){//this loop gets the position of the king for the wanted colour
		while (kingcol < 8){
			if (pieces [board [kingrow] [kingcol] [dim]] [0] == kingcolour && pieces [board [kingrow] [kingcol] [dim]] [1] == 6){break;} //überprüfung ob das gewählte feld ein könig der gewünschten farbe ist
			++kingcol;
		}
		if (pieces [board [kingrow] [kingcol] [dim]] [0] == kingcolour && pieces [board [kingrow] [kingcol] [dim]] [1] == 6){break;} //überprüfung ob das gewählte feld ein könig der gewünschten farbe ist
		++kingrow;
		kingcol = 0;
	}
	while (row < 8){
		while (col < 8){
			if (validatemove(col, row, kingcol, kingrow, 0,playerkingcheckcol)){return 1;}
			++col;
		}
		++row;
		col = 0;
	}
	return 0;
}
void removepiecelog(int dim,int col, int row){
	board [row] [col] [dim] = 0;
}
void movepiecelog(int dim, int startcol, int startrow, int endcol, int endrow){//function to move the selected piece in the memory
	board [startrow] [startcol] [dim] = 0; //clear the field of the figure to move
	board [endrow] [endcol] [dim] = movingpieceid; //place the piece at the target location
}
void removepiecephys(int col, int row){
	
}
void movepiecephys(int startcol, int startrow, int endcol, int endrow){//function to move the selected piece on the board

  }
int main (){ 
	//populate the board array with chess pieces
	//board [row] [col]
	board [0] [0] [0] = 1; //rook
	board [0] [1] [0] = 2; //knight
	board [0] [2] [0] = 3; //bishop
	board [0] [3] [0] = 4; //queen
	board [0] [4] [0] = 5; //king
	board [0] [5] [0] = 6; //bishop
	board [0] [6] [0] = 7; //knight
	board [0] [7] [0] = 8; //rook
	board [1] [0] [0] = 9; //pawn
	board [1] [1] [0] = 10; //pawn
	board [1] [2] [0] = 11; //pawn
	board [1] [3] [0] = 12; //pawn
	board [1] [4] [0] = 13; //pawn
	board [1] [5] [0] = 14; //pawn
	board [1] [6] [0] = 15; //pawn
	board [1] [7] [0] = 16; //pawn
	board [6] [0] [0] = 17; //pawn
	board [6] [1] [0] = 18; //pawn
	board [6] [2] [0] = 19; //pawn
	board [6] [3] [0] = 20; //pawn
	board [6] [4] [0] = 21; //pawn
	board [6] [5] [0] = 22; //pawn
	board [6] [6] [0] = 23; //pawn
	board [6] [7] [0] = 24; //pawn
	board [7] [0] [0] = 25; //rook
	board [7] [1] [0] = 26; //knight
	board [7] [2] [0] = 27; //bishop
	board [7] [3] [0] = 28; //queen
	board [7] [4] [0] = 29; //king
	board [7] [5] [0] = 30; //bishop
	board [7] [6] [0] = 31; //knight
	board [7] [7] [0] = 32; //rook
	//set the starting colour to white
	activeplayercol = 2;
	gamenotover = 1;
	boardcopydimension(0, 1);
	while (gamenotover == 1){
		dumplogicfieldtoshell(1); //for testing purposes comment out later
		readinput();
		readpiecedata(1, inpstartcol, inpstartrow, inpendcol, inpendrow);
		updatecastlingallowed();
		blackismate = checkformate(1, 1);
		whiteismate = checkformate(1, 2);
		movevalid = validatemove(1, inpstartcol, inpstartrow, inpendcol, inpendrow, activeplayercol);
		if (movevalid != 0){
			if (movevalid == 2){ //set variables to make en passant possible on double long pawn move
				enpassantpossible = 0;
				enpassantpossibleon = movingpieceid;
			}
			if (movevalid == 3){ //remove the pawn that just got revolutiond
				removepiecelog(1, inpendcol, inpstartrow);
				removepiecephys(inpendcol, inpstartrow);
			}
			movepiecelog(1, inpstartcol, inpstartrow, inpendcol, inpendrow);
			movepiecephys(inpstartcol, inpstartrow, inpendcol, inpendrow);
			pieces [movingpieceid] [2] = 0;
			enpassantpossible++; //set enpassantpossible to 1 for the next half turn if it has been set to 0 by a doubble long pawn move
			if (activeplayercol == 1){activeplayercol = 2; std::cout << "now whites move" << std::endl;} //change the player colour after a succesful move
			else if (activeplayercol == 2){activeplayercol = 1; std::cout << "now blacks move" << std::endl;} //same as above but for the other direction
			else  {std::cout << "?" << std::endl;} //you are not supposed to be here!!!
		}
	}
}