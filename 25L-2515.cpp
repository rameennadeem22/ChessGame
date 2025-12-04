#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
std::vector<sf::RectangleShape> legalMoveHighlights;             //stores possible positions
sf::RectangleShape highlightSquare(sf::Vector2f(100.f, 100.f));            //outlines selected piece
char board[8][8] = {                                              //character array representing chess board (black lowercase)
    {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
    {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
    {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
};
bool isSquareSelected = false;                    //checks if square is clicked
int selectedRow = -1, selectedCol = -1;          //finds out which square coordinate is selected
const int SQUARE_SIZE = 100;

bool whiteInCheck = false;
bool blackInCheck = false;

bool isCheck(bool isWhiteTurnCheck);

sf::Color squarecolors(int row, int col) {
    if ((row + col) % 2 == 0) {         //alternating boxes have different colours (sum odd or even)
        return sf::Color(255, 255, 255);
    }
    else {
        return sf::Color(200, 162, 200);
    }
}
int whiteScore = 0, blackScore = 0;           //storing score for each piece
int pieceScoring(char piece) {
    int value;
    if (piece == 'P' || piece == 'p') {
        value = 1;
    }
    else if (piece == 'N' || piece == 'n') {
        value = 3;
    }
    else if (piece == 'B' || piece == 'b') {
        value = 3;
    }
    else if (piece == 'R' || piece == 'r') {
        value = 5;
    }
    else if (piece == 'Q' || piece == 'q') {
        value = 9;
    }
    else if (piece == 'K' || piece == 'k') {
        value = 3;
    }
    else {
        value = 0;
    }
    return value;
}

bool isValidPawnMove(int startRow, int startCol, int newRow, int newCol) {
    char piece = board[startRow][startCol];
    if (piece == 'P') {
        if (newCol == startCol && newRow == startRow - 1 && board[newRow][newCol] == '.') {              //normal white pawn move (one box up) and destination is empty
            return true; 
        }
        //BONUS MARKS- PAWN CHECKING FOR TWO MOVES AT START IF USER WANTS
        if (startRow == 6 && newCol == startCol && newRow == startRow - 2 && board[startRow - 1][startCol] == '.' && board[newRow][newCol] == '.') {   //first move check if two squares if user wants
            return true;
        }
        if (newRow == startRow - 1 && (newCol == startCol - 1 || newCol == startCol + 1) && board[newRow][newCol] >= 'a' && board[newRow][newCol] <= 'z') {  //capturing diagonally is enemy is there
            return true;
        }
    }
    if (piece == 'p') {                                                                               //same logic for black pawn move (one box down)
        if (newCol == startCol && newRow == startRow + 1 && board[newRow][newCol] == '.') {
            return true;
        }
        //BLACK PAWN BONUS
        if (startRow == 1 && newCol == startCol && newRow == startRow + 2 && board[startRow + 1][startCol] == '.' && board[newRow][newCol] == '.') {
            return true;
        }
        if (newRow == startRow + 1 && (newCol == startCol - 1 || newCol == startCol + 1) && board[newRow][newCol] >= 'A' && board[newRow][newCol] <= 'Z') {
            return true;
        }
    }
    return false;
}
bool isValidRookMove(int startRow, int startCol, int newRow, int newCol) {                         //move horizontally or vertically if all squares in path are empty
    if (startRow == newRow) {
        if (startCol < newCol) {
            for (int c = startCol + 1; c < newCol; c++) {
                if (board[startRow][c] != '.') {
                    return false;
                }
            }
        }
        else {
            for (int c = startCol - 1; c > newCol; c--) {
                if (board[startRow][c] != '.') {
                    return false;
                }
            }
        }
    }
    else if (startCol == newCol) {
        if (startRow < newRow) {
            for (int r = startRow + 1; r < newRow; r++) {
                if (board[r][startCol] != '.')
                    return false; 
            }
        }
        else {
            for (int r = startRow - 1; r > newRow; r--) {
                if (board[r][startCol] != '.')
                    return false; 
            }
        }
    }
    // Not horizontal or vertical
    else {
        return false;
    }

    // Can't capture own piece
    char piece = board[startRow][startCol];
    char target = board[newRow][newCol];
    if ((piece >= 'A' && piece <= 'Z' && target >= 'A' && target <= 'Z') ||
        (piece >= 'a' && piece <= 'z' && target >= 'a' && target <= 'z')) {
        return false;
    }

    return true;
}
bool isValidBishopMove(int startRow, int startCol, int newRow, int newCol) {
    // Check if move is diagonal
    if (abs(newRow - startRow) != abs(newCol - startCol)) {             //moves diagonally (use absolute difference)
        return false;
    }

    int rowStep;
    int colStep;

    if (newRow > startRow) {
        rowStep = 1;                //moving down or up
    }
    else {
        rowStep = -1;
    } 

    if (newCol > startCol) {
        colStep = 1;          //moving left or right
    }
    else {
        colStep = -1;
    }

    int r = startRow + rowStep;          //starting from one square away from starting (diagonally)
    int c = startCol + colStep;

    // Check that path is empty
    while (r != newRow && c != newCol) {
        if (board[r][c] != '.') {
            return false; 
        }
        r += rowStep;
        c += colStep;
    }

    // Can't capture own piece
    char piece = board[startRow][startCol];
    char target = board[newRow][newCol];
    if ((piece >= 'A' && piece <= 'Z' && target >= 'A' && target <= 'Z') ||
        (piece >= 'a' && piece <= 'z' && target >= 'a' && target <= 'z')) {
        return false;
    }

    return true;
}

bool isValidKnightMove(int startRow, int startCol, int newRow, int newCol) {
    int rowDiff = abs(newRow - startRow);
    int colDiff = abs(newCol - startCol);

    // Knight moves in "L" shape: 2 by 1 or 1 by 2
    if (!((rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2))) {
        return false;
    }

    // Can't capture own piece
    char piece = board[startRow][startCol];
    char target = board[newRow][newCol];
    if ((piece >= 'A' && piece <= 'Z' && target >= 'A' && target <= 'Z') ||
        (piece >= 'a' && piece <= 'z' && target >= 'a' && target <= 'z')) {
        return false;
    }

    return true;
}

bool isValidQueenMove(int startRow, int startCol, int newRow, int newCol) {
    // Rook-like movement (same row or same column)
    if (startRow == newRow || startCol == newCol) {
        // Reusing rook logic and checking if path is clear
        int rowStep = 0;
        int colStep = 0;

        if (startRow == newRow) { // horizontal
            colStep = (newCol > startCol) ? 1 : -1;
        }
        else { // vertical
            rowStep = (newRow > startRow) ? 1 : -1;
        }

        int r = startRow + rowStep;
        int c = startCol + colStep;

        while (r != newRow || c != newCol) {
            if (board[r][c] != '.') return false; // path blocked
            r += rowStep;
            c += colStep;
        }

        return true;
    }

    // Bishop-like movement (diagonal)
    if ((newRow - startRow == newCol - startCol) || (newRow - startRow == startCol - newCol)) {
        int rowStep = (newRow > startRow) ? 1 : -1;
        int colStep = (newCol > startCol) ? 1 : -1;

        if (newCol < startCol) colStep = -1;

        int r = startRow + rowStep;
        int c = startCol + colStep;

        while (r != newRow && c != newCol) {
            if (board[r][c] != '.') return false; // path not clear
            r += rowStep;
            c += colStep;
        }

        return true;
    }

    // Not a valid queen move
    return false;
}

bool isValidKingMove(int startRow, int startCol, int newRow, int newCol) {
    int rowDiff = newRow - startRow;
    int colDiff = newCol - startCol;

    // King can move only one square in any direction
    if ((rowDiff >= -1 && rowDiff <= 1) && (colDiff >= -1 && colDiff <= 1)) {         //column difference and row difference is between 0 and 1
        char piece = board[startRow][startCol];
        char target = board[newRow][newCol];

        if ((piece >= 'A' && piece <= 'Z') && (target >= 'A' && target <= 'Z')) {
            return false; // white can't capture white
        }
        if ((piece >= 'a' && piece <= 'z') && (target >= 'a' && target <= 'z')) {
            return false; // black can't capture black
        }

        return true; // valid move
    }

    return false; // not a valid king move
}
void highlightLegalMoves(int row, int col) {
    legalMoveHighlights.clear(); // remove previous highlights
    char piece = board[row][col];

    for (int r = 0; r < 8; r++) {               //testing validity of all moves
        for (int c = 0; c < 8; c++) {
            bool valid = false;

            if (piece == 'P' || piece == 'p') valid = isValidPawnMove(row, col, r, c);
            else if (piece == 'R' || piece == 'r') valid = isValidRookMove(row, col, r, c);
            else if (piece == 'N' || piece == 'n') valid = isValidKnightMove(row, col, r, c);
            else if (piece == 'B' || piece == 'b') valid = isValidBishopMove(row, col, r, c);
            else if (piece == 'Q' || piece == 'q') valid = isValidQueenMove(row, col, r, c);
            else if (piece == 'K' || piece == 'k') valid = isValidKingMove(row, col, r, c);

            if (valid) {
                sf::RectangleShape highlight(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));              //highlights possible moves to green
                highlight.setPosition(c * SQUARE_SIZE, r * SQUARE_SIZE);
                highlight.setFillColor(sf::Color(0, 255, 0, 100)); // semi-transparent green
                legalMoveHighlights.push_back(highlight);
            }
        }
    }
}


bool isDragging = false;              //becomes true during drag using mouse button
int dragRow = -1, dragCol = -1;       //piece being dragged
int dragMouseX = 0, dragMouseY = 0;      //mouse position during drag
sf::RectangleShape dragRect;  
bool isWhiteTurn = true;             //alternating turns starting with white
void MouseClick(sf::RenderWindow& window);
void handlemouseclick(int mouseX, int mouseY);
//void MovePiece(int newRow, int newCol);
sf::Vector2i getBoardPosition(int mouseX, int mouseY) {
    return sf::Vector2i(mouseX / SQUARE_SIZE, mouseY / SQUARE_SIZE);           //pixel coordinates to board coordinates
}
void updateDraggedPiece(int mouseX, int mouseY) {
    dragMouseX = mouseX;
    dragMouseY = mouseY;                  //updates position of piece being dragged
}
void stopDragging(int mouseX, int mouseY) {             
    if (!isDragging) return;                  
    //convert mouse position to board coordinates
    sf::Vector2i pos = getBoardPosition(mouseX, mouseY);
    int newR = pos.y;
    int newC = pos.x;

//checking too see if it is out of board
   if (newR < 0 || newR > 7 || newC < 0 || newC > 7) {
        std::cout << "Dropped outside board — cancelled\n";
        isDragging = false;
        isSquareSelected = false;
        return;
    }

    char piece = board[dragRow][dragCol];         //piece being dragged
    char target = board[newR][newC];         //what exists at destination
    bool validMove = false;        

    // Check turn and see which pieces can be moved accordingly
    if ((isWhiteTurn && piece >= 'a' && piece <= 'z') ||
        (!isWhiteTurn && piece >= 'A' && piece <= 'Z')) {
        isDragging = false;
        isSquareSelected = false;
        return;
    }

    // Validate moves based on piece type
    if (piece == 'P' || piece == 'p') {
        validMove = isValidPawnMove(dragRow, dragCol, newR, newC);
    }
    else if (piece == 'R' || piece == 'r') {
        validMove = isValidRookMove(dragRow, dragCol, newR, newC);
    }
    else if (piece == 'N' || piece == 'n') {
        validMove = isValidKnightMove(dragRow, dragCol, newR, newC);
    }
    else if (piece == 'B' || piece == 'b') {
        validMove = isValidBishopMove(dragRow, dragCol, newR, newC);
    }
    else if (piece == 'Q' || piece == 'q') {
        validMove = isValidQueenMove(dragRow, dragCol, newR, newC);
    }
    else if (piece == 'K' || piece == 'k') {
        validMove = isValidKingMove(dragRow, dragCol, newR, newC);
    }

    if (validMove) {
        board[newR][newC] = piece;                //place piece at new position using board array (updating it)
        board[dragRow][dragCol] = '.';            //empty the old position with .
        //PAWN PROMOTION TO QUEEN
        if (piece == 'P' && newR == 0) {
            board[newR][newC] = 'Q';
            std::cout << "White Pawn promoted to Queen \n";
        }
        else if (piece == 'p' && newR == 7) {
            board[newR][newC] = 'q';
            std::cout << "Black Pawn promoted to Queen \n";
        }

        //Capturing enemy pieces (if not empty) and updating score
        if (target != '.') {
            std::cout << "Captured " << target << "\n";
            if (isWhiteTurn) {
                whiteScore += pieceScoring(target);
            }
            else {
                blackScore += pieceScoring(target);
            }
            std::cout << "White score= " << whiteScore << std::endl;
            std::cout << "Black score= " << blackScore << std::endl;
            std::ofstream outFile;
            outFile.open("scores.txt", std::ios::out | std::ios::app);
            if (outFile.is_open()) {
                outFile << "White Score: " << whiteScore << ", Black Score: " << blackScore << "\n";
                outFile.close();
            }
        }
        isWhiteTurn = !isWhiteTurn;         //switching turns after a legal move has been made
        whiteInCheck = isCheck(true);         //checking for check in each king
        blackInCheck = isCheck(false);
        if (whiteInCheck) std::cout << "White King is in check!\n";
        if (blackInCheck) std::cout << "Black King is in check!\n";
    }
    else {
        std::cout << "Invalid move! Returning piece\n";
    }
    //RESETTING dragging
    isDragging = false;
    isSquareSelected = false;
    legalMoveHighlights.clear();
}

sf::Vector2i findKing(bool isWhite) {
    char king;
    if (isWhite) {
        king = 'K';
    }
    else {
        king = 'k';
    }

    for (int r = 0; r < 8; r++) {           //search entire board and return its positions.
        for (int c = 0; c < 8; c++) {
            if (board[r][c] == king)
                return sf::Vector2i(r, c);
        }
    }
    return sf::Vector2i(-1, -1); //just as a check- king always exists at some coordinate between 0 and 7
}

bool isCheck(bool isWhiteTurnCheck) {
    sf::Vector2i kingPos = findKing(isWhiteTurnCheck);
    int kr = kingPos.x;      //king row and column
    int kc = kingPos.y;

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            char piece = board[r][c];
            if (piece == '.') continue;       //skipping empty squares

            // Skip same color
            if ((isWhiteTurnCheck && piece >= 'A' && piece <= 'Z') ||
                (!isWhiteTurnCheck && piece >= 'a' && piece <= 'z'))             //making sure friendly moves don't count
                continue;

            bool threat = false;
            if (piece == 'P' || piece == 'p') threat = isValidPawnMove(r, c, kr, kc);
            else if (piece == 'R' || piece == 'r') threat = isValidRookMove(r, c, kr, kc);        //if valid enemy moves exist we say king is in check
            else if (piece == 'N' || piece == 'n') threat = isValidKnightMove(r, c, kr, kc);
            else if (piece == 'B' || piece == 'b') threat = isValidBishopMove(r, c, kr, kc);
            else if (piece == 'Q' || piece == 'q') threat = isValidQueenMove(r, c, kr, kc);
            else if (piece == 'K' || piece == 'k') threat = isValidKingMove(r, c, kr, kc);

            if (threat) return true;      //king is under attack
        }
    }
    return false;        //king is safe
} 

