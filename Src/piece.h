#ifndef _PIECE_H
#define _PIECE_H

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cassert>

namespace chess
{
	template <typename PieceID, typename uint8_t _BoardSize> class Piece;
	template <typename PieceID, typename uint8_t _BoardSize> class Board;
	template <typename PieceID, typename uint8_t _BoardSize> class BoardFunc;

	template <typename PieceID> struct Move;

	enum class PieceName		{ R, N, B, Q, K, P, none };
	enum class PieceColor		{ W, B, none };
	enum class PieceMoveStyle	{ Sliding, Jumping, SlidingDiagonalCapturePromo, none };

	struct MoveUnit
	{
		enum struct FLAG { none = 0, conditional = 1 };
		MoveUnit(int8_t _x, int8_t _y, uint8_t _len, FLAG _flag = FLAG::none, std::string _flag_param = "") : x(_x), y(_y), len(_len), flag(_flag), flag_spec(_flag_param) {}

		int8_t		x;				// x direction
		int8_t		y;				// y direction
		uint8_t		len;			// max horizon 1..len
		FLAG		flag;			// move category (normal or special) 
		std::string flag_spec;		// special moveunit specification
		std::string context_extra;	// extra info on moveunit from a position
	};

	template <typename PieceID>
	struct Move
	{
		uint8_t src_x;
		uint8_t src_y;
		uint8_t dst_x;
		uint8_t dst_y;
		PieceID prev_src_id;
		PieceID prev_dst_id;
		MoveUnit mu;
	};

	template <typename PieceID, typename uint8_t _BoardSize>
	class Piece
	{
		using _Piece = Piece<PieceID, _BoardSize>;
		friend class Board<PieceID, _BoardSize>;

	public:
		Piece(PieceName n = PieceName::none, PieceColor	c = PieceColor::none, PieceMoveStyle m = PieceMoveStyle::none)
			: name(n), color(c), move_style(m)
		{
		}

		Piece(const Piece&) = delete;
		Piece & operator=(const Piece &) = delete;
		Piece(Piece&&) = delete;

	public:
		~Piece() = default;

		PieceName			name;
		PieceColor			color;
		PieceMoveStyle		move_style;

		constexpr static PieceID empty_id() { return 0; }
		static PieceID get_id(PieceName _name, PieceColor _c)
		{
			if (_name == PieceName::none) return 0;
			if (_c == PieceColor::B)
			{
				switch (_name) {
				case PieceName::R: return 1;
				case PieceName::N: return 2;
				case PieceName::B: return 3;
				case PieceName::Q: return 4;
				case PieceName::K: return 5;
				case PieceName::P: return 6;
				default: return 0;
				}
			}
			else
			{
				switch (_name) {
				case PieceName::R: return 7;
				case PieceName::N: return 8;
				case PieceName::B: return 9;
				case PieceName::Q: return 10;
				case PieceName::K: return 11;
				case PieceName::P: return 12;
				default: return 0;
				}
			}
			return 0;
		}

		PieceID get_id() { return get_id(this->name, this->color); }

		static _Piece* get(PieceID id)
		{
			assert(id >= 0);
			assert(id < pieces.size());
			return pieces[id].get();
		}

		static std::string to_str(PieceID id)
		{
			std::string s;
			for (size_t i = 0; i < pieces.size(); i++)
			{
				if (_Piece::get_id(pieces[i]->name, pieces[i]->color) == id)
				{
					if (pieces[i]->color == PieceColor::W) s += "w";
					else if (pieces[i]->color == PieceColor::B) s += "b";
					else s += "-";

					if (pieces[i]->name == PieceName::R) s += "R";
					else if (pieces[i]->name == PieceName::N) s += "N";
					else if (pieces[i]->name == PieceName::B) s += "B";
					else if (pieces[i]->name == PieceName::Q) s += "Q";
					else if (pieces[i]->name == PieceName::K) s += "K";
					else if (pieces[i]->name == PieceName::P) s += "P";
					else s += "-";

					s += " ";
					return s;
				}
			}
			return "-- ";
		}

	private:
		std::vector<MoveUnit> moves;
		static bool is_init;
		static std::vector<std::unique_ptr<_Piece>> pieces;

