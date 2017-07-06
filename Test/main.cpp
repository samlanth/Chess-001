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
    srand((unsigned int)time(NULL));

    // Test TB
    {
        bool DO_BUILD = false;
        {
            chess::TB_Manager<uint8_t, 6>::instance()->clear();
            std::vector<uint8_t> ws;
            std::vector<uint8_t> bs;
            ws.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::W));
            ws.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::Q, chess::PieceColor::W));
            bs.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::B));
            std::vector<std::pair<uint8_t, uint8_t>> sw = chess::PieceSet<uint8_t, 6>::to_set(ws);
            std::vector<std::pair<uint8_t, uint8_t>> sb = chess::PieceSet<uint8_t, 6>::to_set(bs);
            chess::PieceSet<uint8_t, 6> ps(sw, sb);
            chess::TBH_N<uint8_t, 6, 3> TBH_KQvK(ps, chess::TB_TYPE::tb_2v1, chess::TBH_OPTION::try_load_on_build);
            if (DO_BUILD) TBH_KQvK.build(1);
            if (DO_BUILD) TBH_KQvK.save();
            TBH_KQvK.load();

            // check sym
            if (!DO_BUILD)
            {
                chess::TablebaseBase<uint8_t, 6>* sym_tb_w;
                chess::TablebaseBase<uint8_t, 6>* sym_tb_b;
                {
                    // ref is B
                    chess::PieceSet<uint8_t, 6> master(((chess::Tablebase<uint8_t, 6, 3>*)(TBH_KQvK.TB_B()))->piecesID());
                    chess::PieceSet<uint8_t, 6> reverse({ chess::PieceSet<uint8_t, 6>::reverse_color_set(master.bset()), chess::PieceSet<uint8_t, 6>::reverse_color_set(master.wset()) });
                    sym_tb_w = (chess::TablebaseBase<uint8_t, 6>*)new chess::SymmetryTablebase<uint8_t, 6, 3>(( chess::Tablebase<uint8_t, 6, 3>*)TBH_KQvK.TB_B_const());
                    chess::TB_Manager<uint8_t, 6>::instance()->add_sym(reverse.name(chess::PieceColor::W), sym_tb_w);
                    sym_tb_w->load();
                    std::cout << "check_score B KQvK vs sym W kVKQ: " << ((chess::SymmetryTablebase<uint8_t, 6, 3>*)(sym_tb_w))->check_score() << std::endl;
                }
                {
                    // ref is W
                    chess::PieceSet<uint8_t, 6> master(((chess::Tablebase<uint8_t, 6, 3>*)(TBH_KQvK.TB_W()))->piecesID());
                    chess::PieceSet<uint8_t, 6> reverse({ chess::PieceSet<uint8_t, 6>::reverse_color_set(master.bset()), chess::PieceSet<uint8_t, 6>::reverse_color_set(master.wset()) });
                    sym_tb_b = (chess::TablebaseBase<uint8_t, 6>*)new chess::SymmetryTablebase<uint8_t, 6, 3>((chess::Tablebase<uint8_t, 6, 3>*)TBH_KQvK.TB_W_const());
                    chess::TB_Manager<uint8_t, 6>::instance()->add_sym(reverse.name(chess::PieceColor::B), sym_tb_b);
                    sym_tb_b->load();
                    assert(sym_tb_b->is_build());
                    std::cout << "check_score W KQvK vs sym B kVKQ: " << ((chess::SymmetryTablebase<uint8_t, 6, 3>*)(sym_tb_b))->check_score() << std::endl;
                }

                std::vector<uint16_t> sq = { 26, 10, 31 }; // B to play in KvKQ (sym)
                std::cout << "B to play in KvKQ (sym) " << chess::ExactScore_to_string(sym_tb_b->score_v(sq)) << " vs LOSS ";
                for (size_t z = 0; z < sq.size(); z++) std::cout << (int)sq[z] << " "; std::cout << std::endl;
            }
        }

        {
            std::vector<uint8_t> ws1;
            std::vector<uint8_t> bs1;
            ws1.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::W));
            ws1.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::P, chess::PieceColor::W));
            bs1.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::B));
            std::vector<std::pair<uint8_t, uint8_t>> sw1 = chess::PieceSet<uint8_t, 6>::to_set(ws1);
            std::vector<std::pair<uint8_t, uint8_t>> sb1 = chess::PieceSet<uint8_t, 6>::to_set(bs1);
            chess::PieceSet<uint8_t, 6> ps1(sw1, sb1);
            chess::TBH_N<uint8_t, 6, 3> TBH_KPvK(ps1, chess::TB_TYPE::tb_2v1, chess::TBH_OPTION::try_load_on_build);
            if (DO_BUILD) TBH_KPvK.build(1);
            if (DO_BUILD) TBH_KPvK.save();
            TBH_KPvK.load();

            // check sym
            if (!DO_BUILD)
            {
                //ref is W
                chess::PieceSet<uint8_t, 6> master(((chess::Tablebase<uint8_t, 6, 3>*)(TBH_KPvK.TB_W()))->piecesID());
                chess::PieceSet<uint8_t, 6> reverse({ chess::PieceSet<uint8_t, 6>::reverse_color_set(master.bset()), chess::PieceSet<uint8_t, 6>::reverse_color_set(master.wset()) });
                chess::TablebaseBase<uint8_t, 6>* sym_tb = (chess::TablebaseBase<uint8_t, 6>*)new chess::SymmetryTablebase<uint8_t, 6, 3>(( chess::Tablebase<uint8_t, 6, 3>*)TBH_KPvK.TB_W_const());
                chess::TB_Manager<uint8_t, 6>::instance()->add_sym(reverse.name(chess::PieceColor::B), sym_tb);
                sym_tb->load();
                std::cout << "check_score B KPvK vs sym W KPvK: " << ((chess::SymmetryTablebase<uint8_t, 6, 3>*)(sym_tb))->check_score() << std::endl;

                assert(TBH_KPvK.TB_W()->is_build());
                std::vector<uint16_t> sqr = { 35 - 31, 35 - 26, 35 - 10 }; // w to play on KPvK
                std::cout << "W to play on KPvK " << chess::ExactScore_to_string(TBH_KPvK.TB_W()->score_v(sqr)) << " vs WIN ";
                for (size_t z = 0; z < sqr.size(); z++) std::cout << (int)sqr[z] << " "; std::cout << std::endl;
            }
            if (!DO_BUILD)
            {
                //ref is B
                chess::PieceSet<uint8_t, 6> master(((chess::Tablebase<uint8_t, 6, 3>*)(TBH_KPvK.TB_B()))->piecesID());
                chess::PieceSet<uint8_t, 6> reverse({ chess::PieceSet<uint8_t, 6>::reverse_color_set(master.bset()), chess::PieceSet<uint8_t, 6>::reverse_color_set(master.wset()) });
                chess::TablebaseBase<uint8_t, 6>* sym_tb = (chess::TablebaseBase<uint8_t, 6>*)new chess::SymmetryTablebase<uint8_t, 6, 3>((chess::Tablebase<uint8_t, 6, 3>*)TBH_KPvK.TB_B_const());
                chess::TB_Manager<uint8_t, 6>::instance()->add_sym(reverse.name(chess::PieceColor::W), sym_tb);
                sym_tb->load();
                std::cout << "check_score W KPvK vs sym B KPvK: " << ((chess::SymmetryTablebase<uint8_t, 6, 3>*)(sym_tb))->check_score() << std::endl;
            }
        }


        {
            // KQvKQ
            std::vector<uint8_t> w4;
            std::vector<uint8_t> b4;
            w4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::W));
            w4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::Q, chess::PieceColor::W));
            b4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::B));
            b4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::Q, chess::PieceColor::B));
            std::vector<std::pair<uint8_t, uint8_t>> sw4 = chess::PieceSet<uint8_t, 6>::to_set(w4);
            std::vector<std::pair<uint8_t, uint8_t>> sb4 = chess::PieceSet<uint8_t, 6>::to_set(b4);
            chess::PieceSet<uint8_t, 6> ps4(sw4, sb4);
            chess::TBH_N<uint8_t, 6, 4> TBH_4X(ps4, chess::TB_TYPE::tb_2v2, chess::TBH_OPTION::try_load_on_build);
            if (DO_BUILD) TBH_4X.build(1);
            if (DO_BUILD) TBH_4X.save();
            TBH_4X.load();
        }

        {
            // KPvKQ
            std::vector<uint8_t> w4;
            std::vector<uint8_t> b4;
            w4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::W));
            w4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::P, chess::PieceColor::W));
            b4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::B));
            b4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::Q, chess::PieceColor::B));
            std::vector<std::pair<uint8_t, uint8_t>> sw4 = chess::PieceSet<uint8_t, 6>::to_set(w4);
            std::vector<std::pair<uint8_t, uint8_t>> sb4 = chess::PieceSet<uint8_t, 6>::to_set(b4);
            chess::PieceSet<uint8_t, 6> ps4(sw4, sb4);
            chess::TBH_N<uint8_t, 6, 4> TBH_4X(ps4, chess::TB_TYPE::tb_2v2, chess::TBH_OPTION::try_load_on_build);
            if (DO_BUILD) TBH_4X.build(1);
            if (DO_BUILD) TBH_4X.save();
            TBH_4X.load();
        }

        // KPPvK
        {
            std::vector<uint8_t> w4;
            std::vector<uint8_t> b4;
            w4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::W));
            w4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::P, chess::PieceColor::W));
            w4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::P, chess::PieceColor::W));
            b4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::B));
            std::vector<std::pair<uint8_t, uint8_t>> sw4 = chess::PieceSet<uint8_t, 6>::to_set(w4);
            std::vector<std::pair<uint8_t, uint8_t>> sb4 = chess::PieceSet<uint8_t, 6>::to_set(b4);
            chess::PieceSet<uint8_t, 6> ps4(sw4, sb4);
            chess::TBH_N<uint8_t, 6, 4> TBH_KPPvK(ps4, chess::TB_TYPE::tb_3v1, chess::TBH_OPTION::try_load_on_build);
            if (DO_BUILD) TBH_KPPvK.build(1);
            if (DO_BUILD) TBH_KPPvK.save();
            TBH_KPPvK.load();
        }

        // KQvKP
        {
            std::vector<uint8_t> w4;
            std::vector<uint8_t> b4;
            w4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::W));
            w4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::Q, chess::PieceColor::W));
            b4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::B));
            b4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::P, chess::PieceColor::B));
            std::vector<std::pair<uint8_t, uint8_t>> sw4 = chess::PieceSet<uint8_t, 6>::to_set(w4);
            std::vector<std::pair<uint8_t, uint8_t>> sb4 = chess::PieceSet<uint8_t, 6>::to_set(b4);
            chess::PieceSet<uint8_t, 6> ps4(sw4, sb4);
            chess::TBH_N<uint8_t, 6, 4> TBH_4X(ps4, chess::TB_TYPE::tb_2v2, chess::TBH_OPTION::try_load_on_build);
            std::vector<chess::PieceSet<uint8_t, 6>> vz = ps4.get_all_child();
            if (DO_BUILD) TBH_4X.build(1);
            if (DO_BUILD) TBH_4X.save();
            TBH_4X.load();

            std::vector<uint16_t> sq = { 35, 11, 16, 15 }; // B to play on KQvKP
            std::cout << "B to play on KQvKP " << chess::ExactScore_to_string(TBH_4X.TB_B()->score_v(sq)) << " vs LOSS ";
            for (size_t z = 0; z < sq.size(); z++) std::cout << (int)sq[z] << " "; std::cout << std::endl;

            if (TBH_4X.TB_B()->score_v(sq) != chess::ExactScore::LOSS)
            {
                chess::Board<uint8_t, 6> work_board;
                work_board.clear();
                work_board.set_color(chess::PieceColor::B);
                for (size_t z = 0; z < sq.size(); z++) work_board.set_pieceid_at(chess::PieceSet<uint8_t, 6>::ps_to_pieces(ps4)[z], sq[z]);
                std::cout << work_board.to_str() << std::endl;

                {
                    chess::Board<uint8_t, 6> work_board;
                    work_board.clear();
                    work_board.set_color(chess::PieceColor::B);
                    for (size_t z = 0; z < sq.size(); z++) work_board.set_pieceid_at(chess::PieceSet<uint8_t, 6>::ps_to_pieces(ps4)[z], sq[z]);
                    std::cout << work_board.to_str() << std::endl;

                    chess::Board<uint8_t, 6> b = work_board;
                    chess::Board<uint8_t, 6> bt = work_board;
                    chess::TablebaseUtil<uint8_t, 6> tbu;
                    std::cout << "===================================" << std::endl;
                    tbu.expand_position(bt, 1);
                    std::vector<chess::Move<uint8_t>>  m = b.generate_moves();
                    std::list<chess::Move<uint8_t>>  mh = bt.get_history_moves();
                    int n = 0;
                    while (!b.is_final(m))
                    {
                        if (mh.size() == 0) break;
                        b.apply_move(mh.front()); mh.pop_front();
                        std::cout << b.to_str() << std::endl << std::endl;
                        m = b.generate_moves();
                        n++;
                        if (n > 10) break;
                    }
                }
            }

            sq = { 35, 6, 16, 15 }; // W to play on KQvKP
            std::cout << "W to play on KQvKP " << chess::ExactScore_to_string(TBH_4X.TB_W()->score_v(sq)) << " vs WIN ";
            for (size_t z = 0; z < sq.size(); z++) std::cout << (int)sq[z] << " "; std::cout << std::endl;
        }

        // KPvKP
        std::vector<uint8_t> w4;
        std::vector<uint8_t> b4;
        w4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::W));
        w4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::P, chess::PieceColor::W));
        b4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::B));
        b4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::P, chess::PieceColor::B));
        std::vector<std::pair<uint8_t, uint8_t>> sw4 = chess::PieceSet<uint8_t, 6>::to_set(w4);
        std::vector<std::pair<uint8_t, uint8_t>> sb4 = chess::PieceSet<uint8_t, 6>::to_set(b4);
        chess::PieceSet<uint8_t, 6> ps4(sw4, sb4);
        chess::TBH_N<uint8_t, 6, 4> TBH_KPvKP(ps4, chess::TB_TYPE::tb_2v2, chess::TBH_OPTION::try_load_on_build);
        std::vector<chess::PieceSet<uint8_t, 6>> vz = ps4.get_all_child();
        if (DO_BUILD) TBH_KPvKP.build(1);
        if (DO_BUILD) TBH_KPvKP.save();
        TBH_KPvKP.load();

        // KPPvK
        {
            std::vector<uint8_t> w4;
            std::vector<uint8_t> b4;
            w4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::W));
            w4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::P, chess::PieceColor::W));
            w4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::P, chess::PieceColor::W));
            b4.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::B));
            std::vector<std::pair<uint8_t, uint8_t>> sw4 = chess::PieceSet<uint8_t, 6>::to_set(w4);
            std::vector<std::pair<uint8_t, uint8_t>> sb4 = chess::PieceSet<uint8_t, 6>::to_set(b4);
            chess::PieceSet<uint8_t, 6> ps4(sw4, sb4);
            chess::TBH_N<uint8_t, 6, 4> TBH_KPPvK(ps4, chess::TB_TYPE::tb_3v1, chess::TBH_OPTION::try_load_on_build);
            if (DO_BUILD) TBH_KPPvK.build(1);
            if (DO_BUILD) TBH_KPPvK.save();
            TBH_KPPvK.load();
        }

        // expand_position()
        for (int i = 0; i < 1; i++)
        {
            chess::Board<uint8_t, 6> b;
            chess::Board<uint8_t, 6> bcopy;
            chess::Board<uint8_t, 6> bt = b.get_random_position_KPKP(true);
            bcopy = b = bt; // save initial pos
            chess::TablebaseUtil<uint8_t, 6> tbu;

            std::cout << "===================================" << std::endl;
            std::cout << bt.to_str() << std::endl << std::endl;
            tbu.expand_position(bt, 1);

            std::vector<chess::Move<uint8_t>>  m = b.generate_moves();
            std::list<chess::Move<uint8_t>>  mh = bt.get_history_moves();
            int n = 0;
            while (!b.is_final(m))
            {
                if (mh.size() == 0) break;
                b.apply_move(mh.front()); mh.pop_front();
                std::cout << b.to_str() << std::endl << std::endl;
                m = b.generate_moves();
                n++;
                if (n > 30) break;
            }

            // replay
            //bt = bcopy;
            //n = 0;
            //tbu.expand_position(bt, 1);
            //std::cout << b.to_str() << std::endl << std::endl;
            //m = b.generate_moves();
            //mh = bt.get_history_moves();
            //while (!b.is_final(m))
            //{
            //    if (mh.size() == 0) break;
            //    b.apply_move(mh.front()); mh.pop_front();
            //    std::cout << b.to_str() << std::endl << std::endl;
            //    m = b.generate_moves();
            //    n++;
            //    if (n > 30) break;
            //}
        }
    }

    // Test GA
    // Prepare players for GA
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
