#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// SymmetryTablebase
//  number white pieces n_w < number black pieces n_b
//
//
#ifndef _AL_CHESS_TABLEBASE_SymmetryTablebase_HPP
#define _AL_CHESS_TABLEBASE_SymmetryTablebase_HPP

namespace chess
{
    // SymmetryTablebase
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    class SymmetryTablebase : public TablebaseBase<PieceID, _BoardSize>
    {
    public:
        SymmetryTablebase(Tablebase<PieceID, _BoardSize, NPIECE>* refTB) : TablebaseBase(), _refTB(refTB) {}
        ~SymmetryTablebase() {}

        bool is_symmetry_TB() const override { return true; }
        bool is_build() const       override { return _refTB->is_build(); }
        bool load()                 override 
        { 
            if (_refTB->load())
            {
                PieceSet<PieceID, _BoardSize> master(_refTB->piecesID());
                PieceSet<PieceID, _BoardSize> reverse({ PieceSet<PieceID, _BoardSize>::reverse_color_set(master.bset()), PieceSet<PieceID, _BoardSize>::reverse_color_set(master.wset()) });

                std::string nm = reverse.name((_refTB->color() == PieceColor::W) ? PieceColor::B : PieceColor::W);
                TablebaseBase<PieceID, _BoardSize>* t = TB_Manager<PieceID, _BoardSize>::instance()->find_sym(nm);
                if (t == nullptr)
                {
                    TB_Manager<PieceID, _BoardSize>::instance()->add_sym(nm, this);
                }
            }
            else
            {
                return false;
            }
            return true;
        }
        bool save() const           override { return true; /*_refTB->save();*/ }
        void print() const          override {}
        uint8_t getNPIECE() const   override { return _refTB->getNPIECE(); }

        ExactScore score_v(const std::vector<uint16_t>& sq) const override   
        { 
            return reverse_score(_refTB->score_v(reverse_order_sq(sq, false)));
        }

        uint8_t dtc_v(const std::vector<uint16_t>& sq) const override   
        { 
            return _refTB->dtc_v(reverse_order_sq(sq, false));
        }
        
        const Tablebase<PieceID, _BoardSize, NPIECE>* refTB() const { return _refTB; }
        bool check_score() const;

    protected:
        Tablebase<PieceID, _BoardSize, NPIECE>* _refTB;
        std::vector<uint16_t> reverse_order_sq(const std::vector<uint16_t>& sq, bool is_ref_to_sym) const;
    };

    // reverse_order_sq
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    std::vector<uint16_t> SymmetryTablebase<PieceID, _BoardSize, NPIECE>::reverse_order_sq(const std::vector<uint16_t>& sq, bool is_ref_to_sym) const
    {
        std::vector<uint16_t> sq_copy = sq;
        reverse_sq_v(sq_copy, _BoardSize);
        //Re-order piece square
        PieceSet<PieceID, _BoardSize> master(refTB()->piecesID());
        uint16_t nw = master.count_all_piece(PieceColor::W);
        uint16_t nb = master.count_all_piece(PieceColor::B);
        std::vector<uint16_t> sq_reverse;
        if (!is_ref_to_sym)
        {
            for (size_t i = 0; i < nw; i++) sq_reverse.push_back(sq_copy[nb + i]);
            for (size_t i = 0; i < nb; i++) sq_reverse.push_back(sq_copy[0 + i]);
        }
        else
        {
            for (size_t i = 0; i < nb; i++) sq_reverse.push_back(sq_copy[nw + i]);
            for (size_t i = 0; i < nw; i++) sq_reverse.push_back(sq_copy[0 + i]);
        }
        return sq_reverse;
    }

