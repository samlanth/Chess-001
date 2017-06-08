//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Executable
// Unit testing for class Board
//
//
#include "core/testboard.hpp"
#include "ga/galgo_example.hpp"
#include "player/player.hpp"
#include "domain/domain.hpp"
#include "domain/partition.hpp"
#include "persistence/persist.hpp"
#include "feature/feature.hpp"
#include "feature/CondValNode.hpp"
#include <cassert>


//-----------------------------------------------------------------------
// test.exe -v -r -f logfile
// -v (optional): verbose
// -r (optional): show report of tests
// -f file (optional): log into file
//-----------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // Test player class instantiation
    {
        chess::NullPlayer<uint8_t, 8, double, 16> player;
        std::string aname = player.playername();
        assert(aname == "NULLPlayer");
    }

    // Test PartitionManager class instantiation
    chess::PartitionManager<uint8_t, 8, double, 16>::instance()->make_classic_partition();
    chess::Partition<uint8_t, 8, double, 16>* p_classic = chess::PartitionManager<uint8_t, 8, double, 16>::instance()->find_partition("classic");
    assert(p_classic != nullptr);

    // Test PersisteManager class instantiation
    chess::PersistManager::instance();

    //
    {
        chess::ConditionFeature_isOppositeKinCheck<uint8_t, 8, double, 16> fc;
        chess::ValuationFeature_numberMoveForPiece<uint8_t, 8, double, 16> fv(chess::PieceName::K, chess::PieceColor::W);
    }

    //
    {
        chess::DomainPlayer<uint8_t, 8, double, 16>* play1 = new chess::DomainPlayer<uint8_t, 8, double, 16>(std::string("zeromind"), p_classic->name(), "DomainKQvK", "0");
        play1->save();
        delete play1;

        chess::DomainPlayer<uint8_t, 8, double, 16>* play2 = new chess::DomainPlayer<uint8_t, 8, double, 16>(std::string("zeromind"), p_classic->name(), "DomainKQvK", "0");
        play2->load();
    }

    //
    {
        chess::CondValNode<uint8_t, 8, double, 16>* root_node = new chess::CondValNode<uint8_t, 8, double, 16>(nullptr, true, false);
        chess::CondValNode<uint8_t, 8, double, 16>* child_node1 = new chess::CondValNode<uint8_t, 8, double, 16>(root_node, true, false);
        chess::CondValNode<uint8_t, 8, double, 16>* child_node2 = new chess::CondValNode<uint8_t, 8, double, 16>(root_node, false, false);
        delete root_node;

        chess::CondValNode<uint8_t, 8, double, 16>* root_nodea = new chess::CondValNode<uint8_t, 8, double, 16>(nullptr, true, true);
        delete root_nodea; 

        int debug = 1;
    }

    //
    // Test integration with galgo
    galgo_example::galgo_example_001();

    // Test Board
    unittest::cmd_parser cmd(argc, argv);

    if (cmd.has_option("-f"))
    {
        std::string f = cmd.get_option("-f");
        if (f.size() > 0)
        {
            unittest::Logger::instance()->set_file(f);
        }
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
            chess::test::TestBoard<PieceID_1, B_SIZE_1> test_board_1("Board type 1 <uint8_t, 8>");
            test_board_1.do_test(cmd);
        }

        // Board type 2
        {
            using PieceID_2 = uint16_t;
            const uint8_t B_SIZE_2 = 16;
            using Piece_2 = chess::Piece<PieceID_2, B_SIZE_2>;

            Piece_2::init();
            chess::test::TestBoard<PieceID_2, B_SIZE_2> test_board_2("Board type 2 <uint16_t, 16>");
            test_board_2.do_test(cmd);
        }

        // TEST Board type 3
        {
            using PieceID_3 = int;
            const uint8_t B_SIZE_3 = 10;
            using Piece_3 = chess::Piece<PieceID_3, B_SIZE_3>;

            Piece_3::init();
            chess::test::TestBoard<PieceID_3, B_SIZE_3> test_board_3("Board type 3 <int, 8>");
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