bool isCheckmate(bool isWhiteTurnCheck) {
    if (!isCheck(isWhiteTurnCheck)) return false; //see if king is in check because only then can next move be checkmate

    for (int r = 0; r < 8; r++) {         //trying all moves to see if king can be saved
        for (int c = 0; c < 8; c++) {
            char piece = board[r][c];
            if (piece == '.') continue;       //skipping empty squares

            if ((isWhiteTurnCheck && piece >= 'a' && piece <= 'z') ||       //skipping opponent pieces
                (!isWhiteTurnCheck && piece >= 'A' && piece <= 'Z'))
                continue;

            for (int nr = 0; nr < 8; nr++) {           //moving all pieces to every possible valid position
                for (int nc = 0; nc < 8; nc++) {
                    bool validMove = false;
                    if (piece == 'P' || piece == 'p') validMove = isValidPawnMove(r, c, nr, nc);
                    else if (piece == 'R' || piece == 'r') validMove = isValidRookMove(r, c, nr, nc);
                    else if (piece == 'N' || piece == 'n') validMove = isValidKnightMove(r, c, nr, nc);
                    else if (piece == 'B' || piece == 'b') validMove = isValidBishopMove(r, c, nr, nc);
                    else if (piece == 'Q' || piece == 'q') validMove = isValidQueenMove(r, c, nr, nc);
                    else if (piece == 'K' || piece == 'k') validMove = isValidKingMove(r, c, nr, nc);

                    if (validMove) {
                        char target = board[nr][nc];          //moving temporarily saving what is currently at destination
                        board[nr][nc] = piece;     //make temporary move of a piece to save king
                        board[r][c] = '.';         //clear starting position

                        if (!isCheck(isWhiteTurnCheck)) {
                            // undo move
                            board[r][c] = piece;
                            board[nr][nc] = target;
                            return false; // found a move that saves king is not in checkmate
                        }
                        // undo moves so we can go back to original
                        board[r][c] = piece;
                        board[nr][nc] = target;
                    }
                }
            }
        }
    }
    return true; // no move can save king
}
bool isStalemate(bool isWhiteTurnCheck) {                 //king isnt in check but there are no valid moves left. game ends
    if (isCheck(isWhiteTurnCheck)) return false; 

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            char piece = board[r][c];
            if (piece == '.') continue;

            if ((isWhiteTurnCheck && piece >= 'a' && piece <= 'z') ||
                (!isWhiteTurnCheck && piece >= 'A' && piece <= 'Z'))
                continue;

            for (int nr = 0; nr < 8; nr++) {
                for (int nc = 0; nc < 8; nc++) {
                    bool validMove = false;
                    if (piece == 'P' || piece == 'p') validMove = isValidPawnMove(r, c, nr, nc);
                    else if (piece == 'R' || piece == 'r') validMove = isValidRookMove(r, c, nr, nc);
                    else if (piece == 'N' || piece == 'n') validMove = isValidKnightMove(r, c, nr, nc);
                    else if (piece == 'B' || piece == 'b') validMove = isValidBishopMove(r, c, nr, nc);
                    else if (piece == 'Q' || piece == 'q') validMove = isValidQueenMove(r, c, nr, nc);
                    else if (piece == 'K' || piece == 'k') validMove = isValidKingMove(r, c, nr, nc);

                    if (validMove) {
                        char target = board[nr][nc];
                        board[nr][nc] = piece;
                        board[r][c] = '.';

                        if (!isCheck(isWhiteTurnCheck)) {         //same logic of temporarily moving piece like checkmate but if legal moves exist we say its not SM
                            board[r][c] = piece;
                            board[nr][nc] = target;
                            return false; 
                        }

                        board[r][c] = piece;
                        board[nr][nc] = target;
                    }
                }
            }
        }
    }

    return true; // no legal move, king not in check
}



