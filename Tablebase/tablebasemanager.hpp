#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TablebaseManager
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASEMANAGER_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASEMANAGER_HPP

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseManager
    {
    private:
        TablebaseManager() {}

        TablebaseManager(const TablebaseManager&) = delete;
        TablebaseManager & operator=(const TablebaseManager &) = delete;
        TablebaseManager(TablebaseManager&&) = delete;

    public:
        ~TablebaseManager()
        {
            if (_instance.operator bool())
            {
                _instance.release();
            }
        }
        static const TablebaseManager* instance();

        bool add(const std::string& name, Tablebase<PieceID, _BoardSize, 1>* tb) const;
        bool add(const std::string& name, Tablebase<PieceID, _BoardSize, 2>* tb) const;
        bool add(const std::string& name, Tablebase<PieceID, _BoardSize, 3>* tb) const;
 
        Tablebase<PieceID, _BoardSize, 1>* find_1(const std::string& name) const;
        Tablebase<PieceID, _BoardSize, 2>* find_2(const std::string& name) const;
        Tablebase<PieceID, _BoardSize, 3>* find_3(const std::string& name) const;

        // prototype
        Tablebase<PieceID, _BoardSize, 1>* make_1(PieceColor c, const std::vector<std::pair<PieceID, uint8_t>>& p) const;

    public:
        static std::string name_pieces(const std::vector<PieceID>& v, PieceColor color_to_play)
        {
            // assume sorted
            std::string str_pieces;
            if (color_to_play == PieceColor::W) str_pieces = "W_"; else str_pieces = "B_";
            for (auto& vv : v) str_pieces += Piece<PieceID, _BoardSize>::to_str(vv, false);
            return str_pieces;
        }

    private:
        // Not owner
        mutable std::map<std::string, Tablebase<PieceID, _BoardSize, 1>*>  _tbs1;
        mutable std::map<std::string, Tablebase<PieceID, _BoardSize, 2>*>  _tbs2;
        mutable std::map<std::string, Tablebase<PieceID, _BoardSize, 3>*>  _tbs3;

        static std::unique_ptr<TablebaseManager> _instance;
    };

    //_instance
    template <typename PieceID, typename uint8_t _BoardSize>
    std::unique_ptr<TablebaseManager<PieceID, _BoardSize>>
    TablebaseManager<PieceID, _BoardSize>::_instance = nullptr;

    // add
    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseManager<PieceID, _BoardSize>::add(const std::string& name, Tablebase<PieceID, _BoardSize, 1>* tb) const
    {
        if (_instance == nullptr) return false;
        {
            auto iter = _tbs1.find(name);
            if (iter == _tbs1.end())
            {
                _tbs1[name] = tb;
                return true;
            }
            return true;
        }
        return false;
    }
    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseManager<PieceID, _BoardSize>::add(const std::string& name, Tablebase<PieceID, _BoardSize, 2>* tb) const
    {
        if (_instance == nullptr) return false;
        {
            auto iter = _tbs2.find(name);
            if (iter == _tbs2.end())
            {
                _tbs2[name] = tb;
                return true;
            }
            return true;
        }
        return false;
    }
    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseManager<PieceID, _BoardSize>::add(const std::string& name, Tablebase<PieceID, _BoardSize, 3>* tb) const
    {
        if (_instance == nullptr) return false;
        {
            auto iter = _tbs3.find(name);
            if (iter == _tbs3.end())
            {
                _tbs3[name] = tb;
                return true;
            }
            return true;
        }
        return false;
    }

    // find
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase<PieceID, _BoardSize, 1>*  TablebaseManager<PieceID, _BoardSize>::find_1(const std::string& name) const
    {
        if (_instance == nullptr) return nullptr;
        {
            auto iter = _tbs1.find(name);
            if (iter != _tbs1.end())
            {
                return _tbs1[name];
            }
        }
        return nullptr;
    }
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase<PieceID, _BoardSize, 2>*  TablebaseManager<PieceID, _BoardSize>::find_2(const std::string& name) const
    {
        if (_instance == nullptr) return nullptr;
        {
            auto iter = _tbs2.find(name);
            if (iter != _tbs2.end())
            {
                return _tbs2[name];
            }
        }
        return nullptr;
    }
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase<PieceID, _BoardSize, 3>*  TablebaseManager<PieceID, _BoardSize>::find_3(const std::string& name) const
    {
        if (_instance == nullptr) return nullptr;
        {
            auto iter = _tbs3.find(name);
            if (iter != _tbs3.end())
            {
                return _tbs3[name];
            }
        }
        return nullptr;
    }

    // instance()
    template <typename PieceID, typename uint8_t _BoardSize>
    const TablebaseManager<PieceID, _BoardSize>* 
    TablebaseManager<PieceID, _BoardSize>::instance()
    {
        if (_instance == nullptr)
        {
            _instance = std::unique_ptr<TablebaseManager>(new TablebaseManager);
            return _instance.get();
        }
        return  _instance.get();
    }


    // prototype
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase<PieceID, _BoardSize, 1>*  TablebaseManager<PieceID, _BoardSize>::make_1(PieceColor c, const std::vector<std::pair<PieceID, uint8_t>>& p) const
    {
        if (_instance == nullptr) return nullptr;

        {
            // 1v0
            if (c == PieceColor::W)
            {
                std::vector<PieceID> _p_1v0;
                for (size_t j = 0; j < p.size(); j++)
                {
                    if (p[j].second == 1)
                    {
                        _p_1v0.push_back(p[j].first);
                        return new Tablebase_1v0<PieceID, _BoardSize>* (_p_1v0, PieceColor::W);
                    }
                }
            }
            else
            {
                std::vector<PieceID> _p_0v1;
                for (size_t j = 0; j < p.size(); j++)
                {
                    if (p[j].second == 1)
                    {
                        _p_0v1.push_back(p[j].first);
                        return new Tablebase_1v0<PieceID, _BoardSize>* (_p_0v1, PieceColor::B);
                    }
                }
            }
        }
        return nullptr;
    }
};

#endif
