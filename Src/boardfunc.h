#ifndef _BOARDFUNC_H
#define _BOARDFUNC_H

#include "board.h"

namespace chess
{
	template <typename PieceID, typename uint8_t _BoardSize> class Piece;
	template <typename PieceID, typename uint8_t _BoardSize> class Board;
	template <typename PieceID, typename uint8_t _BoardSize> class BoardFunc;

	template <typename PieceID, typename uint8_t _BoardSize>
	class BoardFunc
	{
		using _Piece = Piece<PieceID, _BoardSize>;
		using _Board = Board<PieceID, _BoardSize>;
		using _Move = Move<PieceID>;

	public:
		BoardFunc(_Board& b) : _board(b)
		{
		}

	public:
		size_t cnt_piece(PieceName n, PieceColor c)
		{
			size_t cnt = 0;
			PieceID id = _Piece::get_id(n, c);
			for (auto &v : _board.b)
			{
				if (v == id) cnt++;
			}
			return cnt;
		}

		std::string to_str()
		{
			std::string s;
			PieceID id;
			for (uint8_t j = _BoardSize; j >= 1; j--)
			{
				for (uint8_t i = 0; i < _BoardSize; i++)
				{
					id = _board.get_pieceid_at(i, j - 1);
					s += _Piece::to_str(id);
				}
				s += "\n";
			}
			return s;
		}

	private:
		_Board& _board;
	};
}
#endif