    // bool check_score() const;
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    bool  SymmetryTablebase<PieceID, _BoardSize, NPIECE>::check_score() const
    {
        bool        legal_pos;
        std::vector<uint16_t> sq;

        for (size_t z = 0; z < NPIECE; z++) sq.push_back(0);
        Board<PieceID, _BoardSize>* _work_board = new Board<PieceID, _BoardSize>();

        ExactScore sc;
        ExactScore sc_sym;
        for (uint64_t i = 0; i < refTB()->size_tb(); i++)
        {
            refTB()->square_at_index_v(i, sq);
            assert(sq.size() == NPIECE);

            legal_pos = true;
            for (size_t z = 0; z < NPIECE; z++)
            {
                if (std::count(sq.begin(), sq.end(), sq[z]) > 1) { legal_pos = false; break; }
            }
            if (!legal_pos) continue;
            if (refTB()->do_x_symmetry() && (refTB()->can_translate_x(sq)))  continue;

            std::vector<PieceID> pieces = refTB()->piecesID();

            _work_board->clear();
            _work_board->set_color(refTB()->color());
            for (size_t z = 0; z < NPIECE; z++)  _work_board->set_pieceid_at(pieces[z], sq[z]);
            if (!_work_board->legal_pos())
                continue;

            sc = refTB()->score_v(sq);
            sc_sym = this->score_v(this->reverse_order_sq(sq, true));
            if (sc != reverse_score(sc_sym))
            {
                delete _work_board;
                return false;
            }
        }

        delete _work_board;
        return true;
    }

    // TBH_Symmetry
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    class TBH_Symmetry : public TBH_Base<PieceID, _BoardSize>
    {
        using _Board = Board<PieceID, _BoardSize>;

    public:
        TBH_Symmetry(TBH<PieceID, _BoardSize>* refTBH, TB_TYPE t)
            : TBH_Base<PieceID, _BoardSize>(), _refTBH(refTBH), _type(t)
        {
            assert( t == sym_tb_type(refTBH->tb_type()) );
        }
        ~TBH_Symmetry() {}

        bool is_symmetry_TBH() const override { return true; }
        bool load() override               
        { 
            assert(_type == sym_tb_type(symTBH()->tb_type()));
            if (_refTBH->load())
            {
                // ex: white_to_play 2v1 score <==> reverse score of black_to_play 1v2 (sym_tb is when nw < nb)
                PieceSet<PieceID, _BoardSize> master(_refTBH->_piecesID);
                PieceSet<PieceID, _BoardSize> reverse({ PieceSet<PieceID, _BoardSize>::reverse_color_set(master.bset()), PieceSet<PieceID, _BoardSize>::reverse_color_set(master.wset()) });
                
                TablebaseBase<PieceID, _BoardSize>* tbw = TB_Manager<PieceID, _BoardSize>::instance()->find_sym(reverse.name(PieceColor::B));
                if (tbw == nullptr)
                {
                    TablebaseBase<PieceID, _BoardSize>* sym_tb = (TablebaseBase<PieceID, _BoardSize>*)new SymmetryTablebase<PieceID, _BoardSize, NPIECE>(( Tablebase<PieceID, _BoardSize, NPIECE>* )_refTBH->TB_W_const());
                    TB_Manager<PieceID, _BoardSize>::instance()->add_sym(reverse.name(PieceColor::B), sym_tb);
                }

                TablebaseBase<PieceID, _BoardSize>* tbb = TB_Manager<PieceID, _BoardSize>::instance()->find_sym( reverse.name(PieceColor::W));
                if (tbb == nullptr)
                {
                    TablebaseBase<PieceID, _BoardSize>* sym_tb = (TablebaseBase<PieceID, _BoardSize>*)new SymmetryTablebase<PieceID, _BoardSize, NPIECE>((Tablebase<PieceID, _BoardSize, NPIECE>*)_refTBH->TB_B_const());
                    TB_Manager<PieceID, _BoardSize>::instance()->add_sym(reverse.name(PieceColor::W), sym_tb);
                }
            }
            else
            {
                return false;
            }
            return true;
        }
        bool save() const override          { return _refTBH->save();  }
        bool build(char verbose) override   { return _refTBH->build(verbose); }
        bool is_build() const override      { return _refTBH->is_build(); }

        TBH<PieceID, _BoardSize>* symTBH() const  { return _refTBH; }

    protected:
        TB_TYPE                   _type;
        TBH<PieceID, _BoardSize>* _refTBH;
    };

};
#endif

