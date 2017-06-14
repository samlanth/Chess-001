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

    // Test PartitionManager
    chess::PartitionManager<uint8_t, 8, double, 16>::instance()->make_classic_partition();
    chess::Partition<uint8_t, 8, double, 16>* p_classic = chess::PartitionManager<uint8_t, 8, double, 16>::instance()->find_partition("classic8");
    assert(p_classic != nullptr);


    // Test PersisteManager
    chess::PersistManager::instance();

    // Test FeatureManager
    chess::FeatureManager<uint16_t, 6, float, 32>::instance();

    // Test feature
    {
        chess::ConditionFeature_isOppositeKinCheck<uint8_t, 8, double, 16> fc;
        chess::ValuationFeature_numberMoveForPiece<uint8_t, 8, double, 16> fv(chess::PieceName::K, chess::PieceColor::W);
    }

    // Test DomainPlayer
    {
        chess::DomainPlayer<uint8_t, 8, double, 16>* play1 = new chess::DomainPlayer<uint8_t, 8, double, 16>(chess::PieceColor::W, std::string("wzeromind"), p_classic->name(), chess::Domain<uint8_t, 8, double, 16>::getDomainName(chess::eDomainName::KQvK), "0");
        play1->save();

        chess::GameDB<uint8_t, 8, double, 16>* db = play1->get_domain()->get_game_db();
        assert(db != nullptr);
        delete play1;

        chess::DomainPlayer<uint8_t, 8, double, 16>* play2 = new chess::DomainPlayer<uint8_t, 8, double, 16>(chess::PieceColor::B, std::string("bzeromind"), p_classic->name(), chess::Domain<uint8_t, 8, double, 16>::getDomainName(chess::eDomainName::KQvK), "0");
        play2->load();
        play2->save();
        delete play2;
    }

    // Test game
    {
        // Play few games        
        // 6x6 board
        {
            chess::PartitionManager<uint8_t, 6, double, 16>::instance()->make_classic_partition();
            chess::Partition<uint8_t, 6, double, 16>* p_classic6 = chess::PartitionManager<uint8_t, 6, double, 16>::instance()->find_partition("classic6");
            assert(p_classic6 != nullptr);
            
            chess::DomainPlayer<uint8_t, 6, double, 16>* playW = new chess::DomainPlayer<uint8_t, 6, double, 16>(chess::PieceColor::W, std::string("wmind"), p_classic6->name(), chess::Domain<uint8_t, 6, double, 16>::getDomainName(chess::eDomainName::KQvK), "0");
            chess::DomainPlayer<uint8_t, 6, double, 16>* playB = new chess::DomainPlayer<uint8_t, 6, double, 16>(chess::PieceColor::B, std::string("bmind"), p_classic6->name(), chess::Domain<uint8_t, 6, double, 16>::getDomainName(chess::eDomainName::KQvK), "0");
            
            size_t nc = chess::FeatureManager<uint8_t, 6, double, 16>::instance()->count_cond_features();
            size_t nv = chess::FeatureManager<uint8_t, 6, double, 16>::instance()->count_valu_features();
            // add a node (and mirror) to brain of white player
            {
                chess::ConditionValuationNode<uint8_t, 6, double, 16>* child1 = new chess::ConditionValuationNode<uint8_t, 6, double, 16>(playW->get_root(), true, false);
                chess::ConditionValuationNode<uint8_t, 6, double, 16>* child2 = new chess::ConditionValuationNode<uint8_t, 6, double, 16>(playW->get_root(), false, false);
                chess::ValuationFeature<uint8_t, 6, double, 16>* v;
                for (size_t i = 0; i < nv; i++)
                {
                    v = chess::FeatureManager<uint8_t, 6, double, 16>::instance()->get_valu_feature(i);
                    if (playW->get_domain()->is_valu_feature_valid(*v))
                    {
                        child1->add_valuations(chess::FeatureManager<uint8_t, 6, double, 16>::instance()->get_valu_feature(i));
                        child1->add_weights(0.5);
                    }
                }
                chess::ValuationFeature<uint8_t, 6, double, 16>* v2;
                for (size_t i = 0; i < nv; i++)
                {
                    v2 = chess::FeatureManager<uint8_t, 6, double, 16>::instance()->get_valu_feature(i);
                    if (playW->get_domain()->is_valu_feature_valid(*v2))
                    {
                        child2->add_valuations(chess::FeatureManager<uint8_t, 6, double, 16>::instance()->get_valu_feature(i));
                        child2->add_weights(0.25);
                    }
                }
                child1->add_conditions(chess::FeatureManager<uint8_t, 6, double, 16>::instance()->get_cond_feature(0));
                child1->add_conditions_and_or(true);
                // mirror node reuse child1 cond
            }
            // add a node (and mirror) to brain of black player
            {
                chess::ConditionValuationNode<uint8_t, 6, double, 16>* child1 = new chess::ConditionValuationNode<uint8_t, 6, double, 16>(playB->get_root(), true, false);
                chess::ConditionValuationNode<uint8_t, 6, double, 16>* child2 = new chess::ConditionValuationNode<uint8_t, 6, double, 16>(playB->get_root(), false, false);
                chess::ValuationFeature<uint8_t, 6, double, 16>* v;
                for (size_t i = 0; i < nv; i++)
                {
                    v = chess::FeatureManager<uint8_t, 6, double, 16>::instance()->get_valu_feature(i);
                    if (playB->get_domain()->is_valu_feature_valid(*v))
                    {
                        child1->add_valuations(chess::FeatureManager<uint8_t, 6, double, 16>::instance()->get_valu_feature(i));
                        child1->add_weights(0.45);
                    }
                }
                chess::ValuationFeature<uint8_t, 6, double, 16>* v2;
                for (size_t i = 0; i < nv; i++)
                {
                    v2 = chess::FeatureManager<uint8_t, 6, double, 16>::instance()->get_valu_feature(i);
                    if (playB->get_domain()->is_valu_feature_valid(*v2))
                    {
                        child2->add_valuations(chess::FeatureManager<uint8_t, 6, double, 16>::instance()->get_valu_feature(i));
                        child2->add_weights(0.55);
                    }
                }
                child1->add_conditions(chess::FeatureManager<uint8_t, 6, double, 16>::instance()->get_cond_feature(0));
                child1->add_conditions_and_or(true);
                // mirror node reuse child1 cond
            }

            playW->save();
            playB->save();

            // Test ChessGeneticAlgorithm
            //{
            //    chess::BaseGame_Config cfg{ 100, 1, 100, 1, 4 };
            //    chess::ga::ChessGeneticAlgorithm<uint8_t, 6, double, 16, 10> ga_chess(true, false, playW, playB, cfg, 5, 2, 3, 2, true);
            //    ga_chess.run(false);
            //}

            // Test ChessCoEvolveGA
            {
                chess::BaseGame_Config cfg{ 100, 1, 100, 1, 20 };
                // num_iter, popsize, nbgen, _tournament_n_player, _tournament_n_game, verbose
                chess::ga::ChessCoEvolveGA<uint8_t, 6, double, 16, 10> ga_co(playW, playB, cfg, 500, 3, 1, 2, 1, 1);
                ga_co.run();
            }

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