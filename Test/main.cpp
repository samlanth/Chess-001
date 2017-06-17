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
    // Test PersisteManager
    chess::PersistManager::instance();

    // Prepare players
    {
        // 6x6 board
        {
            // Load a classic partition
            const std::string TestPartition = "classic6";
            chess::Partition<uint8_t, 6, double, 16>* p_classic6 = chess::PartitionManager<uint8_t, 6, double, 16>::instance()->load_partition(TestPartition);
            if (p_classic6 == nullptr)
            {
                // Not found: create partition and save
                chess::PartitionManager<uint8_t, 6, double, 16>::instance()->make_classic_partition();
                p_classic6 = chess::PartitionManager<uint8_t, 6, double, 16>::instance()->find_partition(TestPartition);
                assert(p_classic6 != nullptr);

                p_classic6->save();
            }

            chess::eDomainName RootDomain  = chess::eDomainName::KQvK;
            chess::eDomainName TestDomain1 = chess::eDomainName::KvK;
            chess::eDomainName TestDomain2 = chess::eDomainName::KQvK;
            const std::string TestPlayerW = "wmind_oneall";
            const std::string TestPlayerB = "bmind_oneall";

            // Check KQvK domain exist in the partition
            chess::Domain< uint8_t, 6, double, 16>* p_domain = p_classic6->find_domain(chess::Domain<uint8_t, 6, double, 16>::getDomainName(RootDomain), "0");
            assert(p_domain != nullptr);

            // Create 2 players for RootDomain and childs domains
            chess::DomainPlayer<uint8_t, 6, double, 16>* playW;
            chess::DomainPlayer<uint8_t, 6, double, 16>* playB;
            playW = new chess::DomainPlayer<uint8_t, 6, double, 16>(chess::PieceColor::W, std::string(TestPlayerW), 0, TestPartition, chess::Domain<uint8_t, 6, double, 16>::getDomainName(RootDomain), "0");
            playB = new chess::DomainPlayer<uint8_t, 6, double, 16>(chess::PieceColor::B, std::string(TestPlayerB), 0, TestPartition, chess::Domain<uint8_t, 6, double, 16>::getDomainName(RootDomain), "0");

            // Create RootDomain if not found
            if ( (!playW->load()) || (!playB->load()) )
            {
                delete playW;
                delete playB;
                playW = new chess::DomainPlayer<uint8_t, 6, double, 16>(chess::PieceColor::W, std::string(TestPlayerW), 0, TestPartition, chess::Domain<uint8_t, 6, double, 16>::getDomainName(RootDomain), "0");
                playB = new chess::DomainPlayer<uint8_t, 6, double, 16>(chess::PieceColor::B, std::string(TestPlayerB), 0, TestPartition, chess::Domain<uint8_t, 6, double, 16>::getDomainName(RootDomain), "0");

                // Configure player brain (root)
                chess::PlayerFactoryConfig player_config;
                if (false)
                {
                    // PlayerW+PlayerB [one conditional branch, all valu features]
                    player_config = { chess::CondFeatureSelection::one_random, chess::ValuFeatureSelection::all, true };
                    chess::PlayerFactory<uint8_t, 6, double, 16>::instance()->reconfigPlayerRoot(playW, player_config);
                    chess::PlayerFactory<uint8_t, 6, double, 16>::instance()->reconfigPlayerRoot(playB, player_config);
                }
                else
                {
                    // PlayerW+PlayerB [no conditional branch, , all valu features]
                    player_config = { chess::CondFeatureSelection::none, chess::ValuFeatureSelection::all, true };
                    chess::PlayerFactory<uint8_t, 6, double, 16>::instance()->reconfigPlayerRoot(playW, player_config);
                    chess::PlayerFactory<uint8_t, 6, double, 16>::instance()->reconfigPlayerRoot(playB, player_config);
                }

                // Persist the players
                playW->save();
                playB->save();
            }

            // Test ChessGeneticAlgorithm
            //{
            //    chess::BaseGame_Config cfg{ 100, 1, 100, 1, 2000, 30 };
            //    chess::ga::ChessGeneticAlgorithm<uint8_t, 6, double, 16, 10> ga_chess(true, false, playW, playB, cfg, 5, 2, 3, 2, 1);
            //    ga_chess.run(false);
            //}

            for (int i = 0; i < 1000; i++)
            {
                std::cout << "Global iteration : " << i + 1 << "\n";

                // ChessCoEvolveGA TestDomain1
                {
                    // Load TestDomain1 players
                    playW = new chess::DomainPlayer<uint8_t, 6, double, 16>(chess::PieceColor::W, std::string(TestPlayerW), 0, TestPartition, chess::Domain<uint8_t, 6, double, 16>::getDomainName(TestDomain1), "0");
                    playB = new chess::DomainPlayer<uint8_t, 6, double, 16>(chess::PieceColor::B, std::string(TestPlayerB), 0, TestPartition, chess::Domain<uint8_t, 6, double, 16>::getDomainName(TestDomain1), "0");
                    playW->load();
                    playB->load();

                    // Evolve
                    chess::BaseGame_Config cfg{ 1000, 1, 1000, 1, 20000, 10 };
                    //Param: num_iter, popsize, nbgen, _tournament_n_player, _tournament_n_game, verbose
                    chess::ga::ChessCoEvolveGA<uint8_t, 6, double, 16, 10> ga_co(playW, playB, cfg, 1, 5, 1, 2, 1, 1);
                    ga_co.run();

                    delete playW;
                    delete playB;
                }
                // ChessCoEvolveGA TestDomain2
                {
                    // Load TestDomain2 players
                    playW = new chess::DomainPlayer<uint8_t, 6, double, 16>(chess::PieceColor::W, std::string(TestPlayerW), 0, TestPartition, chess::Domain<uint8_t, 6, double, 16>::getDomainName(TestDomain2), "0");
                    playB = new chess::DomainPlayer<uint8_t, 6, double, 16>(chess::PieceColor::B, std::string(TestPlayerB), 0, TestPartition, chess::Domain<uint8_t, 6, double, 16>::getDomainName(TestDomain2), "0");
                    playW->load();
                    playB->load();

                    // Evolve
                    chess::BaseGame_Config cfg{ 1000, 1, 1000, 1, 20000, 30 };
                    chess::ga::ChessCoEvolveGA<uint8_t, 6, double, 16, 10> ga_co(playW, playB, cfg, 1, 5, 3, 10, 1, 2);
                    ga_co.run();

                    delete playW;
                    delete playB;
                }
            }

        }
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