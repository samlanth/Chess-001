// --------------------------------
// Author: Alain Lanthier, 2017
// --------------------------------
#ifndef _AL_CHESS_PIECE_H
#define _AL_CHESS_PIECE_H

#include "move.h"
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cassert>

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize> class Board;

    enum class PieceName        { R, N, B, Q, K, P, none };
    enum class PieceColor       { W, B, none };
    enum class PieceMoveStyle   { Sliding, Jumping, SlidingDiagonalCapturePromo, none };

    template <typename PieceID, typename uint8_t _BoardSize>
    class Piece
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        friend class Board<PieceID, _BoardSize>;

    public:
        Piece(PieceName n = PieceName::none, PieceColor	c = PieceColor::none, PieceMoveStyle m = PieceMoveStyle::none);
        ~Piece() = default;

        Piece(const Piece&)                 = delete;
        Piece & operator=(const Piece &)    = delete;
        Piece(Piece&&)                      = delete;

        // static
        static const PieceID empty_id();
        static const PieceID get_id(PieceName _name, PieceColor _c);
        static const _Piece* get(PieceID id);
        static void init();
        static const std::string to_str(PieceID id);

        const PieceID           get_id()        const;
        const PieceName         get_name()      const { return name; }
        const PieceColor        get_color()     const { return color; }
        const PieceMoveStyle    get_movestyle() const { return move_style; }
        const PieceName         get_moves()     const { return moves; }

    private:
        PieceName               name;
        PieceColor              color;
        PieceMoveStyle          move_style;
        std::vector<MoveUnit>   moves;

        static bool             is_init;
        static std::vector<std::unique_ptr<_Piece>> pieces;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    inline Piece<PieceID, _BoardSize>::Piece(PieceName n, PieceColor c, PieceMoveStyle m)
        : name(n), color(c), move_style(m) {}
    
    template <typename PieceID, typename uint8_t _BoardSize>
    inline const PieceID Piece<PieceID, _BoardSize>::empty_id() { return 0; }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline const PieceID Piece<PieceID, _BoardSize>::get_id(PieceName _name, PieceColor _c)
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
        else if (_c == PieceColor::W)
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

    template <typename PieceID, typename uint8_t _BoardSize>
    inline const PieceID Piece<PieceID, _BoardSize>::get_id() const 
    { 
        return get_id(this->name, this->color); 
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline const Piece<PieceID, _BoardSize>* Piece<PieceID, _BoardSize>::get(PieceID id)
    {
        assert(pieces.size() > 0);
        assert(id >= 0);
        assert(id < pieces.size());
        return pieces[id].get();
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline const std::string Piece<PieceID, _BoardSize>::to_str(PieceID id)
    {
        std::string s;
        for (const auto &p : pieces)
        {
            if (_Piece::get_id(p->name, p->color) == id)
            {
                if (p->color == PieceColor::W) s += "w";
                else if (p->color == PieceColor::B) s += "b";
                else s += "-";

                if (p->name == PieceName::R) s += "R";
                else if (p->name == PieceName::N) s += "N";
                else if (p->name == PieceName::B) s += "B";
                else if (p->name == PieceName::Q) s += "Q";
                else if (p->name == PieceName::K) s += "K";
                else if (p->name == PieceName::P) s += "P";
                else s += "-";

                s += " ";
                return s;
            }
        }
        return "-- ";
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void Piece<PieceID, _BoardSize>::init()
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
        std::vector<MoveUnit> mv_Q = { { 0,1, n },{ 0,-1, n },{ 1,0, n },{ -1,0, n },{ 1,1, n },{ -1,-1, n },{ 1,-1, n },{ -1,1, n } };
        std::vector<MoveUnit> mv_K = { { 0,1, 1 },{ 0,-1, 1 },{ 1,0, 1 },{ -1,0, 1 },{ 1,1, 1 },{ -1,-1, 1 },{ 1,-1, 1 },{ -1,1, 1 } };
        std::vector<MoveUnit> mv_PW = { { 0,1, 1 } ,{ 1,1, 1 },{ -1,1, 1 } ,   { 0, 2, 1, MoveUnit::FLAG::conditional, "y1" } ,{ -1, 1,1, MoveUnit::FLAG::conditional, "y5_ep" } ,{ 1, 1,1, MoveUnit::FLAG::conditional, "y5_ep" } };
        std::vector<MoveUnit> mv_PB = { { 0,-1, 1 } ,{ -1,-1, 1 },{ 1,-1, 1 } ,{ 0,-2, 1, MoveUnit::FLAG::conditional, "y6" } ,{ -1,-1,1, MoveUnit::FLAG::conditional, "y2_ep" } ,{ 1,-1,1, MoveUnit::FLAG::conditional, "y2_ep" } };

        pieces.at(get_id(PieceName::R, PieceColor::W))->moves = mv_R;
        pieces.at(get_id(PieceName::N, PieceColor::W))->moves = mv_N;
        pieces.at(get_id(PieceName::B, PieceColor::W))->moves = mv_B;
        pieces.at(get_id(PieceName::Q, PieceColor::W))->moves = mv_Q;
        pieces.at(get_id(PieceName::K, PieceColor::W))->moves = mv_K;
        pieces.at(get_id(PieceName::P, PieceColor::W))->moves = mv_PW;

        pieces.at(get_id(PieceName::R, PieceColor::B))->moves = mv_R;
        pieces.at(get_id(PieceName::N, PieceColor::B))->moves = mv_N;
        pieces.at(get_id(PieceName::B, PieceColor::B))->moves = mv_B;
        pieces.at(get_id(PieceName::Q, PieceColor::B))->moves = mv_Q;
        pieces.at(get_id(PieceName::K, PieceColor::B))->moves = mv_K;
        pieces.at(get_id(PieceName::P, PieceColor::B))->moves = mv_PB;

        is_init = true;
    }

};
#endif
