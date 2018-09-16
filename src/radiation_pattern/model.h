#pragma once

#include <util/common/geom/point.h>
#include <util/common/math/vec.h>
#include <util/common/plot/plot.h>
#include <util/common/math/complex.h>

#include <vector>
#include <map>
#include <array>

#include <omp.h>

namespace model
{

    /*****************************************************/
    /*                     params                        */
    /*****************************************************/

    struct parameters
    {
        // system params
        double R, lambda, A;

        // board params
        size_t ncells;

        // other params
        double dx;
    };

    inline parameters make_default_parameters()
    {
        parameters p =
        {
            // system params
            100, 2, 1,

            // board params
            10,

            // other params
            1
        };
        return p;
    }

    /*****************************************************/
    /*                     data                          */
    /*****************************************************/
    
    using board_t = std::vector < std::vector < std::pair < geom::point < double >, bool > > > ;
    using mat_t   = std::vector < std::vector < double > > ;
    using grid_t  = std::vector < std::vector < geom::point < double > > > ;

    /*****************************************************/
    /*                     drawing                       */
    /*****************************************************/

    using points_t = std::vector < geom::point2d_t > ;

    struct plot_data
    {
        util::ptr_t < points_t > data;
        plot::list_drawable < points_t > :: ptr_t plot;
    };

    struct plot_config
    {
        plot::world_t::ptr_t world;
        plot::auto_viewport < points_t > :: ptr_t autoworld;
    };

    struct model_data
    {
        util::ptr_t < parameters > params;
        plot_config config;
        plot_data   penergy_data;
        plot_data   kenergy_data;
        plot_data   senergy_data;
        plot_data   denergy_data;
        board_t     board;
        grid_t      grid;
        mat_t       grid_data;
    };

    inline static plot_data make_plot_data
    (
        plot::palette::pen_ptr pen = plot::palette::pen(0xffffff),
        plot::list_data_format data_format = plot::list_data_format::chain
    )
    {
        plot_data pd;
        pd.data = util::create < points_t > ();
        pd.plot = plot::list_drawable < points_t > :: create
        (
            plot::make_data_source(pd.data),
            nullptr, // no point painter
            pen
        );
        pd.plot->data_format = data_format;
        return pd;
    }

    inline static plot::drawable::ptr_t make_root_drawable
    (
        const plot_config & p,
        std::vector < plot::drawable::ptr_t > layers
    )
    {
        using namespace plot;

        return viewporter::create(
            tick_drawable::create(
                layer_drawable::create(layers),
                const_n_tick_factory<axe::x>::create(
                    make_simple_tick_formatter(6, 8),
                    0,
                    5
                ),
                const_n_tick_factory<axe::y>::create(
                    make_simple_tick_formatter(6, 8),
                    0,
                    5
                ),
                palette::pen(RGB(80, 80, 80)),
                RGB(200, 200, 200)
            ),
            make_viewport_mapper(make_world_mapper < points_t > (p.autoworld))
        );
    }

    inline plot_config make_plot_config()
    {
        plot_config cfg;
        cfg.world = plot::world_t::create();
        cfg.autoworld = plot::min_max_auto_viewport < points_t > :: create();
        return cfg;
    }

    inline model_data make_model_data(const parameters & p = make_default_parameters())
    {
        model_data md;
        md.config = make_plot_config();
        md.params = util::create < parameters > (p);
        md.kenergy_data = make_plot_data(plot::palette::pen(0x0000ff, 2));
        md.penergy_data = make_plot_data(plot::palette::pen(0x0000ff, 2));
        md.senergy_data = make_plot_data(plot::palette::pen(0x0000ff, 2));
        md.denergy_data = make_plot_data(plot::palette::pen(0x0000ff, 2));
        return md;
    }

    inline void update_board(model_data & md)
    {
        if (md.board.size() == md.params->ncells) return;
        md.board.clear();
        md.board.resize(md.params->ncells);
        for (size_t i = 0; i < md.params->ncells; ++i)
        {
            md.board[i].resize(md.params->ncells);

            for (size_t j = 0; j < md.params->ncells; ++j)
            {
                md.board[i][j].first = {
                    (double) j - md.params->ncells / 2.0,
                    (double) i - md.params->ncells / 2.0
                };
            }
        }
    }

    enum view_type { diagram, flat, flat_z };

    inline void update_data(model_data & md, view_type vt)
    {
        double R = md.params->R;
        double w = 2 * R;
        size_t n = (size_t) std::floor(w / md.params->dx);
        double w0 = n * md.params->dx;
        double dx0 = (w - w0) / 2;

        md.grid.resize(n);
        md.grid_data.resize(n);
        for (int i = 0; i < n; ++i)
        {
            md.grid[i].resize(n);
            md.grid_data[i].resize(n);

            #pragma omp parallel for
            for (int j = 0; j < n; ++j)
            {
                auto p = geom::make_point(j * md.params->dx - w / 2,
                                          i * md.params->dx - w / 2);
                double z2 = R * R - p.x * p.x - p.y * p.y;
                if (z2 <= 0) continue;
                double z = std::sqrt(z2);

                math::v3 < math::complex <> > s;

                for (size_t k = 0; k < md.board.size(); ++k)
                for (size_t l = 0; l < md.board.size(); ++l)
                {
                    if (!md.board[k][l].second) continue;
                    auto rv = math::v3 < math::complex <> > (p.x, p.y, z) -
                              math::v3 < math::complex <> > (md.board[k][l].first.x, md.board[k][l].first.y, 0);
                    double d = math::norm(rv);
                    auto v = md.params->A / d * std::exp(math::_i * 2 * M_PI / md.params->lambda * d);
                    s = s + rv / d * v;
                }
                
                math::v3 < math::complex <> > rv = { p.x, p.y, z };
                math::v3 < math::complex <> > rz = { 0, 0, R };

                if ((vt == flat) || (vt == flat_z))
                {
                    md.grid[i][j] = p;
                    md.grid_data[i][j] = math::sqnorm(s * ((vt == flat) ? rv : rz) / R);
                }
                else
                {
                    double e = math::sqnorm(s * rv / R);
                
                    math::v3 <> rvr = { p.x, p.y, z };
                    rvr = rvr / R * e;
                
                    md.grid_data[i][j] = rvr.z;
                    md.grid[i][j] = { rvr.x, rvr.y };
                }
            }
        }
    }
}