// --------------------------------
// Author: Alain Lanthier, 2017
// --------------------------------
#ifndef _BOARD_H
#define _BOARD_H

#include "piece.h"
#include <list>

namespace chess
{
	template <typename PieceID, typename uint8_t _BoardSize> class Piece;
	template <typename PieceID, typename uint8_t _BoardSize> class Board;
	template <typename PieceID, typename uint8_t _BoardSize> class BoardFunc;

	template <typename PieceID, typename uint8_t _BoardSize>
	class Board
	{
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        friend class BoardFunc<PieceID, _BoardSize>;

        public:
        Board(  bool set_classic                = false,
                bool allow_self_check           = true,
                bool check_repeating_move_draw  = false,
                bool check_50_moves_draw        = false)
            : b(    _BoardSize*_BoardSize), 
                    _allow_self_check(allow_self_check),
                    _check_repeating_move_draw(check_repeating_move_draw),
                    _check_50_moves_draw(check_50_moves_draw)
		{
			color_toplay = PieceColor::none;
			for (uint8_t i = 0; i < _BoardSize; i++)
				for (uint8_t j = 0; j < _BoardSize; j++)
					b.at(index_at(i, j)) = _Piece::empty_id(); // default

			if (set_classic) set_classic_pos();
		}

		Board(const Board&) = default;
		Board & operator=(const Board &) = default;
		Board(Board&&) = default;
		~Board() = default;

		uint8_t index_at(uint8_t x, uint8_t y) const { return _BoardSize * y + x; }
		_Piece* piece_at(uint8_t x, uint8_t y) const { return _Piece::get(b.at(index_at(x, y))); }

		PieceID get_pieceid_at(uint8_t x, uint8_t y) const { return b.at(index_at(x, y)); }
		void    set_pieceid_at(PieceID id, uint8_t x, uint8_t y) { b.at(index_at(x, y)) = id; }

		PieceColor get_color() const { return color_toplay; }
		PieceColor get_opposite_color() const
		{
			if (color_toplay == PieceColor::B) return PieceColor::W;
			else if (color_toplay == PieceColor::W) return PieceColor::B;
			return PieceColor::none;
		}
		void set_opposite_color() {
			if (color_toplay == PieceColor::B) color_toplay = PieceColor::W;
			else if (color_toplay == PieceColor::W) color_toplay = PieceColor::B;
		}

		void apply_move(const _Move& m)
		{
			b.at(index_at(m.dst_x, m.dst_y)) = b.at(index_at(m.src_x, m.src_y));
			b.at(index_at(m.src_x, m.src_y)) = _Piece::empty_id();
			if ((m.mu.flag_spec == "y5_ep") || (m.mu.flag_spec == "y2_ep"))
			{
				b.at(index_at(m.src_x + m.mu.x * 1, m.src_y - m.mu.y * 1)) = _Piece::empty_id();
			}
			else if (m.mu.context_extra == "Q") b.at(index_at(m.dst_x, m.dst_y)) = _Piece::get_id(PieceName::Q, color_toplay);
			else if (m.mu.context_extra == "R") b.at(index_at(m.dst_x, m.dst_y)) = _Piece::get_id(PieceName::R, color_toplay);
			else if (m.mu.context_extra == "B") b.at(index_at(m.dst_x, m.dst_y)) = _Piece::get_id(PieceName::B, color_toplay);
			else if (m.mu.context_extra == "N") b.at(index_at(m.dst_x, m.dst_y)) = _Piece::get_id(PieceName::N, color_toplay);
			history_moves.push_back(m);
			set_opposite_color();
		}

		void undo_move()
		{
			if (history_moves.size() == 0) return;

			_Move m = history_moves.back();
			history_moves.pop_back();

			b.at(index_at(m.src_x, m.src_y)) = m.prev_src_id;
			b.at(index_at(m.dst_x, m.dst_y)) = m.prev_dst_id;
			if ((m.mu.flag_spec == "y5_ep") || (m.mu.flag_spec == "y2_ep"))
			{
				if (history_moves.size() > 0)
				{
					_Move mh = history_moves.back();
					b.at(index_at(mh.dst_x, mh.dst_y)) = mh.prev_src_id;
				}
				else
				{
					//..
				}
			}
			set_opposite_color();
		}

		bool can_capture_opposite_king(const std::vector<_Move>& m) const
		{
			PieceID K_id = _Piece::get_id(PieceName::K, get_opposite_color());
			for (auto &mv : m)
			{
				if (mv.prev_dst_id == K_id)
					return true;
			}
			return false;
		}

