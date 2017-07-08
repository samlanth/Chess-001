//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Executable
// Unit testing
//
//
#include "core/chess.hpp"

int main(int argc, char* argv[])
{
    // Test TB
    {
        bool DO_BUILD = true;
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
            //if (!DO_BUILD)
            //{
            //    chess::TablebaseBase<uint8_t, 6>* sym_tb_w;
            //    chess::TablebaseBase<uint8_t, 6>* sym_tb_b;
            //    {
            //        // ref is B
            //        chess::PieceSet<uint8_t, 6> master(((chess::Tablebase<uint8_t, 6, 3>*)(TBH_KQvK.TB_B()))->piecesID());
            //        chess::PieceSet<uint8_t, 6> reverse({ chess::PieceSet<uint8_t, 6>::reverse_color_set(master.bset()), chess::PieceSet<uint8_t, 6>::reverse_color_set(master.wset()) });
            //        sym_tb_w = (chess::TablebaseBase<uint8_t, 6>*)new chess::SymmetryTablebase<uint8_t, 6, 3>(( chess::Tablebase<uint8_t, 6, 3>*)TBH_KQvK.TB_B_const());
            //        chess::TB_Manager<uint8_t, 6>::instance()->add_sym(reverse.name(chess::PieceColor::W), sym_tb_w);
            //        sym_tb_w->load();
            //        std::cout << "check_score B KQvK vs sym W kVKQ: " << ((chess::SymmetryTablebase<uint8_t, 6, 3>*)(sym_tb_w))->check_score() << std::endl;
            //    }
            //    {
            //        // ref is W
            //        chess::PieceSet<uint8_t, 6> master(((chess::Tablebase<uint8_t, 6, 3>*)(TBH_KQvK.TB_W()))->piecesID());
            //        chess::PieceSet<uint8_t, 6> reverse({ chess::PieceSet<uint8_t, 6>::reverse_color_set(master.bset()), chess::PieceSet<uint8_t, 6>::reverse_color_set(master.wset()) });
            //        sym_tb_b = (chess::TablebaseBase<uint8_t, 6>*)new chess::SymmetryTablebase<uint8_t, 6, 3>((chess::Tablebase<uint8_t, 6, 3>*)TBH_KQvK.TB_W_const());
            //        chess::TB_Manager<uint8_t, 6>::instance()->add_sym(reverse.name(chess::PieceColor::B), sym_tb_b);
            //        sym_tb_b->load();
            //        assert(sym_tb_b->is_build());
            //        std::cout << "check_score W KQvK vs sym B kVKQ: " << ((chess::SymmetryTablebase<uint8_t, 6, 3>*)(sym_tb_b))->check_score() << std::endl;
            //    }

            //    std::vector<uint16_t> sq = { 26, 10, 31 }; // B to play in KvKQ (sym)
            //    std::cout << "B to play in KvKQ (sym) " << chess::ExactScore_to_string(sym_tb_b->score_v(sq)) << " vs LOSS ";
            //    for (size_t z = 0; z < sq.size(); z++) std::cout << (int)sq[z] << " "; std::cout << std::endl;
            //}
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

        // KPPvKP
        {
            std::vector<uint8_t> w5;
            std::vector<uint8_t> b5;
            w5.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::W));
            w5.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::P, chess::PieceColor::W));
            w5.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::P, chess::PieceColor::W));
            b5.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::K, chess::PieceColor::B));
            b5.push_back(chess::Piece<uint8_t, 6>::get_id(chess::PieceName::P, chess::PieceColor::B));
            std::vector<std::pair<uint8_t, uint8_t>> sw5 = chess::PieceSet<uint8_t, 6>::to_set(w5);
            std::vector<std::pair<uint8_t, uint8_t>> sb5 = chess::PieceSet<uint8_t, 6>::to_set(b5);
            chess::PieceSet<uint8_t, 6> ps5(sw5, sb5);
            chess::TBH_N<uint8_t, 6, 5> TBH_KPPvKP(ps5, chess::TB_TYPE::tb_3v2, chess::TBH_OPTION::try_load_on_build);
            if (DO_BUILD) TBH_KPPvKP.build(1);
            if (DO_BUILD) TBH_KPPvKP.save();
            TBH_KPPvKP.load();
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


    return 0;
}
