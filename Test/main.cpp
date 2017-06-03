//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Executable
// Unit testing for class Board
//
//
#include "testboard.h"

const std::string default_logfile = "C:\\tmp\\chess_test_log.txt";

//-----------------------------------------------------------------------
// test.exe -v -r -f logfile
// -v (optional): verbose
// -r (optional): show report of tests
// -f file (optional): log into file
//-----------------------------------------------------------------------
int main(int argc, char* argv[])
{
    unittest::cmd_parser cmd(argc, argv);

    bool use_def_log = true;
    if (cmd.has_option("-f"))
    {
        std::string f = cmd.get_option("-f");
        if (f.size() > 0)
        {
            if (unittest::Logger::instance()->set_file(f))
                use_def_log = false;
        }
    }
    if (use_def_log)
    {
        unittest::Logger::instance()->set_file(default_logfile);
    }

    unittest::Logger::instance()->log("---------------------");
    unittest::Logger::instance()->log("Starting main        ");

    try
    {
        // Board type 1
        {
            using PieceID_1 = uint8_t;
            const uint8_t B_SIZE_1 = 8;
            using Piece_1 = chess::Piece<PieceID_1, B_SIZE_1>;

            Piece_1::init();
            chess::test::TestBoard<PieceID_1, B_SIZE_1> test_board_1("Board type 1");
            test_board_1.do_test(cmd);
        }

        // Board type 2
        {
            using PieceID_2 = uint16_t;
            const uint8_t B_SIZE_2 = 16;
            using Piece_2 = chess::Piece<PieceID_2, B_SIZE_2>;

            Piece_2::init();
            chess::test::TestBoard<PieceID_2, B_SIZE_2> test_board_2("Board type 2");
            test_board_2.do_test(cmd);
        }

        // TEST Board type 3
        {
            using PieceID_3 = long;
            const uint8_t B_SIZE_3 = 10;
            using Piece_3 = chess::Piece<PieceID_3, B_SIZE_3>;

            Piece_3::init();
            chess::test::TestBoard<PieceID_3, B_SIZE_3> test_board_3("Board type 3");
            test_board_3.do_test(cmd);
        }
    }
    catch (...)
    {
        unittest::Logger::instance()->log("Ending main - ERROR : exception thrown");
        unittest::Logger::instance()->close();
        return -1;
    }
    unittest::Logger::instance()->log("Ending main");
    unittest::Logger::instance()->close();
    return 0;
}