int main()
{
    // Create window
    sf::RenderWindow window(sf::VideoMode(830, 830), "Chess Game");                   //creating game window
    sf::RectangleShape square(sf::Vector2f(100.f, 100.f));           //setting up squares

    //textures to store image data loaded from files
    sf::Texture whiteKingTexture;
    sf::Texture whiteQueenTexture;
    sf::Texture whiteBishopLTexture, whiteBishopRTexture;
    sf::Texture whiteKnightLTexture, whiteKnightRTexture;
    sf::Texture whiteRookLTexture, whiteRookRTexture;
    sf::Texture whitePawnsTexture[8];

    sf::Texture blackKingTexture;
    sf::Texture blackQueenTexture;
    sf::Texture blackBishopLTexture, blackBishopRTexture;
    sf::Texture blackKnightLTexture, blackKnightRTexture;
    sf::Texture blackRookLTexture, blackRookRTexture;
    sf::Texture blackPawnsTexture[8];

    //drawing sprites
    sf::Sprite whiteKingSprite;
    sf::Sprite whiteQueenSprite;
    sf::Sprite whiteRookLSprite, whiteRookRSprite;
    sf::Sprite whiteBishopLSprite, whiteBishopRSprite;
    sf::Sprite whiteKnightLSprite, whiteKnightRSprite;
    sf::Sprite whitePawnsSprite[8];

    sf::Sprite blackKingSprite;
    sf::Sprite blackQueenSprite;
    sf::Sprite blackRookLSprite, blackRookRSprite;
    sf::Sprite blackBishopLSprite, blackBishopRSprite;
    sf::Sprite blackKnightLSprite, blackKnightRSprite;
    sf::Sprite blackPawnsSprite[8];

    //loading textures from file
    whiteKingTexture.loadFromFile("images/white_king.png");
    whiteQueenTexture.loadFromFile("images/white_queen.png");
    whiteRookLTexture.loadFromFile("images/white_rook.png");
    whiteRookRTexture.loadFromFile("images/white_rook.png");
    whiteBishopLTexture.loadFromFile("images/white_bishop.png");
    whiteBishopRTexture.loadFromFile("images/white_bishop.png");
    whiteKnightLTexture.loadFromFile("images/white_knight.png");
    whiteKnightRTexture.loadFromFile("images/white_knight.png");
    for (int i = 0; i < 8; i++)
        whitePawnsTexture[i].loadFromFile("images/white_pawn.png");

    blackKingTexture.loadFromFile("images/black_king.png");
    blackQueenTexture.loadFromFile("images/black_queen.png");
    blackRookLTexture.loadFromFile("images/black_rook.png");
    blackRookRTexture.loadFromFile("images/black_rook.png");
    blackBishopLTexture.loadFromFile("images/black_bishop.png");
    blackBishopRTexture.loadFromFile("images/black_bishop.png");
    blackKnightLTexture.loadFromFile("images/black_knight.png");
    blackKnightRTexture.loadFromFile("images/black_knight.png");
    for (int i = 0; i < 8; i++)
        blackPawnsTexture[i].loadFromFile("images/black_pawn.png");

    //assigning textures to sprites
    whiteKingSprite.setTexture(whiteKingTexture);
    whiteQueenSprite.setTexture(whiteQueenTexture);
    whiteRookLSprite.setTexture(whiteRookLTexture);
    whiteRookRSprite.setTexture(whiteRookRTexture);
    whiteBishopLSprite.setTexture(whiteBishopLTexture);
    whiteBishopRSprite.setTexture(whiteBishopRTexture);
    whiteKnightLSprite.setTexture(whiteKnightLTexture);
    whiteKnightRSprite.setTexture(whiteKnightRTexture);
    for (int i = 0; i < 8; i++) {
        whitePawnsSprite[i].setTexture(whitePawnsTexture[i]);
    }

    blackKingSprite.setTexture(blackKingTexture);
    blackQueenSprite.setTexture(blackQueenTexture);
    blackRookLSprite.setTexture(blackRookLTexture);
    blackRookRSprite.setTexture(blackRookRTexture);
    blackBishopLSprite.setTexture(blackBishopLTexture);
    blackBishopRSprite.setTexture(blackBishopRTexture);
    blackKnightLSprite.setTexture(blackKnightLTexture);
    blackKnightRSprite.setTexture(blackKnightRTexture);
    for (int i = 0; i < 8; i++) {
        blackPawnsSprite[i].setTexture(blackPawnsTexture[i]);
    }

    //setting scale
    float scale = 0.4f;
    whiteKingSprite.setScale(scale, scale);
    whiteQueenSprite.setScale(scale, scale);
    whiteRookLSprite.setScale(scale, scale);
    whiteRookRSprite.setScale(scale, scale);
    whiteBishopLSprite.setScale(scale, scale);
    whiteBishopRSprite.setScale(scale, scale);
    whiteKnightLSprite.setScale(scale, scale);
    whiteKnightRSprite.setScale(scale, scale);
    for (int i = 0; i < 8; i++) {
        whitePawnsSprite[i].setScale(scale, scale);
    }

    blackKingSprite.setScale(scale, scale);
    blackQueenSprite.setScale(scale, scale);
    blackRookLSprite.setScale(scale, scale);
    blackRookRSprite.setScale(scale, scale);
    blackBishopLSprite.setScale(scale, scale);
    blackBishopRSprite.setScale(scale, scale);
    blackKnightLSprite.setScale(scale, scale);
    blackKnightRSprite.setScale(scale, scale);
    for (int i = 0; i < 8; i++) {
        blackPawnsSprite[i].setScale(scale, scale);
    }

    //positions
    whiteRookLSprite.setPosition(0.f, 700.f);
    whiteKnightLSprite.setPosition(100.f, 700.f);
    whiteBishopLSprite.setPosition(200.f, 700.f);
    whiteQueenSprite.setPosition(300.f, 700.f);
    whiteKingSprite.setPosition(400.f, 700.f);
    whiteBishopRSprite.setPosition(500.f, 700.f);
    whiteKnightRSprite.setPosition(600.f, 700.f);
    whiteRookRSprite.setPosition(700.f, 700.f);

    for (int i = 0; i < 8; i++) {
        whitePawnsSprite[i].setPosition(i * 100.f, 600.f);
    }

    blackRookLSprite.setPosition(0.f, 0.f);
    blackKnightLSprite.setPosition(100.f, 0.f);
    blackBishopLSprite.setPosition(200.f, 0.f);
    blackQueenSprite.setPosition(300.f, 0.f);
    blackKingSprite.setPosition(400.f, 0.f);
    blackBishopRSprite.setPosition(500.f, 0.f);
    blackKnightRSprite.setPosition(600.f, 0.f);
    blackRookRSprite.setPosition(700.f, 0.f);

    for (int i = 0; i < 8; i++) {
        blackPawnsSprite[i].setPosition(i * 100.f, 100.f);
    }
    highlightSquare.setFillColor(sf::Color::Transparent);  
    highlightSquare.setOutlineThickness(4.f);
    highlightSquare.setOutlineColor(sf::Color(255, 20, 147));             //pink outline to show selected piece

    sf::Font font;
    if (!font.loadFromFile("C:\\Users\\hp\\source\\repos\\mygame\\x64\\Debug\\font\\roboto.ttf")) {
        std::cout << "Error loading font\n.";
    }
    // Main game loop
    while (window.isOpen()) {
        // Event handling
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                handlemouseclick(event.mouseButton.x, event.mouseButton.y);
            }
            if (event.type == sf::Event::MouseMoved && isDragging) {          //called as mouse moves and updates if dragged
                updateDraggedPiece(event.mouseMove.x, event.mouseMove.y);
            }
            if (event.type == sf::Event::MouseButtonReleased && isDragging) {
                stopDragging(event.mouseButton.x, event.mouseButton.y);
                if (isCheckmate(isWhiteTurn)) {
                    if (isWhiteTurn) {
                        std::cout << "White is in checkmate. Black wins! \n";
                    }
                    else {
                        std::cout << "Black is in checkmate. White wins! \n";
                    }
                    sf::sleep(sf::seconds(10));
                    window.close();
                }
            }
        }
        

        // Clear screen
        window.clear();
        bool whitecheck = isCheck(true);
        bool blackcheck = isCheck(false);
        //drawing chess board
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                square.setPosition(col * 100.f, row * 100.f);   //setting position of each square and multiplying it with 100 because we set size to 100 pixels
                square.setFillColor(squarecolors(row, col));
                sf::Vector2i whiteKingPos = findKing(true);
                sf::Vector2i blackKingPos = findKing(false);
                if ((whitecheck && row == whiteKingPos.x && col == whiteKingPos.y) || (blackcheck && row == blackKingPos.x && col == blackKingPos.y)) {
                    square.setFillColor(sf::Color(181, 136, 99));           //pink outline if in check
                }
                window.draw(square);
            }
        }
        for (int col = 0; col < 8; col++) {
            sf::Text file;
            file.setFont(font);
            file.setString(std::string(1, 'a' + col));
            file.setCharacterSize(24);
            file.setFillColor(sf::Color::White);
            file.setPosition(col * 100.f + 40, 800.f);
            window.draw(file);
        }
        for (auto& h1 : legalMoveHighlights) {
            window.draw(h1);
        }
        for (int row = 0; row < 8; row++) {
            sf::Text rank;
            rank.setFont(font);
            rank.setString(std::to_string(8 - row));
            rank.setCharacterSize(24);
            rank.setFillColor(sf::Color::White);
            rank.setPosition(800.f, row * 100.f + 35);
            window.draw(rank);
        }
        if (isSquareSelected) {
            window.draw(highlightSquare);
        }

        // 1. Draw all pieces normally except the one being dragged at their current positions
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                if (isDragging && r == dragRow && c == dragCol)
                    continue; // skipping dragged piece

                char piece = board[r][c];
                sf::Sprite temp;

                if (piece == 'K') temp = whiteKingSprite;
                else if (piece == 'Q') temp = whiteQueenSprite;
                else if (piece == 'R') {
                    if (r == 7 && c == 0) temp = whiteRookLSprite;
                    else temp = whiteRookRSprite;
                }
                else if (piece == 'B') {
                    if (r == 7 && c == 2) temp = whiteBishopLSprite;
                    else temp = whiteBishopRSprite;
                }
                else if (piece == 'N') {
                    if (r == 7 && c == 1) temp = whiteKnightLSprite;
                    else temp = whiteKnightRSprite;
                }
                else if (piece == 'P') {
                    if (r == 6) temp = whitePawnsSprite[c];  // Original position of pawn before promotion
                    else temp = whitePawnsSprite[0];  // Moved pawn for promotion
                }
                else if (piece == 'k') temp = blackKingSprite;
                else if (piece == 'q') temp = blackQueenSprite;
                else if (piece == 'r') {
                    if (r == 0 && c == 0) temp = blackRookLSprite;
                    else temp = blackRookRSprite;
                }
                else if (piece == 'b') {
                    if (r == 0 && c == 2) temp = blackBishopLSprite;
                    else temp = blackBishopRSprite;
                }
                else if (piece == 'n') {
                    if (r == 0 && c == 1) temp = blackKnightLSprite;
                    else temp = blackKnightRSprite;
                }
                else if (piece == 'p') {
                    if (r == 1) temp = blackPawnsSprite[c];  // Original position
                    else temp = blackPawnsSprite[0];  // Moved pawn
                }
                else continue; // empty square, skip
                temp.setPosition(c * 100.f, r * 100.f);
                window.draw(temp);
            }
        }

        // 2. Draw dragged piece last so it comes on top of others
        if (isDragging) {
            char piece = board[dragRow][dragCol];
            sf::Sprite temp;

            if (piece == 'K') temp = whiteKingSprite;
            else if (piece == 'Q') temp = whiteQueenSprite;
            else if (piece == 'R') {
                if (dragRow == 7 && dragCol == 0) temp = whiteRookLSprite;
                else temp = whiteRookRSprite;
            }
            else if (piece == 'B') {
                if (dragRow == 7 && dragCol == 2) temp = whiteBishopLSprite;
                else temp = whiteBishopRSprite;
            }
            else if (piece == 'N') {
                if (dragRow == 7 && dragCol == 1) temp = whiteKnightLSprite;
                else temp = whiteKnightRSprite;
            }
            else if (piece == 'P') {
                if (dragRow == 6) temp = whitePawnsSprite[dragCol];
                else temp = whitePawnsSprite[0];
            }
            else if (piece == 'k') temp = blackKingSprite;
            else if (piece == 'q') temp = blackQueenSprite;
            else if (piece == 'r') {
                if (dragRow == 0 && dragCol == 0) temp = blackRookLSprite;
                else temp = blackRookRSprite;
            }
            else if (piece == 'b') {
                if (dragRow == 0 && dragCol == 2) temp = blackBishopLSprite;
                else temp = blackBishopRSprite;
            }
            else if (piece == 'n') {
                if (dragRow == 0 && dragCol == 1) temp = blackKnightLSprite;
                else temp = blackKnightRSprite;
            }
            else if (piece == 'p') {
                if (dragRow == 1) temp = blackPawnsSprite[dragCol];
                else temp = blackPawnsSprite[0];
            }
            temp.setPosition(dragMouseX - 40, dragMouseY - 40);
            window.draw(temp);
        }

        if (isDragging) {
            window.draw(dragRect);
        }
        // Display everything
        window.display();
    }

    return 0;
}

void handlemouseclick(int mouseX, int mouseY) {
    if (isWhiteTurn)
        std::cout << "White's turn\n";
    else
        std::cout << "Black's turn\n";

    sf::Vector2i pos = getBoardPosition(mouseX, mouseY);       //convert mouse position to board coordinates
    int r = pos.y, c = pos.x;
    if (r < 0 || r>7 || c < 0 || c>7) {            //check if position is valid
        return;
    }
    char piece = board[r][c];
    if (piece == '.') {              //check if clicked on an empty square
        return;
    }
    if ((isWhiteTurn && piece >= 'a' && piece <= 'z') || (!isWhiteTurn && piece >= 'A' && piece <= 'Z')) {       //check if clicking opponents
        return;
    }
    //starting dragging from rows and columns and highlighting
    selectedRow = r;
    selectedCol = c;
    isSquareSelected = true;
    highlightSquare.setPosition(selectedCol * 100.f, selectedRow * 100.f);
    isDragging = true;
    dragRow = r;
    dragCol = c;
    dragMouseX = mouseX;
    dragMouseY = mouseY;
    highlightLegalMoves(r, c);
    std::cout << "Dragging: " << piece << " from (" << r << ", " << c << ")\n";
}