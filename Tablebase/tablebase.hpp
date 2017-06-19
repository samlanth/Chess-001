#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Tablebase
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASE_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASE_HPP

#include <tuple>

namespace chess
{
    enum class TB_Marker { none, GRAY };

    constexpr uint64_t  powN(uint8_t v, uint8_t n) { return (n > 0) ? v * powN(v, n - 1) : 1; }
    constexpr uint64_t  TB_size(int boardsize, int N) { return powN(boardsize*boardsize, N); }
    constexpr uint8_t   TB_size_item() { return 3; }    // score = 2 bits + marker = 1 bit
    constexpr uint64_t  TB_size_dim(int boardsize) { return boardsize*boardsize; }

    class TablebaseBase
    {
    public:
    };

    // Tablebase
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    class Tablebase : public TablebaseBase
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _Tablebase = Tablebase<PieceID, _BoardSize, NPIECE>;

    public:
        Tablebase(std::vector<PieceID>& v, PieceColor c) : _color(c), _NPIECE(NPIECE), _bits(0), _work_board(nullptr)
        {
            for (auto& vv : v)
            {
                _piecesID.push_back(vv);
                _pieces.push_back(_Piece::get(vv));
            }
            _work_board = new _Board();
            _is_build = false;
        }
        virtual ~Tablebase()
        {
            delete _work_board;
        }

        virtual bool load() = 0;
        virtual bool save() = 0;
        virtual bool build(char verbose) = 0;

        bool        is_build()                      { return _is_build; }
        PieceColor  color()     const               { return _color; }
        uint8_t     square(uint8_t x, uint8_t y)    { return __BoardSize*y + x; }

        uint64_t index_item(const uint16_t& sq0)  const
        {
            return (sq0) * _size_item;
        }
        uint64_t index_item(const uint16_t& sq0, const uint16_t& sq1)  const
        {
            return ((_dim1*sq0) + sq1) * _size_item;
        }
        uint64_t index_item(const uint16_t& sq0, const uint16_t& sq1, const uint16_t& sq2)  const
        {
            return ((_dim2*sq0) + (_dim1*sq1) + sq2) * _size_item;
        }

        bool bit(const uint64_t& idx_item, const uint8_t& idx_bit)  const
        {
            return (_bits[idx_item + idx_bit] == 1);
        }
        bool bit(const uint16_t& sq0, const uint8_t& idx_bit)  const
        {
            return bit(index_item(sq0), idx_bit);
        }
        bool bit(const uint16_t& sq0, const uint16_t& sq1, const uint8_t& idx_bit)  const
        {
            return bit(index_item(sq0, sq1), idx_bit);
        }
        bool bit(const uint16_t& sq0, const uint16_t& sq1, const uint16_t& sq2, const uint8_t& idx_bit)  const
        {
            return bit(index_item(sq0, sq1, sq2), idx_bit);
        }
        void set_bit(const uint64_t& idx_item, const uint8_t& idx_bit, bool v)
        {
            if (v) _bits[idx_item + idx_bit] = 1;
            else   _bits[idx_item + idx_bit] = 0;
        }

        virtual ExactScore score(const uint16_t& sq0)  const
        {
            return score_at_idx(index_item(sq0));
        }
        virtual ExactScore score(const uint16_t& sq0, const uint16_t& sq1)  const
        {
            return score_at_idx(index_item(sq0, sq1));
        }
        virtual ExactScore score(const uint16_t& sq0, const uint16_t& sq1, const uint16_t& sq2)  const
        {
            return score_at_idx(index_item(sq0, sq1, sq2));
        }

        virtual void set_score(const uint16_t& sq0, ExactScore sc)
        {
            set_score_at_idx(index_item(sq0), sc);
        }
        virtual void set_score(const uint16_t& sq0, const uint16_t& sq1, ExactScore sc)
        {
            set_score_at_idx(index_item(sq0, sq1), sc);
        }
        virtual void set_score(const uint16_t& sq0, const uint16_t& sq1, const uint16_t& sq2, ExactScore sc)
        {
            set_score_at_idx(index_item(sq0, sq1, sq2), sc);
        }

        bool marker(const uint16_t& sq0) const
        {
            return bit(index_item(sq0), 2);
        }
        bool marker(const uint16_t& sq0, const uint16_t& sq1) const
        {
            return bit(index_item(sq0, sq1), 2);
        }
        bool marker(const uint16_t& sq0, const uint16_t& sq1, const uint16_t& sq2) const
        {
            return bit(index_item(sq0, sq1, sq2), 2);
        }
        void set_marker(const uint16_t& sq0, bool v)
        {
            set_bit(index_item(sq0), 2, v);
        }
        void set_marker(const uint16_t& sq0, const uint16_t& sq1, bool v)
        {
            set_bit(index_item(sq0, sq1), 2, v);
        }
        void set_marker(const uint16_t& sq0, const uint16_t& sq1, const uint16_t& sq2, bool v)
        {
            set_bit(index_item(sq0, sq1, sq2), 2, v);
        }

