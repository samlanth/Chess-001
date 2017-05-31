// --------------------------------
// Author: Alain Lanthier, 2017
//
// Unit testing for class Board
// --------------------------------
#include "testboard.h"

// Board type 1
using PieceID_1 = uint8_t;
const uint8_t B_SIZE_1 = 8;
using Piece_1 = chess::Piece<PieceID_1, B_SIZE_1>;

// Board type 2
using PieceID_2 = uint16_t;
const uint8_t B_SIZE_2 = 16;
using Piece_2 = chess::Piece<PieceID_2, B_SIZE_2>;

// static (GLOBAL) data
bool Piece_1::is_init = false;
std::vector <std::unique_ptr<Piece_1>> Piece_1::pieces = std::vector<std::unique_ptr<Piece_1>>();

bool Piece_2::is_init = false;
std::vector <std::unique_ptr<Piece_2>> Piece_2::pieces = std::vector<std::unique_ptr<Piece_2>>();
// static (GLOBAL) data


// Unit testing
int main(int argc, char* argv[])
{
	try
	{
		// TEST Board type 1
		{
			Piece_1::init();
			chess::test::TestBoard<PieceID_1, B_SIZE_1> test_board_1;
			test_board_1.do_test(true);
		}

		// TEST Board type 2
		{
			Piece_2::init();
			chess::test::TestBoard<PieceID_2, B_SIZE_2> test_board_2;
			test_board_2.do_test(true);
		}
	}
	catch (...)
	{
		return -1;
	}
	return 0;
}