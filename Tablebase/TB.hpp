#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Tablebase (TB)
//
//
#ifndef _AL_CHESS_TABLEBASE_TB_HPP
#define _AL_CHESS_TABLEBASE_TB_HPP

namespace chess
{
    constexpr uint64_t  powN(uint8_t v, uint8_t n) { return (n > 0) ? v * powN(v, n - 1) : 1; }
    constexpr uint64_t  TB_size(int boardsize, int N) { return powN(boardsize*boardsize, N); }
    constexpr uint8_t   TB_size_item() { return 3; }    // score = 2 bits + marker = 1 bit
    constexpr uint64_t  TB_size_dim(int boardsize) { return boardsize*boardsize; }

    enum class TB_TYPE {
        tb_unknown,
        tb_0vX, tb_Xv0,
        tb_1v1,
        tb_2v1, tb_2v1_sym,
        tb_3v1, tb_2v2, tb_3v1_sym,
        tb_4v1, tb_3v2, tb_4v1_sym, tb_3v2_sym,
        tb_5v1, tb_4v2, tb_3v3, tb_5v1_sym, tb_4v2_sym,
        tb_6v1, tb_5v2, tb_4v3, tb_6v1_sym, tb_5v2_sym, tb_4v3_sym,
        tb_7v1, tb_6v2, tb_5v3, tb_4v4, //...
        tb_8v1, tb_7v2, tb_6v3, tb_5v4,
        tb_9v1, tb_8v2, tb_7v3, tb_6v4, tb_5v5
    };

    enum class TBH_OPTION { none, try_load_on_build, force_rebuild, memory_map_on_build };

    TB_TYPE sym_tb_type(TB_TYPE t)
    {
        if      (t == TB_TYPE::tb_2v1) return TB_TYPE::tb_2v1_sym;
        else if (t == TB_TYPE::tb_3v1) return TB_TYPE::tb_3v1_sym;
        else if (t == TB_TYPE::tb_4v1) return TB_TYPE::tb_4v1_sym;
        else if (t == TB_TYPE::tb_3v2) return TB_TYPE::tb_3v2_sym;
        return TB_TYPE::tb_unknown;
    }

    std::string TB_TYPE_to_string(TB_TYPE t)
    {
        if (t == TB_TYPE::tb_0vX)           return "tb_0vX";
        else if (t == TB_TYPE::tb_Xv0)      return "tb_Xv0";
        else if (t == TB_TYPE::tb_1v1)      return "tb_1v1";
        else if (t == TB_TYPE::tb_2v1)      return "tb_2v1";
        else if (t == TB_TYPE::tb_2v2)      return "tb_2v2";
        else if (t == TB_TYPE::tb_2v1_sym)  return "tb_2v1_sym";
        else if (t == TB_TYPE::tb_3v1)      return "tb_3v1";
        else if (t == TB_TYPE::tb_3v1_sym)  return "tb_3v1_sym";
        else if (t == TB_TYPE::tb_4v1)      return "tb_4v1";
        else if (t == TB_TYPE::tb_3v2)      return "tb_3v2";
        else if (t == TB_TYPE::tb_4v1_sym)  return "tb_4v1_sym";
        else if (t == TB_TYPE::tb_3v2_sym)  return "tb_3v2_sym";
        return "tb_unknown";
    }