	public:
		static void init()
		{
			if (is_init) return;

			pieces.clear();
			pieces.push_back(std::make_unique<_Piece>(PieceName::none, PieceColor::none, PieceMoveStyle::none));
			pieces.push_back(std::make_unique<_Piece>(PieceName::R, PieceColor::B, PieceMoveStyle::Sliding));
			pieces.push_back(std::make_unique<_Piece>(PieceName::N, PieceColor::B, PieceMoveStyle::Jumping));
			pieces.push_back(std::make_unique<_Piece>(PieceName::B, PieceColor::B, PieceMoveStyle::Sliding));
			pieces.push_back(std::make_unique<_Piece>(PieceName::Q, PieceColor::B, PieceMoveStyle::Sliding));
			pieces.push_back(std::make_unique<_Piece>(PieceName::K, PieceColor::B, PieceMoveStyle::Sliding));
			pieces.push_back(std::make_unique<_Piece>(PieceName::P, PieceColor::B, PieceMoveStyle::SlidingDiagonalCapturePromo));
			pieces.push_back(std::make_unique<_Piece>(PieceName::R, PieceColor::W, PieceMoveStyle::Sliding));
			pieces.push_back(std::make_unique<_Piece>(PieceName::N, PieceColor::W, PieceMoveStyle::Jumping));
			pieces.push_back(std::make_unique<_Piece>(PieceName::B, PieceColor::W, PieceMoveStyle::Sliding));
			pieces.push_back(std::make_unique<_Piece>(PieceName::Q, PieceColor::W, PieceMoveStyle::Sliding));
			pieces.push_back(std::make_unique<_Piece>(PieceName::K, PieceColor::W, PieceMoveStyle::Sliding));
			pieces.push_back(std::make_unique<_Piece>(PieceName::P, PieceColor::W, PieceMoveStyle::SlidingDiagonalCapturePromo));

			uint8_t n = _BoardSize - 1;
			std::vector<MoveUnit> mv_R = { { 0,1, n },{ 0,-1, n },{ 1,0, n },{ -1,0, n } };
			std::vector<MoveUnit> mv_N = { { 1,2, 1 },{ 1,-2, 1 },{ 2,1, 1 },{ 2,-1, 1 },{ -1,-2, 1 },{ -1,2, 1 },{ -2,1, 1 },{ -2,-1, 1 } };
			std::vector<MoveUnit> mv_B = { { 1,1, n },{ -1,-1, n },{ 1,-1, n },{ -1,1, n } };
			std::vector<MoveUnit> mv_Q = { { 0,1, n },{ 0,-1, n },{ 1,0, n },{ 0,-1, n },{ 1,1, n },{ -1,-1, n },{ 1,-1, n },{ -1,1, n } };
			std::vector<MoveUnit> mv_K = { { 0,1, 1 },{ 0,-1, 1 },{ 1,0, 1 },{ 0,-1, 1 },{ 1,1, 1 },{ -1,-1, 1 },{ 1,-1, 1 },{ -1,1, 1 } };
			std::vector<MoveUnit> mv_PW = { { 0,1, 1 } ,{ 1,1, 1 },{ -1,1, 1 } ,{ 0, 2, 1, MoveUnit::FLAG::conditional, "y1" } ,{ -1, 1,1, MoveUnit::FLAG::conditional, "y5_ep" } ,{ 1, 1,1, MoveUnit::FLAG::conditional, "y5_ep" } };
			std::vector<MoveUnit> mv_PB = { { 0,-1, 1 } ,{ -1,-1, 1 },{ 1,-1, 1 } ,{ 0,-2, 1, MoveUnit::FLAG::conditional, "y6" } ,{ -1,-1,1, MoveUnit::FLAG::conditional, "y2_ep" } ,{ 1,-1,1, MoveUnit::FLAG::conditional, "y2_ep" } };

			{
				_Piece* pR = get(get_id(PieceName::R, PieceColor::W)); pR->moves = mv_R;
				_Piece* pN = get(get_id(PieceName::N, PieceColor::W)); pN->moves = mv_N;
				_Piece* pB = get(get_id(PieceName::B, PieceColor::W)); pB->moves = mv_B;
				_Piece* pQ = get(get_id(PieceName::Q, PieceColor::W)); pQ->moves = mv_Q;
				_Piece* pK = get(get_id(PieceName::K, PieceColor::W)); pK->moves = mv_K;
				_Piece* pP = get(get_id(PieceName::P, PieceColor::W)); pP->moves = mv_PW;
			}
			{
				_Piece* pR = get(get_id(PieceName::R, PieceColor::B)); pR->moves = mv_R;
				_Piece* pN = get(get_id(PieceName::N, PieceColor::B)); pN->moves = mv_N;
				_Piece* pB = get(get_id(PieceName::B, PieceColor::B)); pB->moves = mv_B;
				_Piece* pQ = get(get_id(PieceName::Q, PieceColor::B)); pQ->moves = mv_Q;
				_Piece* pK = get(get_id(PieceName::K, PieceColor::B)); pK->moves = mv_K;
				_Piece* pP = get(get_id(PieceName::P, PieceColor::B)); pP->moves = mv_PB;
			}

			is_init = true;
		}
	};
};

#endif
