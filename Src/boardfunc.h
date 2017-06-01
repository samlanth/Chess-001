// --------------------------------
// Author: Alain Lanthier, 2017
// --------------------------------
#ifndef _AL_CHESS_BOARDFUNC_H
#define _AL_CHESS_BOARDFUNC_H

#include "board.h"

namespace chess
{
	template <typename PieceID, typename uint8_t _BoardSize> class Piece;
	template <typename PieceID, typename uint8_t _BoardSize> class Board;

	template <typename PieceID, typename uint8_t _BoardSize>
	class BoardFunc
	{
		using _Piece = Piece<PieceID, _BoardSize>;
		using _Board = Board<PieceID, _BoardSize>;

	public:
		BoardFunc(_Board& b) : _board(b)
		{
		}

	public:
		const size_t cnt_piece(PieceName n, PieceColor c) const
		{
			size_t cnt = 0;
			PieceID id = _Piece::get_id(n, c);
			for (const auto &v : _board.b)
			{
				if (v == id) cnt++;
			}
			return cnt;
		}

		const std::string to_str() const
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
