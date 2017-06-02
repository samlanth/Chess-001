//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Executable
// Unit testing for class Board
//
//
#include "testboard.h"

// Board type 1
using PieceID_1 = uint8_t;
const uint8_t B_SIZE_1 = 8;
using Piece_1 = chess::Piece<PieceID_1, B_SIZE_1>;

// Board type 2
using PieceID_2 = uint16_t;
const uint8_t B_SIZE_2 = 16;
using Piece_2 = chess::Piece<PieceID_2, B_SIZE_2>;

// Board type 3 
using PieceID_3 = long;
const uint8_t B_SIZE_3 = 10;
using Piece_3 = chess::Piece<PieceID_3, B_SIZE_3>;

//-----------------------------------------------------------------------
// test.exe -v -r
// -v (optional): verbose
// -r (optional): show report of tests
//-----------------------------------------------------------------------
int main(int argc, char* argv[])
{
    unittest::cmd_parser cmd(argc, argv);
    bool verbose = false;
    bool report = false;

	try
	{
        if (cmd.has_option("-v")) verbose = true;
        if (cmd.has_option("-r")) report = true;

		// TEST Board type 1
		{
			Piece_1::init();
			chess::test::TestBoard<PieceID_1, B_SIZE_1> test_board_1("TEST Board type 1");
			test_board_1.do_test(verbose, report);
		}

		// TEST Board type 2
		{
			Piece_2::init();
			chess::test::TestBoard<PieceID_2, B_SIZE_2> test_board_2("TEST Board type 2");
			test_board_2.do_test(verbose, report);
		}

        // TEST Board type 3
        {
            Piece_3::init();
            chess::test::TestBoard<PieceID_3, B_SIZE_3> test_board_3("TEST Board type 3");
            test_board_3.do_test(verbose, report);
        }

    }
	catch (...)
	{
		return -1;
	}
	return 0;
}