// --------------------------------
// Author: Alain Lanthier, 2017
// --------------------------------
#ifndef _AL_CHESS_TEST_TESTBOARD_H
#define _AL_CHESS_TEST_TESTBOARD_H

#include "boardfunc.h"
#include "board.h"
#include "piece.h"
#include "unittest.h"

namespace chess
{
	namespace test
	{
		using namespace unittest;

		template <typename PieceID, typename uint8_t _BoardSize>
		class TestBoard
		{
		public:
			TestBoard() {}

			using _Piece		= chess::Piece<PieceID, _BoardSize>;
			using _Move			= chess::Move<PieceID>;
			using _Board		= chess::Board<PieceID, _BoardSize>;
			using _BoardFunc	= chess::BoardFunc<PieceID, _BoardSize>;

			bool check_000(uint32_t) // test default ct()
			{
				_Board board;
				return board.get_color() == chess::PieceColor::none;
			}

			bool check_001(uint32_t) // test generate_moves
			{
				_Board board(true);

				std::vector<_Move> m;
				m = board.generate_moves();

				if (_BoardSize == 8) return (m.size() == 20);
				else return true; // ignore real count
			}

			bool check_002(uint32_t) // test promo
			{
				_Board board(true);
				std::vector<_Move> m;

				board.set_pieceid_at(_Piece::get_id(chess::PieceName::P, chess::PieceColor::B), 2, 2);
				board.set_pieceid_at(_Piece::get_id(chess::PieceName::none, chess::PieceColor::none), 2, 7);
				board.set_pieceid_at(_Piece::get_id(chess::PieceName::P, chess::PieceColor::W), 2, 6);
				m = board.generate_moves();
				if (_BoardSize == 8) return (m.size() == 20 + 12 - 2 + 2);
				else return true; // ignore
			}

			bool check_003(uint32_t) // test undo_move
			{
				_Board board(true,true);
                _BoardFunc bf(board);

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
                    std::cout << "Moves=" << m.size() << std::endl;
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

                        std::cout << bf.to_str() << std::endl; 
					}
				}

				std::list<_Move> h = board.get_history_moves();
				for (size_t i = 0; i < h.size(); i++)
				{
					board.undo_move();

                    std::cout << bf.to_str() << std::endl;
				}

				m = board.generate_moves();
				size_t cnt_move_end = m.size();

				return (cnt_move_beg == cnt_move_end);
			}

			bool check_004(uint32_t) // test cnt piece
			{
				_Board board(true);
				_BoardFunc bf(board);
				return (bf.cnt_piece(PieceName::P, PieceColor::W) == 8);
			}

			bool do_test(bool display = true)
			{
				bool        ret = true;
				uint32_t    id = 0;
				unittest::TTest<TestBoard<PieceID, _BoardSize>> tester = unittest::TTest<TestBoard<PieceID, _BoardSize>>();

                TestBoard test_obj{};
				tester.add(&test_obj, &TestBoard::check_000, id++, "err000");
				tester.add(&test_obj, &TestBoard::check_001, id++, "err001");
				tester.add(&test_obj, &TestBoard::check_002, id++, "err002");
				tester.add(&test_obj, &TestBoard::check_003, id++, "err003");
				tester.add(&test_obj, &TestBoard::check_004, id++, "err004");
				ret = tester.run();
				if (display) { std::cout << tester.report(true) << std::endl; }
				return ret;
			}
		};
	};
};
#endif