        void square_at_index(uint64_t idx, uint16_t& sq0) const
        {
            sq0 = (uint16_t)(idx % _dim1);
        }
        void square_at_index(uint64_t idx, uint16_t& sq0, uint16_t& sq1) const
        {
            sq0 = (uint16_t)(idx / _dim1); sq1 = (uint16_t)(idx % _dim1);
        }
        void square_at_index(uint64_t idx, uint16_t& sq0, uint16_t& sq1, uint16_t& sq2) const
        {
            sq0 = (uint16_t)(idx / _dim2);
            uint64_t t = idx - (sq0 * _dim2);  square_at_index(t, sq1, sq2);
        }

        void print() const;
        std::vector<TablebaseBase*>& children() { return _children; }
        bool check_unknown() const;
        void set_unknown_to_draw();

    protected:
        const uint8_t     _size_item = TB_size_item();
        const uint64_t    _size_tb = TB_size(_BoardSize, NPIECE);
        const uint64_t    _dim0 = 1;
        const uint64_t    _dim1 = TB_size_dim(_BoardSize);
        const uint64_t    _dim2 = TB_size_dim(_BoardSize) * TB_size_dim(_BoardSize);

        const PieceColor                _color;
        const uint8_t                   _NPIECE;
        std::vector<PieceID>            _piecesID;
        std::vector<const _Piece*>      _pieces;
        bool                            _is_build;

        std::bitset<TB_size(_BoardSize, NPIECE) * TB_size_item()>   _bits;
        std::vector<TablebaseBase*>      _children;
        _Board*                         _work_board;


        ExactScore score_at_idx(const uint64_t& idx_item)  const
        {
            bool bit0 = bit(idx_item, 0);
            bool bit1 = bit(idx_item, 1);
            if (bit0 && bit1)    return ExactScore::WIN;
            if (bit0 && (!bit1)) return ExactScore::DRAW;
            if ((!bit0) && bit1) return ExactScore::LOSS;
            return ExactScore::UNKNOWN;
        }
        void set_score_at_idx(const uint64_t& idx_item, ExactScore sc)
        {
            if (sc == ExactScore::WIN)          { set_bit(idx_item, 0, 1); set_bit(idx_item, 1, 1); }
            else if (sc == ExactScore::DRAW)    { set_bit(idx_item, 0, 1); set_bit(idx_item, 1, 0); }
            else if (sc == ExactScore::LOSS)    { set_bit(idx_item, 0, 0); set_bit(idx_item, 1, 1); }
            else                                { set_bit(idx_item, 0, 0); set_bit(idx_item, 1, 0); }
        }

    };

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    void Tablebase<PieceID, _BoardSize, NPIECE>::print() const
    {
        uint64_t n = 0;
        uint64_t n_win = 0;
        uint64_t n_loss = 0;
        uint64_t n_draw = 0;
        ExactScore sc;

        if (this->color() == PieceColor::W) std::cout << "TB Color: White" << std::endl;
        else std::cout << "TB Color: Black" << std::endl;
        std::cout << "TB size: " << this->_size_tb << std::endl;
        std::cout << "TB Number pieces: " << (int)this->_NPIECE << std::endl;
        for (auto& v : _piecesID)
        {
            std::cout << _Piece::to_str(v);
        }
        std::cout << std::endl;
        {
            n = 0; n_win = 0; n_loss = 0; n_draw = 0;
            for (uint64_t i = 0; i < this->_size_tb; i++)
            {
                sc = this->score_at_idx(i * this->_size_item);
                if (sc != ExactScore::UNKNOWN) n++;
                if (sc == ExactScore::WIN) n_win++;
                if (sc == ExactScore::LOSS) n_loss++;
                if (sc == ExactScore::DRAW)
                    n_draw++;
            }
            std::cout << "score positions: " << n << std::endl;
            std::cout << "win  positions:  " << n_win << std::endl;
            std::cout << "loss positions:  " << n_loss << std::endl;
            std::cout << "draw/illegal positions:  " << n_draw << std::endl;
            std::cout << std::endl;
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    bool Tablebase<PieceID, _BoardSize, NPIECE>::check_unknown() const
    {
        uint64_t n = 0;
        ExactScore sc;
        {
            for (uint64_t i = 0; i < this->_size_tb; i++)
            {
                sc = this->score_at_idx(i * this->_size_item);
                if (sc == ExactScore::UNKNOWN) n++;
            }
        }
        if (n > 0) return true;
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    void Tablebase<PieceID, _BoardSize, NPIECE>::set_unknown_to_draw()
    {
        uint64_t n = 0;
        ExactScore sc;
        {
            for (uint64_t i = 0; i < this->_size_tb; i++)
            {
                sc = this->score_at_idx(i * this->_size_item);
                if (sc == ExactScore::UNKNOWN)                      // ILLEGAL position exist in the TB _bits.....
                {
                    set_score_at_idx(i * _size_item, ExactScore::DRAW);
                }
            }
        }
    }
};
#endif

