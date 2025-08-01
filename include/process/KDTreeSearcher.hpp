#pragma once
#include <nanoflann.hpp>
#include <vector>
#include <array>
#include <utility>  
#include <cmath>    

class KDTreeSearcherBand {
public:
    // Data structure
    using Spectrum = std::array<double, 7>;

    KDTreeSearcherBand() = default;

    // Construct KDTree
    explicit KDTreeSearcherBand(const std::vector<Spectrum>& points) {
        setData(points);
    }

    // Give Data
    void setData(const std::vector<Spectrum>& points) {
        cloud_.pts = points;
        index_ = std::make_unique<KDTree_t>(7 /*dim*/, cloud_, nanoflann::KDTreeSingleIndexAdaptorParams(10));
        index_->buildIndex();
    }

    // NN search
    std::pair<size_t, double> findNearest(const Spectrum& query) const {
        size_t ret_index;
        double out_dist_sqr;
        nanoflann::KNNResultSet<double> resultSet(1);
        resultSet.init(&ret_index, &out_dist_sqr);

        nanoflann::SearchParameters params;
        index_->findNeighbors(resultSet, query.data(), params);

        return {ret_index, std::sqrt(out_dist_sqr)};
    }

    // KNN search 
    std::vector<std::pair<size_t, double>> findKNearest(const Spectrum& query, size_t k) const {
        std::vector<size_t> indices(k);
        std::vector<double> dists(k);
    
        nanoflann::KNNResultSet<double> resultSet(k);
        resultSet.init(indices.data(), dists.data());
    
        nanoflann::SearchParameters params;
        index_->findNeighbors(resultSet, query.data(), params);
    
        std::vector<std::pair<size_t, double>> results;
        results.reserve(k);
        for (size_t i = 0; i < k; ++i) {
            results.emplace_back(indices[i], std::sqrt(dists[i]));
        }
        return results;
    }

private:
    // Internal Data structure
    struct SpectrumCloud {
        std::vector<Spectrum> pts;

        // Interface for nanoflann
        inline size_t kdtree_get_point_count() const { return pts.size(); }
        inline double kdtree_get_pt(const size_t idx, const size_t dim) const { return pts[idx][dim]; }

        template <class BBOX>
        bool kdtree_get_bbox(BBOX&) const { return false; }
    };

    using KDTree_t = nanoflann::KDTreeSingleIndexAdaptor<
        nanoflann::L2_Simple_Adaptor<double, SpectrumCloud>,
        SpectrumCloud,
        7 // Dimentions
    >;

    SpectrumCloud cloud_;
    std::unique_ptr<KDTree_t> index_;
};

class KDTreeSearcherCoord {
public:
    // Data structure
    using Point = std::array<double, 2>;

    KDTreeSearcherCoord() = default;

    // Construct KDTree
    explicit KDTreeSearcherCoord(const std::vector<Point>& points) {
        setData(points);
    }

    // Give Data
    void setData(const std::vector<Point>& points) {
        cloud_.pts = points;
        index_ = std::make_unique<KDTree_t>(2 /*dim*/, cloud_, nanoflann::KDTreeSingleIndexAdaptorParams(10));
        index_->buildIndex();
    }

    // KNN search
    std::pair<size_t, double> findNearest(const Point& query) const {
        size_t ret_index;
        double out_dist_sqr;
        nanoflann::KNNResultSet<double> resultSet(1);
        resultSet.init(&ret_index, &out_dist_sqr);

        nanoflann::SearchParameters params;
        index_->findNeighbors(resultSet, query.data(), params);

        return {ret_index, std::sqrt(out_dist_sqr)};
    }

private:
    // Internal Data structure
    struct PointCloud {
        std::vector<Point> pts;

        // Interface for nanoflann
        inline size_t kdtree_get_point_count() const { return pts.size(); }
        inline double kdtree_get_pt(const size_t idx, const size_t dim) const { return pts[idx][dim]; }

        template <class BBOX>
        bool kdtree_get_bbox(BBOX&) const { return false; }
    };

    using KDTree_t = nanoflann::KDTreeSingleIndexAdaptor<
        nanoflann::L2_Simple_Adaptor<double, PointCloud>,
        PointCloud,
        2 // Dimentions
    >;

    PointCloud cloud_;
    std::unique_ptr<KDTree_t> index_;
};