		void set_classic_pos()
		{
			if (_BoardSize < 8) return;	// throw...

			color_toplay = PieceColor::W;
			history_moves.clear();

			PieceID emptyid = _Piece::empty_id();
			for (uint8_t i = 0; i <_BoardSize; i++)
				for (uint8_t j = 0; j <_BoardSize; j++)
					set_pieceid_at(emptyid, i, j);

			set_pieceid_at(_Piece::get_id(chess::PieceName::R, chess::PieceColor::W), 0, 0);
			set_pieceid_at(_Piece::get_id(chess::PieceName::N, chess::PieceColor::W), 1, 0);
			set_pieceid_at(_Piece::get_id(chess::PieceName::B, chess::PieceColor::W), 2, 0);
			set_pieceid_at(_Piece::get_id(chess::PieceName::Q, chess::PieceColor::W), 3, 0);
			set_pieceid_at(_Piece::get_id(chess::PieceName::K, chess::PieceColor::W), 4, 0);
			set_pieceid_at(_Piece::get_id(chess::PieceName::B, chess::PieceColor::W), 5, 0);
			set_pieceid_at(_Piece::get_id(chess::PieceName::N, chess::PieceColor::W), 6, 0);
			set_pieceid_at(_Piece::get_id(chess::PieceName::R, chess::PieceColor::W), 7, 0);
			for (uint8_t i = 0; i<8; i++) set_pieceid_at(_Piece::get_id(chess::PieceName::P, chess::PieceColor::W), i, 1);

			set_pieceid_at(_Piece::get_id(chess::PieceName::R, chess::PieceColor::B), 0, 7);
			set_pieceid_at(_Piece::get_id(chess::PieceName::N, chess::PieceColor::B), 1, 7);
			set_pieceid_at(_Piece::get_id(chess::PieceName::B, chess::PieceColor::B), 2, 7);
			set_pieceid_at(_Piece::get_id(chess::PieceName::Q, chess::PieceColor::B), 3, 7);
			set_pieceid_at(_Piece::get_id(chess::PieceName::K, chess::PieceColor::B), 4, 7);
			set_pieceid_at(_Piece::get_id(chess::PieceName::B, chess::PieceColor::B), 5, 7);
			set_pieceid_at(_Piece::get_id(chess::PieceName::N, chess::PieceColor::B), 6, 7);
			set_pieceid_at(_Piece::get_id(chess::PieceName::R, chess::PieceColor::B), 7, 7);
			for (uint8_t i = 0; i<8; i++) set_pieceid_at(_Piece::get_id(chess::PieceName::P, chess::PieceColor::B), i, 6);
		}

