#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TestBoard<PieceID, _BoardSize>
//
// TestBoard contains unit tests for Board
//
//
#ifndef _AL_CHESS_TEST_TESTBOARD_H
#define _AL_CHESS_TEST_TESTBOARD_H

namespace chess
{
    namespace test
    {
        template <typename PieceID, typename uint8_t _BoardSize>
        class TestBoard
        {
            using _Piece = Piece<PieceID, _BoardSize>;
            using _Move = Move<PieceID>;
            using _Board = Board<PieceID, _BoardSize>;

        public:
            char _verbose;
            std::string _name;

            TestBoard(std::string name) : _name(name) 
            {
                std::stringstream ss;
                ss << "TestBoard (" << _name << ") enter";
                unittest::Logger::instance()->log(ss.str() );
            }

            ~TestBoard()
            {
                std::stringstream ss;
                ss << "TestBoard (" << _name << ") exit";
                unittest::Logger::instance()->log(ss.str());
            }

            bool check_000(uint32_t) // test default ct()
            {
                _Board::reset_to_default_option();
                _Board board;
                return board.get_color() == chess::PieceColor::none;
            }

            bool check_001(uint32_t) // test generate_moves
            {
                _Board::reset_to_default_option();
                _Board board(true);

                std::vector<_Move> m = board.generate_moves();
                if (_BoardSize == 8) return (m.size() == 20);
                else return true; // ignore real count
            }

            bool check_002(uint32_t) // test promo
            {
                _Board::reset_to_default_option();
                _Board board(true);
                if (_BoardSize < 8) return false;

                board.set_pieceid_at(_Piece::get_id(chess::PieceName::P, chess::PieceColor::B), 2, 2);
                board.set_pieceid_at(_Piece::get_id(chess::PieceName::none, chess::PieceColor::none), 2, 7);
                board.set_pieceid_at(_Piece::get_id(chess::PieceName::P, chess::PieceColor::W), 2, 6);
                std::vector<_Move> m = board.generate_moves();
                if ((_BoardSize == 8)&&(!_Board::promo_Q_only())) return (m.size() == 20 + 12 - 2 + 2);
                else if ((_BoardSize == 8) && (_Board::promo_Q_only())) return (m.size() == 20 + 3 - 2 + 2);
                else return true; // ignore
            }

            bool check_003a(uint32_t param) // test undo_move (allow_self_check = true)
            {
                _Board::reset_to_default_option();
                return check_003(param, true);
            }
            bool check_003b(uint32_t param) // test undo_move (allow_self_check = false)
            {
                _Board::reset_to_default_option();
                return check_003(param, false);
            }
            bool check_003(uint32_t param, bool allow_self_check) // test undo_move
            {
                _Board::reset_to_default_option();
                _Board board(true); _Board::set_allow_self_check(allow_self_check);
 
                std::vector<_Move> m;
                m = board.generate_moves();
                size_t cnt_move_beg = m.size();

                size_t ret_index;
                size_t mv_index;
                for (size_t i = 0; i < 100; i++)
                {
                    if (board.is_final(m)) 
                        break;

                    m = board.generate_moves();
                    if (_verbose) std::cout << "Number moves = " << m.size() << std::endl;
                    if (m.size() == 0) break;

                    // if can capture K, shoud do it!
                    if (board.can_capture_opposite_king(m, ret_index))
                    {
                        mv_index = ret_index;
                    }
                    else
                    {
                        // random move
                        mv_index = std::rand() % m.size();
                    }
                    if (mv_index >= 0)
                    {
                        _Move mv = m[mv_index];
                        board.apply_move(mv);
                        if (_verbose) std::cout << board.to_str() << std::endl << std::endl;
                    }
                }

                std::list<_Move> h = board.get_history_moves();
                for (size_t i = 0; i < h.size(); i++)
                {
                    board.undo_move();
                    if (_verbose) std::cout << board.to_str() << std::endl << std::endl;
                }

                m = board.generate_moves();
                size_t cnt_move_end = m.size();

                return (cnt_move_beg == cnt_move_end);
            }

            bool check_004(uint32_t) // test cnt_piece
            {
                _Board::reset_to_default_option();
                _Board board(true);
                return (board.cnt_piece(PieceName::P, PieceColor::W) == 8);
            }

            bool do_test(const unittest::cmd_parser& cmd)
            {
                unittest::TTest<TestBoard<PieceID, _BoardSize>> tester = unittest::TTest<TestBoard<PieceID, _BoardSize>>();
                this->_verbose = cmd.has_option("-v");

                uint32_t    id = 0;
                tester.add_test(this, &TestBoard::check_000,  id++, "err000",  "default ct()");
                tester.add_test(this, &TestBoard::check_001,  id++, "err001",  "generate_moves()");
                tester.add_test(this, &TestBoard::check_002,  id++, "err002",  "check promo");
                tester.add_test(this, &TestBoard::check_003a, id++, "err003a", "undo_move (allow_self_check = true)");
                tester.add_test(this, &TestBoard::check_003b, id++, "err003b", "undo_move (allow_self_check = false)");
                tester.add_test(this, &TestBoard::check_004,  id++, "err004",  "cnt_piece()");

                bool ret = tester.run();
                if (cmd.has_option("-r"))
                { 
                    std::string rep = tester.report(true);
                    std::cout << _name << std::endl;
                    std::cout << rep << std::endl;

                    if (_verbose)
                    {
                        std::cout << "press any key to continu...";
                        int a = _getch();
                        std::cout << std::endl;
                    }
                }
                return ret;
            }
        };
    };
};
#endif
