//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Executable
// Unit testing
//
//
#include "core/chess.hpp"

//-----------------------------------------------------------------------
// test.exe -v -r -f logfile
// -v (optional): verbose
// -r (optional): show report of tests
// -f file (optional): log into file
//-----------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // Test Player
    {
        chess::NullPlayer<uint8_t, 8, double, 16> player;
        std::string aname = player.playername();
        assert(aname == "NULLPlayer");
    }

    // Test PersisteManager
    chess::PersistManager::instance();

    // Test FeatureManager
    chess::FeatureManager<uint16_t, 6, float, 32>::instance();

    // Prepare player
    {
        // Play few games        
        // 6x6 board
        {
            // Load a classic partition
            chess::Partition<uint8_t, 6, double, 16>* p_classic6 = chess::PartitionManager<uint8_t, 6, double, 16>::instance()->load_partition("classic6");
            if (p_classic6==nullptr)
            {
                chess::PartitionManager<uint8_t, 6, double, 16>::instance()->make_classic_partition();
                p_classic6 = chess::PartitionManager<uint8_t, 6, double, 16>::instance()->find_partition("classic6");
                assert(p_classic6 != nullptr);
            }
            
            // Check KQvK domain exist in the partition
            chess::Domain< uint8_t, 6, double, 16>* p_domain = p_classic6->find_domain(chess::Domain<uint8_t, 6, double, 16>::getDomainName(chess::eDomainName::KQvK), "0");
            assert(p_domain != nullptr);

            // Make 2 default (no brain) players
            chess::DomainPlayer<uint8_t, 6, double, 16>* playW;
            chess::DomainPlayer<uint8_t, 6, double, 16>* playB;
            playW = new chess::DomainPlayer<uint8_t, 6, double, 16>(chess::PieceColor::W, std::string("wmind_oneall"), 0, "classic6", chess::Domain<uint8_t, 6, double, 16>::getDomainName(chess::eDomainName::KQvK), "0");
            playB = new chess::DomainPlayer<uint8_t, 6, double, 16>(chess::PieceColor::B, std::string("bmind_oneall"), 0, "classic6", chess::Domain<uint8_t, 6, double, 16>::getDomainName(chess::eDomainName::KQvK), "0");

            // Configure player brain (root)
            chess::PlayerFactoryConfig player_config;
            if (false)
            {
                // PlayerW+PlayerB [one conditional branch, all valu features]
                player_config = { chess::CondFeatureSelection::one_random, chess::ValuFeatureSelection::all };
                chess::PlayerFactory<uint8_t, 6, double, 16>::instance()->reconfigPlayerRoot(playW, player_config);
                chess::PlayerFactory<uint8_t, 6, double, 16>::instance()->reconfigPlayerRoot(playB, player_config);
            }
            else
            {
                // PlayerW+PlayerB [no conditional branch, , all valu features]
                player_config = { chess::CondFeatureSelection::none, chess::ValuFeatureSelection::all };
                chess::PlayerFactory<uint8_t, 6, double, 16>::instance()->reconfigPlayerRoot(playW, player_config);
                chess::PlayerFactory<uint8_t, 6, double, 16>::instance()->reconfigPlayerRoot(playB, player_config);
            }

            // Persist the players
            playW->save();
            playB->save();

            // Detach from partition domains and delete from memory
            playW->detachFromDomains();
            playB->detachFromDomains();
            delete playW;
            delete playB;

            // Reload from persistence
            playW = new chess::DomainPlayer<uint8_t, 6, double, 16>(chess::PieceColor::W, std::string("wmind_oneall"), 0, "classic6", chess::Domain<uint8_t, 6, double, 16>::getDomainName(chess::eDomainName::KQvK), "0");
            playB = new chess::DomainPlayer<uint8_t, 6, double, 16>(chess::PieceColor::B, std::string("bmind_oneall"), 0, "classic6", chess::Domain<uint8_t, 6, double, 16>::getDomainName(chess::eDomainName::KQvK), "0");
            playW->load();
            playB->load();

            // Test ChessGeneticAlgorithm
            //{
            //    chess::BaseGame_Config cfg{ 100, 1, 100, 1, 2000, 30 };
            //    chess::ga::ChessGeneticAlgorithm<uint8_t, 6, double, 16, 10> ga_chess(true, false, playW, playB, cfg, 5, 2, 3, 2, 1);
            //    ga_chess.run(false);
            //}

            // Test ChessCoEvolveGA
            {
                chess::BaseGame_Config cfg{ 1000, 1, 1000, 1, 20000, 10 };
                // num_iter, popsize, nbgen, _tournament_n_player, _tournament_n_game, verbose
                chess::ga::ChessCoEvolveGA<uint8_t, 6, double, 16, 10> ga_co(playW, playB, cfg, 500, 3, 1, 2, 1, 1);
                ga_co.run();
            }

            // Persist the players
            playW->save();
            playB->save();

            delete playW;
            delete playB;
        }
    }

    // Test ConditionValuationNode
    {
        chess::ConditionValuationNode<uint8_t, 8, double, 16>* root_node   = new chess::ConditionValuationNode<uint8_t, 8, double, 16>(nullptr, true, false);
        chess::ConditionValuationNode<uint8_t, 8, double, 16>* child_node1 = new chess::ConditionValuationNode<uint8_t, 8, double, 16>(root_node, true, false);
        chess::ConditionValuationNode<uint8_t, 8, double, 16>* child_node2 = new chess::ConditionValuationNode<uint8_t, 8, double, 16>(root_node, false, false);
        delete root_node;

        chess::ConditionValuationNode<uint8_t, 8, double, 16>* root_nodea = new chess::ConditionValuationNode<uint8_t, 8, double, 16>(nullptr, true, true);
        delete root_nodea; 
    }

    while (true) {}

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