		std::vector<_Move> generate_moves(bool is_recursive_call = false)
		{
			_Piece*  p_src;
			_Piece*  p_dst;
			std::vector<_Move> m;

			for (uint8_t i = 0; i < _BoardSize; i++)
			{
				for (uint8_t j = 0; j < _BoardSize; j++)
				{
					p_src = this->piece_at(i, j);

					if (p_src->color != this->color_toplay) continue;
					if (p_src->name != PieceName::none) // src piece exist
					{
						for(auto &mu : p_src->moves)
						{
							bool prev_path_empty = true;
							for (uint8_t n = 1; n <= mu.len; n++)
							{
								if ((prev_path_empty) &&
									(i + mu.x*n >= 0) && (i + mu.x*n < _BoardSize) &&
									(j + mu.y*n >= 0) && (j + mu.y*n < _BoardSize)
									)
								{
									p_dst = this->piece_at((uint8_t)(i + mu.x*n), (uint8_t)(j + mu.y*n));

									// Move
									_Move mv{ i, j, (uint8_t)(i + mu.x*n), (uint8_t)(j + mu.y*n),  p_src->get_id(), p_dst->get_id(), mu };

									if (p_dst->name == PieceName::none)
									{
										if ((p_src->move_style == PieceMoveStyle::Sliding) || (p_src->move_style == PieceMoveStyle::Jumping))
										{
											m.push_back(mv);
										}
										else if (p_src->move_style == PieceMoveStyle::SlidingDiagonalCapturePromo)
										{
											if (mu.flag == MoveUnit::FLAG::conditional)
											{
												if ((mu.flag_spec == "y1") && (j == 1))
												{
													uint8_t prev_x = (mu.x > 1) ? 1 : mu.x;
													uint8_t prev_y = (mu.y > 1) ? 1 : mu.y;
													uint8_t prev_mv_dst_x = (uint8_t)(i + prev_x * 1);
													uint8_t prev_mv_dst_y = (uint8_t)(j + prev_y * 1);
													PieceID id = get_pieceid_at(prev_mv_dst_x, prev_mv_dst_y);
													if (id == _Piece::empty_id()) // empty path
													{
														m.push_back(mv);
													}
												}
												else if ((mu.flag_spec == "y6") && (j == 6))
												{
													uint8_t prev_x = (mu.x > 1) ? 1 : mu.x;
													uint8_t prev_y = (mu.y > 1) ? 1 : mu.y;
													uint8_t prev_mv_dst_x = (uint8_t)(i + prev_x * 1);
													uint8_t prev_mv_dst_y = (uint8_t)(j + prev_y * 1);
													PieceID id = get_pieceid_at(prev_mv_dst_x, prev_mv_dst_y);
													if (id == _Piece::empty_id()) // empty path
													{
														m.push_back(mv);
													}
												}
												else if ((mu.flag_spec == "y5_ep") && (j == 5) && (get_color() == PieceColor::W) && (history_moves.size() > 0)) // en passant
												{
													_Move mh = history_moves.back();
													if (mh.prev_src_id == _Piece::get_id(PieceName::P, get_opposite_color()))
													{
														if ((mh.src_x == i + mu.x) && (mh.src_y == j + mu.y) && (abs(mh.dst_y - mh.src_y) == 2))
														{
															m.push_back(mv);
														}
													}
												}
												else if ((mu.flag_spec == "y2_ep") && (j == 2) && (get_color() == PieceColor::B) && (history_moves.size() > 0))  // en passant
												{
													_Move mh = history_moves.back();
													if (mh.prev_src_id == _Piece::get_id(PieceName::P, get_opposite_color()))
													{
														if ((mh.src_x == i + mu.x) && (mh.src_y == j + mu.y) && (abs(mh.dst_y - mh.src_y) == 2))
														{
															m.push_back(mv);
														}
													}
												}
											}
											else
											{
												if (std::abs(mu.x) != std::abs(mu.y)) // not diago
												{
													// promo
													if (((p_src->color == PieceColor::W) && (j == 6)) || ((p_src->color == PieceColor::B) && (j == 1)))
													{
														_Move mvQ = mv; mvQ.mu.context_extra = "Q"; m.push_back(mvQ);
														_Move mvR = mv; mvR.mu.context_extra = "R"; m.push_back(mvR);
														_Move mvN = mv; mvN.mu.context_extra = "N"; m.push_back(mvN);
														_Move mvB = mv; mvB.mu.context_extra = "B"; m.push_back(mvB);
													}
													else
													{
														m.push_back(mv);
													}
												}
												else
												{
												}
											}
										}
									}
									// capture dst
									else if (p_dst->color != p_src->color)
									{
										if ((p_src->move_style == PieceMoveStyle::Sliding) || (p_src->move_style == PieceMoveStyle::Jumping))
										{
											m.push_back(mv);
										}
										else if (p_src->move_style == PieceMoveStyle::SlidingDiagonalCapturePromo)
										{
											if (mu.flag == MoveUnit::FLAG::conditional)
											{
											}
											else
											{
												if (std::abs(mu.x) != std::abs(mu.y)) // not diago
												{
												}
												else
												{
													// promo
													if (((p_src->color == PieceColor::W) && (j == 6)) || ((p_src->color == PieceColor::B) && (j == 1)))
													{
														_Move mvQ = mv; mvQ.mu.context_extra = "Q"; m.push_back(mvQ);
														_Move mvR = mv; mvR.mu.context_extra = "R"; m.push_back(mvR);
														_Move mvN = mv; mvN.mu.context_extra = "N"; m.push_back(mvN);
														_Move mvB = mv; mvB.mu.context_extra = "B"; m.push_back(mvB);
													}
													else
													{
														m.push_back(mv);
													}
												}
											}
										}
										prev_path_empty = false;
									}
									else if (p_dst->color == p_src->color) // same color dst
									{
										prev_path_empty = false;
									}
								}
							}
						}
					}
				}
			}

            if (is_recursive_call)
            {
                // ok done
            }
            else if (!_allow_self_check)  // Expensive - to be optimized if needed...
			{
				std::vector<_Move> mm;
				for(auto &mv : m)
				{
					apply_move(mv);
					{
                        std::vector<_Move> m_oppon = generate_moves(true);
						if (!can_capture_opposite_king(m_oppon))
						{
							mm.push_back(mv);
						}
					}
					undo_move();
				}
				return mm;
			}

			return m;
		}

		bool has_piece(PieceName n, PieceColor c) const
		{
			PieceId id = Pieces::get_id(n, c);
			for (auto &m : b)
				if (m == id) return true;
			return false;
		}

		bool is_fnal(const std::vector<_Move>& m) const
		{
			if (!has_piece(PieceName::K, PieceColor:W)) return true;
			if (!has_piece(PieceName::K, PieceColor:B)) return true;
			if (m.size() == 0) return true;

			if (!_allow_self_check)
			{
				if (can_capture_opposite_king())
				{
					// if (...) return true; // classic checkmate
				}
			}

			if (_check_repeating_move_draw)
			{
				// ...
			}

			if (_check_50_moves_draw)
			{
				// ...
			}
			return false;
		}

		std::list<_Move> get_history_moves() const { return history_moves; }

	private:
		PieceColor color_toplay;
		std::vector<PieceID> b;

		// Feature
		bool _allow_self_check;
		bool _check_repeating_move_draw;
		bool _check_50_moves_draw;
		
		std::list<_Move> history_moves; 
	};

};
#endif
