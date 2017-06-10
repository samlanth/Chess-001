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
        chess::DomainPlayer<uint8_t, 8, double, 16>* play1 = new chess::DomainPlayer<uint8_t, 8, double, 16>(std::string("zeromind"), p_classic->name(), chess::Domain<uint8_t, 8, double, 16>::getDomainName(chess::eDomainName::KQvK), "0");
        play1->save();
        delete play1;

        chess::DomainPlayer<uint8_t, 8, double, 16>* play2 = new chess::DomainPlayer<uint8_t, 8, double, 16>(std::string("zeromind"), p_classic->name(), chess::Domain<uint8_t, 8, double, 16>::getDomainName(chess::eDomainName::KQvK), "0");
        play2->load();
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
            
            chess::DomainPlayer<uint8_t, 6, double, 16>* playW = new chess::DomainPlayer<uint8_t, 6, double, 16>(std::string("wmind"), p_classic6->name(), chess::Domain<uint8_t, 6, double, 16>::getDomainName(chess::eDomainName::KQvK), "0");
            chess::DomainPlayer<uint8_t, 6, double, 16>* playB = new chess::DomainPlayer<uint8_t, 6, double, 16>(std::string("bmind"), p_classic6->name(), chess::Domain<uint8_t, 6, double, 16>::getDomainName(chess::eDomainName::KQvK), "0");
            
            size_t nc = chess::FeatureManager<uint8_t, 6, double, 16>::instance()->count_cond_features();
            size_t nv = chess::FeatureManager<uint8_t, 6, double, 16>::instance()->count_valu_features();
            // add a node (and mirror) to brain of white player
            {
                chess::ConditionValuationNode<uint8_t, 6, double, 16>* child1 = new chess::ConditionValuationNode<uint8_t, 6, double, 16>(playW->get_root(), true, false);
                chess::ConditionValuationNode<uint8_t, 6, double, 16>* child2 = new chess::ConditionValuationNode<uint8_t, 6, double, 16>(playW->get_root(), false, false);
                size_t valid_v_feature = 0;
                chess::ValuationFeature<uint8_t, 6, double, 16>* v;
                for (size_t i = 0; i < nv; i++)
                {
                    v = chess::FeatureManager<uint8_t, 6, double, 16>::instance()->get_valu_feature(i);
                    if (playW->get_domain()->is_valu_feature_valid(*v))
                    {
                        valid_v_feature = i;
                        break;
                    }
                }
                size_t valid_v_feature2 = 0;
                chess::ValuationFeature<uint8_t, 6, double, 16>* v2;
                for (size_t i = 0; i < nv; i++)
                {
                    v2 = chess::FeatureManager<uint8_t, 6, double, 16>::instance()->get_valu_feature(i);
                    if (playB->get_domain()->is_valu_feature_valid(*v2))
                    {
                        valid_v_feature2 = i;
                        break;
                    }
                }
                child1->add_conditions(chess::FeatureManager<uint8_t, 6, double, 16>::instance()->get_cond_feature(0));
                child1->add_conditions_and_or(true);
                child1->add_valuations(chess::FeatureManager<uint8_t, 6, double, 16>::instance()->get_valu_feature(valid_v_feature));
                child1->add_weights(0.5);
                // mirror node reuse child1 cond
                child2->add_valuations(chess::FeatureManager<uint8_t, 6, double, 16>::instance()->get_valu_feature(valid_v_feature2));
                child2->add_weights(0.25);
            }

            // setup game
            chess::BaseGame<uint8_t, 6, double, 16> game(*playW, *playB);
            game.set_constraints(100, 2, 100, 2, 10);

            // play games
            double fit = 0;
            chess::ExactScore sc;
            for (size_t i = 0; i < 20; i++)
            {
                game.set_board(playW->get_domain()->get_random_position());
                sc = game.play(true);
                if      (sc == chess::ExactScore::WIN)  fit += 1.0;
                else if (sc == chess::ExactScore::LOSS) fit -= 1.0;
                else if (sc == chess::ExactScore::DRAW) fit += 0.5;
                std::cout << "score= " << fit << " / " << i + 1 << std::endl;
            }

            std::vector< chess::ConditionValuationNode<uint8_t, 6, double, 16>* > vnodes;
            playW->get_root()->get_term_nodes(vnodes);
            // Changing weights[] of these terminal nodes will change player performance
            // GA...

            chess::ga::ChessGeneticAlgorithm<uint8_t, 6, double, 16> ga_chess(playW->get_root(), 10, 5, 2, 1);
            ga_chess.run();

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