    // TablebaseBase
    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseBase
    {
    public:
        TablebaseBase() {}
        virtual ~TablebaseBase() {}

        virtual bool        is_symmetry_TB() const = 0;

        virtual bool        legal(const std::vector<uint16_t>& sq) const = 0;
        virtual ExactScore  score_v(const std::vector<uint16_t>& sq) const = 0;
        virtual uint8_t     dtc_v(const std::vector<uint16_t>& sq) const = 0;

        virtual bool        is_build() const = 0;
        virtual bool        load() = 0;
        virtual bool        save() const = 0;
        virtual void        print() const = 0;
        virtual uint8_t     getNPIECE() const = 0;

        static void order_sq_v(std::vector<uint16_t>& sq, const std::vector<PieceID>& piecesID);
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void TablebaseBase<PieceID, _BoardSize>::order_sq_v(std::vector<uint16_t>& sq, const std::vector<PieceID>& piecesID)
    {
        bool not_done = true;
        while (not_done)
        {
            not_done = false;
            for (size_t i = 0; i < sq.size() - 1; i++)
            {
                // sort
                if ((piecesID[i] == piecesID[i + 1]) && (sq[i] > sq[i + 1]))
                {
                    // swap
                    uint16_t t = sq[i];  sq[i] = sq[i + 1]; sq[i + 1] = t;
                    not_done = true;
                    break;
                }
            }
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    struct STRUCT_PIECE_RANK
    {
        PieceID     ret_id;
        uint16_t    ret_count;
        uint16_t    rank_ret_id;
        uint16_t    ret_instance;
    };

    // Tablebase
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    class Tablebase : public TablebaseBase<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

        friend class TBH_Symmetry<PieceID, _BoardSize, 1>;
        friend class TBH_Symmetry<PieceID, _BoardSize, 2>;
        friend class TBH_Symmetry<PieceID, _BoardSize, 3>;
        friend class TBH_Symmetry<PieceID, _BoardSize, 4>;
        friend class TBH_Symmetry<PieceID, _BoardSize, 5>;

        template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
        uint64_t friend set_mate_score_v(   PieceColor color_to_play, Tablebase<PieceID, _BoardSize, NPIECE>* tb, size_t from, size_t to);

        template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
        uint64_t friend setup_marker_v(TBH<PieceID, _BoardSize>* tbh, PieceColor color_to_play, Tablebase<PieceID, _BoardSize, NPIECE>* tb, Tablebase<PieceID, _BoardSize, NPIECE>* tb_oppo, size_t from, size_t to);

        template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
        uint64_t friend process_marker_v(TBH<PieceID, _BoardSize>* tbh, PieceColor color_to_play, Tablebase<PieceID, _BoardSize, NPIECE>* tb, Tablebase<PieceID, _BoardSize, NPIECE>* tb_oppo, size_t from, size_t to);

        template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
        bool friend build_base_vv(  TBH<PieceID, _BoardSize>* tbh, TablebaseBase<PieceID, _BoardSize>* tb_W, TablebaseBase<PieceID, _BoardSize>* tb_B, char verbose);

        template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
        ExactScore friend generate_child_info(TBH<PieceID, _BoardSize>* tbh, PieceColor color_to_play,
            Tablebase<PieceID, _BoardSize, NPIECE>* tb, Tablebase<PieceID, _BoardSize, NPIECE>* tb_oppo,
            Board<PieceID, _BoardSize>* _work_board,
            std::vector<ExactScore>&    child_sc,
            std::vector<uint8_t>&       child_dtc,
            std::vector<bool>&          child_is_promo,
            std::vector<bool>&          child_is_capture,
            std::vector<bool>&          child_is_pawn,
            std::vector<uint16_t>&      child_sq,
            std::map<size_t, STRUCT_PIECE_RANK<PieceID, _BoardSize>>& map_piece_rank,
            bool&                       exist_child_score,
            uint8_t&                    ret_dtc,
            size_t&                     ret_idx);

    protected:
        const bool                      _do_x_symmetry = true;
        const uint8_t                   _size_item = TB_size_item();
        const uint64_t                  _size_tb = TB_size(_BoardSize, NPIECE);
        const uint64_t                  _dim[5] = { 1, 
                                                    TB_size_dim(_BoardSize), 
                                                    TB_size_dim(_BoardSize) * TB_size_dim(_BoardSize), 
                                                    TB_size_dim(_BoardSize) * TB_size_dim(_BoardSize) * TB_size_dim(_BoardSize),
                                                    TB_size_dim(_BoardSize) * TB_size_dim(_BoardSize) * TB_size_dim(_BoardSize) * TB_size_dim(_BoardSize),
        };

        const PieceColor                _color;         // Side to play
        const uint8_t                   _NPIECE;        // KQvK is 3 pieces
        std::vector<PieceID>            _piecesID;
        std::vector<const _Piece*>      _pieces;
        bool                            _is_build;

        std::bitset<TB_size(_BoardSize, NPIECE) * TB_size_item()>   _bits;  // 2 bit score + 1 bit marker
        std::array<uint8_t, TB_size(_BoardSize, NPIECE)>            _dtc;   // distance to conversion

    public:
        Tablebase(std::vector<PieceID>& v, PieceColor c);
        virtual ~Tablebase() {}

        bool do_x_symmetry() const { return  _do_x_symmetry; }
        bool legal(const std::vector<uint16_t>& sq) const override;

        bool load() override        
        { 
            if (!load_tb()) return false;;
            return true;
        }
        bool save() const override  { return save_tb(); }

        bool        is_symmetry_TB() const override { return false; } // symmetry is number white pieces n_w < n_b
        bool        is_build() const override { return _is_build; }

        PieceColor  color() const       { return _color; }
        uint8_t     getNPIECE() const   override { return _NPIECE; }
        std::vector<PieceID> piecesID() const { return _piecesID; }
        uint64_t    size_tb() const     { return _size_tb; }

        void        print_score(int n)  const;
        void        print_dtc(int n)    const;
        uint8_t     checksum_dtc()      const;

        // score_v
        ExactScore score_v(const std::vector<uint16_t>& sq)  const 
        {
            assert(_NPIECE == sq.size());
            return score_at_idx(index_item_v(sq));
        }

        // set_dtc_v
        void set_dtc_v(std::vector<uint16_t>& sq, uint8_t dtc)
        {
            assert(_NPIECE == sq.size());
            set_dtc_at_idx(index_dtc_v(sq), dtc);
        }

        // set_score_v
        void set_score_v(std::vector<uint16_t>& sq, ExactScore sc)
        {
            assert(_NPIECE == sq.size());
            set_score_at_idx(index_item_v(sq), sc);
        }

        // set_marker_v
        void set_marker_v(const std::vector<uint16_t>& sq, bool v)
        {
            assert(_NPIECE == sq.size());
            set_bit(index_item_v(sq), 2, v);
        }

        // raw board square
        void square_at_index_v(uint64_t idx, std::vector<uint16_t>& sq) const
        {
            assert(_NPIECE == sq.size());
            square_at_index_vv(0, idx, sq);
        }

        // set_marker_v
        bool marker_v(std::vector<uint16_t>& sq)
        {
            assert(_NPIECE == sq.size());
            return bit(index_item_v(sq), 2);
        }

        // dtc_v
        uint8_t dtc_v(const std::vector<uint16_t>& sq)  const 
        {
            assert(_NPIECE == sq.size());
            return dtc_at_idx(index_dtc_v(sq));
        }

        std::string name() const    // Unique name to identify and persist in file stream
        {
            return TB_Manager<PieceID, _BoardSize>::instance()->name_pieces(_piecesID, _color);
        }

        void translate_x(std::vector<uint16_t>& sq)  const
        {
            uint16_t x;
            uint16_t y;
            for (size_t i = 0; i < sq.size(); i++)
            {
                x = (uint16_t)(sq[i] % _BoardSize);
                y = (uint16_t)(sq[i] / _BoardSize);
                sq[i] = (y * _BoardSize) + ((_BoardSize-1) - x); // 6: 0<-->5
            }
        }

        bool can_translate_x(const std::vector<uint16_t>& sq)  const
        {
            PieceID WKid = _Piece::get_id(PieceName::K, PieceColor::W);
            for (size_t i = 0; i < sq.size(); i++)
            {
                if (_piecesID[i] == WKid)
                {
                    double x = sq[i] % _BoardSize;
                    double b = _BoardSize;
                    if (x >= (b / 2.0)) // 6: >= 3, 5: >=2.5
                    {
                        return true;
                    }
                }
            }
            return false;
        }

    protected:
        PieceID pieceID(uint8_t idx) { return _piecesID[idx]; }
        std::vector<uint64_t> get_index_dtc(uint8_t value_dtc, ExactScore value_sc) const;

        bool valid_index(uint64_t index, Board<PieceID, _BoardSize>& _work_board, std::vector<uint16_t>& ret_sq) const;

        void square_at_index_vv(size_t n, uint64_t idx, std::vector<uint16_t>& sq)  const
        {
            if (n >= sq.size()) return;
            sq[n] = (uint16_t)(idx / _dim[sq.size() - (n+1)]);
            uint64_t t = idx - (sq[n] * _dim[sq.size() - (n + 1)]);
            square_at_index_vv(n+1, t, sq);
            if (n==0) order_sq_v(sq);
        }

        void order_sq_v(std::vector<uint16_t>& sq)  const
        {
            TablebaseBase<PieceID, _BoardSize>::order_sq_v(sq, _piecesID);
        }

    protected:
        bool save_tb() const;
        bool save_tb_dtc() const;
        bool read_tb();
        bool read_tb_dtc();
        bool load_tb();
 
        uint16_t square(uint8_t x, uint8_t y) { return _BoardSize*y + x; }

        uint64_t index_item_v(const std::vector<uint16_t>& sq)  const
        {
            if (_do_x_symmetry)
            {
                if (can_translate_x(sq))
                {
                    std::vector<uint16_t> sq_copy = sq;
                    translate_x(sq_copy);

                    uint64_t n = 0;
                    for (size_t i = 0; i < sq_copy.size(); i++) n += (_dim[(sq_copy.size() - (i + 1))] * sq_copy[i]);
                    return n * _size_item;
                }
            }

            uint64_t n = 0;
            for (size_t i = 0; i < sq.size(); i++) n += (_dim[(sq.size() - (i + 1))] * sq[i]);
            return n * _size_item;
        }

        uint64_t index_dtc_v(const std::vector<uint16_t>& sq)  const
        {
            if (_do_x_symmetry)
            {
                if (can_translate_x(sq))
                {
                    std::vector<uint16_t> sq_copy = sq;
                    translate_x(sq_copy);

                    uint64_t n = 0;
                    for (size_t i = 0; i < sq_copy.size(); i++) n += (_dim[(sq_copy.size() - (i + 1))] * sq_copy[i]);
                    return n;
                }
            }

            uint64_t n = 0;
            for (size_t i = 0; i < sq.size(); i++) n += (_dim[(sq.size() - (i + 1))] * sq[i]);
            return n;
        }

        bool bit(const uint64_t& idx_item, const uint8_t& idx_bit)  const
        {
            return (_bits[idx_item + idx_bit] == 1);
        }
        bool bit_v(std::vector<uint16_t>& sq, const uint8_t& idx_bit)  const
        {
            return bit(index_item_v(sq), idx_bit);
        }
         void set_bit(const uint64_t& idx_item, const uint8_t& idx_bit, bool v)
        {
            if (v) _bits[idx_item + idx_bit] = 1;
            else   _bits[idx_item + idx_bit] = 0;
        }

        void print() const override;
        bool check_unknown() const;
        void set_unknown_to_draw();
        void clear_marker();
        void set_build(bool v) { _is_build = v; }

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
            if (sc == ExactScore::WIN) { set_bit(idx_item, 0, 1); set_bit(idx_item, 1, 1); }
            else if (sc == ExactScore::DRAW) { set_bit(idx_item, 0, 1); set_bit(idx_item, 1, 0); }
            else if (sc == ExactScore::LOSS) { set_bit(idx_item, 0, 0); set_bit(idx_item, 1, 1); }
            else { set_bit(idx_item, 0, 0); set_bit(idx_item, 1, 0); }
        }
        uint8_t dtc_at_idx(const uint64_t& idx)  const
        {
            return _dtc[idx];
        }

        void set_marker_at_idx(const uint64_t& idx_item, bool v)
        {
            if (v == true) set_bit(idx_item, 2, 1);
            else  set_bit(idx_item, 2, 0);
        }
        void set_dtc_at_idx(const uint64_t& idx, uint8_t v)
        {
            _dtc[idx] = v;
        }

        void writeBits(std::ostream& os) const;
        void readBits(std::istream& is);
    };

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::legal(const std::vector<uint16_t>& sq) const
    {
        if (sq.size() != NPIECE) return false;
        for (size_t z = 0; z < NPIECE; z++)
        {
            if (std::count(sq.begin(), sq.end(), sq[z]) > 1) 
                return false;
        }
        Board<PieceID, _BoardSize> work_board;
        work_board.clear();
        work_board.set_color(this->color());
        for (size_t z = 0; z < NPIECE; z++)  work_board.set_pieceid_at(this->_piecesID[z], sq[z]);
        if (!work_board.legal_pos())
            return false;

        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::valid_index(uint64_t index, Board<PieceID, _BoardSize>& _work_board, std::vector<uint16_t>& sq) const
    {
        if (sq.size() != NPIECE) return false;
        this->square_at_index_v(index, sq);
        for (size_t z = 0; z < NPIECE; z++)
        {
            if (std::count(sq.begin(), sq.end(), sq[z]) > 1)
                return false;
        }
        if (this->do_x_symmetry() && (this->can_translate_x(sq)))
            return false;

        _work_board.clear();
        _work_board.set_color(this->color());
        for (size_t z = 0; z < NPIECE; z++)  _work_board.set_pieceid_at(this->_piecesID[z], sq[z]);
        if (!_work_board.legal_pos())
            return false;

        return true;
    }

    // print
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline void Tablebase<PieceID, _BoardSize, NPIECE>::print() const
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
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::check_unknown() const
    {
        ExactScore sc;
        {
            for (uint64_t i = 0; i < this->_size_tb; i++)
            {
                sc = this->score_at_idx(i * this->_size_item);
                if (sc == ExactScore::UNKNOWN) 
                    return true;
            }
        }
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline void Tablebase<PieceID, _BoardSize, NPIECE>::set_unknown_to_draw()
    {
        ExactScore sc;
        {
            for (uint64_t i = 0; i < this->_size_tb; i++)
            {
                sc = this->score_at_idx(i * this->_size_item);
                if (sc == ExactScore::UNKNOWN)                          
                {
                    // ILLEGAL position exist in the TB _bits, check legal_pos()...
                    set_score_at_idx(i * _size_item, ExactScore::DRAW); // dtc...
                }
            }
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline void Tablebase<PieceID, _BoardSize, NPIECE>::clear_marker()
    {
        for (uint64_t i = 0; i < this->_size_tb; i++)
        {
            set_marker_at_idx(i * _size_item, false);
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::save_tb() const
    {
        std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("tablebase", name());
        std::ofstream os;
        os.open(f.c_str(), std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
        if (os.good())
        {
            writeBits(os);
            if (!os.bad())
            {
                os.close();
                return save_tb_dtc();
                return true;
            }
        }
        os.close();
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::save_tb_dtc() const
    {
        std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("tablebase", name() + ".dtc");
        std::ofstream os;
        os.open(f.c_str(), std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
        if (os.good())
        {
            os << (size_t)_dtc.size();
            for (uint64_t i = 0; i < _dtc.size(); i++)
            {
                os << (unsigned char)_dtc[i];
            }
            if (!os.bad())
            {
                os.close();
                return true;
            }
        }
        os.close();
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline void Tablebase<PieceID, _BoardSize, NPIECE>::writeBits(std::ostream& os) const
    {
        for (std::size_t bit_pos = 0; bit_pos < _bits.size(); )
        {
            unsigned char currentByte = 0;
            for (int currentBit = 0; currentBit < 8 * sizeof(unsigned char) && bit_pos < _bits.size(); ++currentBit, ++bit_pos)
            {
                currentByte |= _bits[bit_pos] ? (0x80 >> currentBit) : 0;
            }
            os << currentByte;
        }
        //0x80 >> 0 = 0b10000000
        //0x80 >> 1 = 0b01000000
        //0x80 >> 2 = 0b00100000
        //0x80 >> 3 = 0b00010000
        //0x80 >> 4 = 0b00001000
        //0x80 >> 5 = 0b00000100
        //0x80 >> 6 = 0b00000010
        //0x80 >> 7 = 0b00000001
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::load_tb()
    {
        if (_is_build)
        {
            if (is_symmetry_TB()) // futur...
                TB_Manager<PieceID, _BoardSize>::instance()->add_sym(name(), this);
            else
                TB_Manager<PieceID, _BoardSize>::instance()->add(name(), this);
        }
        else
        {
            if (!is_symmetry_TB())
            {
                _is_build = read_tb();
                if (_is_build)
                {
                    TB_Manager<PieceID, _BoardSize>::instance()->add(name(), this);
                }
            }
        }
        return _is_build;
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::read_tb()
    {
        std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("tablebase", name());
        std::ifstream is;
        is.open(f.c_str(), std::ofstream::in | std::ofstream::binary);
        if (is.good())
        {
            readBits(is);
            if (!is.bad())
            {
                is.close();
                return read_tb_dtc();
            }
        }
        is.close();
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::read_tb_dtc()
    {
        std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("tablebase", name() + ".dtc");
        std::ifstream is;
        is.open(f.c_str(), std::ofstream::in | std::ofstream::binary);
        if (is.good())
        {
            size_t n; is >> n;
            assert(n == _dtc.size());
            _dtc.fill(0);

            char* buffer = new char[n];
            is.read(buffer, n);
            if (is.bad())
            {
                is.close();
                return false;
            }
            for (uint64_t i = 0; i < n; i++) _dtc[i] = buffer[i];
            delete[]buffer;

            if (!is.bad())
            {
                is.close();
                return true;
            }
        }
        is.close();
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline uint8_t Tablebase<PieceID, _BoardSize, NPIECE>::checksum_dtc() const
    {
        uint16_t sum1 = 0;
        uint16_t sum2 = 0;
        for (uint64_t i = 0; i < _dtc.size(); i++)
        {
            sum1 = (sum1 + _dtc[i]) % 255;
            sum2 = (sum2 + sum1) % 255;
        }
        return (sum2 << 8) | sum1;
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline void Tablebase<PieceID, _BoardSize, NPIECE>::readBits(std::istream& is)
    {
        std::size_t num_bits = _bits.size();
        _bits.reset();
        std::size_t total_bytes = num_bits / (8 * sizeof(unsigned char)) + 1;
        std::size_t bit_pos = 0;
        for (std::size_t i = 0;
            is &&
            i < total_bytes &&
            bit_pos < num_bits;
            ++i)
        {
            unsigned char currentByte = 0;
            if (is >> currentByte)
            {
                for (int currentBit = 0;
                    currentBit < (8 * sizeof(unsigned char)) &&
                    bit_pos < num_bits;
                    ++currentBit, ++bit_pos)
                {
                    _bits[bit_pos] = (currentByte &  (0x80 >> currentBit)) > 0;
                }
            }
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline void Tablebase<PieceID, _BoardSize, NPIECE>::print_score(int n) const
    {
        ExactScore sc;
        for (uint64_t i = 0; i < this->_size_tb; i++)
        {
            if (i < n)
            {
                sc = this->score_at_idx(i * this->_size_item);
                std::cout << i << " : " << ExactScore_to_string(sc) << std::endl;
            }
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline void Tablebase<PieceID, _BoardSize, NPIECE>::print_dtc(int n) const
    {
        std::vector<long> v_win;
        std::vector<long> v_loss;
        std::vector<long> v_draw;
        for (uint64_t i = 0; i < 256; i++) { v_win.push_back(0); v_loss.push_back(0); v_draw.push_back(0); }

        ExactScore sc;
        for (uint64_t i = 0; i < this->_size_tb; i++)
        {
            sc = this->score_at_idx(i * this->_size_item);
            if (sc == ExactScore::WIN)
            {
                int k = _dtc[i];
                v_win[k] = v_win[k] + 1;
            }
            if (sc == ExactScore::LOSS)
            {
                int k = _dtc[i];
                v_loss[k] = v_loss[k] + 1;
            }
            if (sc == ExactScore::DRAW)
            {
                int k = _dtc[i];
                v_draw[k] = v_draw[k] + 1;
            }
        }
        for (uint64_t i = 0; i < n; i++)
            std::cout << i << " : " << v_win[i] << " " << v_loss[i] << " " << v_draw[i] << std::endl;
    }


    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline std::vector<uint64_t> Tablebase<PieceID, _BoardSize, NPIECE>::get_index_dtc(uint8_t value_dtc, ExactScore value_sc) const
    {
        std::vector<uint64_t> v;
        for (uint64_t i = 0; i < this->_size_tb; i++)
        {
            if ((this->score_at_idx(i * this->_size_item) == value_sc) && (_dtc[i] == value_dtc))
                v.push_back(i);
        }
        return v;
    }

    // ct()
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    Tablebase<PieceID, _BoardSize, NPIECE>::Tablebase(std::vector<PieceID>& v, PieceColor c)
        : TablebaseBase<PieceID, _BoardSize>(), _color(c), _NPIECE(NPIECE), _is_build(false), _bits(0)
    {
        for (auto& vv : v)
        {
            _piecesID.push_back(vv);
            _pieces.push_back(_Piece::get(vv));
        }
        _dtc.fill(0);
        for (uint64_t i = 0; i < this->_size_tb; i++) set_score_at_idx(i* this->_size_item, ExactScore::UNKNOWN);
        clear_marker();
    }

};
#endif
